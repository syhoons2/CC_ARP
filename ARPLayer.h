// ARPLayer.h: interface for the CARPLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARPLAYER_H__C2FE2813_2B95_4D9F_9249_02905FA8F43A__INCLUDED_)
#define AFX_ARPLAYER_H__C2FE2813_2B95_4D9F_9249_02905FA8F43A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseLayer.h"
#include "EthernetLayer.h"

typedef struct _CACHE_ENTRY {
	unsigned char	cache_ipaddr[4];
	unsigned char	cache_enetaddr[6];
	unsigned char	cache_type;
	unsigned short	cache_ttl;		// time to live
} CACHE_ENTRY, *LPCACHE_ENTRY;

typedef struct _PROXY_ENTRY {
	CString*		proxy_device;
	unsigned char	proxy_ipaddr[4];
	unsigned char	proxy_enetaddr[6];
} PROXY_ENTRY, *LPPROXY_ENTRY;

typedef struct CACHE_LIST *LPCACHE_LIST;
struct CACHE_LIST {
	CACHE_ENTRY		cache_entry;
	LPCACHE_LIST	next;
};		// Cache list �ڷ���

typedef struct PROXY_LIST *LPPROXY_LIST;
struct PROXY_LIST {
	PROXY_ENTRY		proxy_entry;
	LPPROXY_LIST	next;
};		// Proxy ARP �ڷ���

class CARPLayer : public CBaseLayer  
{
private:
	inline void		ResetHeader();			// �Һ��� ARP ����κ� �ʱ�ȭ
	CObject* mp_Dlg;
	
public: 
	static UINT		TTLThread(LPVOID pParam);							// �������� �����ð� ������ ������ ���� ������ 
	BOOL			Receive( unsigned char* ppayload, int type = 0 );
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0);
	BOOL			SeekAddr();										// Cache�� ������ �ּ�ã�� �Լ�
	void			SetSeekAddrMsg(unsigned char msg);				// �ּҰ˻� ��Ȳ�� ���� �޽��� ����
	unsigned char	GetSeekAddrMsg();								// �ּҰ˻� ��Ȳ �޽��� ��ȯ
	
	CARPLayer( char *pName );
	virtual ~CARPLayer();
	
	void SetSourceHdAddress(unsigned char *src_addr);	
	void SetSourceProtoAddress(unsigned char *src_addr);
	void SetDestinHdAddress(unsigned char *dst_addr);
	void SetDestinProtoAddress(unsigned char *dst_addr);
	unsigned char* GetSourceHdAddress();	
	unsigned char* GetSourceProtoAddress();	
	unsigned char* GetDestinHdAddress();
	unsigned char* GetDestinProtoAddress();
	
	LPCACHE_LIST GetCacheList();
	LPPROXY_LIST GetProxyList();
	void AddCache(LPCACHE_LIST cache_element);
	void AddProxy(LPPROXY_LIST proxy_element);
	BOOL DeleteCache(unsigned char* ipaddr);
	BOOL DeleteProxy(unsigned char* ipaddr);
	void AllDeleteCache(); 
	void AllDeleteProxy();
	
	bool IsEqualAddr(unsigned char* array1, unsigned char* array2, int cnt);	// BYTE(unsigned char)���� �迭�� �Լ�
	
	typedef struct _ARP_HEADER {
		unsigned short	arp_hdtype;				// ������ �ּ��� Ÿ��
		unsigned short	arp_prototype;			// ���� �ּ��� Ÿ��
		unsigned char	arp_hdlength;			// �ϵ���� �ּ� ����
		unsigned char	arp_protolength;		// �������� �ּ��� ����
		unsigned short	arp_op;					// ARP ���������� ���� ���� �߿��� � �Ϳ� �ش��ϴ��� ����ϱ� ���� �׸�
		unsigned char	arp_srchdaddr[6];		// �۽����� �ϵ���� �ּ� �����ϱ� ���� �׸�
		unsigned char	arp_srcprotoaddr[4];	// �۽����� IP �ּ� �����ϱ� ���� �׸�
		unsigned char	arp_desthdaddr[6];		// �������� �ϵ���� �ּ� �����ϱ� ���� �׸�
		unsigned char	arp_destprotoaddr[4];	// �������� IP �ּ� �����ϱ� ���� �׸�
	} _ARP_HEADER, *PARP_HEADER;
	
	// ĳ�� Ÿ��
	enum {		
			CACHE_TYPE_INVALID = 0x00,		// Invalid
			CACHE_TYPE_DYNAMIC = 0x01,		// Dynamic
			CACHE_TYPE_STATIC  = 0x02 } ;	// Static
		
protected:
	_ARP_HEADER m_sHeader;
	LPCACHE_LIST cache_header;			// Cache�� ������
	LPPROXY_LIST proxy_header;			// Proxy ARP ������
	unsigned char	seekAddrMsg;		// SeekAddr�Լ� ����� ���޵Ǵ� �ּҰ˻� ����/���� ����
	CWinThread* TTLThread_;
};

#endif // !defined(AFX_ARPLAYER_H__C2FE2813_2B95_4D9F_9249_02905FA8F43A__INCLUDED_)
