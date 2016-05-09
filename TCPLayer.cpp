// TCPLayer.cpp: implementation of the CTCPLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "TCPLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCPLayer::CTCPLayer( char *pName)
: CBaseLayer( pName), mp_Dlg( NULL )
{
	ResetHeader();
}

CTCPLayer::~CTCPLayer()
{

}

void CTCPLayer::ResetHeader( )
{
	m_sHeader.tcp_sport		= 0x0000;
	m_sHeader.tcp_dport		= 0x0000;
	m_sHeader.tcp_seq		= 0x00000000;
	m_sHeader.tcp_ack		= 0x00000000;
	m_sHeader.tcp_offset	= 0x00;
	m_sHeader.tcp_flag		= 0x00;
	m_sHeader.tcp_window	= 0x0000;
	m_sHeader.tcp_cksum		= 0x0000;
	m_sHeader.tcp_urgptr	= 0x0000;
	memset(m_sHeader.tcp_data, 0, TCP_DATA);
}

BOOL CTCPLayer::Send(unsigned char *ppayload, int nlength, int type)
{
	// type: 1 => ChatApp => 0x2080, 2 => FileApp => 0x2090

	if(type == 1)
		m_sHeader.tcp_sport = m_sHeader.tcp_dport = 0x2080;
//	else if(type == 2)
//		m_sHeader.tcp_sport = m_sHeader.tcp_dport = 0x2090;
	else
		return FALSE;

	memcpy(m_sHeader.tcp_data, ppayload, nlength > TCP_DATA ? TCP_DATA : nlength);
	
	BOOL bSuccess = FALSE;
	bSuccess = mp_UnderLayer->Send( (unsigned char*)&m_sHeader, (nlength > TCP_DATA ? TCP_DATA : nlength) + TCP_HEADER);

	return bSuccess;
}

BOOL CTCPLayer::Receive( unsigned char* ppayload, int type )
{
	// tcp_dport: 0x2080 => ChatApp전달, 0x2090 => FileApp전달
	PTCP_HEADER pFrame = (PTCP_HEADER) ppayload ;
	BOOL bSuccess = FALSE ;

	if(pFrame->tcp_dport == 0x2080) 
	{
		bSuccess = mp_aUpperLayer[0]->Receive( (unsigned char*)pFrame->tcp_data );
		return bSuccess;
	}
//	else if(pFrame->tcp_dport == 0x2090) {
//		bSuccess = mp_aUpperLayer[1]->Receive( (unsigned char*)pFrame->tcp_data );
//		return bSuccess;
//	}
	else
		return FALSE;
}

