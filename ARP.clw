; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CARPDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ARP.h"

ClassCount=5
Class1=CARPApp
Class2=CARPDlg
Class3=CAboutDlg

ResourceCount=5
Resource1=IDD_ARP_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_CACHE_ADD_DIALOG
Class4=CCacheAddDlg
Resource4=IDD_ABOUTBOX
Class5=CProxyAddDlg
Resource5=IDD_PROXY_ADD_DIALOG

[CLS:CARPApp]
Type=0
HeaderFile=ARP.h
ImplementationFile=ARP.cpp
Filter=N

[CLS:CARPDlg]
Type=0
HeaderFile=ARPDlg.h
ImplementationFile=ARPDlg.cpp
Filter=D
LastObject=IDC_BUTTON_CACHE_ALL_DELETE
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ARPDlg.h
ImplementationFile=ARPDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352

[DLG:IDD_ARP_DIALOG]
Type=1
Class=CARPDlg
ControlCount=33
Control1=IDC_STATIC,button,1342177287
Control2=IDC_LIST_CACHE_TABLE,listbox,1350635777
Control3=IDC_BUTTON_CACHE_DELETE,button,1342242816
Control4=IDC_BUTTON_CACHE_ALL_DELETE,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_IPADDRESS_IP_ADDR,SysIPAddress32,1342242816
Control7=IDC_BUTTON_IP_SEND,button,1342242816
Control8=IDC_STATIC,button,1342177287
Control9=IDC_IPADDRESS_SRC_IP_ADDR,SysIPAddress32,1342242816
Control10=IDC_COMBO_ND,combobox,1344340226
Control11=IDC_BUTTON_SET,button,1342242816
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_EDIT_SRC_ETHER_ADDR,edit,1350631553
Control16=IDC_STATIC,button,1342177287
Control17=IDC_EDIT_DST_ETHER_ADDR,edit,1350631553
Control18=IDC_BUTTON_ETHER_SEND,button,1342242816
Control19=IDC_STATIC,static,1342308352
Control20=IDC_IPADDRESS_DST_IP_ADDR,SysIPAddress32,1342242816
Control21=IDC_BUTTON_CACHE_ADD,button,1342242816
Control22=IDC_STATIC,button,1342177287
Control23=IDC_LIST_PROXY_TABLE,listbox,1350635777
Control24=IDC_BUTTON_PROXY_DELETE,button,1342242816
Control25=IDC_BUTTON_PROXY_ADD,button,1342242816
Control26=IDC_STATIC,button,1342177287
Control27=IDC_EDIT_CHAT_AREA,edit,1352728772
Control28=IDC_EDIT_CHAT_FIELD,edit,1350631552
Control29=IDC_BUTTON_CHAT_SEND,button,1342242816
Control30=IDC_STATIC,static,1342308352
Control31=IDC_BUTTON_PROXY_ALL_DELETE,button,1342242816
Control32=IDC_STATIC,button,1342177287
Control33=IDC_STATIC,static,1342308352

[DLG:IDD_CACHE_ADD_DIALOG]
Type=1
Class=CCacheAddDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT_ETHER_ADDR,edit,1350631553
Control6=IDC_IPADDRESS_IP_ADDR,SysIPAddress32,1342242816

[CLS:CCacheAddDlg]
Type=0
HeaderFile=CacheAddDlg.h
ImplementationFile=CacheAddDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

[DLG:IDD_PROXY_ADD_DIALOG]
Type=1
Class=CProxyAddDlg
ControlCount=8
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_ETHER_ADDR,edit,1350631553
Control7=IDC_IPADDRESS_IP_ADDR,SysIPAddress32,1342242816
Control8=IDC_COMBO_DEVICE,combobox,1344340226

[CLS:CProxyAddDlg]
Type=0
HeaderFile=ProxyAddDlg.h
ImplementationFile=ProxyAddDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CProxyAddDlg

