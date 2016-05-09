// ARPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ARP.h"
#include "ARPDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARPDlg dialog

CARPDlg::CARPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CARPDlg::IDD, pParent), CBaseLayer( "ARPdlg" )
{
	//{{AFX_DATA_INIT(CARPDlg)
	m_srcEthernetAddr = _T("");
	m_message = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// ARP LAYER �߰�
	m_LayerMgr.AddLayer( new CTCPLayer( "TCP" ) );
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) );
	m_LayerMgr.AddLayer( new CEthernetLayer( "ETHERNET" ) );
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) );
	m_LayerMgr.AddLayer( this );

	// LAYER ���� ����
	m_LayerMgr.ConnectLayers( "NI ( *ETHERNET ( *ARP +IP ( -ARP *TCP ( *ARPdlg ) ) ) ) )" );

	// LAYER�� dialog ������ �Ҵ�
	m_IPLayer = ( CIPLayer* )m_LayerMgr.GetLayer( "IP" );
	m_ARPLayer = ( CARPLayer* )m_LayerMgr.GetLayer( "ARP" );
	m_EtherLayer = ( CEthernetLayer* )m_LayerMgr.GetLayer( "ETHERNET" );
	m_NILayer = ( CNILayer* )m_LayerMgr.GetLayer( "NI" );

	m_SendReady = TRUE;		
}

void CARPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CARPDlg)
	DDX_Control(pDX, IDC_EDIT_DST_ETHER_ADDR, m_dstEthernetAddr);
	DDX_Control(pDX, IDC_LIST_PROXY_TABLE, m_ProxyList);
	DDX_Control(pDX, IDC_IPADDRESS_IP_ADDR, m_IPAddr);
	DDX_Control(pDX, IDC_LIST_CACHE_TABLE, m_CacheList);
	DDX_Control(pDX, IDC_IPADDRESS_SRC_IP_ADDR, m_srcIP);
	DDX_Control(pDX, IDC_IPADDRESS_DST_IP_ADDR, m_dstIP);
	DDX_Control(pDX, IDC_COMBO_ND, m_NIC);
	DDX_Text(pDX, IDC_EDIT_SRC_ETHER_ADDR, m_srcEthernetAddr);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CARPDlg, CDialog) //��ư�� ID�� �ڵ鷯 �Լ��� �����Ű�� ��ũ��
	//{{AFX_MSG_MAP(CARPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_ND, OnSelchangeComboNd)
	ON_BN_CLICKED(IDC_BUTTON_CACHE_ALL_DELETE, OnButtonCacheAllDelete)
	ON_BN_CLICKED(IDC_BUTTON_CACHE_DELETE, OnButtonCacheDelete)
	ON_BN_CLICKED(IDC_BUTTON_IP_SEND, OnButtonIpSend)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnButtonSet)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ETHER_SEND, OnButtonEtherSend)
	//ON_BN_CLICKED(IDC_BUTTON_CACHE_ADD, OnButtonCacheAdd)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_ADD, OnButtonProxyAdd)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_DELETE, OnButtonProxyDelete)
	//ON_BN_CLICKED(IDC_BUTTON_PROXY_ALL_DELETE, OnButtonProxyAllDelete)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_SRC_ETHER_ADDR, &CARPDlg::OnEnChangeEditSrcEtherAddr)
	ON_EN_CHANGE(IDC_EDIT_DST_ETHER_ADDR, &CARPDlg::OnEnChangeEditDstEtherAddr)
	ON_LBN_SELCHANGE(IDC_LIST_CACHE_TABLE, &CARPDlg::OnLbnSelchangeListCacheTable)
	ON_LBN_SELCHANGE(IDC_LIST_PROXY_TABLE, &CARPDlg::OnLbnSelchangeListProxyTable)
	ON_BN_CLICKED(IDC_BUTTON_PROXY_ALL_DELETE, &CARPDlg::OnBnClickedButtonProxyAllDelete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CARPDlg message handlers

BOOL CARPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// ��Ʈ��ũ ��ġ ���� ȹ�� �� ���
	
	GetSystemEtherInfo();		// ��Ʈ��ũ ��ġ ���� ȹ��
	
	int CardNo = 0;
	
	while( SystemEtherInfo[CardNo].InstallCardNo != -1 ) 
	{
		// �޺��ڽ��� ���� ��Ʈ��ũ ����� ����Ʈ�� �ѷ��ش�
		m_NIC.AddString( ( LPCTSTR )SystemEtherInfo[CardNo].CardDesc );
		CardNo++;
	}
	
	//		��ư ���� �ʱ�ȭ
	( CEdit* )GetDlgItem( IDC_EDIT_SRC_ETHER_ADDR )->EnableWindow( false );
	( CListBox* )GetDlgItem( IDC_LIST_CACHE_TABLE )->EnableWindow( false );
	//( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ADD )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_CACHE_DELETE )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ALL_DELETE )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_PROXY_ADD )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_PROXY_DELETE )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_IP_SEND )->EnableWindow( false );
	( CIPAddressCtrl *)GetDlgItem( IDC_IPADDRESS_IP_ADDR )->EnableWindow( false );
	( CEdit* )GetDlgItem( IDC_EDIT_DST_ETHER_ADDR )->EnableWindow( false );
	( CButton* )GetDlgItem( IDC_BUTTON_ETHER_SEND )->EnableWindow( false );
	

	// ARP CACHE TABLE ��� ������Ʈ ���� ������
	pCacheUpdateThread = ::AfxBeginThread( CARPDlg::CacheUpdate, this );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// ������ �� �޼��� ó��
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "�����Ͻðڽ��ϱ� ?", 
			"EXIT", 
			MB_YESNO | MB_ICONQUESTION ) 
			== IDNO )
			return ;
		else EndofProgram( ) ;		// �Ҵ� ���� Layer���� ������Ų��.
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CARPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		LPCACHE_LIST cache_list = m_ARPLayer->GetCacheList()->next;
		LPPROXY_LIST proxy_list = m_ARPLayer->GetProxyList()->next;
		
		CString list;
		
		// ĳ��, ���Ͻ� ���̺� ����		
		m_CacheList.ResetContent();	// Cache List ����
		m_ProxyList.ResetContent();	// Cache List ����
		
		while(cache_list != NULL) 
		{
			if(cache_list->cache_entry.cache_type == 0x00)
			{
				// ARP ��û�� ������ ���� ��� ���ּ� "??-??-??-??-??-??"�� ����
				list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: \?\?-\?\?-\?\?-\?\?-\?\?-\?\?] [%7s]", 
					cache_list->cache_entry.cache_ipaddr[0], cache_list->cache_entry.cache_ipaddr[1], 
					cache_list->cache_entry.cache_ipaddr[2], cache_list->cache_entry.cache_ipaddr[3], 
					"Incomplete");
				m_CacheList.AddString(list);
				cache_list = cache_list->next;
			}
			else
			{
				list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: %02x-%02x-%02x-%02x-%02x-%02x] [%7s]", 
					cache_list->cache_entry.cache_ipaddr[0], cache_list->cache_entry.cache_ipaddr[1], 
					cache_list->cache_entry.cache_ipaddr[2], cache_list->cache_entry.cache_ipaddr[3], 
					cache_list->cache_entry.cache_enetaddr[0], cache_list->cache_entry.cache_enetaddr[1], 
					cache_list->cache_entry.cache_enetaddr[2], cache_list->cache_entry.cache_enetaddr[3], 
					cache_list->cache_entry.cache_enetaddr[4], cache_list->cache_entry.cache_enetaddr[5], 
					"Complete");
				m_CacheList.AddString(list);
				cache_list = cache_list->next;
			}
		}

		while(proxy_list != NULL) 
		{
			// Proxy Table�� ���� ����
			list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: %02x-%02x-%02x-%02x-%02x-%02x] [Added Table]",
				proxy_list->proxy_entry.proxy_ipaddr[0], proxy_list->proxy_entry.proxy_ipaddr[1], 
				proxy_list->proxy_entry.proxy_ipaddr[2], proxy_list->proxy_entry.proxy_ipaddr[3], 
				proxy_list->proxy_entry.proxy_enetaddr[0], proxy_list->proxy_entry.proxy_enetaddr[1], 
				proxy_list->proxy_entry.proxy_enetaddr[2], proxy_list->proxy_entry.proxy_enetaddr[3], 
				proxy_list->proxy_entry.proxy_enetaddr[4], proxy_list->proxy_entry.proxy_enetaddr[5]);
			
			m_ProxyList.AddString(list);
			proxy_list = proxy_list->next;
		}
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CARPDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CARPDlg::SetARPLayer(CARPLayer* arp)
{
	m_ARPLayer = arp;
}

