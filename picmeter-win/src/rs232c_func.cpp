//
// RS-232C からデータを取得するスレッド
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : 小数点受信可能

#include "stdafx.h"
#include <process.h>
#include <time.h>
#include "rs232c_func.h"

#define BUFFERSIZE 1024
#define TIMEOUT 50
#define MAXFILEPATH 2048

// 外部から参照、設定するための変数
volatile char _str_comReceived[BUFFERSIZE];	// 受信完了後、外部参照用に提供される受信データ
volatile BOOL _b_InThread;					// スレッド動作中のフラグ
volatile BOOL _b_ThreadExit;				// スレッドを終了させる指令
volatile char _chr_comEnd;					// データ行の最後を示す文字
volatile BOOL _b_debug_out;					// デバッグ出力を行う
volatile int _n_data_out;					// データのファイル出力を行う  1=生データ、2=変換後
volatile char _str_out_filename[MAXFILEPATH];	// デバッグ出力、データ出力を行うファイル名
volatile double _n_Data;					// 変換後のデータ

volatile char _str_delimit[10];				// データの区切り文字
volatile int _n_data_format;				// 10進数=0, 16進数=1
volatile int _n_column;						// データ位置 （カラム）
volatile char _str_portno[10];				// ポート名, "COM1" などの文字列
volatile DWORD _n_com_speed;				// シリアルポートのスピード DCB形式
volatile double _n_factor;					// 変換乗数
volatile double _n_ycut;					// 変換Ｙ切片
volatile double _n_max;						// 最大値
volatile double _n_min;						// 最小値
volatile int _n_ignore;						// この文字数以下はエラーとして無視

// スレッド内部利用
char _str_comRawBuf[BUFFERSIZE];			// RS-232C で受信した生データ（内部処理専用）
char _str_comRingBuf[BUFFERSIZE];			// RS-232C で受信した生データを一時ためて置く

// RS232C 設定のための変数
HANDLE _h_comHandle;						// ファイルハンドル（COMポート)
DCB _comDCB;								// COMポート設定用、SetCommState 等で利用
COMSTAT _comStat;
COMMTIMEOUTS _comTimeout;
OVERLAPPED _comOverlapEvent;
OVERLAPPED _comOverlapRrceive;
ULONG _comErrorMask;

// 
FILE *fi;									// デバッグ・データ出力用ファイル


