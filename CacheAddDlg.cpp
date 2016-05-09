// CacheAddDlg.cpp : implementation file

// ������� ����
#include "stdafx.h"
#include "ARP.h"
#include "CacheAddDlg.h"

#ifdef _DEBUGs
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// �⺻ Ŭ���� ������
CCacheAddDlg::CCacheAddDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CCacheAddDlg::IDD, pParent)
{
   m_etherAddr = _T("");
}

// �ܼ�â�� ���α׷� ������ ������ ��ȯ�� ���� �Լ�
void CCacheAddDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_IPADDRESS_IP_ADDR, m_IPAddr);
   DDX_Text(pDX, IDC_EDIT_ETHER_ADDR, m_etherAddr);
}

// Ư�� Message ó���� �ϴ� �Լ� ���̺��� �����ϴ� ��ũ��
BEGIN_MESSAGE_MAP(CCacheAddDlg, CDialog)
END_MESSAGE_MAP()

// Cache Table���� Ok��ư�� ������ �� ó���ϴ� �Լ�
void CCacheAddDlg::OnOK() 
{   
   UpdateData(true);

   // boolean������ ������ �����ν�, ����� �ԷµǾ��� �� Ȯ���� �� �ִ�.
   bool isRight = false;

   if(m_etherAddr.GetLength() == 17) 
   {
      int i = 0;

      for(i; i < 17; i++) 
      {
         char c = m_etherAddr.GetAt(i);
         
         /*
         Mac�ּ� �� 3�� ��� �ڸ��� ��ġ�� �ִ� ���ڴ� �׻� Ư�������̹Ƿ� �˻��� �ʿ䰡 ����.
         �� �̿��� �ڸ������� 16������ �°� ���ڳ� ���ڸ� �Է��ߴ� �� üũ�Ѵ�.
         */
         if( (i + 1) % 3 == 0 )
         {
            if( c != '-' )
               break;
         }
         else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) )
            break;
      }
      
      // ��ü Mac �ּ��� �ڸ� ���� üũ�Ѵ�.
      if(i == 17)
         isRight = true;
   }
   
   // Mac�ּҳ� IP�ּ� �� �Էµ��� ���� �׸��� �ִٸ� �޽��� â�� ���� �����Ѵ�.
   if(m_etherAddr.IsEmpty() || m_IPAddr.IsBlank()) 
   {
      AfxMessageBox( "�Էµ��� ���� �׸��� �ֽ��ϴ�." );
      return;
   }
   
   // ���� ���ǹ����� ���ǿ� �������� ������ boolean������ false�� �ȴ�. �� ��쿡�� �޽��� â�� ���� �����Ѵ�.
   else if( isRight == false ) 
   {
      AfxMessageBox( "�ּ������� �ùٸ��� �ʽ��ϴ�.( FORMAT : xx-xx-xx-xx-xx-xx )" );
      return;
   }
   
   m_IPAddr.GetAddress(m_buffer.buffer_IPAddr[0], m_buffer.buffer_IPAddr[1], m_buffer.buffer_IPAddr[2], m_buffer.buffer_IPAddr[3]);
   
   // ���� '-' ������ �߶� �Է¹��� �� �ֵ��� �����Ѵ�.
   for(int i = 0; i < 10; i++)
   {
      CString temp;
      unsigned char char_temp[10];
      
      AfxExtractSubString(temp, m_etherAddr, i, '-');
      sscanf(temp,"%x", &char_temp[i]);
      m_buffer.buffer_EtherAddr[i] = char_temp[i];
   }
   
   CDialog::OnOK();
}

// Cache Table���� �Է��� ���뿡 ���� getter �޼ҵ��̴�.
Cache_BufferAddr CCacheAddDlg::GetBufferAddr()
{
   return m_buffer;
}