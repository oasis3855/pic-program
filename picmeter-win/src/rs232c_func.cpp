//
// RS-232C ����f�[�^���擾����X���b�h
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : �����_��M�\

#include "stdafx.h"
#include <process.h>
#include <time.h>
#include "rs232c_func.h"

#define BUFFERSIZE 1024
#define TIMEOUT 50
#define MAXFILEPATH 2048

// �O������Q�ƁA�ݒ肷�邽�߂̕ϐ�
volatile char _str_comReceived[BUFFERSIZE];	// ��M������A�O���Q�Ɨp�ɒ񋟂�����M�f�[�^
volatile BOOL _b_InThread;					// �X���b�h���쒆�̃t���O
volatile BOOL _b_ThreadExit;				// �X���b�h���I��������w��
volatile char _chr_comEnd;					// �f�[�^�s�̍Ō����������
volatile BOOL _b_debug_out;					// �f�o�b�O�o�͂��s��
volatile int _n_data_out;					// �f�[�^�̃t�@�C���o�͂��s��  1=���f�[�^�A2=�ϊ���
volatile char _str_out_filename[MAXFILEPATH];	// �f�o�b�O�o�́A�f�[�^�o�͂��s���t�@�C����
volatile double _n_Data;					// �ϊ���̃f�[�^

volatile char _str_delimit[10];				// �f�[�^�̋�؂蕶��
volatile int _n_data_format;				// 10�i��=0, 16�i��=1
volatile int _n_column;						// �f�[�^�ʒu �i�J�����j
volatile char _str_portno[10];				// �|�[�g��, "COM1" �Ȃǂ̕�����
volatile DWORD _n_com_speed;				// �V���A���|�[�g�̃X�s�[�h DCB�`��
volatile double _n_factor;					// �ϊ��搔
volatile double _n_ycut;					// �ϊ��x�ؕ�
volatile double _n_max;						// �ő�l
volatile double _n_min;						// �ŏ��l
volatile int _n_ignore;						// ���̕������ȉ��̓G���[�Ƃ��Ė���

// �X���b�h�������p
char _str_comRawBuf[BUFFERSIZE];			// RS-232C �Ŏ�M�������f�[�^�i����������p�j
char _str_comRingBuf[BUFFERSIZE];			// RS-232C �Ŏ�M�������f�[�^���ꎞ���߂Ēu��

// RS232C �ݒ�̂��߂̕ϐ�
HANDLE _h_comHandle;						// �t�@�C���n���h���iCOM�|�[�g)
DCB _comDCB;								// COM�|�[�g�ݒ�p�ASetCommState ���ŗ��p
COMSTAT _comStat;
COMMTIMEOUTS _comTimeout;
OVERLAPPED _comOverlapEvent;
OVERLAPPED _comOverlapRrceive;
ULONG _comErrorMask;

// 
FILE *fi;									// �f�o�b�O�E�f�[�^�o�͗p�t�@�C��


