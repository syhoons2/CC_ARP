// ARPDlg.h : header file
//

#if !defined(AFX_ARPDLG_H__10688702_FF3C_4660_BBB4_98483192FCA9__INCLUDED_)
#define AFX_ARPDLG_H__10688702_FF3C_4660_BBB4_98483192FCA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CacheAddDlg.h"
#include "ProxyAddDlg.h"

#include "ARPLayer.h"
#include "EthernetLayer.h"
#include "TCPLayer.h"
#include "IPLayer.h"
#include "NILayer.h"
#include "LayerManager.h"
#include "BaseLayer.h"


/////////////////////////////////////////////////////////////////////////////
// CARPDlg dialog

class CARPDlg : public CDialog, public CBaseLayer
{
// Construction
public:
	CARPDlg(CWnd* pParent = NULL);						// standard constructor
	void GetSystemEtherInfo();							// ��Ʈ��ũ ��� ���ȭ
	void SetARPLayer(CARPLayer* arp);
	static UINT InitGratuitousARP(LPVOID pParam);		// �ּ� ������ Gratuitous ARP �ڵ������� ���� ������ �Լ�
	static UINT ReceiveThread(LPVOID pParam);			// ��Ŷ ������ ���� ������ �Լ�
	static UINT SendIPThread(LPVOID pParam);			// IP�ּ����� ���� ȣ��Ʈ �˻��� ���α׷��� ���°��� ���� ���� ������
	static UINT SendGratuitousThread(LPVOID pParam);	// Gratuitous ARP ������ ���α׷��� ���°��� ���� ���� ������
	static UINT CacheUpdate( LPVOID pParam );			// ĳ�� ����Ʈ ������Ʈ�ϱ����� ������

	//BOOL Receive( unsigned char* ppayload, int type = 0 );
	void printCache();
	void EndofProgram();

	
	CWinThread* pCacheUpdateThread;

// Dialog Data
	//{{AFX_DATA(CARPDlg)
	enum { IDD = IDD_ARP_DIALOG };
	CEdit				m_dstEthernetAddr;
	CListBox			m_ProxyList;
	CIPAddressCtrl		m_IPAddr;
	CListBox			m_CacheList;
	CIPAddressCtrl		m_srcIP;
	CIPAddressCtrl		m_dstIP;
	CComboBox			m_NIC;
	CString				m_srcEthernetAddr;
	CString				m_message;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CARPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

	// ��Ʈ��ũ ��� ����� ���� ����ü
	struct SystemEtherInfo 
	{
		int InstallCardNo;
		char CardName[MAX_ADAPTER_NAME_LENGTH + 4 + 22];
		char CardDesc[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
		unsigned char MacAddr[6];
		char IPAddr[16];
	};		

protected: //�ڵ鷯 �Լ��� ����
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CARPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeComboNd();
	afx_msg void OnButtonCacheAllDelete();
	afx_msg void OnButtonCacheDelete();
	afx_msg void OnButtonIpSend();
	afx_msg void OnButtonSet();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonEtherSend();
	afx_msg void OnButtonCacheAdd();
	afx_msg void OnButtonProxyAdd();
	afx_msg void OnButtonProxyDelete();
	afx_msg void OnButtonProxyAllDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

	CIPLayer*		m_IPLayer;
	CARPLayer*		m_ARPLayer;
	CEthernetLayer*	m_EtherLayer;
	CNILayer*		m_NILayer;
	CLayerManager	m_LayerMgr;

	SystemEtherInfo SystemEtherInfo[MAX_DEVICE];

	CWinThread*		receiveThread_;

	BOOL			m_SendReady;		// ä�� ���ɿ���
	
public:
	afx_msg void OnBnClickedButtonCacheAdd();
	afx_msg void OnEnChangeEditSrcEtherAddr();
	afx_msg void OnIpnFieldchangedIpaddressDstIpAddr(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditDstEtherAddr();
	afx_msg void OnLbnSelchangeListCacheTable();
	afx_msg void OnLbnSelchangeListProxyTable();
	afx_msg void OnBnClickedButtonProxyAllDelete();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARPDLG_H__10688702_FF3C_4660_BBB4_98483192FCA9__INCLUDED_)
