#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>


int main(int argc, char** argv)
{

    int sfd, rt;
    struct sockaddr_un uaddr;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);      //SOCK_DGRAM 不支持监听
    if(sfd == -1) {
        perror("socket");
        return 1;
    }

    if(0 == access("./unix_file", F_OK))
        unlink("./unix_file");

    memset(&uaddr, 0, sizeof(uaddr));
    uaddr.sun_family = AF_UNIX;
    strncpy(uaddr.sun_path, "./unix_file", strlen("/unix_file") + 1);

    rt = bind(sfd, (struct sockaddr*)&uaddr, sizeof(uaddr));
    if(rt == -1) {
        perror("bind");
        return 1;
    }

    rt = listen(sfd, 10);
    if(rt == -1) {
        perror("listen");
        return 1;
    }

    struct sockaddr_un cunaddr;
    char buf[1024];
    int len;
    struct epoll_event evn;

    int efd = epoll_create(1);
    if(efd == -1) {
        perror("epoll_create");
        return 1;
    }

    memset(&evn, 0, sizeof(evn));
    evn.events = EPOLLIN;
    evn.data.fd = sfd;
    rt = epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &evn);

    struct epoll_event envs[12];
    struct epoll_event *pe;
    while(1) {
        memset(&envs, 0, sizeof(envs[0]) * 12);
        
        
        rt = epoll_wait(efd, envs, 12, 0); 

        if(rt == -1) {
            if(errno == EINTR)
                continue;
            perror("epoll_wait");
            return 1;
        }

        for(int i = 0; i < rt ; i++) {
            memset(buf, 0, sizeof(buf));
            pe = &envs[i];

            if(pe->data.fd == sfd) {
                
                rt = accept(sfd, NULL, NULL);
                if(rt == -1) {
                    perror("accept");
                    continue;
                } 

                memset(&evn, 0, sizeof(evn));
                evn.events = EPOLLIN;
                evn.data.fd = rt;
                rt = epoll_ctl(efd, EPOLL_CTL_ADD, rt, &evn);
                if(rt == -1) {
                    perror("epoll_ctl");
                }

                continue;
            }

            len = read(pe->data.fd, buf, sizeof(buf));
            if(len == 0) {
                rt = epoll_ctl(efd, EPOLL_CTL_DEL, pe->data.fd, NULL);
                if(rt == -1) {
                    perror("epoll_ctl");
                }
                continue;
            }
            printf("client say: %s\n", buf);
        }
        
    }



    return 0;
}