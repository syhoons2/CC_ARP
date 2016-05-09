// CacheAddDlg.cpp : implementation file

// 헤더파일 참조
#include "stdafx.h"
#include "ARP.h"
#include "CacheAddDlg.h"

#ifdef _DEBUGs
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 기본 클래스 생성자
CCacheAddDlg::CCacheAddDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CCacheAddDlg::IDD, pParent)
{
   m_etherAddr = _T("");
}

// 콘솔창과 프로그램 사이의 데이터 교환을 위한 함수
void CCacheAddDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_IPADDRESS_IP_ADDR, m_IPAddr);
   DDX_Text(pDX, IDC_EDIT_ETHER_ADDR, m_etherAddr);
}

// 특정 Message 처리를 하는 함수 테이블을 정의하는 매크로
BEGIN_MESSAGE_MAP(CCacheAddDlg, CDialog)
END_MESSAGE_MAP()

// Cache Table에서 Ok버튼을 눌렀을 때 처리하는 함수
void CCacheAddDlg::OnOK() 
{   
   UpdateData(true);

   // boolean형으로 변수를 둠으로써, 제대로 입력되었는 지 확인할 수 있다.
   bool isRight = false;

   if(m_etherAddr.GetLength() == 17) 
   {
      int i = 0;

      for(i; i < 17; i++) 
      {
         char c = m_etherAddr.GetAt(i);
         
         /*
         Mac주소 중 3의 배수 자리에 위치해 있는 문자는 항상 특수문자이므로 검사할 필요가 없다.
         그 이외의 자리에서는 16진수에 맞게 숫자나 문자를 입력했는 지 체크한다.
         */
         if( (i + 1) % 3 == 0 )
         {
            if( c != '-' )
               break;
         }
         else if( !( ( c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9') ) )
            break;
      }
      
      // 전체 Mac 주소의 자릿 수를 체크한다.
      if(i == 17)
         isRight = true;
   }
   
   // Mac주소나 IP주소 중 입력되지 않은 항목이 있다면 메시지 창을 띄우고 리턴한다.
   if(m_etherAddr.IsEmpty() || m_IPAddr.IsBlank()) 
   {
      AfxMessageBox( "입력되지 않은 항목이 있습니다." );
      return;
   }
   
   // 위의 조건문에서 조건에 부합하지 않으면 boolean변수가 false가 된다. 이 경우에도 메시지 창을 띄우고 리턴한다.
   else if( isRight == false ) 
   {
      AfxMessageBox( "주소형식이 올바르지 않습니다.( FORMAT : xx-xx-xx-xx-xx-xx )" );
      return;
   }
   
   m_IPAddr.GetAddress(m_buffer.buffer_IPAddr[0], m_buffer.buffer_IPAddr[1], m_buffer.buffer_IPAddr[2], m_buffer.buffer_IPAddr[3]);
   
   // 문자 '-' 단위로 잘라서 입력받을 수 있도록 설정한다.
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

// Cache Table에서 입력한 내용에 대한 getter 메소드이다.
Cache_BufferAddr CCacheAddDlg::GetBufferAddr()
{
   return m_buffer;
}