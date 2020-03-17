// MySocket.cpp: 实现文件
//

#include "pch.h"
#include "SMTPServer.h"
#include "MySocket.h"
#include"SMTPServerDlg.h"
#include<string>
#include <atlimage.h>
#include <regex>
#include <sstream>
#include <fstream>
#include"Bash64.h"

// MySocket

MySocket::MySocket()
{
	m_length = 0;
	m_count = 0;
	m_text = _T("");
	m_filename =_T("");
	memset(m_content, 0, sizeof(m_content));
	m_text.Empty();
}

MySocket::~MySocket()
{
}


// MySocket 成员函数


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CSMTPServerDlg* dlg = (CSMTPServerDlg*)theApp.m_pMainWnd;
	if (m_state && (m_count != 4))//开始会话
	{
		memset(m_content, 0, sizeof(m_content));
		m_length = Receive(m_content,sizeof(m_content));//接收数据
		CString cstr(m_content);
		
		dlg->m_log.AddString(_T("C: ")+cstr);//显示数据
		m_count++;
		m_state = false;
		AsyncSelect(FD_WRITE);//调用OnSend函数
	}
	if (m_state && (m_count == 4))//在这里接收邮件内容数据
	{
		memset(m_content, 0, sizeof(m_content));
		m_length = Receive(m_content, sizeof(m_content));//接收数据
		dlg->mc_content.AddString(_T("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));
		//转换换行格式显示在ListBox控件中
		char con_buf[sizeof(m_content)];
		int i_out = 0, i_in = 0;
		for (int i = 0;; i++)
		{
			memset(con_buf, 0, sizeof(con_buf));
			i_in = 0;
			while (m_content[i_out] != '\n' && m_content[i_out] != '\0' && m_content[i_out] != 0)
			{
				con_buf[i_in++] = m_content[i_out++];

			}
			i_out++;
			if (i_in > 1) con_buf[i_in++] = '\n';
			CString rec(con_buf, i_in);
			dlg->mc_content.AddString(rec);
			m_text = m_text + rec;
			//以“\r\n.\r\n”结束数据接收
			if ((m_content[i_out] == '.' && m_content[i_out + 1] == '\r' && m_content[i_out + 2] == '\n'))
			{
				m_count++;
				m_state = false;
				ShowText();
				ShowImage();
				break;
			}
			if (m_content[i_out] == 0) break;
		}
		AsyncSelect(FD_WRITE);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}


void MySocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CSMTPServerDlg* dlg = (CSMTPServerDlg*)theApp.m_pMainWnd;//获取对话框句柄
	if (!m_state && (m_count == 0))//服务已经准备好
	{
		char buf[100] = "220  Mail Server Is Ready For Mail\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S: 220 Mail Server Is Ready For Mail\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 1))//接收“主机域名”的请求命令成功完成
	{
		char buf[100] = "250  OK 127.0.0.1\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:250  OK 127.0.0.1\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 2))//接收“发送者电子邮件地址”的请求命令成功完成
	{
		char buf[100] = "250  Sender OK\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:250  Sender OK\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 3))//接收“接收者电子邮件地址”的请求命令成功完成
	{
		char buf[100] = "250  Receiver OK\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:250  Receiver OK\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 4))//可以发送邮件内容
	{
		char buf[100] = "354  Go ahead. End with <CRLF>.<CRLF>\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:354  Go ahead. End with <CRLF>.<CRLF>\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 5))//接收“接收者电子邮件地址”的请求命令成功完成
	{
		char buf[100] = "250  Message accepted for delivery!\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:250  Message accepted for delivery!\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 6))//系统状态、系统帮忙应答
	{
		char buf[100] = "220 Mail Server Is Ready For Mail\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:220 Mail Server Is Ready For Mail\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	if (!m_state && (m_count == 6))//会话结束
	{
		char buf[100] = "221 Quit, Goodbye!\r\n";
		strcpy_s(m_content,buf);
		Send(m_content , strlen(m_content));
		dlg->m_log.AddString(_T("S:221 Quit, Goodbye!\r\n"));
		m_state = true;
		AsyncSelect(FD_READ);//触发OnReceive函数
	}
	CAsyncSocket::OnSend(nErrorCode);
}


void MySocket::ShowText()
{
	CSMTPServerDlg* dlg = (CSMTPServerDlg*)theApp.m_pMainWnd;//获取对话框句柄
	CString TEXT;
	CString SUBJECT;
	TEXT.Empty();
	dlg->mc_text.ResetContent();//清空列表框
	if (m_text.Find(_T("Content-Transfer-Encoding: 7bit")) != -1)
		//正文是7bit编码，无需解码
	{
		int pos1 = m_text.Find(_T("Content-Transfer-Encoding: 7bit")) + 33;
		int pos2 = m_text.Find(_T("-----") + pos1 + 1);
		TEXT = m_text.Mid(pos1, pos2 - pos1 - 2);
	}
	else
	{
		if (m_text.Find(_T("Content-Transfer-Encoding: base64")) != -1)
		{
			//如果是base64编码

			CString mailcontent;
			m_text.Remove('\r');
			m_text.Remove('\n');
			int pos1 = m_text.Find(_T("Content-Transfer-Encoding: base64")) + 33;
			int pos2 = m_text.Find(_T("------") , pos1 + 1);
			mailcontent = m_text.Mid(pos1, pos2 - pos1);
			if (mailcontent.IsEmpty())
			{
				dlg->mc_text.AddString(L"邮件正文解析错误！");
			}

			string ccc(CW2A(mailcontent.GetString()));
			string rrr;
			rrr=base64_decode(ccc);
			TEXT = rrr.c_str();
		
		}
		if (m_text.Find(_T("Subject: =?GB2312?B?")) != -1)
		{
			CString subject;
			m_text.Remove('\r');
			m_text.Remove('\n');
			int pos1 = m_text.Find(_T("Subject: =?GB2312?B?")) + 20;
			int pos2 = m_text.Find(_T("?="), pos1 + 1);
			subject = m_text.Mid(pos1, pos2 - pos1);
			string ccc = (CW2A(subject.GetString()));
			string rrr;
			rrr = base64_decode(ccc);
			SUBJECT = rrr.c_str();
			dlg->mc_subject.AddString(SUBJECT);
		}
		if (m_text.Find(_T("Subject: ")) != -1&&(m_text.Find(_T("Subject: =?GB2312?B?")) == -1))
		{
			CString subject;
			m_text.Remove('\r');
			m_text.Remove('\n');
			int pos1 = m_text.Find(_T("Subject: ")) + 9;
			int pos2 = m_text.Find(_T("X-Priority: "), pos1 + 1);
			subject = m_text.Mid(pos1, pos2 - pos1);
			dlg->mc_subject.AddString(subject);
		}

		if (!TEXT.IsEmpty())
		{
			int pos_begin = 0, pos_end = TEXT.Find('\r', 1);
			while (pos_end != -1)
			{
				CString temp;
				temp= TEXT.Mid(pos_begin, pos_end - pos_begin);
				dlg->mc_text.AddString(temp);
				pos_begin = pos_end + 4;
				pos_end = TEXT.Find('\r', pos_begin + 1);
			}

		}
	}

	// TODO: 在此处添加实现代码.
}


void MySocket::ShowImage()
{
	// TODO: 在此处添加实现代码.
	CSMTPServerDlg* dlg = (CSMTPServerDlg*)theApp.m_pMainWnd;
	CString  ImageContent;	//存放正文base64编码
	m_text.Remove('\r');
	m_text.Remove('\n');
	int pos1 = m_text.Find(_T("filename"));
	if (pos1 == -1)	//如果没有附件则返回
		return;
	int pos2 = m_text.Find('"', pos1 + 7);
	int pos3 = m_text.Find('"', pos2 + 1);
	m_filename = m_text.Mid(pos2 + 1, pos3 - pos2 - 1);	//获取附件名称	
	dlg->mc_filename = m_filename;
	dlg->UpdateData(false);									//显示附件名称在控件中
	int pos4 = m_text.Find(_T("------="), pos3 + 1);
	ImageContent = m_text.Mid(pos3 + 1, pos4 - pos3 - 1);
	string iii(CW2A(ImageContent.GetString()));

	string ima = base64_decode(iii);
	fstream of(m_filename, ios_base::out | ios_base::binary);
	of << ima << endl;
	of.close();

	if (!m_image.IsNull())
	{
		m_image.Detach();
	}
	
	m_image.Load(m_filename);

	if (!m_image.IsNull())
	{
		int height, width;
		height = m_image.GetHeight();
		width = m_image.GetWidth();
		
		CRect rect;
		CRect rect1;
		dlg->m_picture.GetClientRect(&rect);
		CDC* pDc = dlg->m_picture.GetDC();
		SetStretchBltMode(pDc->m_hDC, STRETCH_HALFTONE);
		if (width <= rect.Width() && height <= rect.Width()) //小图片，不缩放
		{
			rect1 = CRect(rect.TopLeft(), CSize(width, height));
			m_image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
			return ;
		}
		else
		{
			float xScale = (float)rect.Width() / (float)width;
			float yScale = (float)rect.Height() / (float)height;
			float ScaleIndex = (xScale >= yScale?xScale: yScale);
			rect1 = CRect(rect.TopLeft(), CSize((int)width * ScaleIndex, (int)height * ScaleIndex));
			m_image.StretchBlt(pDc->m_hDC, rect1, SRCCOPY); //将图片画到Picture控件表示的矩形区域
		}
	
			}
}






