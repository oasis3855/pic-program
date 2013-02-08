//
// RS-232C ����f�[�^���擾����X���b�h
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#ifndef __RS232C_FUNC
#define __RS232C_FUNC

// �O������Q�ƁA�ݒ肷�邽�߂̕ϐ�
extern volatile char _str_comReceived[];		// ��M������A�O���Q�Ɨp�ɒ񋟂�����M�f�[�^
extern volatile BOOL _b_InThread;				// �X���b�h���쒆�̃t���O
extern volatile BOOL _b_ThreadExit;				// �X���b�h���I��������w��
extern volatile char _chr_comEnd;				// �f�[�^�s�̍Ō����������
extern volatile BOOL _b_debug_out;				// �f�o�b�O�o�͂��s��
extern volatile int _n_data_out;				// ���f�[�^�̃t�@�C���o�͂��s��
extern volatile char _str_out_filename[];		// �f�o�b�O�o�́A�f�[�^�o�͂��s���t�@�C����
extern volatile double _n_Data;					// �ϊ���̃f�[�^

extern volatile char _str_delimit[];			// �f�[�^�̋�؂蕶��
extern volatile int _n_data_format;				// 10�i��=0, 16�i��=1
extern volatile int _n_column;					// �f�[�^�ʒu �i�J�����j
extern volatile char _str_portno[];				// �|�[�g��, "COM1" �Ȃǂ̕�����
extern volatile DWORD _n_com_speed;				// �V���A���|�[�g�̃X�s�[�h DCB�`��
extern volatile double _n_factor;				// �ϊ��搔
extern volatile double _n_ycut;					// �ϊ��x�ؕ�
extern volatile double _n_max;						// �ő�l
extern volatile double _n_min;						// �ŏ��l
extern volatile int _n_ignore;					// ���̕������ȉ��̓G���[�Ƃ��Ė���

extern char _str_comRawBuf[];					// RS-232C �Ŏ�M�������f�[�^

// �V���A���|�[�g����f�[�^��ǂݍ��݁A���߁A�f�[�^�o�͂���X���b�h
void com_read_thread_main(void  *pVoid);

// �V���A���|�[�g�̏������ƊJ��
BOOL com_init(void);
void com_close(void);

// Raw �ǂݍ���
DWORD com_read_raw(DWORD nCnt);
// �ǂݍ��݃f�[�^�̏���
DWORD com_read_line(void);
BOOL com_pharse_line(DWORD nReadSize);
double com_cut_data(char *buf, int column, int mode);

// �f�o�b�O�p�t�@�C���ւ̏o��
void _debug_binstr_write(char *buf, int nLen);

#endif
