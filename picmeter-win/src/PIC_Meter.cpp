// PIC_Meter.cpp : アプリケーション用クラスの定義を行います。
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : 小数点受信可能

#include "stdafx.h"
#include "PIC_Meter.h"
#include "PIC_MeterDlg.h"
#include "DlgSetup.h"
#include "rs232c_func.h"
#include <process.h>			// スレッド関連

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp

BEGIN_MESSAGE_MAP(CPIC_MeterApp, CWinApp)
	//{{AFX_MSG_MAP(CPIC_MeterApp)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp クラスの構築

CPIC_MeterApp::CPIC_MeterApp()
{
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CPIC_MeterApp オブジェクト

CPIC_MeterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp クラスの初期化

BOOL CPIC_MeterApp::InitInstance()
{
	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さくしたけ
	//  れば以下の特定の初期化ルーチンの中から不必要なものを削除して
	//  ください。

#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL 内で MFC を使う場合はここをコールしてください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクする場合はここをコールしてください。
#endif

	CDlgSetup dlgSetup;					// 設定ダイアログ
	CPIC_MeterDlg dlg;					// メイン・ダイアログ

	// 設定ダイアログ
	dlgSetup.m_cmb_portno = 0;					// COM1
	dlgSetup.m_cmb_speed = 5;					// 9600bps
	dlgSetup.m_cmb_lineend = 2;					// 行末はLF (0x0a)
	dlgSetup.m_check_delimit_sp = TRUE;			// 区切り文字＝スペース
	dlgSetup.m_check_delimit_com = TRUE;		// 区切り文字＝コンマ
	dlgSetup.m_check_delimit_tab = TRUE;		// 区切り文字＝タブ
	dlgSetup.m_check_delimit_crlf = FALSE;		// 区切り文字無効＝改行
	dlgSetup.m_check_1_use = 1;					// (このパラメータは未使用)
	dlgSetup.m_cmb_1_data_format = 0;			// 10進数=0, 16進数=1
	dlgSetup.m_edit_1_column = 1;				// 文字列の３個目のデータが温度データ
	dlgSetup.m_edit_1_min = 0.0;				// 最小値
	dlgSetup.m_edit_1_max = 35.0;				// 最大値
	dlgSetup.m_edit_1_factor = 1.0;				// 変換グラフの傾き
	dlgSetup.m_edit_1_ycut = 0.0;				// y切片=0
	dlgSetup.m_edit_1_title = "温度";
	dlgSetup.m_edit_1_metric = "℃";
	dlgSetup.m_edit_xscale = 600;				// 横軸600秒(10分)
	dlgSetup.m_edit_ignore = 1;					// この文字数以下はエラー
	dlgSetup.m_cmb_file_out = 0;				// ファイル出力しない

	if(dlgSetup.DoModal() == IDCANCEL)
	{	// キャンセルボタンが押された場合、終了する
		return FALSE;
	}

	// 設定ダイアログの内容を読み込む
	sprintf((char*)::_str_portno, "COM%d", dlgSetup.m_cmb_portno+1);	// ポート名 "COMx"
	switch(dlgSetup.m_cmb_speed)				// 通信速度 DCB形式
	{
		case 0:_n_com_speed=CBR_300;break;
		case 1:_n_com_speed=CBR_600;break;
		case 2:_n_com_speed=CBR_1200;break;
		case 3:_n_com_speed=CBR_2400;break;
		case 4:_n_com_speed=CBR_4800;break;
		case 5:_n_com_speed=CBR_9600;break;
		case 6:_n_com_speed=CBR_14400;break;
		default:_n_com_speed=CBR_2400;break;
	}
	switch(dlgSetup.m_cmb_lineend)				// 行末記号
	{
		case 0:_chr_comEnd=(char)0;break;
		case 1:_chr_comEnd=(char)0x0d;break;
		case 2:_chr_comEnd=(char)0x0a;break;
		case 3:_chr_comEnd=(char)',';break;
		default:_chr_comEnd=(char)0x0a;break;
	}
	::strcpy((char*)_str_delimit, "");					// データ区切り文字
	if(dlgSetup.m_check_delimit_sp) ::strcat((char*)_str_delimit, " ");
	if(dlgSetup.m_check_delimit_com) ::strcat((char*)_str_delimit, ",");
	if(dlgSetup.m_check_delimit_crlf) ::strcat((char*)_str_delimit, "\r\n");
	if(!::strlen((char*)_str_delimit)) ::strcat((char*)_str_delimit, " ");

	_n_data_format = dlgSetup.m_cmb_1_data_format;	// 10進数=0, 16進数=1
	_n_column = dlgSetup.m_edit_1_column;
	_n_min = dlgSetup.m_edit_1_min;
	_n_max = dlgSetup.m_edit_1_max;
	_n_factor = dlgSetup.m_edit_1_factor;
	_n_ycut = dlgSetup.m_edit_1_ycut;
	dlg.m_strTitle = dlgSetup.m_edit_1_title;
	dlg.m_strMetric = dlgSetup.m_edit_1_metric;
	dlg.m_nXscale = dlgSetup.m_edit_xscale;
	_n_ignore = dlgSetup.m_edit_ignore;
	switch(dlgSetup.m_cmb_file_out)
	{
		case 0:		// 出力無し
			_b_debug_out=FALSE; _n_data_out=0;break;
		case 1:		// 生データ出力
			_b_debug_out=FALSE; _n_data_out=1;break;
		case 2:		// 変換後データ出力
			_b_debug_out=FALSE; _n_data_out=2;break;
		case 3:		// デバッグ出力
			_b_debug_out=TRUE; _n_data_out=0;break;
		default:	// 出力無し
			_b_debug_out=FALSE; _n_data_out=0;break;
	}

	// デバッグ（データ）ファイル名の指定
	if(_b_debug_out || _n_data_out != 0)
	{
		CFileDialog dlgFile(FALSE, ".txt", "debug_data.txt", OFN_CREATEPROMPT|OFN_EXPLORER|OFN_HIDEREADONLY,
					"Text File (*.TXT)|*.txt|Data File (*.DAT)|*.dat|All FIles|*.*||");
		if(dlgFile.DoModal()==TRUE)
			::strcpy((char*)_str_out_filename, dlgFile.GetPathName());
	}




	// 初期値の設定（グローバル変数）
	strcpy((char*)_str_comReceived, "PIC_Meter Debug Text");
	_b_InThread = FALSE;
	_b_ThreadExit = FALSE;

	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <ｷｬﾝｾﾙ> で消された時のコードを
		//       記述してください。
	}

	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