void CARPDlg::GetSystemEtherInfo()
{	
	int CardNo = 0;	
	IP_ADAPTER_INFO AdapterInfo[MAX_DEVICE];
	
	
	// ��� ����� ����
	DWORD dwBufLen = sizeof( AdapterInfo );		// Save memory size of buffer
	DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
		AdapterInfo,							// [out] buffer to receive data
		&dwBufLen );							// [in] size of receive data buffer
	
	if ( dwStatus != ERROR_SUCCESS )
	{	
		AfxMessageBox( "Error!" );
	}
	
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

	// ���� ��� ����� ������ 
	do 
	{
		SystemEtherInfo[CardNo].InstallCardNo = CardNo; 
		memcpy( SystemEtherInfo[CardNo].CardName, "rpcap://\\Device\\NPF_", 22 );
		strcat( ( char* )SystemEtherInfo[CardNo].CardName, ( char* )pAdapterInfo->AdapterName );
		memcpy( SystemEtherInfo[CardNo].CardDesc, pAdapterInfo->Description, MAX_ADAPTER_DESCRIPTION_LENGTH + 4 );
		memcpy( SystemEtherInfo[CardNo].MacAddr, ( unsigned char* )pAdapterInfo->Address, 6 );
		memcpy( SystemEtherInfo[CardNo].IPAddr, pAdapterInfo->IpAddressList.IpAddress.String, 16 );
		
		pAdapterInfo = pAdapterInfo->Next;
		CardNo++;
	}
	while( pAdapterInfo);
	
	// ī�尡 ���� ��� 
	if ( !CardNo ) 
	{
		AfxMessageBox( "No Network Device." );
		return;
	}
	
	SystemEtherInfo[CardNo].InstallCardNo = -1;  // ��Ʈ��ũ ����̽� ����Ʈ�� �� ǥ��
}

