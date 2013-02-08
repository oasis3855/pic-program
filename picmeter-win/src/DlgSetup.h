// DlgSetup.h : �w�b�_�[ �t�@�C���ł��B
// Ver 1.0
// (C) 2005 INOUE. Hirokazu


#if !defined(AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_)
#define AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetup.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetup �_�C�A���O

class CDlgSetup : public CDialog
{
// �R���X�g���N�V����
public:
	int m_init_cmb_lineend;
	int m_init_cmb_speed;
	int m_init_cmb_portno;
	int m_init_cmb_file_out;
	int m_init_cmb_1_data_format;
	CDlgSetup(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
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


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CDlgSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CDlgSetup)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_DLGSETUP_H__C8153228_DC9D_4145_8021_A69286B4C419__INCLUDED_)
