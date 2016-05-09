// IPLayer.cpp: implementation of the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "IPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPLayer::CIPLayer( char *pName)
: CBaseLayer( pName ), mp_Dlg( NULL )
{
	  ResetHeader();
}

CIPLayer::~CIPLayer()
{

}

void CIPLayer::ResetHeader()
{
		m_sHeader.ip_verlen		= 0x00;
		m_sHeader.ip_tos		= 0x00;
		m_sHeader.ip_len		= 0x0000;
		m_sHeader.ip_id			= 0x0000;
		m_sHeader.ip_fragoff	= 0x0000;
		m_sHeader.ip_ttl		= 0x00;
		m_sHeader.ip_proto		= 0x00;
		m_sHeader.ip_cksum		= 0x0000;
		memset(m_sHeader.ip_src, 0, 4);
		memset(m_sHeader.ip_dst, 0, 4);
		memset(m_sHeader.ip_data, 0, IP_DATA);
}


void CIPLayer::SetSourceAddress(unsigned char *src_addr) 
{
	memcpy(m_sHeader.ip_src, src_addr, 4);
}

void CIPLayer::SetDestinAddress(unsigned char *dst_addr) 
{
	memcpy(m_sHeader.ip_dst, dst_addr, 4);
}

unsigned char* CIPLayer::GetSourceAddress() 
{
	return (unsigned char*)m_sHeader.ip_src;
}
unsigned char* CIPLayer::GetDestinAddress() 
{
	return (unsigned char*)m_sHeader.ip_dst;
}
		
BOOL CIPLayer::Send(unsigned char *ppayload, int nlength, int type) 
{
	m_sHeader.ip_len = htons( ( nlength > IP_DATA ? IP_DATA : nlength ) );
	memcpy(m_sHeader.ip_data, ppayload, nlength > IP_DATA ? IP_DATA : nlength);

	// �ּ� ������ ��ȭ���ڳ����� �̷�����Ƿ� IPLayer������ ���� �����͸� �״�� Encapsulation�� ����		
	BOOL bSuccess = FALSE;
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, (nlength > IP_DATA ? IP_DATA : nlength) + IP_HEADER);
	
	return bSuccess;
}

BOOL CIPLayer::Receive( unsigned char* ppayload, int type ) 
{
	PIP_HEADER pFrame = (PIP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;	
	bool isOK = false;		// �����㰡 ���θ� ���� ����
	int i = 0;	
	pFrame->ip_len = ntohs( pFrame->ip_len );


	for(i = 0; i < 4; i++)
		if(pFrame->ip_dst[i] != m_sHeader.ip_src[i])
			break;
	if(i == 4)
		isOK = true;

		
	for(i = 0; i < 4; i++)
		if(pFrame->ip_src[i] != m_sHeader.ip_src[i])
			break;
	if(i != 4) {
		for(i = 0; i < 4; i++)
			if(pFrame->ip_dst[i] != 255)
				break;
		if(i == 4)
			isOK = true;
	}	
	// ���� �㰡�� ������������ ������ ����
	if(isOK)
	{
		bSuccess = mp_aUpperLayer[0]->Receive( (unsigned char*)pFrame->ip_data );
		if(bSuccess == FALSE)
			::AfxMessageBox("Error while Receiving something.");
		return bSuccess;
	}

	else
		return FALSE;
}