// �޺��ڽ����� ��Ʈ��ũ ��� ���õǸ� MAC�ּҿ� IP�ּҸ� ����
void CARPDlg::OnSelchangeComboNd() 
{
	// TODO: Add your control notification handler code here
	unsigned char *addr = SystemEtherInfo[m_NIC.GetCurSel( )].MacAddr;	// ���õ� ����� MAC�ּҸ� �����ͼ� addr�� ����
	CString dstmacaddr;	// Gratuitous ARP �Է� â�� �ڵ����� Ehternet Address�� �ö������ �����ϱ� ���� ����

	// Ehternet Address�� ����
	m_srcEthernetAddr.Format( "%02x-%02x-%02x-%02x-%02x-%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );
	dstmacaddr.Format( "%02x-%02x-%02x-%02x-%02x-%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );

	// Gratuitous ARP Ehternet �ּ� ���
	m_dstEthernetAddr.SetSel(0, -1);
	m_dstEthernetAddr.Clear();
	m_dstEthernetAddr.ReplaceSel(dstmacaddr);
	

	BYTE ip_Addr[4];
	char c_temp[4];
	
	for( int i = 0; i < 4; i++ ) 
	{
		memcpy( c_temp, SystemEtherInfo[m_NIC.GetCurSel( )].IPAddr + i*4, 3 );	// ���� IP�ּҸ� �����ͼ� �ӽ÷� ����
		c_temp[3] = '\0';
		ip_Addr[i] = atoi( c_temp );
	}
	
	m_srcIP.SetAddress( ip_Addr[0], ip_Addr[1], ip_Addr[2], ip_Addr[3] ); // SOURCE IP ADDRESS ����
	
	UpdateData( false );
	
}

void CARPDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here

	char name[5];
	( CButton* )GetDlgItem( IDC_BUTTON_SET )->GetWindowText( name, 5 );

	if( strcmp( name, "SET" ) == 0) //	��ư �̸��� 'SET'�̸�
	{	
		UpdateData(true);
		
		// Ethernet �ּҰ� ����� �ԷµǾ������� �Ǻ�
		bool isRight = false;	//	�ùٸ��� �ԷµǾ����� �Ǻ��ϴ� ����
		
		//	���̰� 17��( 12( ���� �ּ� ���� ) + 5( '-'�� ���� ) )�� 
		if( m_srcEthernetAddr.GetLength( ) == 17 ) 
		{	
			int i = 0;
			for(i; i < 17; i++)
			{
				char c = m_srcEthernetAddr.GetAt(i);	//	�ѱ��ھ� �о�ͼ� �˻��Ѵ�.
				
				//	'-'���ڰ� �ִ� ���� ��ġ�� 3��°, 6��°, 9��°, 12��°, 15��°
				//	��, 3�� �����°�� ��ġ �ϱ� ������ �� ���ڴ� �˻��� �ʿ䰡 ����
				if( (i + 1) % 3 == 0 ) 
				{
					if( c != '-' )
						break;
				}
				//	�� ������ ���ڿ� ���ؼ��� 16������ �°� ����� �ԷµǾ�����
				//	�˻� �ϰ� �ùٸ� �ּҰ� �ԷµǾ�����
				//	�� ��� �˻簡 ����ǰ� i�� ���� 17�� �ȴ�.
				else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) ){
					break;
				}
					
			}
			if(i==17)
				isRight = true;
		}


		// 3���� �ּ� �� �ϳ��� �Է��� ���� �ʾ�����
		if( m_srcEthernetAddr.IsEmpty( ) || m_srcIP.IsBlank( ))
		{	
			AfxMessageBox( "�Էµ��� ���� �׸��� �ֽ��ϴ�." );
			return;
		}
		// �ùٸ� �ּ������� �ƴϸ�
		else if(isRight == false) 
		{		
			AfxMessageBox( "�ּ������� �ùٸ��� �ʽ��ϴ�.( FORMAT : xx-xx-xx-xx-xx-xx )" );
			return;
		}
		// �� �Է��� �Ǿ��ٸ� IP, Ethernet �ּҵ��� �����Ѵ�.
		else 
		{		
			unsigned char ip_srcAddr[4];
			unsigned char ip_dstAddr[4];
			unsigned char ether_srcAddr[12];
			CString temp;
			
			//	�� ���ڸ� �������� �ּҸ� �ɰ� ���⼭�� '-'����
			//	�� �ɰ��� ���ڴ� ������ �ּ��̹Ƿ� ������ �д�.
			for( int i = 0; i < 12; i++ ) 
			{
				AfxExtractSubString( temp, m_srcEthernetAddr, i, '-' );												
				sscanf( temp,"%x",&ether_srcAddr[i] );	
																	
			}

			// ���� �ԷµǾ��ִ� IP�ּҵ� ���� �ӽ� ���� ������ ������ �д�.
			m_srcIP.GetAddress( ( BYTE )ip_srcAddr[0], ( BYTE )ip_srcAddr[1], ( BYTE )ip_srcAddr[2], ( BYTE )ip_srcAddr[3] );

			// �� ���̾ Destination�� Source �ּ� Set
			m_IPLayer->SetSourceAddress( ( unsigned char* )ip_srcAddr );
			m_IPLayer->SetDestinAddress( ( unsigned char* )ip_dstAddr );
			m_ARPLayer->SetSourceHdAddress( ( unsigned char* )ether_srcAddr );
			m_ARPLayer->SetSourceProtoAddress( ( unsigned char* )ip_srcAddr );
			m_ARPLayer->SetDestinProtoAddress( ( unsigned char* )ip_dstAddr );
			m_EtherLayer->SetSourceAddress( ( unsigned char* )ether_srcAddr );

			// ��Ŷ �ۼ��ſ� ���� ��Ʈ��ũ ��� ������ �� ��Ŷ ���� �����带 ������
			if( m_NILayer->setND( SystemEtherInfo[m_NIC.GetCurSel( )].CardName ) )
				receiveThread_ = AfxBeginThread( ReceiveThread, ( LPVOID )m_NILayer );
		}
		//	SET ��ư�� ������ �� ��ư ���� ����
		( CListBox* )GetDlgItem( IDC_LIST_CACHE_TABLE )->EnableWindow( true );
		//( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ADD )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_CACHE_DELETE )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ALL_DELETE )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_PROXY_ADD )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_PROXY_DELETE )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_IP_SEND )->EnableWindow( true );
		( CIPAddressCtrl *)GetDlgItem( IDC_IPADDRESS_IP_ADDR )->EnableWindow( true );
		( CEdit* )GetDlgItem( IDC_EDIT_DST_ETHER_ADDR )->EnableWindow( true );
		( CButton* )GetDlgItem( IDC_BUTTON_ETHER_SEND )->EnableWindow( true );
		( CComboBox* )GetDlgItem( IDC_COMBO_ND )->EnableWindow( false );
		( CIPAddressCtrl* )GetDlgItem( IDC_IPADDRESS_SRC_IP_ADDR )->EnableWindow( false );
		( CIPAddressCtrl* )GetDlgItem( IDC_IPADDRESS_DST_IP_ADDR )->EnableWindow( false );
		( ( CButton* )GetDlgItem( IDC_BUTTON_SET ))->SetWindowText("RESET");
	}

	// ��ư �̸��� "RESET"�� ���
	else 
	{
		// �缳���� �ϸ� ������ �������Ƿ� ������ ����
		TerminateThread(receiveThread_->m_hThread, 0);		 

		// RESET ��ư�� ������ �� ��ư ���� ����
		( CListBox* )GetDlgItem( IDC_LIST_CACHE_TABLE )->EnableWindow( false );
		//( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ADD )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_CACHE_DELETE )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_CACHE_ALL_DELETE )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_PROXY_ADD )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_PROXY_DELETE )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_IP_SEND )->EnableWindow( false );
		( CIPAddressCtrl *)GetDlgItem( IDC_IPADDRESS_IP_ADDR )->EnableWindow( false );
		( CEdit* )GetDlgItem( IDC_EDIT_DST_ETHER_ADDR )->EnableWindow( false );
		( CButton* )GetDlgItem( IDC_BUTTON_ETHER_SEND )->EnableWindow( false );
		( CComboBox* )GetDlgItem( IDC_COMBO_ND )->EnableWindow( true );
		( CIPAddressCtrl* )GetDlgItem( IDC_IPADDRESS_SRC_IP_ADDR )->EnableWindow( true );
		( CIPAddressCtrl* )GetDlgItem( IDC_IPADDRESS_DST_IP_ADDR )->EnableWindow( true );
		( (CButton* )GetDlgItem( IDC_BUTTON_SET ))->SetWindowText( "SET" );
	}
}
/*
BOOL CARPDlg::Receive(unsigned char *ppayload, int type)
{	
	// ä���� ���
	if( type == 1 ) 
	{
		//	�޼��� ������ ������
		if( ppayload == NULL ) 
		{	
			m_SendReady = TRUE;		//	���� ��� ���¸� TRUE�� ����
			KillTimer( 1 );			//	Ÿ�̸� ����
			return TRUE;			//	���� �Ϸ�
		}

		// ���ŵ� �޽����� ä��â�� ���
		CString receiveMessage;
		receiveMessage.Format( "You : %s\r\n", ppayload );	
		
		return TRUE;
	}	
	else
		return FALSE;
}*/


