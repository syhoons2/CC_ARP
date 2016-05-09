// BaseLayer.cpp: implementation of the CBaseLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ARP.h"
#include "BaseLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseLayer::CBaseLayer( char* pName ) 
: m_nUpperLayerCount( 0 ), mp_UnderLayer( NULL )
{
	m_pLayerName = pName ;
}

CBaseLayer::~CBaseLayer()
{

}

void CBaseLayer::SetUnderUpperLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderUpperLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}	
	this->mp_UnderLayer = pUULayer ;
	pUULayer->SetUpperLayer(this) ;

}

void CBaseLayer::SetUpperUnderLayer(CBaseLayer *pUULayer)
{
	if ( !pUULayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperUnderLayer] The variable , 'pUULayer' is NULL" ) ;
#endif
		return ;			
	}
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUULayer ;
	pUULayer->SetUnderLayer(this) ;
}

void CBaseLayer::SetUpperLayer(CBaseLayer *pUpperLayer )
{
	if ( !pUpperLayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUpperLayer] The variable , 'pUpperLayer' is NULL" ) ;
#endif
		return ;			
	}

	// UpperLayer is added..
	this->mp_aUpperLayer[ m_nUpperLayerCount++ ] = pUpperLayer ;
}

void CBaseLayer::SetUnderLayer(CBaseLayer *pUnderLayer)
{
	if ( !pUnderLayer ) // if the pointer is null, 
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::SetUnderLayer] The variable , 'pUnderLayer' is NULL\n" ) ;
#endif
		return ;			
	}

	// UnderLayer assignment..
	this->mp_UnderLayer = pUnderLayer ;
}

CBaseLayer* CBaseLayer::GetUpperLayer(int nindex)
{
	if ( nindex < 0						||
		 nindex >= m_nUpperLayerCount	||
		 m_nUpperLayerCount < 0			)
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUpperLayer] There is no UpperLayer in Array..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_aUpperLayer[ nindex ] ;
}

CBaseLayer* CBaseLayer::GetUnderLayer()
{
	if ( !mp_UnderLayer )
	{
#ifdef _DEBUG
		TRACE( "[CBaseLayer::GetUnderLayer] There is not a UnerLayer..\n" ) ;
#endif 
		return NULL ;
	}

	return mp_UnderLayer ;
}

char* CBaseLayer::GetLayerName()
{
	return m_pLayerName ; 
}

// 호스트가 네트워크로 데이터를 전송하기 전에 2바이트 데이터에 대한 바이트 오더링하는 함수
unsigned short CBaseLayer::htons( unsigned short x )
{
	return (
			( ( x & 0x00ffU ) << 8) |
			( ( x & 0xff00U ) >> 8)
			);
}

// 호스트가 네트워크로부터 데이터를 수신할 때, short 데이터를 바이트 오더링 할 경우 사용되는 함수
unsigned short CBaseLayer::ntohs( unsigned short x )
{
	return (
			( ( x & 0x00ffU ) << 8 ) |
			( ( x & 0xff00U ) >> 8 )
			);
}
