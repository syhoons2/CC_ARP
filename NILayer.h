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
	BOOL			Receive( );		// ��Ʈ��ũī��κ��� ��Ŷ ����
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0 );	// ��Ʈ��ũī�忡 ��Ŷ �۽�
	BOOL			setND( char *dvName );		// dvName�� ��� ��Ŷ �ۼ����� ���� �غ�
	char*			getND();
	void			SetDestinAddress( unsigned char* pAddress );
	void			SetSourceAddress( unsigned char* pAddress );
	unsigned char*	GetDestinAddress( );
	unsigned char*	GetSourceAddress( ) ;

	CNILayer( char *pName);
	virtual ~CNILayer();

private:	
	char *deviceName;				// ��Ŷ �ۼ����� ���� ���� ����
	pcap_t *adhandle;				// ��Ŷ �ۼ����� ���� ����� �ڵ鷯
	unsigned char dstaddr[6];
	unsigned char srcaddr[6];
};


#endif // !defined(AFX_NILAYER_H__E47ABD41_C64D_434D_AFB4_6314D133FF0B__INCLUDED_)