UINT CARPDlg::ReceiveThread(LPVOID pParam) 
{
	( ( CNILayer* )pParam )->Receive( );	// �����带 �̿��� NILayer�� ���ú� �Լ��� ȣ��
	
	return 0;
}

void CARPDlg::OnButtonCacheAdd() 
{
	// TODO: Add your control notification handler code here
	CCacheAddDlg dlg;		//	ĳ�ø� �߰��� ���̾�α� ������ �ϳ� �߰�
	
	// ���ο� dialog�� �Էµ� ������ ������ ARP CACHE LIST�� �߰��Ѵ�
	if(dlg.DoModal() == IDOK) 
	{
		LPCACHE_LIST cache_element = (LPCACHE_LIST)malloc(sizeof(CACHE_LIST));
		Cache_BufferAddr temp_BufferAddr = dlg.GetBufferAddr();
		
		memcpy(cache_element->cache_entry.cache_enetaddr, temp_BufferAddr.buffer_EtherAddr, 6);
		memcpy(cache_element->cache_entry.cache_ipaddr, temp_BufferAddr.buffer_IPAddr, 4);		
		cache_element->cache_entry.cache_type = m_ARPLayer->CACHE_TYPE_STATIC;
		cache_element->next = NULL;
		
		m_ARPLayer->AddCache(cache_element);
		
		Invalidate();
	}
}

void CARPDlg::OnButtonCacheDelete() 
{
	// TODO: Add your control notification handler code here
	if(m_CacheList.GetCurSel() == LB_ERR)
		return;

	// ����Ʈ �ڽ��� �ִ� IP�� �����Ͽ� ����
	CString temp_IPAddr;
	unsigned char IPAddr[10];
	m_CacheList.GetText(m_CacheList.GetCurSel(), temp_IPAddr);

	int offset = temp_IPAddr.Find("IP:");
	temp_IPAddr = temp_IPAddr.Right(temp_IPAddr.GetLength() - offset -4);
	offset = temp_IPAddr.Find("[MAC:");
	temp_IPAddr = temp_IPAddr.Left(offset);
	offset = temp_IPAddr.Find("]");
	temp_IPAddr = temp_IPAddr.Left(offset);
	
	for(int i = 0; i < 10; i++)
	{
		CString temp;
		unsigned char char_temp[10];
		
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp[i]);
		IPAddr[i] = char_temp[i];
	}
	
	m_ARPLayer->DeleteCache( IPAddr );	// �־��� IP�� ���� Cache element ����
	
	Invalidate();
}

