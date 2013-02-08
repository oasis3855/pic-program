// PIC_Meter.h : PIC_METER アプリケーションのメイン ヘッダー ファイルです。
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

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CPIC_MeterApp:
// このクラスの動作の定義に関しては PIC_Meter.cpp ファイルを参照してください。
//

class CPIC_MeterApp : public CWinApp
{
public:
	CPIC_MeterApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CPIC_MeterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CPIC_MeterApp)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。




#endif // !defined(AFX_PIC_METER_H__429753FA_58AD_42EB_8321_03411D460FF6__INCLUDED_)
