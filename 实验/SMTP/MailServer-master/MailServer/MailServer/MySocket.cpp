// MySocket.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MailServer.h"
#include "MySocket.h"
#include "MailServerDlg.h"
#include <atlimage.h>
#include <regex>
#include <sstream>
#include <fstream>



using namespace std;

// MySocket

MySocket::MySocket()
	: m_nLength(0)  
	, m_count(0)
	, m_state(false)
	, m_content_string(_T(""))
	, m_filename(_T(""))
{
	memset(m_szBuffer,0,sizeof(m_szBuffer));
	m_content_string.Empty();
}

MySocket::~MySocket()
{
}


// MySocket ��Ա����

void MySocket::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CMailServerDlg* dlg = (CMailServerDlg*)theApp.m_pMainWnd; //��ȡ�Ի�����
	
	if(!m_state&&m_count==0){
			//�����׼����
		char buffff[100]={"220  Simple Mail Server Readly for Mail\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 220  Simple Mail Server Readly for Mail\r\n"));
		m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}

	if(!m_state&&m_count==1){
			//���ա���������������������ɹ����
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"250  OK 127.0.0.1\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 250  OK 127.0.0.1\r\n"));
		m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}

	if(!m_state&&m_count==2){
			//���ա������ߵ����ʼ���ַ������������ɹ����
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"250  Sender OK\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 250  Sender OK\r\n"));
		m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}

		if(!m_state&&m_count==3){
			//���ա������ߵ����ʼ���ַ������������ɹ����
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"250  Receiver OK\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 250  Receiver OK\r\n"));
			m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}
		
	
	if(!m_state&&m_count==4){
			//���Է����ʼ�����
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"354  Go ahead. End with <CRLF>.<CRLF>\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 354  Go ahead. End with <CRLF>.<CRLF>\r\n"));
			m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}

	if(!m_state&&m_count==5){
			//���ա������ߵ����ʼ���ַ������������ɹ����
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"250  Message accepted for delivery!\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 250  Message accepted for delivery!\r\n"));
			m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}

	if(!m_state&&m_count==6){
			//ϵͳ״̬��ϵͳ��æӦ��
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		char buffff[100]={"221 Quit, Goodbye!\r\n"};
		strcpy_s(m_szBuffer,buffff);
		Send(m_szBuffer,strlen(m_szBuffer)); 
		dlg->mc_server.AddString(_T("S: 221 Quit, Goodbye!\r\n"));
			m_state=true;
		AsyncSelect(FD_READ);	//����OnReceive����
	}


	CAsyncSocket::OnSend(nErrorCode);
}


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CMailServerDlg* dlg = (CMailServerDlg*)theApp.m_pMainWnd; //��ȡ�Ի�����

	if(m_state&&m_count!=4){   
			//��ʼ�Ự
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		m_nLength =Receive(m_szBuffer,sizeof(m_szBuffer),0); //��������
		CString rec(m_szBuffer);
		dlg->mc_server.AddString(_T("C: ")+rec);//��ʾ
		m_count++;
		m_state=false;
		AsyncSelect(FD_WRITE); //����OnSend����
	}

	if(m_state&&m_count==4){
			//����������ʼ���������
		memset(m_szBuffer,0,sizeof(m_szBuffer));
		m_nLength =Receive(m_szBuffer,sizeof(m_szBuffer),0); //��������
		dlg->mc_mail.AddString(_T("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"));

		//ת�����и�ʽ��ʾ��ListBox�ؼ���
		char content_buff[528192];
		int i_out=0,i_in;	
		for(int i=1;;i++){
			memset(content_buff,0,sizeof(content_buff));
			i_in=0;
			while(m_szBuffer[i_out]!='\n'&&m_szBuffer[i_out]!='\0'&&m_szBuffer[i_out]!=0){
				content_buff[i_in++]=m_szBuffer[i_out++];
			}
			i_out++;
			if(i_in>1)
				content_buff[i_in++]='\n';
			CString rec(content_buff,i_in);
			dlg->mc_mail.AddString(rec);//��ʾ
			m_content_string=m_content_string+rec;
			//�ԡ�\r\n.\r\n���������ݽ���
			if((m_szBuffer[i_out]=='.'&&m_szBuffer[i_out+1]=='\r'&&m_szBuffer[i_out+2]=='\n')){
				m_count++;
				m_state=false;
				//�ʼ����Ľ�����ʾ
				ShowData();
				//�ʼ�������ʾ
				ShowImage();
				break;
			}
			if(m_szBuffer[i_out]==0)
				break;
		};

		AsyncSelect(FD_WRITE); //����OnSend����
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


//��ʾ�ʼ�����
void MySocket::ShowData()		
{
	CMailServerDlg* dlg = (CMailServerDlg*)theApp.m_pMainWnd; //��ȡ�Ի�����
	CString TEXT;		//��Ҫ��ʾ����Ļ�ϵ�����
	TEXT.Empty();

	dlg->mc_text.ResetContent();	//����б��

	if(m_content_string.Find(_T("Content-Transfer-Encoding: 7bit"))!=-1){
		//������7bit���룬������룡
		int pos1=m_content_string.Find(_T("Content-Transfer-Encoding: 7bit"))+33;
		int pos2=m_content_string.Find(_T("------"),pos1+1);
		TEXT=m_content_string.Mid(pos1,pos2-pos1-2);
	}	
	else{
		if(m_content_string.Find(_T("Content-Transfer-Encoding: base64"))!=-1){
				//������base64���룬���н��룡
			CString  mailContent;	//������ĵ�base64����
			m_content_string.Remove('\r');
			m_content_string.Remove('\n');
			int pos1=m_content_string.Find(_T("Content-Transfer-Encoding: base64"))+33;
			int pos2=m_content_string.Find(_T("------"),pos1+1);
			mailContent=m_content_string.Mid(pos1,pos2-pos1);
			if(mailContent.IsEmpty())
				dlg->mc_text.AddString(L"�ʼ����Ľ�������");
			//ɾ��'\r\n';
			int mailContent_size;
			TEXT=Text_DecodeBase64(mailContent,&mailContent_size);
		}
	}

	if(!TEXT.IsEmpty()){
	//��ʾ����Ļ��
		int pos_begin=0,pos_end=TEXT.Find('\r',1);
		while(pos_end!=-1)
		{
			CString temp=TEXT.Mid(pos_begin ,pos_end - pos_begin );
			dlg->mc_text.AddString(temp);
			pos_begin=pos_end+4;
			pos_end=TEXT.Find('\r',pos_begin+1);
		}
	}
}

//��ʾͼƬ
void MySocket::ShowImage(void)
{
	CMailServerDlg* dlg = (CMailServerDlg*)theApp.m_pMainWnd; //��ȡ�Ի�����
	CString  ImageContent;	//�������base64����
	m_content_string.Remove('\r');
	m_content_string.Remove('\n');
	int pos1=m_content_string.Find(_T("filename"));
	if(pos1==-1)	//���û�и����򷵻�
		return;
	int pos2=m_content_string.Find('"',pos1+7);
	int pos3=m_content_string.Find('"',pos2+1);
	m_filename=m_content_string.Mid(pos2+1,pos3-pos2-1);	//��ȡ��������	
	dlg->m_filename=m_filename;
	dlg->UpdateData(false);									//��ʾ���������ڿؼ���
	int pos4=m_content_string.Find(_T("------="),pos3+1);
	ImageContent=m_content_string.Mid(pos3+1,pos4-pos3-1);

	std::vector<char> bytes;
	Image_DecodeBase64(ImageContent, bytes);
	std::fstream of(m_filename, std::ios_base::out | std::ios_base::binary);;
	of.write(static_cast<const char*>(&bytes[0]), bytes.size());
	of.close();
	

	//��ʾ����ͼƬ
	if(!m_image.IsNull()) {  //ȷ��λͼ��ǰ�Ƿ����ڼ���
		m_image.Detach();
	}

	m_image.Load(m_filename);
	if (!m_image.IsNull()) {

		SetStretchBltMode(dlg->mc_picture.GetDC()->GetSafeHdc(), HALFTONE);
		CRect dest;
		dlg->mc_picture.GetClientRect(&dest);

		float nRatioImage = m_image.GetWidth() / static_cast<float>(m_image.GetHeight());
		float nRatioDest = dest.Width() / static_cast<float>(dest.Height());
		CRect rectDraw = dest;
		if (nRatioImage > nRatioDest) {    //�����ڿռ������ʾ
			rectDraw.SetRect((dest.Width()/2)-(rectDraw.right/2),(dest.Height()/2)-((rectDraw.right / nRatioImage)/2),(dest.Width()/2)+(rectDraw.right/2),(dest.Height()/2)+((rectDraw.right / nRatioImage)/2) );
		}
		else if (nRatioImage < nRatioDest) {
			rectDraw.SetRect((dest.Width()/2)-((rectDraw.bottom * nRatioImage)/2), (dest.Height()/2)-( rectDraw.bottom/2), (dest.Width()/2)+((rectDraw.bottom * nRatioImage)/2),(dest.Height()/2)+( rectDraw.bottom/2));
		}
		m_image.Draw(dlg->mc_picture.GetDC()->GetSafeHdc(), rectDraw);
	}

	
}


//����base64���뺯��
CString MySocket::Text_DecodeBase64(CString inpt,int * len)	//����base64���뺯��
{
	unsigned char * base64=(unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 

	int n,i,j,pad;  
    unsigned char *p;  
    static unsigned char *dst;  
    *len=0;  
     pad=0;  
     n=inpt.GetLength();  
	CString src;
	src.GetBufferSetLength(n);

     for(i=0;i<n;i++)  
		 src.SetAt(i,inpt[i]);
   
     while(n>0&&src[n-1]=='=') {  
         src.SetAt(n-1,0);
         pad++;  
         n--;  
     }  
     for(i=0;i<n;i++)   {
         p=(unsigned char *)strchr((const char *)base64,(int)src[i]);  
         if(!p)  
              break;  
         src.SetAt(i,p-(unsigned char *)base64);
     }  

		dst=(unsigned char *)malloc(n*3/4+1);  
		for(i=0,j=0;i<n;i+=4,j+=3) {  
         dst[j]=(src[i]<<2) + ((src[i+1]&0x30)>>4);  
         dst[j+1]=((src[i+1]&0x0F)<<4) + ((src[i+2]&0x3C)>>2);  
         dst[j+2]=((src[i+2]&0x03)<<6) + src[i+3];  

         *len+=3;  
     }  
     *len-=pad;  
    return CString(dst);  
}


//ͼƬbase64���뺯��
void MySocket::Image_DecodeBase64(CString inpt, std::vector<char>& bytes) {
	unsigned char * base64=(unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
	int n,i,j,pad;  
    unsigned char *p;  
     pad=0;  
     n=inpt.GetLength();  
	CString src;
	src.GetBufferSetLength(n);

     for(i=0;i<n;i++)  
		 src.SetAt(i,inpt[i]);
   
     while(n>0&&src[n-1]=='=') {  
         src.SetAt(n-1,0);
         pad++;  
         n--;  
     }  
     for(i=0;i<n;i++)   {
         p=(unsigned char *)strchr((const char *)base64,(int)src[i]);  
         if(!p)  
              break;  
         src.SetAt(i,p-(unsigned char *)base64);
     }   
		for(i=0,j=0;i<n;i+=4,j+=3) {  //�ֽ���
		byte threeByte[3];
		threeByte[0]=(src[i]<<2) + ((src[i+1]&0x30)>>4);
		threeByte[1]=((src[i+1]&0x0F)<<4) + ((src[i+2]&0x3C)>>2);
		threeByte[2]=((src[i+2]&0x03)<<6) + src[i+3];
		for(auto j = 0; j < 3; j++) {
				bytes.push_back(threeByte[j]);
			}
     } 
    return;
}