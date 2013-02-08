// DlgSetup.cpp : インプリメンテーション ファイル
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#include "stdafx.h"
#include "pic_meter.h"
#include "DlgSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetup ダイアログ


CDlgSetup::CDlgSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetup)
	m_check_1_use = FALSE;
	m_edit_1_title = _T("");
	m_edit_1_metric = _T("");
	m_edit_1_column = 0;
	m_edit_1_factor = 0.0;
	m_edit_1_max = 0;
	m_edit_1_min = 0;
	m_edit_1_ycut = 0.0;
	m_edit_xscale = 0;
	m_check_delimit_com = FALSE;
	m_check_delimit_crlf = FALSE;
	m_check_delimit_sp = FALSE;
	m_check_delimit_tab = FALSE;
	m_edit_ignore = 0;
	m_cmb_portno = -1;
	m_cmb_file_out = -1;
	m_cmb_1_data_format = -1;
	m_cmb_lineend = -1;
	m_cmb_speed = -1;
	//}}AFX_DATA_INIT
}


void CDlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetup)
	DDX_Check(pDX, IDC_CHECK_1_USE, m_check_1_use);
	DDX_Text(pDX, IDC_EDIT_1_TITLE, m_edit_1_title);
	DDX_Text(pDX, IDC_EDIT_1_METRIC, m_edit_1_metric);
	DDX_Text(pDX, IDC_EDIT_1_COLUMN, m_edit_1_column);
	DDX_Text(pDX, IDC_EDIT_1_FACTOR, m_edit_1_factor);
	DDX_Text(pDX, IDC_EDIT_1_MAX, m_edit_1_max);
	DDX_Text(pDX, IDC_EDIT_1_MIN, m_edit_1_min);
	DDX_Text(pDX, IDC_EDIT_1_YCUT, m_edit_1_ycut);
	DDX_Text(pDX, IDC_EDIT_XSCALE, m_edit_xscale);
	DDX_Check(pDX, IDC_CHECK_DELIMIT_COM, m_check_delimit_com);
	DDX_Check(pDX, IDC_CHECK_DELIMIT_CRLF, m_check_delimit_crlf);
	DDX_Check(pDX, IDC_CHECK_DELIMIT_SP, m_check_delimit_sp);
	DDX_Check(pDX, IDC_CHECK_DELIMIT_TAB, m_check_delimit_tab);
	DDX_Text(pDX, IDC_EDIT_IGNORE, m_edit_ignore);
	DDX_CBIndex(pDX, IDC_CMB_PORTNO, m_cmb_portno);
	DDX_CBIndex(pDX, IDC_CMB_FILE_OUT, m_cmb_file_out);
	DDX_CBIndex(pDX, IDC_CMB_1_DATA_FORMAT, m_cmb_1_data_format);
	DDX_CBIndex(pDX, IDC_CMB_LINEEND, m_cmb_lineend);
	DDX_CBIndex(pDX, IDC_CMB_SPEED, m_cmb_speed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetup, CDialog)
	//{{AFX_MSG_MAP(CDlgSetup)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetup メッセージ ハンドラ



