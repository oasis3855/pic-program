// PIC_Meter.h : PIC_METER �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//
// Ver 1.0
// (C) 2005 INOUE. Hirokazu

#if !defined(AFX_PIC_METER_H__429753FA_58AD_42EB_8321_03411D460FF6__INCLUDED_)
#define AFX_PIC_METER_H__429753FA_58AD_42EB_8321_03411D460FF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// ���C�� �V���{��

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� PIC_Meter.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CPIC_MeterApp : public CWinApp
{
public:
	CPIC_MeterApp();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CPIC_MeterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����

	//{{AFX_MSG(CPIC_MeterApp)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B




#endif // !defined(AFX_PIC_METER_H__429753FA_58AD_42EB_8321_03411D460FF6__INCLUDED_)
