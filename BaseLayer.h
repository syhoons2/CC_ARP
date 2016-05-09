// BaseLayer.h: interface for the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASELAYER_H__3032FBC4_56EB_4407_AC11_3C4CC57E6B62__INCLUDED_)
#define AFX_BASELAYER_H__3032FBC4_56EB_4407_AC11_3C4CC57E6B62__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBaseLayer  
{
public:
	CBaseLayer( char* pName = NULL );
	virtual ~CBaseLayer();

	char*			GetLayerName( );
	
	CBaseLayer*		GetUnderLayer( );
	CBaseLayer*		GetUpperLayer( int nindex );
	void			SetUnderUpperLayer( CBaseLayer* pUULayer = NULL );
	void			SetUpperUnderLayer( CBaseLayer* pUULayer = NULL );
	void			SetUnderLayer( CBaseLayer* pUnderLayer = NULL ) ;
	void			SetUpperLayer( CBaseLayer* pUpperLayer = NULL ) ;
	unsigned short  htons( unsigned short x );
	unsigned short	ntohs( unsigned short x );

	// param : unsigned char*	- the data of the upperlayer
	//         int				- the length of data
	virtual	BOOL	Send( unsigned char* ppayload, int nlength, int type = 0 ) 	{ return FALSE ; }
	// param : unsigned char*	- the data of the underlayer
	virtual	BOOL	Receive( unsigned char* ppayload, int type = 0) 	{ return FALSE ; }
	virtual	BOOL	Receive( ) 	{ return FALSE ; }
	
protected:
	char*			m_pLayerName;
	CBaseLayer*		mp_UnderLayer;							// UnderLayer pointer
	CBaseLayer*		mp_aUpperLayer[ MAX_LAYER_NUMBER ];		// UpperLayer pointer
	int				m_nUpperLayerCount;						// UpperLayer Count

};

#endif // !defined(AFX_BASELAYER_H__3032FBC4_56EB_4407_AC11_3C4CC57E6B62__INCLUDED_)