void CARPDlg::OnButtonCacheAllDelete() 
{
	// TODO: Add your control notification handler code here
	m_ARPLayer->AllDeleteCache();	// ��� element ����
	Invalidate();
}


// ARP, Gratuitous ARP ������
void CARPDlg::OnButtonIpSend() 
{
	// TODO: Add your control notification handler code here
	// ARPLayer�� �̸� ����
	SetARPLayer( ( CARPLayer* )m_ARPLayer );

	Sleep(1000);
	AfxBeginThread(SendIPThread, (LPVOID)this);
}

// ARP ������
UINT CARPDlg::SendIPThread(LPVOID pParam) 
{	

	CARPDlg* dlg = (CARPDlg*)pParam;
	
	if(dlg->m_IPAddr.IsBlank()) 
	{
		AfxMessageBox("IP Address is NOT inputed.");
		return 0;
	}
	
	unsigned char temp_IPAddr[4];
	dlg->m_IPAddr.GetAddress(temp_IPAddr[0], temp_IPAddr[1], temp_IPAddr[2], temp_IPAddr[3]);
	
	unsigned char temp_targetIP[4];
	memcpy(temp_targetIP, dlg->m_ARPLayer->GetDestinProtoAddress(), 4);
	
	dlg->m_ARPLayer->SetDestinProtoAddress(temp_IPAddr);
	
	dlg->EnableWindow(false);
	if( dlg->m_ARPLayer->Send(NULL, 0) == FALSE )
		AfxMessageBox("The IP Address is not existed.");
	else
		AfxMessageBox("The IP Address found.");
	dlg->EnableWindow(true);
	
	dlg->m_ARPLayer->SetDestinProtoAddress(temp_targetIP);
	
	dlg->m_IPAddr.ClearAddress();
	dlg->Invalidate();
	
	return 0;
}

void CARPDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	// 3 => ARP
	CString failMessage;

	if(nIDEvent == 3) 
	{
		m_ARPLayer->SetSeekAddrMsg(2);
		KillTimer(3);
	}	// ARP seekAddr time out
	
	CDialog::OnTimer(nIDEvent);
}

UINT CARPDlg::CacheUpdate( LPVOID pParam )
{
	CARPDlg* dlg = ( CARPDlg* )pParam;
	
	while(1)
	{
		dlg->printCache();	// ĳ�����̺� ���� 
		Sleep( 2000 );		// 2�ʸ��� 
	}
	
	return 0;
}

void CARPDlg::printCache()
{
	LPCACHE_LIST cache_list = m_ARPLayer->GetCacheList()->next;
	LPPROXY_LIST proxy_list = m_ARPLayer->GetProxyList()->next;

	CString list;
	
	// ĳ��, ���Ͻ� ���̺� ����
	
	m_CacheList.ResetContent();	// Cache List ����
	m_ProxyList.ResetContent();	// Proxy List ����

	while(cache_list != NULL) 
	{
		if(cache_list->cache_entry.cache_type == 0x00)
		{
			// ARP ��û�� ������ ���� ��� ���ּ� "??-??-??-??-??-??"�� ����
				list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: \?\?-\?\?-\?\?-\?\?-\?\?-\?\?] [%7s]", 
				cache_list->cache_entry.cache_ipaddr[0], cache_list->cache_entry.cache_ipaddr[1], 
				cache_list->cache_entry.cache_ipaddr[2], cache_list->cache_entry.cache_ipaddr[3], 
				"Incomplete");
			m_CacheList.AddString(list);
			cache_list = cache_list->next;
		}
		else
		{
				list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: %02x-%02x-%02x-%02x-%02x-%02x] [%7s]",
				cache_list->cache_entry.cache_ipaddr[0], cache_list->cache_entry.cache_ipaddr[1], 
				cache_list->cache_entry.cache_ipaddr[2], cache_list->cache_entry.cache_ipaddr[3], 
				cache_list->cache_entry.cache_enetaddr[0], cache_list->cache_entry.cache_enetaddr[1], 
				cache_list->cache_entry.cache_enetaddr[2], cache_list->cache_entry.cache_enetaddr[3], 
				cache_list->cache_entry.cache_enetaddr[4], cache_list->cache_entry.cache_enetaddr[5], 
				"Complete");
			m_CacheList.AddString(list);
			cache_list = cache_list->next;
		}
	}
	while(proxy_list != NULL) 
	{
		// Proxy Table�� ���� �߰�
		list.Format("[IP: %3d.%3d.%3d.%3d] [MAC: %02x-%02x-%02x-%02x-%02x-%02x] [Added Table]",
			proxy_list->proxy_entry.proxy_ipaddr[0], proxy_list->proxy_entry.proxy_ipaddr[1], 
			proxy_list->proxy_entry.proxy_ipaddr[2], proxy_list->proxy_entry.proxy_ipaddr[3], 
			proxy_list->proxy_entry.proxy_enetaddr[0], proxy_list->proxy_entry.proxy_enetaddr[1], 
			proxy_list->proxy_entry.proxy_enetaddr[2], proxy_list->proxy_entry.proxy_enetaddr[3], 
			proxy_list->proxy_entry.proxy_enetaddr[4], proxy_list->proxy_entry.proxy_enetaddr[5]);
		
		m_ProxyList.AddString(list);
		proxy_list = proxy_list->next;
	}
}

