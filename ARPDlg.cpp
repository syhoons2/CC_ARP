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

	// ARP LAYER 추가
	m_LayerMgr.AddLayer( new CTCPLayer( "TCP" ) );
	m_LayerMgr.AddLayer( new CIPLayer( "IP" ) );
	m_LayerMgr.AddLayer( new CARPLayer( "ARP" ) );
	m_LayerMgr.AddLayer( new CEthernetLayer( "ETHERNET" ) );
	m_LayerMgr.AddLayer( new CNILayer( "NI" ) );
	m_LayerMgr.AddLayer( this );

	// LAYER 관계 설정
	m_LayerMgr.ConnectLayers( "NI ( *ETHERNET ( *ARP +IP ( -ARP *TCP ( *ARPdlg ) ) ) ) )" );

	// LAYER를 dialog 변수에 할당
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

BEGIN_MESSAGE_MAP(CARPDlg, CDialog) //버튼의 ID와 핸들러 함수를 연결시키는 매크로
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

	// 네트워크 장치 정보 획득 및 출력
	
	GetSystemEtherInfo();		// 네트워크 장치 정보 획득
	
	int CardNo = 0;
	
	while( SystemEtherInfo[CardNo].InstallCardNo != -1 ) 
	{
		// 콤보박스에 얻어온 네트워크 장비의 리스트를 뿌려준다
		m_NIC.AddString( ( LPCTSTR )SystemEtherInfo[CardNo].CardDesc );
		CardNo++;
	}
	
	//		버튼 상태 초기화
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
	

	// ARP CACHE TABLE 목록 업데이트 위한 스레드
	pCacheUpdateThread = ::AfxBeginThread( CARPDlg::CacheUpdate, this );
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CARPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// 종료할 때 메세지 처리
	if ( nID == SC_CLOSE )
	{
		if ( MessageBox( "종료하시겠습니까 ?", 
			"EXIT", 
			MB_YESNO | MB_ICONQUESTION ) 
			== IDNO )
			return ;
		else EndofProgram( ) ;		// 할당 받은 Layer들을 해제시킨다.
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
		
		// 캐시, 프록시 테이블 갱신		
		m_CacheList.ResetContent();	// Cache List 비우기
		m_ProxyList.ResetContent();	// Cache List 비우기
		
		while(cache_list != NULL) 
		{
			if(cache_list->cache_entry.cache_type == 0x00)
			{
				// ARP 요청시 응답이 없을 경우 맥주소 "??-??-??-??-??-??"로 셋팅
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
			// Proxy Table에 정보 셋팅
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
	
	
	// 장비 목록을 읽음
	DWORD dwBufLen = sizeof( AdapterInfo );		// Save memory size of buffer
	DWORD dwStatus = GetAdaptersInfo(			// Call GetAdapterInfo
		AdapterInfo,							// [out] buffer to receive data
		&dwBufLen );							// [in] size of receive data buffer
	
	if ( dwStatus != ERROR_SUCCESS )
	{	
		AfxMessageBox( "Error!" );
	}
	
	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

	// 얻은 장비 목록을 정리함 
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
	
	// 카드가 없을 경우 
	if ( !CardNo ) 
	{
		AfxMessageBox( "No Network Device." );
		return;
	}
	
	SystemEtherInfo[CardNo].InstallCardNo = -1;  // 네트워크 디바이스 리스트의 끝 표시
}

// 콤보박스에서 네트워크 장비가 선택되면 MAC주소와 IP주소를 셋팅
void CARPDlg::OnSelchangeComboNd() 
{
	// TODO: Add your control notification handler code here
	unsigned char *addr = SystemEtherInfo[m_NIC.GetCurSel( )].MacAddr;	// 선택된 장비의 MAC주소를 가져와서 addr에 저장
	CString dstmacaddr;	// Gratuitous ARP 입력 창에 자동으로 Ehternet Address가 올라오도록 저장하기 위한 변수

	// Ehternet Address를 저장
	m_srcEthernetAddr.Format( "%02x-%02x-%02x-%02x-%02x-%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );
	dstmacaddr.Format( "%02x-%02x-%02x-%02x-%02x-%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5] );

	// Gratuitous ARP Ehternet 주소 출력
	m_dstEthernetAddr.SetSel(0, -1);
	m_dstEthernetAddr.Clear();
	m_dstEthernetAddr.ReplaceSel(dstmacaddr);
	

	BYTE ip_Addr[4];
	char c_temp[4];
	
	for( int i = 0; i < 4; i++ ) 
	{
		memcpy( c_temp, SystemEtherInfo[m_NIC.GetCurSel( )].IPAddr + i*4, 3 );	// 현재 IP주소를 가져와서 임시로 저장
		c_temp[3] = '\0';
		ip_Addr[i] = atoi( c_temp );
	}
	
	m_srcIP.SetAddress( ip_Addr[0], ip_Addr[1], ip_Addr[2], ip_Addr[3] ); // SOURCE IP ADDRESS 셋팅
	
	UpdateData( false );
	
}

void CARPDlg::OnButtonSet() 
{
	// TODO: Add your control notification handler code here

	char name[5];
	( CButton* )GetDlgItem( IDC_BUTTON_SET )->GetWindowText( name, 5 );

	if( strcmp( name, "SET" ) == 0) //	버튼 이름이 'SET'이면
	{	
		UpdateData(true);
		
		// Ethernet 주소가 제대로 입력되었는지를 판별
		bool isRight = false;	//	올바르게 입력되었는지 판별하는 변수
		
		//	길이가 17개( 12( 순수 주소 길이 ) + 5( '-'의 개수 ) )면 
		if( m_srcEthernetAddr.GetLength( ) == 17 ) 
		{	
			int i = 0;
			for(i; i < 17; i++)
			{
				char c = m_srcEthernetAddr.GetAt(i);	//	한글자씩 읽어와서 검사한다.
				
				//	'-'문자가 있는 곳의 위치는 3번째, 6번째, 9번째, 12번째, 15번째
				//	즉, 3의 배수번째에 위치 하기 때문에 이 문자는 검사할 필요가 없음
				if( (i + 1) % 3 == 0 ) 
				{
					if( c != '-' )
						break;
				}
				//	그 나머지 문자에 대해서는 16진수에 맞게 제대로 입력되었는지
				//	검사 하고 올바른 주소가 입력되었으면
				//	이 모든 검사가 통과되고 i의 값은 17이 된다.
				else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) ){
					break;
				}
					
			}
			if(i==17)
				isRight = true;
		}


		// 3가지 주소 중 하나라도 입력이 되지 않았으면
		if( m_srcEthernetAddr.IsEmpty( ) || m_srcIP.IsBlank( ))
		{	
			AfxMessageBox( "입력되지 않은 항목이 있습니다." );
			return;
		}
		// 올바른 주소형식이 아니면
		else if(isRight == false) 
		{		
			AfxMessageBox( "주소형식이 올바르지 않습니다.( FORMAT : xx-xx-xx-xx-xx-xx )" );
			return;
		}
		// 잘 입력이 되었다면 IP, Ethernet 주소들을 세팅한다.
		else 
		{		
			unsigned char ip_srcAddr[4];
			unsigned char ip_dstAddr[4];
			unsigned char ether_srcAddr[12];
			CString temp;
			
			//	한 문자를 기준으로 주소를 쪼갬 여기서는 '-'문자
			//	각 쪼개진 문자는 각각의 주소이므로 저장해 둔다.
			for( int i = 0; i < 12; i++ ) 
			{
				AfxExtractSubString( temp, m_srcEthernetAddr, i, '-' );												
				sscanf( temp,"%x",&ether_srcAddr[i] );	
																	
			}

			// 현재 입력되어있는 IP주소도 각각 임시 저장 변수에 저장해 둔다.
			m_srcIP.GetAddress( ( BYTE )ip_srcAddr[0], ( BYTE )ip_srcAddr[1], ( BYTE )ip_srcAddr[2], ( BYTE )ip_srcAddr[3] );

			// 각 레이어에 Destination과 Source 주소 Set
			m_IPLayer->SetSourceAddress( ( unsigned char* )ip_srcAddr );
			m_IPLayer->SetDestinAddress( ( unsigned char* )ip_dstAddr );
			m_ARPLayer->SetSourceHdAddress( ( unsigned char* )ether_srcAddr );
			m_ARPLayer->SetSourceProtoAddress( ( unsigned char* )ip_srcAddr );
			m_ARPLayer->SetDestinProtoAddress( ( unsigned char* )ip_dstAddr );
			m_EtherLayer->SetSourceAddress( ( unsigned char* )ether_srcAddr );

			// 패킷 송수신에 쓰일 네트워크 장비를 선택한 후 패킷 수신 스레드를 시작함
			if( m_NILayer->setND( SystemEtherInfo[m_NIC.GetCurSel( )].CardName ) )
				receiveThread_ = AfxBeginThread( ReceiveThread, ( LPVOID )m_NILayer );
		}
		//	SET 버튼을 눌렀을 때 버튼 상태 설정
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

	// 버튼 이름이 "RESET"일 경우
	else 
	{
		// 재설정을 하면 연결이 끊어지므로 쓰레드 종료
		TerminateThread(receiveThread_->m_hThread, 0);		 

		// RESET 버튼을 눌렀을 때 버튼 상태 설정
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
	// 채팅일 경우
	if( type == 1 ) 
	{
		//	메세지 내용이 없으면
		if( ppayload == NULL ) 
		{	
			m_SendReady = TRUE;		//	수신 대기 상태를 TRUE로 설정
			KillTimer( 1 );			//	타이머 종료
			return TRUE;			//	전송 완료
		}

		// 수신된 메시지를 채팅창에 출력
		CString receiveMessage;
		receiveMessage.Format( "You : %s\r\n", ppayload );	
		
		return TRUE;
	}	
	else
		return FALSE;
}*/


UINT CARPDlg::ReceiveThread(LPVOID pParam) 
{
	( ( CNILayer* )pParam )->Receive( );	// 쓰레드를 이용해 NILayer의 리시브 함수를 호출
	
	return 0;
}

void CARPDlg::OnButtonCacheAdd() 
{
	// TODO: Add your control notification handler code here
	CCacheAddDlg dlg;		//	캐시를 추가할 다이얼로그 변수를 하나 추가
	
	// 새로운 dialog에 입력된 내용을 가져와 ARP CACHE LIST에 추가한다
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

	// 리스트 박스에 있는 IP값 추출하여 삭제
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
	
	m_ARPLayer->DeleteCache( IPAddr );	// 주어진 IP를 갖는 Cache element 제거
	
	Invalidate();
}

void CARPDlg::OnButtonCacheAllDelete() 
{
	// TODO: Add your control notification handler code here
	m_ARPLayer->AllDeleteCache();	// 모든 element 제거
	Invalidate();
}


// ARP, Gratuitous ARP 스레드
void CARPDlg::OnButtonIpSend() 
{
	// TODO: Add your control notification handler code here
	// ARPLayer를 미리 설정
	SetARPLayer( ( CARPLayer* )m_ARPLayer );

	Sleep(1000);
	AfxBeginThread(SendIPThread, (LPVOID)this);
}

// ARP 스레드
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
		dlg->printCache();	// 캐시테이블 갱신 
		Sleep( 2000 );		// 2초마다 
	}
	
	return 0;
}

void CARPDlg::printCache()
{
	LPCACHE_LIST cache_list = m_ARPLayer->GetCacheList()->next;
	LPPROXY_LIST proxy_list = m_ARPLayer->GetProxyList()->next;

	CString list;
	
	// 캐시, 프록시 테이블 갱신
	
	m_CacheList.ResetContent();	// Cache List 비우기
	m_ProxyList.ResetContent();	// Proxy List 비우기

	while(cache_list != NULL) 
	{
		if(cache_list->cache_entry.cache_type == 0x00)
		{
			// ARP 요청시 응답이 없을 경우 맥주소 "??-??-??-??-??-??"로 셋팅
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
		// Proxy Table에 정보 추가
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
	memcpy( temp_targetIP, arp->GetDestinProtoAddress( ), 4 );		// 이전 목적지 주소(임시저장)
	
	arp->SetDestinProtoAddress( arp->GetSourceProtoAddress( ) );	//	목적지 주소를 내 주소로 세팅
	arp->Send( NULL, 0 );											//	데이터 없이 ARP를 전송함
	arp->SetDestinProtoAddress( temp_targetIP );					//	전송 후에는 이전 목적지 주소로 반환함
	
	return 0;
}

UINT CARPDlg::SendGratuitousThread( LPVOID pParam ) 
{
	CARPDlg* dlg = (CARPDlg*)pParam;
	
	CString m_EtherAddr;
	dlg->m_dstEthernetAddr.GetWindowText(m_EtherAddr);

	// Ethernet 주소가 제대로 입력되었는지를 판별
	bool isRight = false;	//	올바르게 입력되었는지 판별하는 변수
	
	//	길이가 17개( 12( 순수 주소 길이 ) + 5( '-'의 개수 ) )면 
	if(m_EtherAddr.GetLength() == 17)
	{
		int i = 0;
		for(i; i < 17; i++) 
		{
			char c = m_EtherAddr.GetAt(i);	//	한글자씩 읽어와서 검사한다.
			
			//	'-'문자가 있는 곳의 위치는 3번째, 6번째, 9번째, 12번째, 15번째
			//	즉, 3의 배수번째에 위치 하기 때문에 이 문자는 검사할 필요가 없음
			if( (i + 1) % 3 == 0 ) 
			{
				if( c != '-' )
					break;
			}
			//	그 나머지 문자에 대해서는 16진수에 맞게 제대로 입력되었는지
			//	검사 하고 올바른 주소가 입력되었으면
			//	이 모든 검사가 통과되고 i의 값은 17이 된다.
			else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) )
				break;
		}
		
		if(i == 17)
			isRight = true;
	}
	
	if(m_EtherAddr.IsEmpty()) 
	{
		AfxMessageBox( "입력되지 않은 항목이 있습니다.");
		return 0;
	}
	
	else if(isRight == false) 
	{
		AfxMessageBox("주소 형식이 올바르지 않습니다( FORMAT : xx-xx-xx-xx-xx-xx )");
		return 0;
	}
	
	unsigned char temp_targetIP[4];
	memcpy(temp_targetIP, dlg->m_ARPLayer->GetDestinProtoAddress(), 4);		// arp 목적지 IP 임시 저장
	

	unsigned char temp_srchd[10];
	for(int i = 0; i < 10; i++) 
	{
		CString temp;
		unsigned char char_temp[10];
		AfxExtractSubString(temp, m_EtherAddr, i, '-');
		sscanf(temp,"%x", &char_temp[i]);
		temp_srchd[i] = char_temp[i];
	}
	
	// 입력된 Mac주소를 출발지 Mac주소로 바꿈
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

	// dlg에서 가져온 content를 통해 주소목록 추가
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
	m_ARPLayer->AllDeleteProxy();	// 모든 element 제거
	Invalidate();
}

// 레이어 매니저를 통해 할당받은 레이어를 해제함
void CARPDlg::EndofProgram( )
{
	m_LayerMgr.DeAllocLayer( ) ;
}

void CARPDlg::OnEnChangeEditSrcEtherAddr()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// __super::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CARPDlg::OnEnChangeEditDstEtherAddr()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// __super::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CARPDlg::OnLbnSelchangeListCacheTable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CARPDlg::OnLbnSelchangeListProxyTable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CARPDlg::OnBnClickedButtonProxyAllDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
