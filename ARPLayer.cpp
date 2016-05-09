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

	// 캐시테이블 자동 제거 스레드
	TTLThread_ = ::AfxBeginThread( TTLThread, ( LPVOID )this );
	
}

CARPLayer::~CARPLayer()
{
	// TTL 스레드 종료
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

//		MAC, IP 주소 세팅 부분
// 출발주소 복사(Mac주소)
void CARPLayer::SetSourceHdAddress( unsigned char *src_addr ) 
{
	memcpy( m_sHeader.arp_srchdaddr, src_addr, 6 );
}
// 출발 주소의 IP주소 복사
void CARPLayer::SetSourceProtoAddress( unsigned char *src_addr ) 
{
	memcpy( m_sHeader.arp_srcprotoaddr, src_addr, 4 );
}
// 도착 주소 복사(Mac주소)
void CARPLayer::SetDestinHdAddress( unsigned char *dst_addr ) 
{
	memcpy( m_sHeader.arp_desthdaddr, dst_addr, 6 );
}
// 도착 주소 복사(IP주소)
void CARPLayer::SetDestinProtoAddress( unsigned char *dst_addr ) 
{
	memcpy( m_sHeader.arp_destprotoaddr, dst_addr, 4 );
}
// 출발주소 반환(Mac주소)
unsigned char* CARPLayer::GetSourceHdAddress( ) 
{
	return m_sHeader.arp_srchdaddr;
}
// 출발주소 반환(IP주소)
unsigned char* CARPLayer::GetSourceProtoAddress () 
{
	return m_sHeader.arp_srcprotoaddr;
}
// 목적지주소 반환(Mac주소)
unsigned char* CARPLayer::GetDestinHdAddress( ) 
{
	return m_sHeader.arp_desthdaddr;
}
// 목적지주소 반환(IP주소)
unsigned char* CARPLayer::GetDestinProtoAddress( ) 
{
	return m_sHeader.arp_destprotoaddr;
}

// Cachelist 반환
LPCACHE_LIST CARPLayer::GetCacheList() 
{
	return cache_header;
}

// Proxylist 반환
LPPROXY_LIST CARPLayer::GetProxyList() 
{
	return proxy_header;
}

// 캐시, 프록시 리스트에 대한 Add, Delete관련 함수
// Cache목록에 추가
void CARPLayer::AddCache( LPCACHE_LIST cache_element ) 
{
	cache_element->next = cache_header->next;
	cache_header->next = cache_element;	
}

// Proxy목록에 추가
void CARPLayer::AddProxy( LPPROXY_LIST proxy_element ) 
{
	proxy_element->next = proxy_header->next;
	proxy_header->next = proxy_element;
}

// 캐시목록에서 제거
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
		}	// 제거할 주소 발견시 이전 노드의 next에 제거할 주소의 다음 노드 연결, 그리고 제거

		before = current;
		current = current->next;
	}

	if(current == NULL)
		return FALSE;

	return TRUE;
}
// 프록시 목록에서 제거(캐시목록 제거와 동일함)
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
		}	// 제거할 주소 발견시 이전 노드의 next에 제거할 주소의 다음 노드 연결, 그리고 제거

		before = current;
		current = current->next;
	}

	if(current == NULL)
		return FALSE;

	return TRUE;
}

// 모든 캐시 제거
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
	}	// 리스트를 돌며 모든 entry 제거
}

// 모든 프록시 제거
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
	}	// 리스트를 돌며 모든 entry 제거
}

