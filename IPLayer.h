// IPLayer.h: interface for the CIPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPLAYER_H__568F14D1_9765_4ADD_A6F3_3264CB9FC25D__INCLUDED_)
#define AFX_IPLAYER_H__568F14D1_9765_4ADD_A6F3_3264CB9FC25D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CIPLayer : public CBaseLayer  
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;

public:
	BOOL			Receive( unsigned char* ppayload, int type = 0 );
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0);
	CIPLayer( char *pName );
	virtual ~CIPLayer();

	void SetSourceAddress(unsigned char *src_addr);	
	void SetDestinAddress(unsigned char *dst_addr);	
	unsigned char* GetSourceAddress();	
	unsigned char* GetDestinAddress();

	typedef struct _IP_HEADER {
		unsigned char	ip_verlen;				// IP VERSION
		unsigned char	ip_tos;					// TYPE OF SERVICE
		unsigned short	ip_len;					// TOTAL PACKET LENGTH
		unsigned short	ip_id;					// DATAGRAM ID
		unsigned short	ip_fragoff;				// FRAGMENT OFFSET
		unsigned char	ip_ttl;					// TIME TO LIVE, IN GATEWAY HOPS
		unsigned char	ip_proto;				// IP PROTOCOL
		unsigned short	ip_cksum;				// HEADER CHECKSUM
		unsigned char	ip_src[4];				// IP ADDRESS OF SOURCE
		unsigned char	ip_dst[4];				// IP ADDRESS OF DESTINATION
		unsigned char	ip_data[IP_DATA];		// VARIABLE LENGTH DATA
	} _IP_HEADER, *PIP_HEADER ;

protected:
	_IP_HEADER m_sHeader;
};

#endif // !defined(AFX_IPLAYER_H__568F14D1_9765_4ADD_A6F3_3264CB9FC25D__INCLUDED_)
