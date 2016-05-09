// EthernetLayer.h: interface for the CEthernetLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ETHERNETLAYER_H__E6E1CF64_F0DF_41FB_9FF2_1C1A29231D29__INCLUDED_)
#define AFX_ETHERNETLAYER_H__E6E1CF64_F0DF_41FB_9FF2_1C1A29231D29__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "NILayer.h"

class CEthernetLayer : public CBaseLayer  
{
private:
	inline void		ResetHeader( );

public:
	BOOL			Receive( unsigned char* ppayload, int type = 0 ) ;
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0 );
	void			SetDestinAddress( unsigned char* pAddress );
	void			SetSourceAddress( unsigned char* pAddress );
	unsigned char*	GetDestinAddress( );
	unsigned char*	GetSourceAddress( ) ;

	CEthernetLayer( char* pName );
	virtual ~CEthernetLayer();

	typedef struct _ETHERNET_HEADER {

		unsigned char	enet_dstaddr[6];		// 상대방 주소(6바이트)
		unsigned char	enet_srcaddr[6];		// 자기 주소(6바이트)
		unsigned short	enet_type;				// frame data type , 2
		unsigned char	enet_data[ ETHER_DATA ] ; // data 부분은 46-1500가 되어야 하고 최대는 1500이다.
	} _ETHERNET_HEADER, *PETHERNET_HEADER ;

protected:
	_ETHERNET_HEADER	m_sHeader ;				// Ethernet header에 대한 구조체 변수를 선언해 놓는다.

};

#endif // !defined(AFX_ETHERNETLAYER_H__E6E1CF64_F0DF_41FB_9FF2_1C1A29231D29__INCLUDED_)
