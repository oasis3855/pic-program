// PIC_MeterDlg.cpp : インプリメンテーション ファイル
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : 小数点受信可能

#include "stdafx.h"
#include "PIC_Meter.h"
#include "PIC_MeterDlg.h"
#include "rs232c_func.h"
#include <process.h>			// スレッド関連


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// タイマー ID
#define TIMER_ID	155


/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterDlg ダイアログ

CPIC_MeterDlg::CPIC_MeterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIC_MeterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPIC_MeterDlg)
		// メモ: この位置に ClassWizard によってメンバの初期化が追加されます。
	//}}AFX_DATA_INIT
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPIC_MeterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPIC_MeterDlg)
	DDX_Control(pDX, IDC_GRAPH_AREA, m_graph_area_ctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPIC_MeterDlg, CDialog)
	//{{AFX_MSG_MAP(CPIC_MeterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_CANCELMODE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterDlg メッセージ ハンドラ

BOOL CPIC_MeterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
	nTimerID = 0;

	nPreviousData = -1;		// 前回のデータ (-1というありえない値を設定しておく)

	//**************************
	// デバイスコンテキストの生成と初期化
	//**************************
	//リアルＤＣ
	GraphWndDC = m_graph_area_ctrl.GetDC();		// グラフエリアのDCを得る
	if(GraphWndDC == NULL)
	{
		bGraphWndDCEnable = FALSE;
		MessageBox("グラフィック初期化失敗");
	}
	else
	{
		bGraphWndDCEnable = TRUE;
		// 描画領域の大きさを得る
		m_graph_area_ctrl.GetWindowRect(&GraphWndRect);
		nGraphWndWidth = GraphWndRect.right - GraphWndRect.left;
		nGraphWndHeight = GraphWndRect.bottom - GraphWndRect.top;
		// エレメントの描画位置の基準座標を決定する
		zGx = 5;	// グラフの左上 X
		zGy = 5;	// グラフの左上 Y
		zGsizeX = (int)((double)nGraphWndWidth*0.65) - 10;	// グラフの大きさ X
		zGsizeY = nGraphWndHeight - 10;						// グラフの大きさ Y
		zT1x = (int)((double)nGraphWndWidth*0.65) + 2;		// テキストエリア左上 X
		zT1y = 5;											// テキストエリア左上 Y
		zT2x = (int)((double)nGraphWndWidth*0.65) + 5;		// テキストエリア左上 X
		zT2y = (int)((double)nGraphWndHeight*0.3);			// テキストエリア左上 Y
		zT3x = nGraphWndWidth - 40;							// テキストエリア左上 X
		zT3y = nGraphWndHeight - 20;						// テキストエリア左上 Y

		// グラフ横１ピクセルあたりの秒数
		nGraphSec =  m_nXscale / zGsizeX;
		if(nGraphSec<0) nGraphSec = 1;
		// グラフ縦軸変換値を計算
		nDataMin = (double)_n_min * _n_factor + _n_ycut;
		nDataMax = (double)_n_max * _n_factor + _n_ycut;
		nGraphFactor = (double)zGsizeY/(nDataMax-nDataMin);

		// グラフの最終アップデート時刻 （初期値）
		::time(&tmLastGraphShift);
		// 最終描画時刻 （初期値）
		::time(&tmLastUpdated);

		// バックグラウンドのブラシ（塗りつぶし色）
		cBackColor = 0x800000;		// 0xBBGGRR
		BrushBack.CreateSolidBrush(cBackColor);

		// ワークエリア用ＤＣ
		if(!MemDC.CreateCompatibleDC(GraphWndDC))
		{	// エラー
			bMemDCEnable = FALSE;
			MessageBox("仮想グラフィック初期化失敗");
		}
		else bMemDCEnable = TRUE;

		// ワークエリア用ビットマップ
		if(!MemBmp.CreateCompatibleBitmap(GraphWndDC,nGraphWndWidth+1,nGraphWndHeight+1))
		{	// エラー
			bMemBmpEnable = FALSE;
			MessageBox("仮想ビットマップ初期化失敗");
		}
		else bMemBmpEnable = TRUE;

		// ワークエリアＤＣにビットマップを割付
		if(bMemDCEnable && bMemBmpEnable)
		{
			if(!MemDC.SelectObject(MemBmp))
			{	// エラー
				MessageBox("仮想グラフィック結合失敗");
				MemBmp.DeleteObject();
				MemDC.DeleteDC();
				bMemDCEnable = FALSE;
				bMemBmpEnable = FALSE;
			}
		}
	}

	// 描画のためのデバイスが一つでも確保できなければ、全て開放
	if(!bGraphWndDCEnable || !bMemDCEnable || !bMemBmpEnable)
	{
		// メインダイアログのＤＣ
		if(bGraphWndDCEnable) ReleaseDC(GraphWndDC);
		bGraphWndDCEnable = FALSE;
		// ワークエリアＤＣ
		if(bMemDCEnable) MemDC.DeleteDC();
		bMemDCEnable = FALSE;
		if(bMemBmpEnable) MemBmp.DeleteObject();
		bMemBmpEnable = FALSE;
	}

	//**************************
	// タイマーの設定
	//**************************
	nTimerID = this->SetTimer(TIMER_ID, 200, NULL);	// 200msec 後に ON_TIMER に制御が移る

	if(!nTimerID)
	{
		this->MessageBox("System Timer is not available", "PIC_Meter", MB_OK|MB_ICONSTOP);
		// ウインドウに WM_CLOSE メッセージを送り、終了させる
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}

	//**************************
	// スレッドの開始 （RS-232Cからデータを取得するスレッド）
	//**************************
	if(_beginthread(com_read_thread_main, 0, NULL)==-1)
	{
		this->MessageBox("Thread is not available", "PIC_Meter", MB_OK|MB_ICONSTOP);
		// ウインドウに WM_CLOSE メッセージを送り、終了させる
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}
	::Sleep(200*3);								// スレッド起動のため、ウエイト

	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void CPIC_MeterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
