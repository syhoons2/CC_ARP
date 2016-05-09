// NILayer.h: interface for the CNILayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILAYER_H__E47ABD41_C64D_434D_AFB4_6314D133FF0B__INCLUDED_)
#define AFX_NILAYER_H__E47ABD41_C64D_434D_AFB4_6314D133FF0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CNILayer : public CBaseLayer
{
private:
	CObject* mp_Dlg;

public:
	BOOL			Receive( );		// 네트워크카드로부터 패킷 수신
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0 );	// 네트워크카드에 패킷 송신
	BOOL			setND( char *dvName );		// dvName의 장비를 패킷 송수신을 위해 준비
	char*			getND();
	void			SetDestinAddress( unsigned char* pAddress );
	void			SetSourceAddress( unsigned char* pAddress );
	unsigned char*	GetDestinAddress( );
	unsigned char*	GetSourceAddress( ) ;

	CNILayer( char *pName);
	virtual ~CNILayer();

private:	
	char *deviceName;				// 패킷 송수신을 위해 쓰일 장비명
	pcap_t *adhandle;				// 패킷 송수신을 위해 사용할 핸들러
	unsigned char dstaddr[6];
	unsigned char srcaddr[6];
};


#endif // !defined(AFX_NILAYER_H__E47ABD41_C64D_434D_AFB4_6314D133FF0B__INCLUDED_)
