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
};		// Cache list 자료형

typedef struct PROXY_LIST *LPPROXY_LIST;
struct PROXY_LIST {
	PROXY_ENTRY		proxy_entry;
	LPPROXY_LIST	next;
};		// Proxy ARP 자료형

class CARPLayer : public CBaseLayer  
{
private:
	inline void		ResetHeader();			// 불변의 ARP 헤더부분 초기화
	CObject* mp_Dlg;
	
public: 
	static UINT		TTLThread(LPVOID pParam);							// 저장목록의 일정시간 지연후 삭제를 위한 스레드 
	BOOL			Receive( unsigned char* ppayload, int type = 0 );
	BOOL			Send( unsigned char* ppayload, int nlength, int type = 0);
	BOOL			SeekAddr();										// Cache에 없을시 주소찾기 함수
	void			SetSeekAddrMsg(unsigned char msg);				// 주소검색 상황에 따른 메시지 전달
	unsigned char	GetSeekAddrMsg();								// 주소검색 상황 메시지 반환
	
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
	
	bool IsEqualAddr(unsigned char* array1, unsigned char* array2, int cnt);	// BYTE(unsigned char)형의 배열비교 함수
	
	typedef struct _ARP_HEADER {
		unsigned short	arp_hdtype;				// 물리적 주소의 타입
		unsigned short	arp_prototype;			// 논리적 주소의 타입
		unsigned char	arp_hdlength;			// 하드웨어 주소 길이
		unsigned char	arp_protolength;		// 프로토콜 주소의 길이
		unsigned short	arp_op;					// ARP 프로토콜의 여러 동작 중에서 어떤 것에 해당하는지 명시하기 위한 항목
		unsigned char	arp_srchdaddr[6];		// 송신자의 하드웨어 주소 저장하기 위한 항목
		unsigned char	arp_srcprotoaddr[4];	// 송신자의 IP 주소 저장하기 위한 항목
		unsigned char	arp_desthdaddr[6];		// 수신자의 하드웨어 주소 저장하기 위한 항목
		unsigned char	arp_destprotoaddr[4];	// 수신자의 IP 주소 저장하기 위한 항목
	} _ARP_HEADER, *PARP_HEADER;
	
	// 캐쉬 타입
	enum {		
			CACHE_TYPE_INVALID = 0x00,		// Invalid
			CACHE_TYPE_DYNAMIC = 0x01,		// Dynamic
			CACHE_TYPE_STATIC  = 0x02 } ;	// Static
		
protected:
	_ARP_HEADER m_sHeader;
	LPCACHE_LIST cache_header;			// Cache에 저장목록
	LPPROXY_LIST proxy_header;			// Proxy ARP 저장목록
	unsigned char	seekAddrMsg;		// SeekAddr함수 사용중 전달되는 주소검색 실패/성공 여부
	CWinThread* TTLThread_;
};

#endif // !defined(AFX_ARPLAYER_H__C2FE2813_2B95_4D9F_9249_02905FA8F43A__INCLUDED_)
