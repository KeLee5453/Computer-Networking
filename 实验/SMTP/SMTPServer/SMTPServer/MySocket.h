#pragma once

// MySocket 命令目标
#include <string>
using namespace std;
class MySocket : public CAsyncSocket
{
public:
	MySocket();
	virtual ~MySocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	unsigned long long m_length;//消息长度
	char m_content[524288];//消息内容
	int m_count;//记录进行到哪一步
	bool m_state;//标记，false：表示由Send打开，true标记由Receive()打开
//	CString text;
	CString m_text;//邮件正文
	CImage m_image;//图片附件
	CString m_filename;//附件名称
	void ShowText();
	void ShowImage();
//	string t_decode(string input);
//	char* t_decode(const char* input);
//	void i_decode(FILE* inputFile, FILE* outputFile);
};


