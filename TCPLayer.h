// TCPLayer.h: interface for the CTCPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPLAYER_H__07D562CA_1E44_4BD3_98CF_AE7482F84493__INCLUDED_)
#define AFX_TCPLAYER_H__07D562CA_1E44_4BD3_98CF_AE7482F84493__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CTCPLayer : public CBaseLayer  
{
private:
	inline void		ResetHeader();
	CObject* mp_Dlg;
	
public:
	BOOL			Receive( unsigned char* ppayload, int type = 0 );
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0 );
	CTCPLayer( char *pName);
	virtual ~CTCPLayer();
	
	typedef struct _TCP_HEADER {
		unsigned short	tcp_sport;				// SOURCE PORT
		unsigned short	tcp_dport;				// DESTINATION PORT
		unsigned int	tcp_seq;				// SEQUENCE NUMBER
		unsigned int	tcp_ack;				// ACKNOWLEDGED SEQUENCE
		unsigned char	tcp_offset;				// NO USE
		unsigned char	tcp_flag;				// CONTROL FLAG
		unsigned short	tcp_window;				// NO USE
		unsigned short	tcp_cksum;				// CHECK SUM
		unsigned short	tcp_urgptr;				// NO USE
		unsigned char	tcp_data[TCP_DATA];		// DATA PART
	} _TCP_HEADER, *PTCP_HEADER;
	
	
protected:
	_TCP_HEADER m_sHeader;	

};

#endif // !defined(AFX_TCPLAYER_H__07D562CA_1E44_4BD3_98CF_AE7482F84493__INCLUDED_)
