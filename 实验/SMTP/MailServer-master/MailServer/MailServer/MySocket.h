#pragma once
#include <string>
#include <atlimage.h>
#include <vector>
// MySocket ����Ŀ��



class MySocket : public CAsyncSocket
{
public:
	MySocket();
	virtual ~MySocket();
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);


	 
    UINT m_nLength;			// ��Ϣ���� 
	char m_szBuffer[524288];//��Ϣ����
	int m_count;			// ���ڼ���,ͬ��Send()��Receive()����ʾ��ǰִ�е��ڼ��i
	bool  m_state;			//���״̬��false����ʾ��Send�򿪣�true�����Receive()��
	CString Text_DecodeBase64(CString inpt,int * len);	//base64���뺯��
	void ShowData();		//��ʾ�ʼ�����
	CString m_content_string;
	CImage m_image;
	void ShowImage(void);
	CString m_filename;			//��������
	void MySocket::Image_DecodeBase64(CString dataFromPicStart, std::vector<char>& bytes);
};

