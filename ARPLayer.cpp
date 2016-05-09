// ARPLayer.cpp: implementation of the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "ARPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CARPLayer::CARPLayer( char *pName )
: CBaseLayer( pName ), mp_Dlg(NULL)
{
	ResetHeader();

	cache_header = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
	cache_header->next = NULL;
	proxy_header = (LPPROXY_LIST)malloc( sizeof( PROXY_LIST ) );
	proxy_header->next = NULL;

	// ĳ�����̺� �ڵ� ���� ������
	TTLThread_ = ::AfxBeginThread( TTLThread, ( LPVOID )this );
	
}

CARPLayer::~CARPLayer()
{
	// TTL ������ ����
	TerminateThread( TTLThread_->m_hThread, 0 );
}

// ARP_Head reset
void CARPLayer::ResetHeader()
{
		m_sHeader.arp_hdtype = 0x0001;	
		m_sHeader.arp_prototype = 0x0800;
		m_sHeader.arp_hdlength = 6;
		m_sHeader.arp_protolength = 4;
		m_sHeader.arp_op = 0x0000;

		memset( m_sHeader.arp_srchdaddr, 0, 6 );
		memset( m_sHeader.arp_srcprotoaddr, 0, 4 );
		memset( m_sHeader.arp_desthdaddr, 0, 6 );
		memset( m_sHeader.arp_destprotoaddr, 0, 4 );
}

//		MAC, IP �ּ� ���� �κ�
// ����ּ� ����(Mac�ּ�)
void CARPLayer::SetSourceHdAddress( unsigned char *src_addr ) 
{
	memcpy( m_sHeader.arp_srchdaddr, src_addr, 6 );
}
// ��� �ּ��� IP�ּ� ����
void CARPLayer::SetSourceProtoAddress( unsigned char *src_addr ) 
{
	memcpy( m_sHeader.arp_srcprotoaddr, src_addr, 4 );
}
// ���� �ּ� ����(Mac�ּ�)
void CARPLayer::SetDestinHdAddress( unsigned char *dst_addr ) 
{
	memcpy( m_sHeader.arp_desthdaddr, dst_addr, 6 );
}
// ���� �ּ� ����(IP�ּ�)
void CARPLayer::SetDestinProtoAddress( unsigned char *dst_addr ) 
{
	memcpy( m_sHeader.arp_destprotoaddr, dst_addr, 4 );
}
// ����ּ� ��ȯ(Mac�ּ�)
unsigned char* CARPLayer::GetSourceHdAddress( ) 
{
	return m_sHeader.arp_srchdaddr;
}
// ����ּ� ��ȯ(IP�ּ�)
unsigned char* CARPLayer::GetSourceProtoAddress () 
{
	return m_sHeader.arp_srcprotoaddr;
}
// �������ּ� ��ȯ(Mac�ּ�)
unsigned char* CARPLayer::GetDestinHdAddress( ) 
{
	return m_sHeader.arp_desthdaddr;
}
// �������ּ� ��ȯ(IP�ּ�)
unsigned char* CARPLayer::GetDestinProtoAddress( ) 
{
	return m_sHeader.arp_destprotoaddr;
}

// Cachelist ��ȯ
LPCACHE_LIST CARPLayer::GetCacheList() 
{
	return cache_header;
}

// Proxylist ��ȯ
LPPROXY_LIST CARPLayer::GetProxyList() 
{
	return proxy_header;
}

// ĳ��, ���Ͻ� ����Ʈ�� ���� Add, Delete���� �Լ�
// Cache��Ͽ� �߰�
void CARPLayer::AddCache( LPCACHE_LIST cache_element ) 
{
	cache_element->next = cache_header->next;
	cache_header->next = cache_element;	
}

// Proxy��Ͽ� �߰�
void CARPLayer::AddProxy( LPPROXY_LIST proxy_element ) 
{
	proxy_element->next = proxy_header->next;
	proxy_header->next = proxy_element;
}