//*********************
// RS-232C ����񓯊��Ńf�[�^��ǂݍ��ރX���b�h
//*********************
void com_read_thread_main(void *pVoid)
{
	DWORD nEventMask;
	DWORD nErrorMask;
	DWORD nReadSize;
	double n;

	// �ϊ���f�[�^�o�͂ɗ��p����A���ݎ����𓾂邽�߂̈ꎞ�ϐ�
	struct tm tm_Tm;
	time_t tm_Time_t;

	// �X���b�h���쒆�������t���O�B�Ăяo�����X���b�h���瓮��m�F���邽�߂ɃZ�b�g
	_b_InThread = TRUE;

	// �f�[�^�o�͗p�t�@�C�����J��
	fi = NULL;		// ���S�̂��߁A�ꉞNULL�����Ă���
	if(_b_debug_out || _n_data_out != 0)
	{	// �f�o�b�O�E�f�[�^�p�t�@�C�����J��
		fi = ::fopen((char*)_str_out_filename, "wb");
		if(fi == NULL)
		{
			_b_debug_out = FALSE;
			_n_data_out = 0;
		}
	}

	// �V���A���|�[�g���J���āA�����ݒ肷��
	if(com_init() == FALSE)
	{
		if(_b_debug_out) fputs("com_init, RS-232C initialize error, thread exit\r\n", fi);
		com_close();
		_b_InThread = FALSE;
		_endthread();
		return;
	}

	// �V���A���|�[�g���f�[�^����M���A��M���������񂩂����f�[�^�����o��
	for(;;)
	{
		// �V���A���|�[�g�ɓ��蕶�������͂��ꂽ�ꍇ�ɁA�C�x���g�𔭐������邽�߂̐ݒ�
		// ���蕶���́Acom_init()�֐�����DCB���g���Đݒ肵�����̂𗘗p����
		::SetCommMask(_h_comHandle, EV_RXFLAG);
		::WaitCommEvent(_h_comHandle, &nEventMask, &_comOverlapEvent);

		// �V���A���|�[�g�̃C�x���g�����҂�
		for(;;)
		{
			// �C�x���g�A�܂���300�~���b �҂�
			if(::WaitForSingleObject(_comOverlapEvent.hEvent, 300) == WAIT_OBJECT_0)
			{	// �C�x���g�����������ꍇ
				::ClearCommError(_h_comHandle, &nErrorMask, &_comStat);
				::ResetEvent(_comOverlapEvent.hEvent);
				if(nErrorMask|CE_IOE)
				{
					::PurgeComm(_h_comHandle, PURGE_RXABORT);
				}
				break;					// for���[�v�𔲂���
			}
			// �X���b�h�N�����̃X���b�h����A�I�����b�Z�[�W���󂯂��ꍇ�A�X���b�h���I������
			if(_b_ThreadExit == TRUE)
			{
				::strcpy((char*)_str_comReceived, "Thread exit");
				com_close();			// COM�|�[�g�����
				if(_b_debug_out || _n_data_out != 0)
				{						// �f�o�b�O�p�t�@�C�������
					if(fi != NULL) ::fclose(fi);
				}
				_b_InThread = FALSE;
				_endthread();			// �X���b�h�I��
				return;
			}

		}

		// �V���A���|�[�g���當�������M����
		for(;;)
		{
			if(_b_debug_out) fputs("string memory cleared\r\n", fi);	// �f�o�b�O�o��

			::memset(_str_comRawBuf, 0, BUFFERSIZE);
			::memset(_str_comRingBuf, 0, BUFFERSIZE);

			if((nReadSize = com_read_line()) == 0)
			{	// �ǂݍ��݃T�C�Y���[���̂Ƃ�
				if(_b_debug_out) fprintf(fi, "com_read_line  nReadSize = %d\r\n", nReadSize);	// �f�o�b�O�o��
				break;
			}
			if(_b_debug_out)
			{	// �f�o�b�O�o��
				fprintf(fi, "com_read_line  nReadSize = %d\r\n", nReadSize);
				fputs("com_read_line  _str_comRawBuf = \r\n", fi);
				_debug_binstr_write(_str_comRawBuf, nReadSize);
			}
			for(;;)
			{
				if(com_pharse_line(nReadSize) == FALSE)
				{	// �����񂪊������Ȃ������Ƃ�
					if(_b_debug_out) fputs("line string not completed, wait next\r\n==============\r\n", fi); // �f�o�b�O�o��
					break;	// ���̓ǂݍ��݂�҂�
				}
				else
				{	// �����񂪓���ꂽ�Ƃ�
					if(::strlen(_str_comRingBuf) > (UINT)_n_ignore)
					{	// �����񒷂����A�w��l�ȏ�̓f�[�^������Ƃ��Ĉ���
						::strcpy((char*)_str_comReceived, _str_comRingBuf);
						if(_b_debug_out) fprintf(fi, "line string completed, _str_comReceived = %s\r\n", _str_comReceived); // �f�o�b�O�o��
						if(_n_data_out == 1) fprintf(fi, "%s\r\n", _str_comReceived);

						// �����񂩂�f�[�^��؂�o���A���l�ɕϊ�����
						n = com_cut_data((char*)_str_comReceived, _n_column, _n_data_format);
						if(n<(double)_n_min || n>(double)_n_max)
						{	// �ϊ���̒l���A�͈͂𒴂��Ă���ꍇ�̓G���[
							if(_b_debug_out) fprintf(fi, "data is out of range, data = %lf\r\n==============\r\n", n); // �f�o�b�O�o��
						}
						else
						{	// �f�[�^�ϊ�����
							_n_Data = (double)n * _n_factor + _n_ycut;
							if(_b_debug_out) fprintf(fi, "data ok, data = %lf (raw = %lf)\r\n==============\r\n", _n_Data, n); // �f�o�b�O�o��
							if(_n_data_out == 2)
							{	// �ϊ���f�[�^�o��
								::time(&tm_Time_t);
								tm_Tm = *localtime(&tm_Time_t);
								fprintf(fi, "%02d:%02d:%02d %lf\r\n", tm_Tm.tm_hour, tm_Tm.tm_min, tm_Tm.tm_sec, _n_Data);
							}
						}
					}
					else
					{	// �����񒷂����A�w��l�ȉ��̓G���[�Ƃ��ăX�L�b�v����
						if(_b_debug_out) fprintf(fi, "line string short, dropped, _str_comRingBuf = %s\r\n==============\r\n", _str_comRingBuf); // �f�o�b�O�o��
					}
				}
			}
		}
	}
}


