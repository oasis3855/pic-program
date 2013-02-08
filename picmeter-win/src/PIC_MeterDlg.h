// PIC_MeterDlg.h : ヘッダー ファイル
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#if !defined(AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_)
#define AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterDlg ダイアログ

class CPIC_MeterDlg : public CDialog
{
// 構築
public:
	void EndDialogSeq(void);				// タイマー停止、ＤＣ開放を行う後始末関数
	CPIC_MeterDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

	UINT nTimer;							// ダイアログを定期的に更新するタイマー用
	UINT nTimerID;							// ダイアログを定期的に更新するタイマー用

	double nPreviousData;					// 受信データを前回データと比較するための変数
	CString m_strMetric;					// 単位 （表示に使う）
	CString m_strTitle;						// 表示タイトル （表示に使う）

	CBitmap MemBmp;							// ワークエリアＢＭＰ
	CDC MemDC;								// ワークエリアＤＣ
	CDC *GraphWndDC;						// リアルＤＣ
	BOOL bMemDCEnable, bMemBmpEnable;		// ワークエリアＤＣ，ＢＭＰが取得できたとき TRUE

	RECT GraphWndRect;						// グラフ表示エリアの大きさ測定に使う
	int nGraphWndWidth, nGraphWndHeight;	// グラフ表示エリアの縦横ピクセルサイズ
	BOOL bGraphWndDCEnable;					// グラフ表示エリアＤＣが取得できたとき TRUE
	CBrush BrushBack;						// グラフ背景塗り潰し用のブラシ
	COLORREF cBackColor, cForeColor;		// グラフ領域の描画色、背景色

	int zGx, zGsizeX, zGy, zGsizeY;			// グラフ領域のグラフエリア座標
	int zT1x, zT1y, zT2x, zT2y, zT3x, zT3y;	// グラフ領域のテキストエリア座標
	double nGraphFactor, nDataMin, nDataMax;// 実データをグラフで描く場合の変換係数

	int m_nXscale;							// グラフの表示秒数
	int nGraphSec;							// グラフを横シフトさせるべき単位秒数
	time_t tmLastGraphShift;				// グラフを横シフトさせるタイミングを取るための変数

	time_t tmLastUpdated;					// 最終描画した時間


// ダイアログ データ
	//{{AFX_DATA(CPIC_MeterDlg)
	enum { IDD = IDD_PIC_METER_DIALOG };
	CStatic	m_graph_area_ctrl;
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CPIC_MeterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	HICON m_hIcon;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CPIC_MeterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_)