// ĳ�ø�Ͽ��� ����
BOOL CARPLayer::DeleteCache(unsigned char* ipaddr) 
{
	LPCACHE_LIST before, current;

	before = cache_header;
	current = before->next;

	while(current != NULL)
	{
		if( IsEqualAddr( current->cache_entry.cache_ipaddr, ipaddr, 4) ) 
		{
			before->next = current->next;
			free(current);
			break;
		}	// ������ �ּ� �߽߰� ���� ����� next�� ������ �ּ��� ���� ��� ����, �׸��� ����

		before = current;
		current = current->next;
	}

	if(current == NULL)
		return FALSE;

	return TRUE;
}
// ���Ͻ� ��Ͽ��� ����(ĳ�ø�� ���ſ� ������)
BOOL CARPLayer::DeleteProxy(unsigned char* ipaddr) 
{
	LPPROXY_LIST before, current;

	before = proxy_header;
	current = before->next;

	while(current != NULL) 
	{
		if( IsEqualAddr( current->proxy_entry.proxy_ipaddr, ipaddr, 4) ) 
		{
			before->next = current->next;
			free(current);
			break;			
		}	// ������ �ּ� �߽߰� ���� ����� next�� ������ �ּ��� ���� ��� ����, �׸��� ����

		before = current;
		current = current->next;
	}

	if(current == NULL)
		return FALSE;

	return TRUE;
}

// ��� ĳ�� ����
void CARPLayer::AllDeleteCache() 
{
	LPCACHE_LIST temp_list, temp_element;
	temp_list = cache_header->next;
	cache_header->next = NULL;

	while(temp_list != NULL)
	{
		temp_element = temp_list;
		temp_list = temp_list->next;
		free(temp_element);
	}	// ����Ʈ�� ���� ��� entry ����
}

// ��� ���Ͻ� ����
void CARPLayer::AllDeleteProxy() 
{
	LPPROXY_LIST temp_list, temp_element;
	temp_list = proxy_header->next;
	proxy_header->next = NULL;
	
	while(temp_list != NULL) 
	{
		temp_element = temp_list;
		temp_list = temp_list->next;
		free(temp_element);
	}	// ����Ʈ�� ���� ��� entry ����
}