//*********************
// RS232C��������
//*********************
BOOL com_init(void)
{
	_comOverlapEvent.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	_comOverlapEvent.Internal = 0;
	_comOverlapEvent.InternalHigh = 0;
	_comOverlapEvent.Offset = 0;
	_comOverlapEvent.OffsetHigh = 0;

	_comOverlapRrceive.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	_comOverlapRrceive.Internal = 0;
	_comOverlapRrceive.InternalHigh = 0;
	_comOverlapRrceive.Offset = 0;
	_comOverlapRrceive.OffsetHigh = 0;

	_h_comHandle = ::CreateFile((LPCSTR)::_str_portno, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
							FILE_FLAG_OVERLAPPED, NULL);
	if(_h_comHandle == INVALID_HANDLE_VALUE)
		return FALSE;

	if(!::GetCommState(_h_comHandle, &_comDCB)) return FALSE;

	if(!::GetCommTimeouts(_h_comHandle, &_comTimeout)) return FALSE;

	_comDCB.BaudRate = _n_com_speed;			// �ʐM���x
	_comDCB.fParity = NOPARITY;
	_comDCB.ByteSize = 8;
	_comDCB.StopBits = ONESTOPBIT;				// �X�g�b�v�r�b�g = 1
	_comDCB.fRtsControl = RTS_CONTROL_DISABLE;
	_comDCB.fOutxDsrFlow = FALSE;				// �t���[���䖳��
	_comDCB.fDsrSensitivity = FALSE;
	_comDCB.fAbortOnError = FALSE;
	_comDCB.EvtChar = _chr_comEnd;				// �C�x���g�𔭐�������s���̕���(char)

	_comTimeout.ReadIntervalTimeout = MAXWORD;
	_comTimeout.ReadTotalTimeoutConstant = 0;
	_comTimeout.ReadTotalTimeoutMultiplier = 50;

	if(!::SetCommState(_h_comHandle, &_comDCB)) return FALSE;

	if(!::SetCommTimeouts(_h_comHandle, &_comTimeout)) return FALSE;

	return TRUE;
}

//*********************
// RS232C�����
//*********************
void com_close(void)
{
	if(_h_comHandle == INVALID_HANDLE_VALUE) return;

	::SetCommMask(_h_comHandle, 0);
	::PurgeComm(_h_comHandle, PURGE_RXCLEAR);
	::PurgeComm(_h_comHandle, PURGE_TXCLEAR);
	::PurgeComm(_h_comHandle, PURGE_RXABORT);
	::PurgeComm(_h_comHandle, PURGE_TXABORT);

	::CloseHandle(_comOverlapRrceive.hEvent);
	::CloseHandle(_comOverlapEvent.hEvent);
	::CloseHandle(_h_comHandle);

	return;
}

//*********************
// RS232C��M�o�b�t�@���A�f�[�^���擾����
//*********************
DWORD com_read_raw(DWORD nCnt)
{
	DWORD nReadSize = 0;
	DWORD nEndtime;

	if(!::ReadFile(_h_comHandle, _str_comRawBuf, nCnt, &nReadSize, &_comOverlapRrceive))
	{	// �ǂݍ��݂����s�����ꍇ
		nReadSize = 0;
		if(::GetLastError() == ERROR_IO_PENDING)
		{	// �񓯊��ǂݎ�肪�������Ă��Ȃ��ꍇ
			// OVERLAPPED �\���̂Ŏw�肵���C�x���g���V�O�i����Ԃ܂ő҂�
			nEndtime = ::GetTickCount() + 200;		// Windows�N������̃~���b����200����
			while(!::GetOverlappedResult(_h_comHandle, &_comOverlapRrceive,  &nReadSize, FALSE))
			{	// �ǂݍ��݂��������̊ԁA�҂��Â���
				::Sleep(5);		// 5msec �҂�
				if(::GetTickCount() > nEndtime) break;	// 200�~���b�҂��ēǂݍ��߂Ȃ��Ƃ��́A�G���[�I��
			}
		}
		else
		{	// ���̑��̓ǂݎ��G���[�̏ꍇ
		}
	}
	::ResetEvent(_comOverlapRrceive.hEvent);
	return nReadSize;
}

