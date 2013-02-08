// PIC_MeterDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : �����_��M�\

#include "stdafx.h"
#include "PIC_Meter.h"
#include "PIC_MeterDlg.h"
#include "rs232c_func.h"
#include <process.h>			// �X���b�h�֘A


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// �^�C�}�[ ID
#define TIMER_ID	155


/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterDlg �_�C�A���O

CPIC_MeterDlg::CPIC_MeterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIC_MeterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPIC_MeterDlg)
		// ����: ���̈ʒu�� ClassWizard �ɂ���ă����o�̏��������ǉ�����܂��B
	//}}AFX_DATA_INIT
	// ����: LoadIcon �� Win32 �� DestroyIcon �̃T�u�V�[�P���X��v�����܂���B
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
// CPIC_MeterDlg ���b�Z�[�W �n���h��

BOOL CPIC_MeterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���̃_�C�A���O�p�̃A�C�R����ݒ肵�܂��B�t���[�����[�N�̓A�v���P�[�V�����̃��C��
	// �E�B���h�E���_�C�A���O�łȂ����͎����I�ɐݒ肵�܂���B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R����ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R����ݒ�
	
	// TODO: ���ʂȏ��������s�����͂��̏ꏊ�ɒǉ����Ă��������B
	nTimerID = 0;

	nPreviousData = -1;		// �O��̃f�[�^ (-1�Ƃ������肦�Ȃ��l��ݒ肵�Ă���)

	//**************************
	// �f�o�C�X�R���e�L�X�g�̐����Ə�����
	//**************************
	//���A���c�b
	GraphWndDC = m_graph_area_ctrl.GetDC();		// �O���t�G���A��DC�𓾂�
	if(GraphWndDC == NULL)
	{
		bGraphWndDCEnable = FALSE;
		MessageBox("�O���t�B�b�N���������s");
	}
	else
	{
		bGraphWndDCEnable = TRUE;
		// �`��̈�̑傫���𓾂�
		m_graph_area_ctrl.GetWindowRect(&GraphWndRect);
		nGraphWndWidth = GraphWndRect.right - GraphWndRect.left;
		nGraphWndHeight = GraphWndRect.bottom - GraphWndRect.top;
		// �G�������g�̕`��ʒu�̊���W�����肷��
		zGx = 5;	// �O���t�̍��� X
		zGy = 5;	// �O���t�̍��� Y
		zGsizeX = (int)((double)nGraphWndWidth*0.65) - 10;	// �O���t�̑傫�� X
		zGsizeY = nGraphWndHeight - 10;						// �O���t�̑傫�� Y
		zT1x = (int)((double)nGraphWndWidth*0.65) + 2;		// �e�L�X�g�G���A���� X
		zT1y = 5;											// �e�L�X�g�G���A���� Y
		zT2x = (int)((double)nGraphWndWidth*0.65) + 5;		// �e�L�X�g�G���A���� X
		zT2y = (int)((double)nGraphWndHeight*0.3);			// �e�L�X�g�G���A���� Y
		zT3x = nGraphWndWidth - 40;							// �e�L�X�g�G���A���� X
		zT3y = nGraphWndHeight - 20;						// �e�L�X�g�G���A���� Y

		// �O���t���P�s�N�Z��������̕b��
		nGraphSec =  m_nXscale / zGsizeX;
		if(nGraphSec<0) nGraphSec = 1;
		// �O���t�c���ϊ��l���v�Z
		nDataMin = (double)_n_min * _n_factor + _n_ycut;
		nDataMax = (double)_n_max * _n_factor + _n_ycut;
		nGraphFactor = (double)zGsizeY/(nDataMax-nDataMin);

		// �O���t�̍ŏI�A�b�v�f�[�g���� �i�����l�j
		::time(&tmLastGraphShift);
		// �ŏI�`�掞�� �i�����l�j
		::time(&tmLastUpdated);

		// �o�b�N�O���E���h�̃u���V�i�h��Ԃ��F�j
		cBackColor = 0x800000;		// 0xBBGGRR
		BrushBack.CreateSolidBrush(cBackColor);

		// ���[�N�G���A�p�c�b
		if(!MemDC.CreateCompatibleDC(GraphWndDC))
		{	// �G���[
			bMemDCEnable = FALSE;
			MessageBox("���z�O���t�B�b�N���������s");
		}
		else bMemDCEnable = TRUE;

		// ���[�N�G���A�p�r�b�g�}�b�v
		if(!MemBmp.CreateCompatibleBitmap(GraphWndDC,nGraphWndWidth+1,nGraphWndHeight+1))
		{	// �G���[
			bMemBmpEnable = FALSE;
			MessageBox("���z�r�b�g�}�b�v���������s");
		}
		else bMemBmpEnable = TRUE;

		// ���[�N�G���A�c�b�Ƀr�b�g�}�b�v�����t
		if(bMemDCEnable && bMemBmpEnable)
		{
			if(!MemDC.SelectObject(MemBmp))
			{	// �G���[
				MessageBox("���z�O���t�B�b�N�������s");
				MemBmp.DeleteObject();
				MemDC.DeleteDC();
				bMemDCEnable = FALSE;
				bMemBmpEnable = FALSE;
			}
		}
	}

	// �`��̂��߂̃f�o�C�X����ł��m�ۂł��Ȃ���΁A�S�ĊJ��
	if(!bGraphWndDCEnable || !bMemDCEnable || !bMemBmpEnable)
	{
		// ���C���_�C�A���O�̂c�b
		if(bGraphWndDCEnable) ReleaseDC(GraphWndDC);
		bGraphWndDCEnable = FALSE;
		// ���[�N�G���A�c�b
		if(bMemDCEnable) MemDC.DeleteDC();
		bMemDCEnable = FALSE;
		if(bMemBmpEnable) MemBmp.DeleteObject();
		bMemBmpEnable = FALSE;
	}

	//**************************
	// �^�C�}�[�̐ݒ�
	//**************************
	nTimerID = this->SetTimer(TIMER_ID, 200, NULL);	// 200msec ��� ON_TIMER �ɐ��䂪�ڂ�

	if(!nTimerID)
	{
		this->MessageBox("System Timer is not available", "PIC_Meter", MB_OK|MB_ICONSTOP);
		// �E�C���h�E�� WM_CLOSE ���b�Z�[�W�𑗂�A�I��������
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}

	//**************************
	// �X���b�h�̊J�n �iRS-232C����f�[�^���擾����X���b�h�j
	//**************************
	if(_beginthread(com_read_thread_main, 0, NULL)==-1)
	{
		this->MessageBox("Thread is not available", "PIC_Meter", MB_OK|MB_ICONSTOP);
		// �E�C���h�E�� WM_CLOSE ���b�Z�[�W�𑗂�A�I��������
		PostMessage(WM_CLOSE, 0, 0);
		return TRUE;
	}
	::Sleep(200*3);								// �X���b�h�N���̂��߁A�E�G�C�g

	return TRUE;  // TRUE ��Ԃ��ƃR���g���[���ɐݒ肵���t�H�[�J�X�͎����܂���B
}

// �����_�C�A���O�{�b�N�X�ɍŏ����{�^����ǉ�����Ȃ�΁A�A�C�R����`�悷��
// �R�[�h���ȉ��ɋL�q����K�v������܂��BMFC �A�v���P�[�V������ document/view
// ���f�����g���Ă���̂ŁA���̏����̓t���[�����[�N�ɂ�莩���I�ɏ�������܂��B

void CPIC_MeterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// �N���C�A���g�̋�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R����`�悵�܂��B
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// �V�X�e���́A���[�U�[���ŏ����E�B���h�E���h���b�O���Ă���ԁA
// �J�[�\����\�����邽�߂ɂ������Ăяo���܂��B
HCURSOR CPIC_MeterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//*********************
// �^�C�}�[�ŋN�������BRS-232C�X���b�h�̍쐬�����f�[�^��ǂݍ��݁A�O���t��`�����C���֐�
//*********************
void CPIC_MeterDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	CString strTmp;
	CFont fnt;
	CPen pen;
	time_t tmNow;
	BOOL i;

	::time(&tmNow);		// ���ݎ����𓾂�

	if(nPreviousData != _n_Data || tmNow - tmLastUpdated >= 1)
	{	// �O��̃f�[�^�Ƒ��Ⴊ�������ꍇ�A�P�b�ȏ�o�߂����Ƃ�

		::time(&tmLastUpdated);

		if(bGraphWndDCEnable)
		{	// �O���t�B�b�N����
			cForeColor = 0xffd7d7;

			// �o�b�N�O���E���h
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

			// �e�L�X�g�G���A
			MemDC.SetBkColor(cBackColor);
			MemDC.SetTextColor(cForeColor);
			// �^�C�g���E�P�ʁi���{��j= MS�S�V�b�N 10.0�|�C���g
			if(!fnt.CreatePointFont(100, "MS Gothic"))
				fnt.CreatePointFont(100, "�l�r �S�V�b�N");		// �t�H���g������Windows 95�n�΍�
			MemDC.SelectObject(&fnt);
			MemDC.TextOut(zT1x,zT1y,m_strTitle);
			MemDC.TextOut(zT3x,zT3y,m_strMetric);
			fnt.DeleteObject();
			// �f�[�^ �i�����j= Arial Narrow 34.0�|�C���g
			fnt.CreatePointFont(340, "Arial Narrow");
			MemDC.SelectObject(&fnt);
			strTmp.Format("%2.2lf", _n_Data);
			MemDC.TextOut(zT2x,zT2y,strTmp);
			fnt.DeleteObject();

			// �O���t�̍��V�t�g
			if(tmNow - tmLastGraphShift > nGraphSec)
			{
				i = MemDC.BitBlt(zGx, zGy, zGsizeX, zGsizeY, &MemDC, zGx+1, zGy, SRCCOPY);

				::time(&tmLastGraphShift);
			}
			// �O���t
			MemDC.SetPixel(zGx+zGsizeX-1, zGy+zGsizeY-1-(int)((_n_Data-nDataMin)*nGraphFactor), cForeColor);

			// ���z�c�b �� ���A���c�b �ɃR�s�[
			i = GraphWndDC->BitBlt(0, 0, nGraphWndWidth, nGraphWndHeight, &MemDC, 0, 0, SRCCOPY);

		}

		// �e�L�X�g���� �i��M�f�[�^��\������j
		this->SetDlgItemText(IDC_TXT_RAWDATA, (char*)::_str_comReceived);
	}

	CDialog::OnTimer(nIDEvent);
}


//*********************
// �^�C�}�[��~�A�c�b�J�����s����n���֐�
//*********************
void CPIC_MeterDlg::EndDialogSeq()
{
	// �^�C�}�[�̍폜
	if(nTimerID != 0) KillTimer(nTimerID);
	nTimerID = 0;

	// �X���b�h�̍폜
	int i;
	_b_ThreadExit = TRUE;
	for(i=0; i<100; i++)
	{
		if(_b_InThread == FALSE) break;
		::Sleep(200);
	}

	// DC,Bitmap �̊J��
	// ���C���_�C�A���O�̂c�b
	if(bGraphWndDCEnable) ReleaseDC(GraphWndDC);
	bGraphWndDCEnable = FALSE;
	// ���[�N�G���A�c�b
	if(bMemDCEnable) MemDC.DeleteDC();
	bMemDCEnable = FALSE;
	if(bMemBmpEnable) MemBmp.DeleteObject();
	bMemBmpEnable = FALSE;
}

void CPIC_MeterDlg::OnClose() 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������

	EndDialogSeq();		// �I�����̈�A�̏���

	CDialog::OnClose();
}

void CPIC_MeterDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������

	EndDialogSeq();		// �I�����̈�A�̏���

}

