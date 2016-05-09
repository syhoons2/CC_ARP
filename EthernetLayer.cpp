// EthernetLayer.cpp: implementation of the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "EthernetLayer.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEthernetLayer::CEthernetLayer( char* pName )
: CBaseLayer( pName )
{
	ResetHeader( ) ;
}

CEthernetLayer::~CEthernetLayer()
{
}

void CEthernetLayer::ResetHeader()
{
	memset( m_sHeader.enet_dstaddr, 0, 6 ) ;
	memset( m_sHeader.enet_srcaddr, 0, 6 ) ;
	memset( m_sHeader.enet_data, 0, ETHER_DATA ) ;
	m_sHeader.enet_type = 0 ;
}

unsigned char* CEthernetLayer::GetSourceAddress()
{
	return (unsigned char*)m_sHeader.enet_srcaddr ;
}

unsigned char* CEthernetLayer::GetDestinAddress()
{
	return (unsigned char*)m_sHeader.enet_dstaddr ;
}

void CEthernetLayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy( m_sHeader.enet_srcaddr, pAddress, 6 );
	((CNILayer*)mp_UnderLayer)->SetSourceAddress(pAddress);		// NILayer의 sourceAddress도 같이 자동 설정
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy( m_sHeader.enet_dstaddr, pAddress, 6 ) ;
	((CNILayer*)mp_UnderLayer)->SetDestinAddress(pAddress);		// NILayer의 destinAddress도 같이 자동 설정
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength, int type)
{
	// type: 1 => 데이터 전송 => 0x0800,  2 => ARP 기능 수행 => 0x0806
	memcpy( m_sHeader.enet_data, ppayload, nlength > ETHER_DATA ? ETHER_DATA : nlength ) ;

	if(type == 1)
		m_sHeader.enet_type = htons( 0x0800 );	//	바이트 오더링 해서 전송
	else if(type == 2)
		m_sHeader.enet_type = htons( 0x0806 );	//	바이트 오더링 해서 전송
	else
		return FALSE;

	BOOL bSuccess = FALSE ;

	// 주소 설정은 대화상자내에서 이루어지므로 EtherenetLayer에서는 받은 데이터를 그대로 Encapsulation후 전달
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, (nlength > ETHER_DATA ? ETHER_DATA : nlength) + ETHER_HEADER );
	

	return bSuccess ;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload, int type )
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;
	bool isOK = false;		// 수신허가 여부를 위한 변수
	int i = 0;

	pFrame->enet_type = ntohs( pFrame->enet_type );		// 수신할때도 바이트 오더링을 하여 수신함


	for(i = 0; i < 6; i++)
		if(pFrame->enet_dstaddr[i] != m_sHeader.enet_srcaddr[i])
			break;
	if(i == 6)
		isOK = true;

	
	for(i = 0; i < 6; i++)
		if(pFrame->enet_srcaddr[i] != m_sHeader.enet_srcaddr[i])
			break;
	if(i != 6) 
	{
		for(i = 0; i < 6; i++)
			if(pFrame->enet_dstaddr[i] != 0xff)
				break;
		if(i == 6)
			isOK = true;
	}

	// 수신 허가시 상위계층으로 데이터 전달
	if(isOK)
	{
		// enet_type: 0x0800 => IP로 전달,  0x0806 => ARP로 전달
		if(pFrame->enet_type == 0x0800) 
		{
			bSuccess = mp_aUpperLayer[1]->Receive( (unsigned char*)pFrame->enet_data );
			return bSuccess;
		}
		else if(pFrame->enet_type == 0x0806) 
		{
			bSuccess = mp_aUpperLayer[0]->Receive( (unsigned char*)pFrame->enet_data );
			return bSuccess;
		}
	}	
	
	return FALSE;
}
