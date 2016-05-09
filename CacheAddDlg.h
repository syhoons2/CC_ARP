#if !defined(AFX_CACHEADDDLG_H__1D331F1B_18B1_4A16_A5B6_822A02F8E0F2__INCLUDED_)
#define AFX_CACHEADDDLG_H__1D331F1B_18B1_4A16_A5B6_822A02F8E0F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CacheAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCacheAddDlg dialog

struct Cache_BufferAddr {
	unsigned char buffer_IPAddr[4];
	unsigned char buffer_EtherAddr[10];
};	// Cache Element의 content를 이루기 위한 구조체

class CCacheAddDlg : public CDialog
{
// Construction
public:
	Cache_BufferAddr GetBufferAddr();
	CCacheAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCacheAddDlg)
	enum { IDD = IDD_CACHE_ADD_DIALOG };
	CIPAddressCtrl	m_IPAddr;
	CString	m_etherAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCacheAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCacheAddDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	Cache_BufferAddr m_buffer;	// Cache Element의 content
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CACHEADDDLG_H__1D331F1B_18B1_4A16_A5B6_822A02F8E0F2__INCLUDED_)
