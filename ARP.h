// ARP.h : main header file for the ARP application
//

#if !defined(AFX_ARP_H__A591B1C4_B5D7_43E5_B151_83E3A774D868__INCLUDED_)
#define AFX_ARP_H__A591B1C4_B5D7_43E5_B151_83E3A774D868__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CARPApp:
// See ARP.cpp for the implementation of this class
//

class CARPApp : public CWinApp
{
public:
	CARPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CARPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARP_H__A591B1C4_B5D7_43E5_B151_83E3A774D868__INCLUDED_)