// Send( unsigned char* ppayload, int nlength, int type )
BOOL CARPLayer::Send( unsigned char* ppayload, int nlength, int type) 
{
	BOOL bSuccess = FALSE;

	if( ppayload == NULL ) 
	{
		bSuccess = SeekAddr();
		return bSuccess;
	}	// Gratuitous�� �ܼ��� IP�ּҸ� ���� ȣ��Ʈ�� �˻�

	int i;
	for( i = 0; i < 4; i++ )		//	��ε�ĳ��Ʈ �˻� ��ƾ
		if( m_sHeader.arp_destprotoaddr[i] != 255 )	
			break;
		// ��ε�ĳ��Ʈ ����̸�
		if( i == 4 ) 
		{					
			unsigned char broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };	// �ּ� ff-ff-ff-ff-ff-ff�� �����ϰ�
			( ( CEthernetLayer* )mp_UnderLayer)->SetDestinAddress( broadcast );		// �̴��� ���̾ ������ �ּ� �����ϰ�
			bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );					// ������ ����
			return bSuccess;
		}
		
		// ��ε�ĳ��Ʈ�� �ƴϸ� �´� �ּҸ� ã�ƾ� �Ѵ�.
		LPCACHE_LIST seekList = cache_header->next;

		// ����Ʈ ������ ã��
		while( seekList != NULL ) 
		{		
			// ĳ�ø���Ʈ�� ARP ������ �ּҰ� �ְ�
			if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
			{
				// �� ĳ�� Ÿ���� INVALID��
				if( seekList->cache_entry.cache_type == CACHE_TYPE_INVALID )
					return FALSE;	//	FALSE�� ��ȯ�Ѵ�.
			// INVALID�� �ƴϸ�
			else 
			{
				// �ּ� �����ϰ� Ethernet���� ������ ����
				( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( seekList->cache_entry.cache_enetaddr );
				bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );
				return bSuccess;
			}
		}
		seekList = seekList->next;
	}
	
	// List���� ����. => SeekAddr�޼ҵ带 ���� ������ IP�ּҸ� �������� ���� ȣ��Ʈ �˻� => �˻� ������ ������ ����
	if( SeekAddr( ) == TRUE ) 
	{

		bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );
		return bSuccess;
	}	

	return FALSE;	// SeekAddr ���н� FALSE ��ȯ
}
// Cache�� ������ �ּ�ã�� �Լ�
BOOL CARPLayer::SeekAddr( ) 
{
	// ���� IP�ּҿ� ������ IP�ּҰ� ���� ������� seekList
	if( ! IsEqualAddr( m_sHeader.arp_srcprotoaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
	{
		LPCACHE_LIST seekList = cache_header->next;

		while( seekList != NULL ) 
		{
			// seekList�� ĳ���� entry���� ĳ���� ip�ּҿ� arp�� ������ �ּҰ� ��ġ�� ���
			if( IsEqualAddr(seekList->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
			{
				// cache�� ttl�� 180���� ����
				seekList->cache_entry.cache_ttl = 180;
				// type�� CACHE_TYPE_INVALIDŸ������ �ٲ۴�.
				seekList->cache_entry.cache_type = CACHE_TYPE_INVALID;
				break;
			}

			seekList = seekList->next;
		}
		// seekList�� NULL�̶��
		if( seekList == NULL )
		{
			// �� ã�����ϴ� ������ ĳ�ÿ� ���ٸ� �߰��Ѵ�.
			LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
			memcpy( cache_element->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 );
			cache_element->cache_entry.cache_ttl = 180;
			cache_element->cache_entry.cache_type = CACHE_TYPE_INVALID;
			cache_element->next = NULL;
			AddCache( cache_element );
		}
	}	// Gratuitous ARP�� �ƴҰ�� => Cache List�� Element �ӽû���(INVALID)

	int try_cnt;	
	// 3������ �ݺ������� �˻� �ǽ�
	for(try_cnt = 0; try_cnt < 3; try_cnt++) 
	{
		seekAddrMsg = 0;

		// ARP ��û �޼����� �ѹ� ������ �׿� ���� ���� �޽����� ��ٸ��� 2�ʵ��� ������ ��ٸ�
		::SetTimer(AfxGetMainWnd()->GetSafeHwnd(), 3, 2000, NULL);	// ARP Timer �۵�

		m_sHeader.arp_op = 0x0001;		// ARP ��û �޽���
		unsigned char broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
		( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( broadcast );
		mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

		while( seekAddrMsg == 0 );		// ARP ���� �޽����� �����ϰų� Timeout �ɶ����� ���

		if( seekAddrMsg == 1 )
			break;						// ARP ���� �޽��� ����
		else if( seekAddrMsg == 2 )
			continue;					// Timeout
		else
			try_cnt = 2;				// Gratuitous ARP���� �ڽ��� IP�� ������ IP�߽߰� �˻� �����ߴ�
	}

	if(try_cnt == 3)
		return FALSE;					// �˻��õ� 3ȸ �ʰ��� FALSE ��ȯ

	return TRUE;
}
// AddrMsg�� �����Ѵ�.
void CARPLayer::SetSeekAddrMsg(unsigned char msg) 
{
	seekAddrMsg = msg;
}
// seekAddrMsg�� ���´�.
unsigned char CARPLayer::GetSeekAddrMsg() 
{
	return seekAddrMsg;
}

BOOL CARPLayer::Receive( unsigned char* ppayload, int type) 
{

	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;	

	if(pFrame->arp_op == 0x0001) {		//	ARP Request
		// ���� ��Ŷ�� ������ � ���� arp_�� ������ �ּҿ�, m_sHeader�� ������ IP�ּҰ� �����Ѱ��
		if( ! IsEqualAddr(pFrame->arp_srcprotoaddr, m_sHeader.arp_srcprotoaddr, 4) ) 
		{
			LPCACHE_LIST seekList = cache_header->next;
			//seekList�� ������� �������
			while(seekList != NULL) {
				//ĳ���� IP�ּҿ� �޾ƿ� ��Ŷ�� �� ���� ����IP�ּҰ� �����ϴٸ�
				if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 ) ) 
				{
					//ĳ���� �̴����ּҿ� �޾ƿ� ��Ŷ�� ������ ���� �����ּҸ� �����Ѵ�. �׸��� ttl�� type�� �����Ͽ��ش�.
					memcpy(seekList->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6);
					seekList->cache_entry.cache_ttl = 1200;
					seekList->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
					break;
				}
				
				seekList = seekList->next;
			}
			// seekList�� ����ִٸ�
			if( seekList == NULL ) 
			{
				// ĳ�ø���Ʈ ���� ����
				LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
				// �� �̴����ּҿ� IP�ּҸ� �����ϰ� ttl�� type�� �ٲپ� �ش�
				memcpy( cache_element->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
				memcpy( cache_element->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 );
				cache_element->cache_entry.cache_ttl = 1200;
				cache_element->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
				cache_element->next = NULL;			
			// �׸��� ĳ�ÿ� �����Ͽ� �ش�.
			AddCache( cache_element );
			}
		}	
		// �ڽ��� �ּ����� ������ �����κ��� ARP�� ���޵Ȱ��� �ƴ϶�� �켱 ���ŵ� ARP�� �ּ��� ����/�ֽ�ȭ
		// ��Ŷ�� ����� ������IP�ּҿ� m_sHeader�� ����IP�ּҰ� ������ ���
		if( IsEqualAddr( pFrame->arp_destprotoaddr, m_sHeader.arp_srcprotoaddr, 4 ) ) 
		{
			m_sHeader.arp_op = 0x0002;	// arp_op�� 0x0002�� ����

			unsigned char temp_targetIP[4];
			memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );		// arp�� ������ IP �ӽ�����

			memcpy( m_sHeader.arp_desthdaddr, pFrame->arp_srchdaddr, 6 );
			memcpy( m_sHeader.arp_destprotoaddr, pFrame->arp_srcprotoaddr, 4 );

			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

			mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

			memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );	// arp�� ������ IP ���󺹱�
		}	// �ڽ��� �ּ����� ���ŵ� ARP�� �������� ���ٸ� �ڽ��� �ּ����� ���� �޽����� ����

		else 
		{
			LPPROXY_LIST seekList = proxy_header->next;
			while( seekList != NULL ) 
			{
				if( IsEqualAddr( seekList->proxy_entry.proxy_ipaddr, pFrame->arp_destprotoaddr, 4 ) ) 
				{
					m_sHeader.arp_op = 0x0002;

					unsigned char temp_targetIP[4];
					memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );	// arp�� ������ IP �ӽ�����

					unsigned char temp_sourceIP[4];
					memcpy( temp_sourceIP, m_sHeader.arp_srcprotoaddr, 4 );	// arp�� ����� IP �ӽ�����

					memcpy( m_sHeader.arp_desthdaddr, pFrame->arp_srchdaddr, 6 );
					memcpy( m_sHeader.arp_destprotoaddr, pFrame->arp_srcprotoaddr, 4 );
					memcpy( m_sHeader.arp_srcprotoaddr, pFrame->arp_destprotoaddr, 4 );	/* �ڽ��� IP �ּ����� ���ŵ� 
																						   ARP�� ������IP�� ���� */

					( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

					mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

					memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );	// arp�� ������ IP ���󺹱�
					memcpy( m_sHeader.arp_srcprotoaddr, temp_sourceIP, 4 );		// arp�� ����� IP ���󺹱�
				}
				seekList = seekList->next;
			}					
		}	// ���ŵ� ARP�� �������� �ڽ��� �ּҰ� �ƴҽ� Proxy ARP ����

		return TRUE;
	}

	else if( pFrame->arp_op == 0x0002 ) 
	{		// ARP�� Reply �϶�
		// seekList�� ĳ���� header�� ���� �� ����Ų��
		LPCACHE_LIST seekList = cache_header->next;
		// seekList�� NULL�϶�����
		while( seekList != NULL )
		{
			// seekList�� ĳ���� IP�ּҿ� �޾ƿ� ���� ����IP�ּҰ� ������� ���� �����ϰ� ����.
			if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 ) ) 
			{
				memcpy( seekList->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
				seekList->cache_entry.cache_ttl = 1200;
				seekList->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
				break;
			}
			
			seekList = seekList->next;
		}
		// seekList�� ����ִ� ���
		if( seekList == NULL ) 
		{
			// ��������
			LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
			// IP, MAC�ּ� ���� ���� , ttl, type����
			memcpy( cache_element->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
			memcpy( cache_element->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 );

			cache_element->cache_entry.cache_ttl = 1200;
			cache_element->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
			cache_element->next = NULL;

			AddCache( cache_element );
		}	// ������ ARP�� �ּ����� ����/�ֽ�ȭ

		
		// �����ּҰ� �������
		if( IsEqualAddr( pFrame->arp_srcprotoaddr, m_sHeader.arp_srcprotoaddr, 4 ) ) 
		{

			::KillTimer( AfxGetMainWnd( )->GetSafeHwnd( ), 3 );
			seekAddrMsg = 3;	// SeekAddr �����ߴ�

			CString errorMsg;

			errorMsg.Format( "Duplicate IP address sent from Ethernet address: %02x-%02x-%02x-%02x-%02x-%02x",
				pFrame->arp_srchdaddr[0], pFrame->arp_srchdaddr[1], pFrame->arp_srchdaddr[2], 
				pFrame->arp_srchdaddr[3], pFrame->arp_srchdaddr[4], pFrame->arp_srchdaddr[5] );

			::AfxMessageBox( errorMsg );	// Duplicate IP address ��� �޽��� ���

			m_sHeader.arp_op = 0x0001;
			
			unsigned char broadcastEther[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( broadcastEther );

			unsigned char temp_targetIP[4];
			memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );		// arp�� ������ IP �ӽ�����
			
			unsigned char temp_sourceEther[6];
			memcpy( temp_sourceEther, m_sHeader.arp_srchdaddr, 6 );		// arp�� ����� MAC�ּ� �ӽ�����

			memcpy( m_sHeader.arp_srchdaddr, pFrame->arp_srchdaddr, 6 );	/* �ٸ� ȣ��Ʈ���� IP�ּ����� ����ȭ�ϱ� ����
																		   �ڽ��� MAC�ּҸ� ���� ARP�� MAC�ּҷ� ���� */

			memset( m_sHeader.arp_destprotoaddr, 0, 4 );					/* �ּҸ� ã�� ������ �ƴϹǷ� ������ IP��
																		   0.0.0.0���� ����	*/
			
			mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );
			
			memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );		// arp�� ������ IP ���󺹱�
			memcpy( m_sHeader.arp_srchdaddr, temp_sourceEther, 6 );		// arp�� ����� MAC�ּ� ���󺹱�
		}	/* ������ ARP�� �ּ����� �ڽ��� �ּ����� ���ٸ� Duplicate IP�̹Ƿ� ��� �޽��� ���
			   => �ٸ� ȣ��Ʈ���� ����� Cache ����� ����ȭ�ϱ� ���� Gratuitous ��õ� */

		else 
		{
			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

			::KillTimer( AfxGetMainWnd( )->GetSafeHwnd( ), 3 );		//	ARPŸ�̸� ����

			seekAddrMsg = 1;
		}	// ������ ���� �޽����� �������� ��� SeekAddr�� �����ϰ�, ������ ARP�� MAC�ּ����� Ethernet ������ ����

		return TRUE;
	}

	return FALSE;
}



