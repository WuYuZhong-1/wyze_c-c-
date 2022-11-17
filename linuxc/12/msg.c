#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

struct msgdata {
    long msgtype;
    char data[512];
};


int main(int argc, char** argv)
{
    char cwd[512] = {0};
    char *pcwd;
    key_t key;
    int   msg;
    int rt;
    struct msqid_ds msqid;

    pcwd = getcwd(cwd, sizeof(cwd));
    if(pcwd == NULL) {
        perror("getcwd");
        return 1;
    }

    key = ftok(pcwd, 1234);

    msg =  msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    // msg = msgget(key, IPC_CREAT | 0666);
    if(msg == -1) {
        perror("msgget");
        return 1;
    }

    struct msgdata send_data;
    int count = 10;
    for(int i = 0; i < count; ++i) {
        memset(&send_data, 0, sizeof(send_data));
        send_data.msgtype = i + 1;                  //这个参数必须大于0
        snprintf(send_data.data, sizeof(send_data.data),"msg info count = %d\n", i+ 1 );
        rt = msgsnd(msg, &send_data, strlen(send_data.data) + 1, IPC_NOWAIT);
        if(rt != 0) {
            if( errno == EAGAIN) {
                puts(" msg buff is full");
                break;
            }
            else if( errno == EINTR) {
                --i;
                continue;
            }
            else if( errno == EIDRM) {
                puts(" msg is rm");
                return 1;
            }
            else {
                perror("msgsnd");
                return 1;
            }
        }
    }
    puts("send success");

    //特殊取消息队列
    struct msgdata rcv_data;
try:
    memset(&rcv_data, 0, sizeof(rcv_data));
    rt = msgrcv(msg, &rcv_data, sizeof(rcv_data.data), 5, IPC_NOWAIT);
    if(rt < 0) {
        if(errno == EINTR) 
            goto try;
        else if( errno == E2BIG) {
            puts("rcv buff is too small");
        }
        else if( errno = ENOMSG) {
            puts("no type is 5 message");
        }
    }
    else {
        printf("msgtype = %ld\nmsgata = %s\n", rcv_data.msgtype, rcv_data.data);
    }


    for(int i = 0; i < 9; ++i) {
        memset(&rcv_data, 0, sizeof(rcv_data));
        rt = msgrcv(msg, &rcv_data, sizeof(rcv_data.data), 0, IPC_NOWAIT);
        if(rt < 0) {
            if(errno == EINTR) { 
                --i;
                continue;
            }
            else if( errno == E2BIG) {
                puts("rcv buff is too small");
                break;
            }
            else if( errno = ENOMSG) {
                puts("no type is 5 message");
                break;
            }
        }
        else {
            printf("msgtype = %ld\nmsgata = %s\n", rcv_data.msgtype, rcv_data.data);
        }
    }



    // printf("pid = %d\n", getpid());
    sleep(2);

    memset(&msqid, 0, sizeof(msqid));
    rt = msgctl(msg, IPC_STAT, &msqid);
    if(rt == -1) {
        perror("msgctl");
        return 1;
    }
    printf("msg_qbytes = %ld\n", msqid.msg_qbytes);
    printf("msg_qnum = %ld\n", msqid.msg_qnum);
    while(msqid.msg_qnum != 0) {
        usleep(1000);
    }

    rt = msgctl(msg, IPC_RMID, NULL);
    if(rt == -1) {
        perror("msgctl");
        return 1;
    }

    return 0;
}