void CARPDlg::OnButtonEtherSend() 
{
	// TODO: Add your control notification handler code here
	AfxBeginThread(SendGratuitousThread, (LPVOID)this);
}

UINT CARPDlg::InitGratuitousARP( LPVOID pParam )
{
	CARPLayer* arp = ( CARPLayer* )pParam;

	unsigned char temp_targetIP[4];
	memcpy( temp_targetIP, arp->GetDestinProtoAddress( ), 4 );		// ���� ������ �ּ�(�ӽ�����)
	
	arp->SetDestinProtoAddress( arp->GetSourceProtoAddress( ) );	//	������ �ּҸ� �� �ּҷ� ����
	arp->Send( NULL, 0 );											//	������ ���� ARP�� ������
	arp->SetDestinProtoAddress( temp_targetIP );					//	���� �Ŀ��� ���� ������ �ּҷ� ��ȯ��
	
	return 0;
}

UINT CARPDlg::SendGratuitousThread( LPVOID pParam ) 
{
	CARPDlg* dlg = (CARPDlg*)pParam;
	
	CString m_EtherAddr;
	dlg->m_dstEthernetAddr.GetWindowText(m_EtherAddr);

	// Ethernet �ּҰ� ����� �ԷµǾ������� �Ǻ�
	bool isRight = false;	//	�ùٸ��� �ԷµǾ����� �Ǻ��ϴ� ����
	
	//	���̰� 17��( 12( ���� �ּ� ���� ) + 5( '-'�� ���� ) )�� 
	if(m_EtherAddr.GetLength() == 17)
	{
		int i = 0;
		for(i; i < 17; i++) 
		{
			char c = m_EtherAddr.GetAt(i);	//	�ѱ��ھ� �о�ͼ� �˻��Ѵ�.
			
			//	'-'���ڰ� �ִ� ���� ��ġ�� 3��°, 6��°, 9��°, 12��°, 15��°
			//	��, 3�� �����°�� ��ġ �ϱ� ������ �� ���ڴ� �˻��� �ʿ䰡 ����
			if( (i + 1) % 3 == 0 ) 
			{
				if( c != '-' )
					break;
			}
			//	�� ������ ���ڿ� ���ؼ��� 16������ �°� ����� �ԷµǾ�����
			//	�˻� �ϰ� �ùٸ� �ּҰ� �ԷµǾ�����
			//	�� ��� �˻簡 ����ǰ� i�� ���� 17�� �ȴ�.
			else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) )
				break;
		}
		
		if(i == 17)
			isRight = true;
	}
	
	if(m_EtherAddr.IsEmpty()) 
	{
		AfxMessageBox( "�Էµ��� ���� �׸��� �ֽ��ϴ�.");
		return 0;
	}
	
	else if(isRight == false) 
	{
		AfxMessageBox("�ּ� ������ �ùٸ��� �ʽ��ϴ�( FORMAT : xx-xx-xx-xx-xx-xx )");
		return 0;
	}
	
	unsigned char temp_targetIP[4];
	memcpy(temp_targetIP, dlg->m_ARPLayer->GetDestinProtoAddress(), 4);		// arp ������ IP �ӽ� ����
	

	unsigned char temp_srchd[10];
	for(int i = 0; i < 10; i++) 
	{
		CString temp;
		unsigned char char_temp[10];
		AfxExtractSubString(temp, m_EtherAddr, i, '-');
		sscanf(temp,"%x", &char_temp[i]);
		temp_srchd[i] = char_temp[i];
	}
	
	// �Էµ� Mac�ּҸ� ����� Mac�ּҷ� �ٲ�
	((CEthernetLayer*)dlg->m_ARPLayer->GetUnderLayer())->SetSourceAddress(temp_srchd);
	dlg->m_ARPLayer->SetSourceHdAddress(temp_srchd);	
	dlg->m_ARPLayer->SetDestinProtoAddress(dlg->m_ARPLayer->GetSourceProtoAddress());
	((CEdit*)AfxGetMainWnd()->GetDlgItem( IDC_EDIT_SRC_ETHER_ADDR ))->SetWindowText(m_EtherAddr);
	
	dlg->EnableWindow(false);											
	dlg->m_ARPLayer->Send(NULL, 0);
	AfxMessageBox("SEND GRATUITOUS ARP");
	
	dlg->EnableWindow(true);		
	
	dlg->m_ARPLayer->SetDestinProtoAddress(temp_targetIP);
	
	dlg->m_dstEthernetAddr.SetSel(0, -1);
	dlg->m_dstEthernetAddr.Clear();
	dlg->Invalidate();
	
	return 0;
}