//*********************
// RS-232C から非同期でデータを読み込むスレッド
//*********************
void com_read_thread_main(void *pVoid)
{
	DWORD nEventMask;
	DWORD nErrorMask;
	DWORD nReadSize;
	double n;

	// 変換後データ出力に利用する、現在時刻を得るための一時変数
	struct tm tm_Tm;
	time_t tm_Time_t;

	// スレッド動作中を示すフラグ。呼び出し元スレッドから動作確認するためにセット
	_b_InThread = TRUE;

	// データ出力用ファイルを開く
	fi = NULL;		// 安全のため、一応NULLを入れておく
	if(_b_debug_out || _n_data_out != 0)
	{	// デバッグ・データ用ファイルを開く
		fi = ::fopen((char*)_str_out_filename, "wb");
		if(fi == NULL)
		{
			_b_debug_out = FALSE;
			_n_data_out = 0;
		}
	}

	// シリアルポートを開いて、初期設定する
	if(com_init() == FALSE)
	{
		if(_b_debug_out) fputs("com_init, RS-232C initialize error, thread exit\r\n", fi);
		com_close();
		_b_InThread = FALSE;
		_endthread();
		return;
	}

	// シリアルポートよりデータを受信し、受信した文字列から特定データを取り出す
	for(;;)
	{
		// シリアルポートに特定文字が入力された場合に、イベントを発生させるための設定
		// 特定文字は、com_init()関数中でDCBを使って設定したものを利用する
		::SetCommMask(_h_comHandle, EV_RXFLAG);
		::WaitCommEvent(_h_comHandle, &nEventMask, &_comOverlapEvent);

		// シリアルポートのイベント発生待ち
		for(;;)
		{
			// イベント、または300ミリ秒 待つ
			if(::WaitForSingleObject(_comOverlapEvent.hEvent, 300) == WAIT_OBJECT_0)
			{	// イベントが発生した場合
				::ClearCommError(_h_comHandle, &nErrorMask, &_comStat);
				::ResetEvent(_comOverlapEvent.hEvent);
				if(nErrorMask|CE_IOE)
				{
					::PurgeComm(_h_comHandle, PURGE_RXABORT);
				}
				break;					// forループを抜ける
			}
			// スレッド起動元のスレッドから、終了メッセージを受けた場合、スレッドを終了する
			if(_b_ThreadExit == TRUE)
			{
				::strcpy((char*)_str_comReceived, "Thread exit");
				com_close();			// COMポートを閉じる
				if(_b_debug_out || _n_data_out != 0)
				{						// デバッグ用ファイルを閉じる
					if(fi != NULL) ::fclose(fi);
				}
				_b_InThread = FALSE;
				_endthread();			// スレッド終了
				return;
			}

		}

		// シリアルポートから文字列を受信する
		for(;;)
		{
			if(_b_debug_out) fputs("string memory cleared\r\n", fi);	// デバッグ出力

			::memset(_str_comRawBuf, 0, BUFFERSIZE);
			::memset(_str_comRingBuf, 0, BUFFERSIZE);

			if((nReadSize = com_read_line()) == 0)
			{	// 読み込みサイズがゼロのとき
				if(_b_debug_out) fprintf(fi, "com_read_line  nReadSize = %d\r\n", nReadSize);	// デバッグ出力
				break;
			}
			if(_b_debug_out)
			{	// デバッグ出力
				fprintf(fi, "com_read_line  nReadSize = %d\r\n", nReadSize);
				fputs("com_read_line  _str_comRawBuf = \r\n", fi);
				_debug_binstr_write(_str_comRawBuf, nReadSize);
			}
			for(;;)
			{
				if(com_pharse_line(nReadSize) == FALSE)
				{	// 文字列が完結しなかったとき
					if(_b_debug_out) fputs("line string not completed, wait next\r\n==============\r\n", fi); // デバッグ出力
					break;	// 次の読み込みを待つ
				}
				else
				{	// 文字列が得られたとき
					if(::strlen(_str_comRingBuf) > (UINT)_n_ignore)
					{	// 文字列長さが、指定値以上はデータ文字列として扱う
						::strcpy((char*)_str_comReceived, _str_comRingBuf);
						if(_b_debug_out) fprintf(fi, "line string completed, _str_comReceived = %s\r\n", _str_comReceived); // デバッグ出力
						if(_n_data_out == 1) fprintf(fi, "%s\r\n", _str_comReceived);

						// 文字列からデータを切り出し、数値に変換する
						n = com_cut_data((char*)_str_comReceived, _n_column, _n_data_format);
						if(n<(double)_n_min || n>(double)_n_max)
						{	// 変換後の値が、範囲を超えている場合はエラー
							if(_b_debug_out) fprintf(fi, "data is out of range, data = %lf\r\n==============\r\n", n); // デバッグ出力
						}
						else
						{	// データ変換完了
							_n_Data = (double)n * _n_factor + _n_ycut;
							if(_b_debug_out) fprintf(fi, "data ok, data = %lf (raw = %lf)\r\n==============\r\n", _n_Data, n); // デバッグ出力
							if(_n_data_out == 2)
							{	// 変換後データ出力
								::time(&tm_Time_t);
								tm_Tm = *localtime(&tm_Time_t);
								fprintf(fi, "%02d:%02d:%02d %lf\r\n", tm_Tm.tm_hour, tm_Tm.tm_min, tm_Tm.tm_sec, _n_Data);
							}
						}
					}
					else
					{	// 文字列長さが、指定値以下はエラーとしてスキップする
						if(_b_debug_out) fprintf(fi, "line string short, dropped, _str_comRingBuf = %s\r\n==============\r\n", _str_comRingBuf); // デバッグ出力
					}
				}
			}
		}
	}
}


//*********************
// RS232Cを初期化
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

	_comDCB.BaudRate = _n_com_speed;			// 通信速度
	_comDCB.fParity = NOPARITY;
	_comDCB.ByteSize = 8;
	_comDCB.StopBits = ONESTOPBIT;				// ストップビット = 1
	_comDCB.fRtsControl = RTS_CONTROL_DISABLE;
	_comDCB.fOutxDsrFlow = FALSE;				// フロー制御無し
	_comDCB.fDsrSensitivity = FALSE;
	_comDCB.fAbortOnError = FALSE;
	_comDCB.EvtChar = _chr_comEnd;				// イベントを発生させる行末の文字(char)

	_comTimeout.ReadIntervalTimeout = MAXWORD;
	_comTimeout.ReadTotalTimeoutConstant = 0;
	_comTimeout.ReadTotalTimeoutMultiplier = 50;

	if(!::SetCommState(_h_comHandle, &_comDCB)) return FALSE;

	if(!::SetCommTimeouts(_h_comHandle, &_comTimeout)) return FALSE;

	return TRUE;
}

