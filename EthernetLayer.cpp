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
	((CNILayer*)mp_UnderLayer)->SetSourceAddress(pAddress);		// NILayer�� sourceAddress�� ���� �ڵ� ����
}

void CEthernetLayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy( m_sHeader.enet_dstaddr, pAddress, 6 ) ;
	((CNILayer*)mp_UnderLayer)->SetDestinAddress(pAddress);		// NILayer�� destinAddress�� ���� �ڵ� ����
}

BOOL CEthernetLayer::Send(unsigned char *ppayload, int nlength, int type)
{
	// type: 1 => ������ ���� => 0x0800,  2 => ARP ��� ���� => 0x0806
	memcpy( m_sHeader.enet_data, ppayload, nlength > ETHER_DATA ? ETHER_DATA : nlength ) ;

	if(type == 1)
		m_sHeader.enet_type = htons( 0x0800 );	//	����Ʈ ������ �ؼ� ����
	else if(type == 2)
		m_sHeader.enet_type = htons( 0x0806 );	//	����Ʈ ������ �ؼ� ����
	else
		return FALSE;

	BOOL bSuccess = FALSE ;

	// �ּ� ������ ��ȭ���ڳ����� �̷�����Ƿ� EtherenetLayer������ ���� �����͸� �״�� Encapsulation�� ����
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, (nlength > ETHER_DATA ? ETHER_DATA : nlength) + ETHER_HEADER );
	

	return bSuccess ;
}

BOOL CEthernetLayer::Receive( unsigned char* ppayload, int type )
{
	PETHERNET_HEADER pFrame = (PETHERNET_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;
	bool isOK = false;		// �����㰡 ���θ� ���� ����
	int i = 0;

	pFrame->enet_type = ntohs( pFrame->enet_type );		// �����Ҷ��� ����Ʈ �������� �Ͽ� ������


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

	// ���� �㰡�� ������������ ������ ����
	if(isOK)
	{
		// enet_type: 0x0800 => IP�� ����,  0x0806 => ARP�� ����
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
