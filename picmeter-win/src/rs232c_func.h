//
// RS-232C からデータを取得するスレッド
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#ifndef __RS232C_FUNC
#define __RS232C_FUNC

// 外部から参照、設定するための変数
extern volatile char _str_comReceived[];		// 受信完了後、外部参照用に提供される受信データ
extern volatile BOOL _b_InThread;				// スレッド動作中のフラグ
extern volatile BOOL _b_ThreadExit;				// スレッドを終了させる指令
extern volatile char _chr_comEnd;				// データ行の最後を示す文字
extern volatile BOOL _b_debug_out;				// デバッグ出力を行う
extern volatile int _n_data_out;				// 生データのファイル出力を行う
extern volatile char _str_out_filename[];		// デバッグ出力、データ出力を行うファイル名
extern volatile double _n_Data;					// 変換後のデータ

extern volatile char _str_delimit[];			// データの区切り文字
extern volatile int _n_data_format;				// 10進数=0, 16進数=1
extern volatile int _n_column;					// データ位置 （カラム）
extern volatile char _str_portno[];				// ポート名, "COM1" などの文字列
extern volatile DWORD _n_com_speed;				// シリアルポートのスピード DCB形式
extern volatile double _n_factor;				// 変換乗数
extern volatile double _n_ycut;					// 変換Ｙ切片
extern volatile double _n_max;						// 最大値
extern volatile double _n_min;						// 最小値
extern volatile int _n_ignore;					// この文字数以下はエラーとして無視

extern char _str_comRawBuf[];					// RS-232C で受信した生データ

// シリアルポートからデータを読み込み、解釈、データ出力するスレッド
void com_read_thread_main(void  *pVoid);

// シリアルポートの初期化と開放
BOOL com_init(void);
void com_close(void);

// Raw 読み込み
DWORD com_read_raw(DWORD nCnt);
// 読み込みデータの処理
DWORD com_read_line(void);
BOOL com_pharse_line(DWORD nReadSize);
double com_cut_data(char *buf, int column, int mode);

// デバッグ用ファイルへの出力
void _debug_binstr_write(char *buf, int nLen);

#endif