//*********************
// RS232C���A1�s��M����
//*********************
DWORD com_read_line(void)
{
	DWORD nReadSize = 0;

	::ClearCommError(_h_comHandle, &_comErrorMask, &_comStat);
	if(_comStat.cbInQue == 0) return 0;		// ���̓o�b�t�@�i�L���[�j�ɉ��������ꍇ�A�I��

	if(_comStat.cbInQue > BUFFERSIZE)
	{	// �V�X�e���o�b�t�@�ɁABUFFERSIZE ��葽���̃f�[�^������Ƃ�
		nReadSize = com_read_raw(BUFFERSIZE);
	}
	else
	{
		nReadSize = com_read_raw(_comStat.cbInQue);
	}

	return nReadSize;		// �ǂݍ��񂾃o�C�g����Ԃ�
}


//*********************
// RS232C���A1�s��M����
//*********************
BOOL com_pharse_line(DWORD nReadSize)
{
	static UINT nReadPoint = 0;
	static UINT nWritePoint = 0;
	static UINT nTempBufPoint = 0;
	static char strTempBuf[BUFFERSIZE];

	if(_b_debug_out)	// �f�o�b�O�o��
		fprintf(fi, "com_pharse_line  init\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d, nReadSize = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint, nReadSize);

	while(nReadPoint<nReadSize && nWritePoint<BUFFERSIZE)
	{
		// �o�b�t�@�ɖ������̃f�[�^�����݂���ꍇ
		if((strTempBuf[nWritePoint++] = _str_comRawBuf[nReadPoint++]) == _chr_comEnd)
		{	// ��؂蕶���ɒB�����Ƃ�
			strTempBuf[nWritePoint] = (char)NULL;	// ������I����NULL��t��
			::strcpy(_str_comRingBuf, strTempBuf);
			if(_b_debug_out)
			{	// �f�o�b�O�o��
				fprintf(fi, "com_pharse_line  detect _chr_comEnd\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint);
				fputs("com_pharse_line  strTempBuf = ", fi);
				_debug_binstr_write(strTempBuf, nWritePoint-1);
				fprintf(fi, "com_pharse_line  _str_comRingBuf = %s", _str_comRingBuf);
			}
			nWritePoint = 0;		// �������݃|�C���^��擪�ɖ߂�
			return TRUE;			// �����񊮌��ŏI��
		}
	}

	// ��؂蕶���ɒB����O�ɁARaw�o�b�t�@�I�[�ɒB����
	if(_b_debug_out)
	{	// �f�o�b�O�o��
		fprintf(fi, "com_pharse_line  remain unfinished char\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint);
		fputs("com_pharse_line  strTempBuf = ", fi);
		_debug_binstr_write(strTempBuf, nWritePoint-1);
	}
	nReadPoint = 0;		// �ǂݍ��݃|�C���^��擪�ɖ߂�

	return FALSE;		// �����񖢊����ŏI��
}

//*********************
// �����񂩂�f�[�^��؂�o��
// column �ڂ̃e�L�X�g�f�[�^���A�����ɂ���imode 10�i��=0, 16�i��=1)
//*********************
double com_cut_data(char *buf, int column, int mode)
{
	char strTemp[BUFFERSIZE];
	char *strOutput;
	int i;

	strcpy(strTemp, buf);		// strtok �̕�����j��̂��߁A�R�s�[�������̂𗘗p

	strOutput = ::strtok(strTemp, (char*)_str_delimit);	// �P�ڂ�؂�o��
	for(i=1; i<column && strOutput!=NULL; i++)
	{
		strOutput = ::strtok(NULL, (char*)_str_delimit);
	}

	if(i<column || strOutput==NULL) return(_n_min - 1);		// column�ڂ̃f�[�^�͑��݂��Ȃ�

	if(mode == 1)
	{	// 16�i�����[�h�̂Ƃ�
		sscanf(strOutput, "%x", &i);
		return (double)i;
	}

	return atof(strOutput);		// 10�i�����[�h�̂Ƃ�

}


//*********************
// �����o�b�t�@�̃f�o�b�O�o�� �i�s����NULL�łȂ��ꍇ�̑Ώ��j
//*********************
void _debug_binstr_write(char *buf, int nLen)
{
	int i;

	for(i=0; i<nLen; i++)
	{
		fputc(*(buf+i), fi);
	}

	fprintf(fi, "\r\n");
}


