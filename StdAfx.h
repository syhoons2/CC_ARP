// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F716C1E9_21E7_4181_BF15_B4BC84728C65__INCLUDED_)
#define AFX_STDAFX_H__F716C1E9_21E7_4181_BF15_B4BC84728C65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define HAVE_REMOTE

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include "pcap.h"
#include "remote-ext.h"
#include "process.h"
#include "Iphlpapi.h"
#pragma comment(lib, "Iphlpapi.lib")

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
#define MAX_LAYER_NUMBER	0xff
#define MAX_DEVICE			16

#define ETHER_HEADER		14
#define IP_HEADER			20
#define TCP_HEADER			20
#define ARP_HEADER			28

#define TCP_DATA			(1488 - TCP_HEADER - IP_HEADER)
#define IP_DATA				(1488 - IP_HEADER)
#define ETHER_DATA			1488		// 1488인 이유 : 네트워크 카드로 전송가능한 최대 패킷크기가 1514byte이므로

// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F716C1E9_21E7_4181_BF15_B4BC84728C65__INCLUDED_)
