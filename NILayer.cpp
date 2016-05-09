// NILayer.cpp: implementation of the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "NILayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNILayer::CNILayer(char* pName)
: CBaseLayer( pName ), mp_Dlg( NULL )
{
	deviceName = NULL;

}

CNILayer::~CNILayer()
{

}

BOOL CNILayer::setND( char* dvName ) 
{
	char errbuf[PCAP_ERRBUF_SIZE];

	if(deviceName != NULL)
		free(deviceName);

	deviceName = (char*)malloc(sizeof(char)*( strlen(dvName)+1 ));	// ���ڿ��� '\0'�� ���� +1
	memcpy(deviceName, dvName, strlen(dvName)+1);

	// �н� ���� �غ�
	if ( (adhandle= pcap_open(deviceName,       // name of the device
		65536,									// portion of the packet to capture. 
												// 65536 guarantees that the whole packet will be captured on all the link layers
		PCAP_OPENFLAG_PROMISCUOUS,		  	    // promiscuous mode => �������� ���� LAN���� ��Ŷ ����
		1000,								    // read timeout
		NULL,						            // authentication on the remote machine
		errbuf								    // error buffer
		) ) == NULL)
        return FALSE;
	else 
		return TRUE;
}

char* CNILayer::getND() 
{
	return deviceName;
}

unsigned char* CNILayer::GetSourceAddress()
{
	return (unsigned char*)srcaddr ;
}

unsigned char* CNILayer::GetDestinAddress()
{
	return (unsigned char*)dstaddr ;
}

void CNILayer::SetSourceAddress(unsigned char *pAddress)
{
	memcpy( srcaddr, pAddress, 6 );
}

void CNILayer::SetDestinAddress(unsigned char *pAddress)
{
	memcpy( dstaddr, pAddress, 6 ) ;
}

BOOL CNILayer::Send(unsigned char *ppayload, int nlength, int type)
{
	unsigned char *packet;

	packet = (unsigned char*)malloc(sizeof(unsigned char)*(nlength + 12));
	memcpy((char*)packet, (char*)GetDestinAddress(), 6);
	memcpy((char*)(packet+6), (char*)GetSourceAddress(), 6);
	memcpy((char*)(packet+12), (char*)ppayload, nlength);

	if (pcap_sendpacket( adhandle, (u_char*)packet, nlength + 12) != 0 )
        return FALSE;

    return TRUE;
}

BOOL CNILayer::Receive(  )
{
	int res;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	unsigned char *packet = NULL;
	BOOL bSuccess = FALSE;

	while((res = pcap_next_ex( adhandle, &header, &pkt_data)) >= 0) 
	{
        if(res == 0)
            continue;
		CString s;

		packet = (unsigned char*)malloc(sizeof(unsigned char) * (header->len - 12));
        memcpy(packet, (unsigned char*)(pkt_data + 12), (header->len - 12));		
		mp_aUpperLayer[0]->Receive(packet);		// ��Ŷ ���� 12bit(������ �����ּ� + ����� �����ּ�)�� ���ŵ� ������ ����
	}
    
    if(res == -1)
		return FALSE;		// ������ ����
	else
		return TRUE;		// ���� ����
}
