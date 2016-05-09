#if !defined(AFX_PROXYADDDLG_H__421AB93E_5FB2_463E_AFCF_E1B6FC80A1D8__INCLUDED_)
#define AFX_PROXYADDDLG_H__421AB93E_5FB2_463E_AFCF_E1B6FC80A1D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxyAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxyAddDlg dialog

struct Proxy_BufferAddr {
	CString device;
	unsigned char buffer_IPAddr[4];
	unsigned char buffer_EtherAddr[10];
};	// Proxy Element�� content�� �̷�� ���� ����ü

class CProxyAddDlg : public CDialog
{
// Construction
public:
	void GetSystemEtherInfo();
	Proxy_BufferAddr GetBufferAddr();
	CProxyAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProxyAddDlg)
	enum { IDD = IDD_PROXY_ADD_DIALOG };
	CIPAddressCtrl	m_IPAddr;
	CComboBox	cb_device;
	CString	m_etherAddr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxyAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	struct SystemEtherInfo {
		int InstallCardNo;
		char CardName[MAX_ADAPTER_NAME_LENGTH + 4 + 22];;
		char CardDesc[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
		unsigned char MacAddr[6];
		char IPAddr[16];
	};		// ��Ʈ��ũ ��� ����� ���� ����ü

protected:

	// Generated message map functions
	//{{AFX_MSG(CProxyAddDlg)
	virtual void OnOK();
	afx_msg void OnSelchangeComboDevice();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
protected:
	Proxy_BufferAddr m_buffer;		// Proxy Element�� content

	SystemEtherInfo SystemEtherInfo[MAX_DEVICE];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYADDDLG_H__421AB93E_5FB2_463E_AFCF_E1B6FC80A1D8__INCLUDED_)
