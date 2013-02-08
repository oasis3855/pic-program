// PIC_Meter.cpp : �A�v���P�[�V�����p�N���X�̒�`���s���܂��B
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu
// Ver 1.1 : 2013/02/09 : �����_��M�\

#include "stdafx.h"
#include "PIC_Meter.h"
#include "PIC_MeterDlg.h"
#include "DlgSetup.h"
#include "rs232c_func.h"
#include <process.h>			// �X���b�h�֘A

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp

BEGIN_MESSAGE_MAP(CPIC_MeterApp, CWinApp)
	//{{AFX_MSG_MAP(CPIC_MeterApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp �N���X�̍\�z

CPIC_MeterApp::CPIC_MeterApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CPIC_MeterApp �I�u�W�F�N�g

CPIC_MeterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp �N���X�̏�����

BOOL CPIC_MeterApp::InitInstance()
{
	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������������
	//  ��Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ��̂��폜����
	//  ���������B

#ifdef _AFXDLL
	Enable3dControls();			// ���L DLL ���� MFC ���g���ꍇ�͂������R�[�����Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N����ꍇ�͂������R�[�����Ă��������B
#endif

	CDlgSetup dlgSetup;					// �ݒ�_�C�A���O
	CPIC_MeterDlg dlg;					// ���C���E�_�C�A���O

	// �ݒ�_�C�A���O
	dlgSetup.m_cmb_portno = 0;					// COM1
	dlgSetup.m_cmb_speed = 5;					// 9600bps
	dlgSetup.m_cmb_lineend = 2;					// �s����LF (0x0a)
	dlgSetup.m_check_delimit_sp = TRUE;			// ��؂蕶�����X�y�[�X
	dlgSetup.m_check_delimit_com = TRUE;		// ��؂蕶�����R���}
	dlgSetup.m_check_delimit_tab = TRUE;		// ��؂蕶�����^�u
	dlgSetup.m_check_delimit_crlf = FALSE;		// ��؂蕶�����������s
	dlgSetup.m_check_1_use = 1;					// (���̃p�����[�^�͖��g�p)
	dlgSetup.m_cmb_1_data_format = 0;			// 10�i��=0, 16�i��=1
	dlgSetup.m_edit_1_column = 1;				// ������̂R�ڂ̃f�[�^�����x�f�[�^
	dlgSetup.m_edit_1_min = 0.0;				// �ŏ��l
	dlgSetup.m_edit_1_max = 35.0;				// �ő�l
	dlgSetup.m_edit_1_factor = 1.0;				// �ϊ��O���t�̌X��
	dlgSetup.m_edit_1_ycut = 0.0;				// y�ؕ�=0
	dlgSetup.m_edit_1_title = "���x";
	dlgSetup.m_edit_1_metric = "��";
	dlgSetup.m_edit_xscale = 600;				// ����600�b(10��)
	dlgSetup.m_edit_ignore = 1;					// ���̕������ȉ��̓G���[
	dlgSetup.m_cmb_file_out = 0;				// �t�@�C���o�͂��Ȃ�

	if(dlgSetup.DoModal() == IDCANCEL)
	{	// �L�����Z���{�^���������ꂽ�ꍇ�A�I������
		return FALSE;
	}

	// �ݒ�_�C�A���O�̓��e��ǂݍ���
	sprintf((char*)::_str_portno, "COM%d", dlgSetup.m_cmb_portno+1);	// �|�[�g�� "COMx"
	switch(dlgSetup.m_cmb_speed)				// �ʐM���x DCB�`��
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
	switch(dlgSetup.m_cmb_lineend)				// �s���L��
	{
		case 0:_chr_comEnd=(char)0;break;
		case 1:_chr_comEnd=(char)0x0d;break;
		case 2:_chr_comEnd=(char)0x0a;break;
		case 3:_chr_comEnd=(char)',';break;
		default:_chr_comEnd=(char)0x0a;break;
	}
	::strcpy((char*)_str_delimit, "");					// �f�[�^��؂蕶��
	if(dlgSetup.m_check_delimit_sp) ::strcat((char*)_str_delimit, " ");
	if(dlgSetup.m_check_delimit_com) ::strcat((char*)_str_delimit, ",");
	if(dlgSetup.m_check_delimit_crlf) ::strcat((char*)_str_delimit, "\r\n");
	if(!::strlen((char*)_str_delimit)) ::strcat((char*)_str_delimit, " ");

	_n_data_format = dlgSetup.m_cmb_1_data_format;	// 10�i��=0, 16�i��=1
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
		case 0:		// �o�͖���
			_b_debug_out=FALSE; _n_data_out=0;break;
		case 1:		// ���f�[�^�o��
			_b_debug_out=FALSE; _n_data_out=1;break;
		case 2:		// �ϊ���f�[�^�o��
			_b_debug_out=FALSE; _n_data_out=2;break;
		case 3:		// �f�o�b�O�o��
			_b_debug_out=TRUE; _n_data_out=0;break;
		default:	// �o�͖���
			_b_debug_out=FALSE; _n_data_out=0;break;
	}

	// �f�o�b�O�i�f�[�^�j�t�@�C�����̎w��
	if(_b_debug_out || _n_data_out != 0)
	{
		CFileDialog dlgFile(FALSE, ".txt", "debug_data.txt", OFN_CREATEPROMPT|OFN_EXPLORER|OFN_HIDEREADONLY,
					"Text File (*.TXT)|*.txt|Data File (*.DAT)|*.dat|All FIles|*.*||");
		if(dlgFile.DoModal()==TRUE)
			::strcpy((char*)_str_out_filename, dlgFile.GetPathName());
	}




	// �����l�̐ݒ�i�O���[�o���ϐ��j
	strcpy((char*)_str_comReceived, "PIC_Meter Debug Text");
	_b_InThread = FALSE;
	_b_ThreadExit = FALSE;

	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <��ݾ�> �ŏ����ꂽ���̃R�[�h��
		//       �L�q���Ă��������B
	}

	// �_�C�A���O�������Ă���A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n������́A
	// �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}