void CARPDlg::OnButtonProxyAdd() 
{
	// TODO: Add your control notification handler code here
	CProxyAddDlg dlg;

	// dlg���� ������ content�� ���� �ּҸ�� �߰�
	if(dlg.DoModal() == IDOK) 
	{
		LPPROXY_LIST proxy_element = (LPPROXY_LIST)malloc(sizeof(PROXY_LIST));
		
		Proxy_BufferAddr temp_BufferAddr = dlg.GetBufferAddr();
		
		proxy_element->proxy_entry.proxy_device = new CString(temp_BufferAddr.device);
		memcpy(proxy_element->proxy_entry.proxy_enetaddr, temp_BufferAddr.buffer_EtherAddr, 6);
		memcpy(proxy_element->proxy_entry.proxy_ipaddr, temp_BufferAddr.buffer_IPAddr, 4);
		proxy_element->next = NULL;
		
		m_ARPLayer->AddProxy(proxy_element);
		
		Invalidate();
	}	
}

void CARPDlg::OnButtonProxyDelete() 
{
	// TODO: Add your control notification handler code here
	if(m_ProxyList.GetCurSel() == LB_ERR)
		return;

	CString temp_IPAddr;
	unsigned char IPAddr[10];
	m_ProxyList.GetText(m_ProxyList.GetCurSel(), temp_IPAddr);
	
	int offset = temp_IPAddr.Find("IP:");
	temp_IPAddr = temp_IPAddr.Right(temp_IPAddr.GetLength() - offset -4);
	offset = temp_IPAddr.Find("[MAC:");
	temp_IPAddr = temp_IPAddr.Left(offset);
	offset = temp_IPAddr.Find("]");
	temp_IPAddr = temp_IPAddr.Left(offset);
	
	for(int i = 0; i < 10; i++) 
	{
		CString temp;
		unsigned char char_temp[10];
		
		AfxExtractSubString(temp, temp_IPAddr, i, '.');
		sscanf(temp,"%d", &char_temp[i]);
		IPAddr[i] = char_temp[i];
	}
	
	m_ARPLayer->DeleteProxy(IPAddr);
	
	Invalidate();
}

void CARPDlg::OnButtonProxyAllDelete() 
{
	// TODO: Add your control notification handler code here
	m_ARPLayer->AllDeleteProxy();	// ��� element ����
	Invalidate();
}

// ���̾� �Ŵ����� ���� �Ҵ���� ���̾ ������
void CARPDlg::EndofProgram( )
{
	m_LayerMgr.DeAllocLayer( ) ;
}

void CARPDlg::OnEnChangeEditSrcEtherAddr()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// __super::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CARPDlg::OnEnChangeEditDstEtherAddr()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// __super::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CARPDlg::OnLbnSelchangeListCacheTable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CARPDlg::OnLbnSelchangeListProxyTable()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CARPDlg::OnBnClickedButtonProxyAllDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