HCURSOR CPIC_MeterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//*********************
// タイマーで起動される。RS-232Cスレッドの作成したデータを読み込み、グラフを描くメイン関数
//*********************
void CPIC_MeterDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CString strTmp;
	CFont fnt;
	CPen pen;
	time_t tmNow;
	BOOL i;

	::time(&tmNow);		// 現在時刻を得る

	if(nPreviousData != _n_Data || tmNow - tmLastUpdated >= 1)
	{	// 前回のデータと相違があった場合、１秒以上経過したとき

		::time(&tmLastUpdated);

		if(bGraphWndDCEnable)
		{	// グラフィック処理
			cForeColor = 0xffd7d7;

			// バックグラウンド
			if(nPreviousData == -1)
			{
				MemDC.SelectObject(BrushBack);
				MemDC.FillSolidRect(0,0,nGraphWndWidth,nGraphWndHeight, cBackColor);
				pen.CreatePen(PS_SOLID, 2, cForeColor);
				MemDC.SelectObject(&pen);
				MemDC.MoveTo(zGx, zGy+zGsizeY);
				MemDC.LineTo(zGx+zGsizeX, zGy+zGsizeY);
				pen.DeleteObject();

			}
			else
			{
				MemDC.SelectObject(BrushBack);
				MemDC.FillSolidRect(zT1x,0,nGraphWndWidth-zT1x,nGraphWndHeight, cBackColor);
			}
			nPreviousData = _n_Data;

			// テキストエリア
			MemDC.SetBkColor(cBackColor);
			MemDC.SetTextColor(cForeColor);
			// タイトル・単位（日本語）= MSゴシック 10.0ポイント
			if(!fnt.CreatePointFont(100, "MS Gothic"))
				fnt.CreatePointFont(100, "ＭＳ ゴシック");		// フォントが無いWindows 95系対策
			MemDC.SelectObject(&fnt);
			MemDC.TextOut(zT1x,zT1y,m_strTitle);
			MemDC.TextOut(zT3x,zT3y,m_strMetric);
			fnt.DeleteObject();
			// データ （数字）= Arial Narrow 34.0ポイント
			fnt.CreatePointFont(340, "Arial Narrow");
			MemDC.SelectObject(&fnt);
			strTmp.Format("%2.2lf", _n_Data);
			MemDC.TextOut(zT2x,zT2y,strTmp);
			fnt.DeleteObject();

			// グラフの左シフト
			if(tmNow - tmLastGraphShift > nGraphSec)
			{
				i = MemDC.BitBlt(zGx, zGy, zGsizeX, zGsizeY, &MemDC, zGx+1, zGy, SRCCOPY);

				::time(&tmLastGraphShift);
			}
			// グラフ
			MemDC.SetPixel(zGx+zGsizeX-1, zGy+zGsizeY-1-(int)((_n_Data-nDataMin)*nGraphFactor), cForeColor);

			// 仮想ＤＣ → リアルＤＣ にコピー
			i = GraphWndDC->BitBlt(0, 0, nGraphWndWidth, nGraphWndHeight, &MemDC, 0, 0, SRCCOPY);

		}

		// テキスト処理 （受信データを表示する）
		this->SetDlgItemText(IDC_TXT_RAWDATA, (char*)::_str_comReceived);
	}

	CDialog::OnTimer(nIDEvent);
}


//*********************
// タイマー停止、ＤＣ開放を行う後始末関数
//*********************
void CPIC_MeterDlg::EndDialogSeq()
{
	// タイマーの削除
	if(nTimerID != 0) KillTimer(nTimerID);
	nTimerID = 0;

	// スレッドの削除
	int i;
	_b_ThreadExit = TRUE;
	for(i=0; i<100; i++)
	{
		if(_b_InThread == FALSE) break;
		::Sleep(200);
	}

	// DC,Bitmap の開放
	// メインダイアログのＤＣ
	if(bGraphWndDCEnable) ReleaseDC(GraphWndDC);
	bGraphWndDCEnable = FALSE;
	// ワークエリアＤＣ
	if(bMemDCEnable) MemDC.DeleteDC();
	bMemDCEnable = FALSE;
	if(bMemBmpEnable) MemBmp.DeleteObject();
	bMemBmpEnable = FALSE;
}

void CPIC_MeterDlg::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください

	EndDialogSeq();		// 終了時の一連の処理

	CDialog::OnClose();
}

void CPIC_MeterDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください

	EndDialogSeq();		// 終了時の一連の処理

}

