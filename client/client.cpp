#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
using namespace std;
#define ERR_EXIT(m) \
        do \
        { \
                perror(m); \
                exit(EXIT_FAILURE); \
        } while(0)

int main(void)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("connect");

    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if (getsockname(sock, (struct sockaddr*)&localaddr, &addrlen) < 0)
    ERR_EXIT("getsockname");

    std::cout<<"ip="<<inet_ntoa(localaddr.sin_addr)<<
        " port="<<ntohs(localaddr.sin_port)<<std::endl;
    char name[5];
    cout<<"please input  your name"<<endl;
    cin>>name;
    write(sock,name,sizeof(name));
    fcntl(sock,F_SETFL,O_NONBLOCK);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK );
    int pid = fork();
    if(pid!=0)//父进程 read;
    {
        char buf[1024]={0};
        int n=0;
        while(true)
        {
            n = read(STDIN_FILENO,buf,sizeof(buf));
            if(n<=0)
                continue;
            buf[n-1]='\0';
            strcat(buf,"-");
            strcat(buf,name);
            write(sock,buf,strlen(buf));
            sleep(1);
        }

    }
    else
    {
        while(true)
        {
            char buf[1024]={0};
            string strbuf;
            int counter=0;
            while(true)
            {
                counter = read(sock,buf,sizeof(buf));
                if(counter<0)
                    break;
                strbuf.append(buf,counter);
                memset(buf,0,sizeof(buf));

                if(counter<sizeof(buf))
                {
                    break;
                }
                sleep(1);
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
            //cout<<"receive buf:  ";
            cout<<strbuf<<endl;
            //usleep(100000);
            sleep(1);




        }

    }



    close(sock);

    return 0;
}
