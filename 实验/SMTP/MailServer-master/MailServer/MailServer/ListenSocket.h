#pragma once
#include "MySocket.h"
// ListenSocket ����Ŀ��

class ListenSocket : public CAsyncSocket
{
public:
	ListenSocket();
	virtual ~ListenSocket();

	MySocket *m_pSocket; //ָ��һ�����ӵ�socket����
	virtual void OnAccept(int nErrorCode);
};


