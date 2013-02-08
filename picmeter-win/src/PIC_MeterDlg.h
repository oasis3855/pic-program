// PIC_MeterDlg.h : �w�b�_�[ �t�@�C��
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#if !defined(AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_)
#define AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterDlg �_�C�A���O

class CPIC_MeterDlg : public CDialog
{
// �\�z
public:
	void EndDialogSeq(void);				// �^�C�}�[��~�A�c�b�J�����s����n���֐�
	CPIC_MeterDlg(CWnd* pParent = NULL);	// �W���̃R���X�g���N�^

	UINT nTimer;							// �_�C�A���O�����I�ɍX�V����^�C�}�[�p
	UINT nTimerID;							// �_�C�A���O�����I�ɍX�V����^�C�}�[�p

	double nPreviousData;					// ��M�f�[�^��O��f�[�^�Ɣ�r���邽�߂̕ϐ�
	CString m_strMetric;					// �P�� �i�\���Ɏg���j
	CString m_strTitle;						// �\���^�C�g�� �i�\���Ɏg���j

	CBitmap MemBmp;							// ���[�N�G���A�a�l�o
	CDC MemDC;								// ���[�N�G���A�c�b
	CDC *GraphWndDC;						// ���A���c�b
	BOOL bMemDCEnable, bMemBmpEnable;		// ���[�N�G���A�c�b�C�a�l�o���擾�ł����Ƃ� TRUE

	RECT GraphWndRect;						// �O���t�\���G���A�̑傫������Ɏg��
	int nGraphWndWidth, nGraphWndHeight;	// �O���t�\���G���A�̏c���s�N�Z���T�C�Y
	BOOL bGraphWndDCEnable;					// �O���t�\���G���A�c�b���擾�ł����Ƃ� TRUE
	CBrush BrushBack;						// �O���t�w�i�h��ׂ��p�̃u���V
	COLORREF cBackColor, cForeColor;		// �O���t�̈�̕`��F�A�w�i�F

	int zGx, zGsizeX, zGy, zGsizeY;			// �O���t�̈�̃O���t�G���A���W
	int zT1x, zT1y, zT2x, zT2y, zT3x, zT3y;	// �O���t�̈�̃e�L�X�g�G���A���W
	double nGraphFactor, nDataMin, nDataMax;// ���f�[�^���O���t�ŕ`���ꍇ�̕ϊ��W��

	int m_nXscale;							// �O���t�̕\���b��
	int nGraphSec;							// �O���t�����V�t�g������ׂ��P�ʕb��
	time_t tmLastGraphShift;				// �O���t�����V�t�g������^�C�~���O����邽�߂̕ϐ�

	time_t tmLastUpdated;					// �ŏI�`�悵������


// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPIC_MeterDlg)
	enum { IDD = IDD_PIC_METER_DIALOG };
	CStatic	m_graph_area_ctrl;
	//}}AFX_DATA

	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CPIC_MeterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	HICON m_hIcon;

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
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
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PIC_METERDLG_H__659186EA_1EDA_4CDE_AB8F_17DD393C74ED__INCLUDED_)