//*********************
// RS232Cを閉じる
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
// RS232C受信バッファより、データを取得する
//*********************
DWORD com_read_raw(DWORD nCnt)
{
	DWORD nReadSize = 0;
	DWORD nEndtime;

	if(!::ReadFile(_h_comHandle, _str_comRawBuf, nCnt, &nReadSize, &_comOverlapRrceive))
	{	// 読み込みが失敗した場合
		nReadSize = 0;
		if(::GetLastError() == ERROR_IO_PENDING)
		{	// 非同期読み取りが完了していない場合
			// OVERLAPPED 構造体で指定したイベントがシグナル状態まで待つ
			nEndtime = ::GetTickCount() + 200;		// Windows起動からのミリ秒数に200足す
			while(!::GetOverlappedResult(_h_comHandle, &_comOverlapRrceive,  &nReadSize, FALSE))
			{	// 読み込みが未処理の間、待ちつづける
				::Sleep(5);		// 5msec 待つ
				if(::GetTickCount() > nEndtime) break;	// 200ミリ秒待って読み込めないときは、エラー終了
			}
		}
		else
		{	// その他の読み取りエラーの場合
		}
	}
	::ResetEvent(_comOverlapRrceive.hEvent);
	return nReadSize;
}

//*********************
// RS232Cより、1行受信する
//*********************
DWORD com_read_line(void)
{
	DWORD nReadSize = 0;

	::ClearCommError(_h_comHandle, &_comErrorMask, &_comStat);
	if(_comStat.cbInQue == 0) return 0;		// 入力バッファ（キュー）に何も無い場合、終了

	if(_comStat.cbInQue > BUFFERSIZE)
	{	// システムバッファに、BUFFERSIZE より多くのデータがあるとき
		nReadSize = com_read_raw(BUFFERSIZE);
	}
	else
	{
		nReadSize = com_read_raw(_comStat.cbInQue);
	}

	return nReadSize;		// 読み込んだバイト数を返す
}


//*********************
// RS232Cより、1行受信する
//*********************
BOOL com_pharse_line(DWORD nReadSize)
{
	static UINT nReadPoint = 0;
	static UINT nWritePoint = 0;
	static UINT nTempBufPoint = 0;
	static char strTempBuf[BUFFERSIZE];

	if(_b_debug_out)	// デバッグ出力
		fprintf(fi, "com_pharse_line  init\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d, nReadSize = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint, nReadSize);

	while(nReadPoint<nReadSize && nWritePoint<BUFFERSIZE)
	{
		// バッファに未処理のデータが存在する場合
		if((strTempBuf[nWritePoint++] = _str_comRawBuf[nReadPoint++]) == _chr_comEnd)
		{	// 区切り文字に達したとき
			strTempBuf[nWritePoint] = (char)NULL;	// 文字列終了のNULLを付加
			::strcpy(_str_comRingBuf, strTempBuf);
			if(_b_debug_out)
			{	// デバッグ出力
				fprintf(fi, "com_pharse_line  detect _chr_comEnd\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint);
				fputs("com_pharse_line  strTempBuf = ", fi);
				_debug_binstr_write(strTempBuf, nWritePoint-1);
				fprintf(fi, "com_pharse_line  _str_comRingBuf = %s", _str_comRingBuf);
			}
			nWritePoint = 0;		// 書き込みポインタを先頭に戻す
			return TRUE;			// 文字列完結で終了
		}
	}

	// 区切り文字に達する前に、Rawバッファ終端に達した
	if(_b_debug_out)
	{	// デバッグ出力
		fprintf(fi, "com_pharse_line  remain unfinished char\r\n  nReadPoint = %d, nWritePoint = %d, nTempBufPoint = %d\r\n", nReadPoint, nWritePoint, nTempBufPoint);
		fputs("com_pharse_line  strTempBuf = ", fi);
		_debug_binstr_write(strTempBuf, nWritePoint-1);
	}
	nReadPoint = 0;		// 読み込みポインタを先頭に戻す

	return FALSE;		// 文字列未完結で終了
}

//*********************
// 文字列からデータを切り出す
// column 個目のテキストデータを、数字にする（mode 10進数=0, 16進数=1)
//*********************
double com_cut_data(char *buf, int column, int mode)
{
	char strTemp[BUFFERSIZE];
	char *strOutput;
	int i;

	strcpy(strTemp, buf);		// strtok の文字列破壊のため、コピーしたものを利用

	strOutput = ::strtok(strTemp, (char*)_str_delimit);	// １個目を切り出す
	for(i=1; i<column && strOutput!=NULL; i++)
	{
		strOutput = ::strtok(NULL, (char*)_str_delimit);
	}

	if(i<column || strOutput==NULL) return(_n_min - 1);		// column個目のデータは存在しない

	if(mode == 1)
	{	// 16進数モードのとき
		sscanf(strOutput, "%x", &i);
		return (double)i;
	}

	return atof(strOutput);		// 10進数モードのとき

}


//*********************
// 文字バッファのデバッグ出力 （行末がNULLでない場合の対処）
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


