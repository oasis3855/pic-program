// DlgSetup.h : ヘッダー ファイルです。
// Ver 1.0
// (C) 2005 INOUE. Hirokazu


#if !defined(AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_)
#define AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetup.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetup ダイアログ

class CDlgSetup : public CDialog
{
// コンストラクション
public:
	int m_init_cmb_lineend;
	int m_init_cmb_speed;
	int m_init_cmb_portno;
	int m_init_cmb_file_out;
	int m_init_cmb_1_data_format;
	CDlgSetup(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CDlgSetup)
	enum { IDD = IDD_DLG_SETUP };
	BOOL	m_check_1_use;
	CString	m_edit_1_title;
	CString	m_edit_1_metric;
	int		m_edit_1_column;
	double	m_edit_1_factor;
	double	m_edit_1_max;
	double	m_edit_1_min;
	double	m_edit_1_ycut;
	int		m_edit_xscale;
	BOOL	m_check_delimit_com;
	BOOL	m_check_delimit_crlf;
	BOOL	m_check_delimit_sp;
	BOOL	m_check_delimit_tab;
	int		m_edit_ignore;
	int		m_cmb_portno;
	int		m_cmb_file_out;
	int		m_cmb_1_data_format;
	int		m_cmb_lineend;
	int		m_cmb_speed;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CDlgSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CDlgSetup)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_)
