#include<sys/socket.h>
#include<arpa/inet.h> 
#include<unistd.h>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

int sock;

int sys_send(void* ctx, const uchar* buf, uint size) {
    return send(sock , buf , size , 0);
}

int sys_recv(void* ctx, uchar* buf, uint size) {
    return recv(sock , buf, size , 0);
}

int sys_flush(void* ctx) {
    return flush(sock);
}


int sys_init(void** pctx) {
    struct sockaddr_in server;
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        return -1;
    }
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(11222);
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        return -2;
    }
}
int sys_stop(void* ctx) {
    close(sock);
    return 0;
}

int sys_get_transport(void** ctx) {
    *ctx = (void*) sock;
}

int sys_rel_transport(void* ctx) {
}
