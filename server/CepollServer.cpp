#include "CepollServer.h"

auto_ptr<CepollServer>CepollServer::m_server;




CepollServer::CepollServer() {
}

CepollServer::~CepollServer() {
}

bool CepollServer::InitServer(int port)
{
    m_listenAddr.sin_family = AF_INET;
    m_listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_listenAddr.sin_port   = htons(port);

    //create listen socket;
    m_listenfd = socket(AF_INET,SOCK_STREAM,0);
    if(m_listenfd<0)
    {
        perror("create socket failed!");
        return false;
    }

    //支持端口复用
    int iReuseaddr_on = 1;
    setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&iReuseaddr_on,sizeof(iReuseaddr_on) );

    //bind
    if(0!=bind(m_listenfd,(sockaddr*)&m_listenAddr,sizeof(m_listenAddr)))
    {
        perror("bind failed!");
        return false;
    }

    //listen
    if(0!=listen(m_listenfd,10))
    {
        perror("listen failed!");
        return false;
    }

    //create epoll
    m_efd = epoll_create(EPOLL_SIZE);

    //set no block
    if(0>fcntl(m_efd,F_SETFL,O_NONBLOCK))
    {
        perror("set no block failed!");
        return false;
    }
    //add listen socket to epoll
    addfd(m_listenfd);
    return true;
}
void CepollServer::addfd(int fd, char *p)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(m_efd,EPOLL_CTL_ADD,fd,&ev);
    //set no block
    //fcntl(fd,F_SETFL,fcntl(fd,F_GETFD,0)|O_NONBLOCK);
    fcntl(fd,F_SETFL,O_NONBLOCK);

    cout<<p<<" add to epoll successful!"<<endl;

}


void CepollServer::Run()
{
    //
    static struct epoll_event event,events[EPOLL_SIZE];
    while(true)
    {
        int epoll_events_count = epoll_wait(m_efd,events,EPOLL_SIZE,-1);
        for(int i=0;i<epoll_events_count;i++)
        {
            int userfd = events[i].data.fd;
            if(userfd==m_listenfd)//new user
            {
                sockaddr_in userAddr;
                socklen_t userAddr_len = sizeof(userAddr);
                int newuserfd = accept(m_listenfd,(sockaddr*)&userAddr,&userAddr_len);
                printf("client connection from: %s:% d userfd = %d \n",
                                inet_ntoa(userAddr.sin_addr),
                                ntohs(userAddr.sin_port),
                                newuserfd);
                char name[255]={0};
                read(newuserfd,name,sizeof(name)-1);
                user_fd_map.insert(make_pair(name,newuserfd));
                addfd(newuserfd,name);

            }
            else if(events[i].events&EPOLLIN)
            {
                char buf[1024]={0};
                string strbuf;
                int counter=0;
                while(true)
                {
                    counter = read(userfd,buf,sizeof(buf));
                    if(counter<0)
                        break;
                    strbuf.append(buf,counter);
                    memset(buf,0,sizeof(buf));

                    if(counter<sizeof(buf))
                    {
                        break;
                    }
                }
                if(counter==-1)
                {
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                    {
                        continue;//buf is empty
                    }
                    else
                    {
                        perror("read failed!");
                    }
                }
                if(counter==0)
                {
                    cout<<"connect is disconnecting!"<<endl;
                    close(userfd);
                    epoll_ctl(m_efd,EPOLL_CTL_DEL,userfd,&events[i]);
                    continue;
                }
                //cout<<"receive buf: "<<strbuf<<endl;
                auto fromfd = strbuf.find_last_of("-");
                auto tofd = strbuf.find_first_of("-");
                auto iter =user_fd_map.find(strbuf.substr(0,tofd));

                //cout<<tofd->second<<endl;
                if(iter!=user_fd_map.end())
                {
                    //string s = strbuf.substr(tofd+1,fromfd-1);
                    strbuf = strbuf.substr(fromfd+1)+"--say-you->: "+strbuf.substr(tofd+1,fromfd-tofd-1);
                    fd_msg_map.insert(make_pair(iter->second,strbuf));
                    event.data.fd=iter->second;
                    event.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(m_efd,EPOLL_CTL_MOD,event.data.fd,&event);
                }
                else//unknown user
                {
                    //TODO:
                    //strbuf clear;
                    strbuf.clear();
                }


            }
            else if(events[i].events&EPOLLOUT)
            {
                char buf[255]={0};
                auto a = fd_msg_map.find(userfd);
                string strtemp;//deal message
                if(a!=fd_msg_map.end())
                {
                    strtemp = a->second;
                    strcpy(buf,strtemp.c_str());
                    fd_msg_map.erase(a);
                    int  pos=0;
                    int  need_be_write_count=sizeof(buf);
                    int counter;
                    while(1)
                    {
                        counter = write(userfd,buf+pos,sizeof(buf)-pos);
                        if(counter<0)
                            break;
                        if(counter<need_be_write_count)
                        {
                            pos+=counter;
                            need_be_write_count-=counter;
                        }
                        else
                        {
                            break;//write complete;
                        }

                    }
                    if(counter==-1)
                    {
                        if(errno==EAGAIN||errno==EWOULDBLOCK)
                        {
                            continue;//buf is full
                        }
                    }
                    if(counter==0)
                    {
                        cout<<"client close"<<endl;
                        close(userfd);
                        epoll_ctl(m_efd,EPOLL_CTL_DEL,userfd,&events[i]);
                        continue;
                    }
                }
                event.data.fd = userfd;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(m_efd,EPOLL_CTL_MOD,userfd,&event);


            }

        }
    }

}