// Send( unsigned char* ppayload, int nlength, int type )
BOOL CARPLayer::Send( unsigned char* ppayload, int nlength, int type) 
{
	BOOL bSuccess = FALSE;

	if( ppayload == NULL ) 
	{
		bSuccess = SeekAddr();
		return bSuccess;
	}	// Gratuitous나 단순히 IP주소를 갖는 호스트의 검색

	int i;
	for( i = 0; i < 4; i++ )		//	브로드캐스트 검사 루틴
		if( m_sHeader.arp_destprotoaddr[i] != 255 )	
			break;
		// 브로드캐스트 모드이면
		if( i == 4 ) 
		{					
			unsigned char broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };	// 주소 ff-ff-ff-ff-ff-ff로 세팅하고
			( ( CEthernetLayer* )mp_UnderLayer)->SetDestinAddress( broadcast );		// 이더넷 레이어에 목적지 주소 세팅하고
			bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );					// 데이터 전송
			return bSuccess;
		}
		
		// 브로드캐스트가 아니면 맞는 주소를 찾아야 한다.
		LPCACHE_LIST seekList = cache_header->next;

		// 리스트 끝까지 찾음
		while( seekList != NULL ) 
		{		
			// 캐시리스트에 ARP 목적지 주소가 있고
			if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
			{
				// 그 캐시 타입이 INVALID면
				if( seekList->cache_entry.cache_type == CACHE_TYPE_INVALID )
					return FALSE;	//	FALSE를 반환한다.
			// INVALID가 아니면
			else 
			{
				// 주소 세팅하고 Ethernet으로 데이터 전송
				( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( seekList->cache_entry.cache_enetaddr );
				bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );
				return bSuccess;
			}
		}
		seekList = seekList->next;
	}
	
	// List내에 없다. => SeekAddr메소드를 통해 목적지 IP주소를 기준으로 연결 호스트 검색 => 검색 성공시 데이터 전송
	if( SeekAddr( ) == TRUE ) 
	{

		bSuccess = mp_UnderLayer->Send( ppayload, nlength, 1 );
		return bSuccess;
	}	

	return FALSE;	// SeekAddr 실패시 FALSE 반환
}
// Cache에 없을시 주소찾는 함수
BOOL CARPLayer::SeekAddr( ) 
{
	// 시작 IP주소와 목적지 IP주소가 같지 않은경우 seekList
	if( ! IsEqualAddr( m_sHeader.arp_srcprotoaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
	{
		LPCACHE_LIST seekList = cache_header->next;

		while( seekList != NULL ) 
		{
			// seekList의 캐시의 entry에서 캐시의 ip주소와 arp의 목적지 주소가 일치할 경우
			if( IsEqualAddr(seekList->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 ) ) 
			{
				// cache의 ttl을 180으로 세팅
				seekList->cache_entry.cache_ttl = 180;
				// type을 CACHE_TYPE_INVALID타입으로 바꾼다.
				seekList->cache_entry.cache_type = CACHE_TYPE_INVALID;
				break;
			}

			seekList = seekList->next;
		}
		// seekList가 NULL이라면
		if( seekList == NULL )
		{
			// 즉 찾고자하는 정보가 캐시에 없다면 추가한다.
			LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
			memcpy( cache_element->cache_entry.cache_ipaddr, m_sHeader.arp_destprotoaddr, 4 );
			cache_element->cache_entry.cache_ttl = 180;
			cache_element->cache_entry.cache_type = CACHE_TYPE_INVALID;
			cache_element->next = NULL;
			AddCache( cache_element );
		}
	}	// Gratuitous ARP가 아닐경우 => Cache List에 Element 임시생성(INVALID)

	int try_cnt;	
	// 3번까지 반복적으로 검색 실시
	for(try_cnt = 0; try_cnt < 3; try_cnt++) 
	{
		seekAddrMsg = 0;

		// ARP 요청 메세지를 한번 보내고서 그에 대한 응답 메시지를 기다리는 2초동안 응답을 기다림
		::SetTimer(AfxGetMainWnd()->GetSafeHwnd(), 3, 2000, NULL);	// ARP Timer 작동

		m_sHeader.arp_op = 0x0001;		// ARP 요청 메시지
		unsigned char broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
		( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( broadcast );
		mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

		while( seekAddrMsg == 0 );		// ARP 응답 메시지가 도착하거나 Timeout 될때까지 대기

		if( seekAddrMsg == 1 )
			break;						// ARP 응답 메시지 도착
		else if( seekAddrMsg == 2 )
			continue;					// Timeout
		else
			try_cnt = 2;				// Gratuitous ARP에서 자신의 IP와 동일한 IP발견시 검색 강제중단
	}

	if(try_cnt == 3)
		return FALSE;					// 검색시도 3회 초과시 FALSE 반환

	return TRUE;
}
// AddrMsg를 세팅한다.
void CARPLayer::SetSeekAddrMsg(unsigned char msg) 
{
	seekAddrMsg = msg;
}
// seekAddrMsg를 얻어온다.
unsigned char CARPLayer::GetSeekAddrMsg() 
{
	return seekAddrMsg;
}

BOOL CARPLayer::Receive( unsigned char* ppayload, int type) 
{

	PARP_HEADER pFrame = (PARP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;	

	if(pFrame->arp_op == 0x0001) {		//	ARP Request
		// 들어온 패킷의 정보를 까서 얻은 arp_의 도착지 주소와, m_sHeader의 도착지 IP주소가 동일한경우
		if( ! IsEqualAddr(pFrame->arp_srcprotoaddr, m_sHeader.arp_srcprotoaddr, 4) ) 
		{
			LPCACHE_LIST seekList = cache_header->next;
			//seekList가 비어있지 않은경우
			while(seekList != NULL) {
				//캐시의 IP주소와 받아온 패킷을 깐 것의 시작IP주소가 동일하다면
				if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 ) ) 
				{
					//캐시의 이더넷주소에 받아온 패킷을 깠을때 나온 시작주소를 저장한다. 그리고 ttl과 type을 변경하여준다.
					memcpy(seekList->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6);
					seekList->cache_entry.cache_ttl = 1200;
					seekList->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
					break;
				}
				
				seekList = seekList->next;
			}
			// seekList가 비어있다면
			if( seekList == NULL ) 
			{
				// 캐시리스트 공간 생성
				LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
				// 각 이더넷주소와 IP주소를 저장하고 ttl과 type을 바꾸어 준다
				memcpy( cache_element->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
				memcpy( cache_element->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 );
				cache_element->cache_entry.cache_ttl = 1200;
				cache_element->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
				cache_element->next = NULL;			
			// 그리고 캐시에 저장하여 준다.
			AddCache( cache_element );
			}
		}	
		// 자신의 주소지와 동일한 곳으로부터 ARP가 전달된것이 아니라면 우선 수신된 ARP의 주소지 저장/최신화
		// 패킷을 깐것의 목적지IP주소와 m_sHeader의 시작IP주소가 동일한 경우
		if( IsEqualAddr( pFrame->arp_destprotoaddr, m_sHeader.arp_srcprotoaddr, 4 ) ) 
		{
			m_sHeader.arp_op = 0x0002;	// arp_op에 0x0002를 세팅

			unsigned char temp_targetIP[4];
			memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );		// arp의 목적지 IP 임시저장

			memcpy( m_sHeader.arp_desthdaddr, pFrame->arp_srchdaddr, 6 );
			memcpy( m_sHeader.arp_destprotoaddr, pFrame->arp_srcprotoaddr, 4 );

			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

			mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

			memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );	// arp의 목적지 IP 원상복구
		}	// 자신의 주소지와 수신된 ARP의 목적지가 같다면 자신의 주소지를 응답 메시지로 전달

		else 
		{
			LPPROXY_LIST seekList = proxy_header->next;
			while( seekList != NULL ) 
			{
				if( IsEqualAddr( seekList->proxy_entry.proxy_ipaddr, pFrame->arp_destprotoaddr, 4 ) ) 
				{
					m_sHeader.arp_op = 0x0002;

					unsigned char temp_targetIP[4];
					memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );	// arp의 목적지 IP 임시저장

					unsigned char temp_sourceIP[4];
					memcpy( temp_sourceIP, m_sHeader.arp_srcprotoaddr, 4 );	// arp의 출발지 IP 임시저장

					memcpy( m_sHeader.arp_desthdaddr, pFrame->arp_srchdaddr, 6 );
					memcpy( m_sHeader.arp_destprotoaddr, pFrame->arp_srcprotoaddr, 4 );
					memcpy( m_sHeader.arp_srcprotoaddr, pFrame->arp_destprotoaddr, 4 );	/* 자신의 IP 주소지를 수신된 
																						   ARP의 목적지IP로 수정 */

					( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

					mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );

					memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );	// arp의 목적지 IP 원상복구
					memcpy( m_sHeader.arp_srcprotoaddr, temp_sourceIP, 4 );		// arp의 출발지 IP 원상복구
				}
				seekList = seekList->next;
			}					
		}	// 수신된 ARP의 목적지가 자신의 주소가 아닐시 Proxy ARP 수행

		return TRUE;
	}

	else if( pFrame->arp_op == 0x0002 ) 
	{		// ARP가 Reply 일때
		// seekList가 캐시의 header의 다음 것 가르킨다
		LPCACHE_LIST seekList = cache_header->next;
		// seekList가 NULL일때까지
		while( seekList != NULL )
		{
			// seekList의 캐시의 IP주소와 받아온 것의 시작IP주소가 같은경우 위와 동일하게 세팅.
			if( IsEqualAddr( seekList->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 ) ) 
			{
				memcpy( seekList->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
				seekList->cache_entry.cache_ttl = 1200;
				seekList->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
				break;
			}
			
			seekList = seekList->next;
		}
		// seekList가 비어있는 경우
		if( seekList == NULL ) 
		{
			// 공간생성
			LPCACHE_LIST cache_element = ( LPCACHE_LIST )malloc( sizeof( CACHE_LIST ) );
			// IP, MAC주소 각각 저장 , ttl, type저장
			memcpy( cache_element->cache_entry.cache_enetaddr, pFrame->arp_srchdaddr, 6 );
			memcpy( cache_element->cache_entry.cache_ipaddr, pFrame->arp_srcprotoaddr, 4 );

			cache_element->cache_entry.cache_ttl = 1200;
			cache_element->cache_entry.cache_type = CACHE_TYPE_DYNAMIC;
			cache_element->next = NULL;

			AddCache( cache_element );
		}	// 도착한 ARP의 주소지를 저장/최신화

		
		// 시작주소가 같은경우
		if( IsEqualAddr( pFrame->arp_srcprotoaddr, m_sHeader.arp_srcprotoaddr, 4 ) ) 
		{

			::KillTimer( AfxGetMainWnd( )->GetSafeHwnd( ), 3 );
			seekAddrMsg = 3;	// SeekAddr 강제중단

			CString errorMsg;

			errorMsg.Format( "Duplicate IP address sent from Ethernet address: %02x-%02x-%02x-%02x-%02x-%02x",
				pFrame->arp_srchdaddr[0], pFrame->arp_srchdaddr[1], pFrame->arp_srchdaddr[2], 
				pFrame->arp_srchdaddr[3], pFrame->arp_srchdaddr[4], pFrame->arp_srchdaddr[5] );

			::AfxMessageBox( errorMsg );	// Duplicate IP address 경고 메시지 출력

			m_sHeader.arp_op = 0x0001;
			
			unsigned char broadcastEther[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( broadcastEther );

			unsigned char temp_targetIP[4];
			memcpy( temp_targetIP, m_sHeader.arp_destprotoaddr, 4 );		// arp의 목적지 IP 임시저장
			
			unsigned char temp_sourceEther[6];
			memcpy( temp_sourceEther, m_sHeader.arp_srchdaddr, 6 );		// arp의 출발지 MAC주소 임시저장

			memcpy( m_sHeader.arp_srchdaddr, pFrame->arp_srchdaddr, 6 );	/* 다른 호스트들의 IP주소지를 정상화하기 위해
																		   자신의 MAC주소를 응답 ARP의 MAC주소로 수정 */

			memset( m_sHeader.arp_destprotoaddr, 0, 4 );					/* 주소를 찾는 목적이 아니므로 목적지 IP를
																		   0.0.0.0으로 셋팅	*/
			
			mp_UnderLayer->Send( ( unsigned char* )&m_sHeader, ARP_HEADER, 2 );
			
			memcpy( m_sHeader.arp_destprotoaddr, temp_targetIP, 4 );		// arp의 목적지 IP 원상복구
			memcpy( m_sHeader.arp_srchdaddr, temp_sourceEther, 6 );		// arp의 출발지 MAC주소 원상복구
		}	/* 도착한 ARP의 주소지가 자신의 주소지와 같다면 Duplicate IP이므로 경고 메시지 출력
			   => 다른 호스트들의 변경된 Cache 목록을 정상화하기 위해 Gratuitous 재시도 */

		else 
		{
			( ( CEthernetLayer* )mp_UnderLayer )->SetDestinAddress( pFrame->arp_srchdaddr );

			::KillTimer( AfxGetMainWnd( )->GetSafeHwnd( ), 3 );		//	ARP타이머 종료

			seekAddrMsg = 1;
		}	// 도착한 응답 메시지가 정상적인 경우 SeekAddr을 중지하고, 도착한 ARP의 MAC주소지로 Ethernet 목적지 설정

		return TRUE;
	}

	return FALSE;
}



// 캐시테이블 TTL 데이터 삭제 관련 스레드
UINT CARPLayer::TTLThread( LPVOID pParam ) 
{
	CARPLayer* arp = ( CARPLayer* )pParam;
	LPCACHE_LIST temp_list = NULL;
	LPCACHE_LIST delete_element = NULL;

	while( true ) 
	{
		Sleep( 1000 );			// 1초씩 갱신

		temp_list = arp->cache_header->next;

		while( temp_list != NULL )
		{
			if( temp_list->cache_entry.cache_type != CACHE_TYPE_STATIC )	// STATIC은 영구보존
				if(--( temp_list->cache_entry.cache_ttl ) == 0) 
				{				// 1초단위로 감산후 0이 되면 저장목록에서 제거
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

// array1로 넘어온 주소와 array2로 넘어온 주소가 cnt길이만큼 같은 경우 반환값 true
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