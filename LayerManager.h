// LayerManager.h: interface for the CLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LAYERMANAGER_H__AEB38DEC_EF27_44BD_9DA6_E63B548CB960__INCLUDED_)
#define AFX_LAYERMANAGER_H__AEB38DEC_EF27_44BD_9DA6_E63B548CB960__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"

class CLayerManager  
{
private:
	typedef struct _NODE { 

		char			token[ 50 ] ;
		struct _NODE*	next ;
		
	} NODE, *PNODE ;
	
public:
	void			DeAllocLayer( );
	
	void			ConnectLayers( char* pcList );
	CBaseLayer*		GetLayer( char* pName );
	CBaseLayer*		GetLayer( int nindex );
	void			AddLayer( CBaseLayer* pLayer );
	
	CLayerManager( ) ;
	virtual ~CLayerManager( ) ;
	
private:
	// about stack...
	int				m_nTop;
	CBaseLayer*		mp_Stack[ MAX_LAYER_NUMBER ];
	
	CBaseLayer*		Top( );
	CBaseLayer*		Pop( );
	void			Push( CBaseLayer* pLayer );
	
    PNODE			mp_sListHead;
	PNODE			mp_sListTail;
	
	// about Link Layer...
	void			LinkLayer( PNODE pNode );
	
	inline void		AddNode( PNODE pNode );
	inline PNODE	AllocNode( char* pcName );
	void			MakeList( char* pcList );
	
	int				m_nLayerCount;
	CBaseLayer*		mp_aLayers[ MAX_LAYER_NUMBER ] ;
	
};


#endif // !defined(AFX_LAYERMANAGER_H__AEB38DEC_EF27_44BD_9DA6_E63B548CB960__INCLUDED_)