// ĳ�����̺� TTL ������ ���� ���� ������
UINT CARPLayer::TTLThread( LPVOID pParam ) 
{
	CARPLayer* arp = ( CARPLayer* )pParam;
	LPCACHE_LIST temp_list = NULL;
	LPCACHE_LIST delete_element = NULL;

	while( true ) 
	{
		Sleep( 1000 );			// 1�ʾ� ����

		temp_list = arp->cache_header->next;

		while( temp_list != NULL )
		{
			if( temp_list->cache_entry.cache_type != CACHE_TYPE_STATIC )	// STATIC�� ��������
				if(--( temp_list->cache_entry.cache_ttl ) == 0) 
				{				// 1�ʴ����� ������ 0�� �Ǹ� �����Ͽ��� ����
					delete_element = temp_list;
					temp_list = temp_list->next;
					arp->DeleteCache( delete_element->cache_entry.cache_ipaddr );
					continue;
				}

			temp_list = temp_list->next;
		}
	}

	return 0;
}

// array1�� �Ѿ�� �ּҿ� array2�� �Ѿ�� �ּҰ� cnt���̸�ŭ ���� ��� ��ȯ�� true
bool CARPLayer::IsEqualAddr(unsigned char* array1, unsigned char* array2, int cnt)
{
	int i;
	for(i = 0; i < cnt; i++)
		if(array1[i] != array2[i])
			break;

	if(i == cnt)
		return true;

	return false;
}