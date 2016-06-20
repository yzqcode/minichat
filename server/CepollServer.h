#ifndef CEPOLLSERVER_H
#define CEPOLLSERVER_H
#include "stdafx.h"

class CepollServer
{
public:
    ~CepollServer();
    static CepollServer* GetInstance()
    {
    	if(nullptr==m_server.get())
        {
    		m_server = auto_ptr<CepollServer>(new CepollServer);
        }
        return m_server.get();
    }
    bool InitServer(int port=SERVER_PORT);
    void Run();
    void addfd(int fd,char* p="listenfd");

private:
	static auto_ptr<CepollServer> m_server;
	CepollServer();
	CepollServer(const CepollServer&);
	CepollServer& operator=(const CepollServer&);
private:
    int m_listenfd;
    int m_efd;    
    sockaddr_in m_listenAddr;
    map<string,int>user_fd_map;
    map<int,string>fd_msg_map;
};






#endif // CEPOLLSERVER_H
