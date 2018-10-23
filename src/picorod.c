#include <stdlib.h>
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

int sys_init(void** pctx);
int sys_stop(void* ctx);
int sys_get_transport(void* ctx);
int sys_rel_transport(void* ctx);
int sys_send(void* ctx, const uchar* buf, uint size);
int sys_recv(void* ctx, uchar* buf, uint size);

char *cachename = "default";
uint cachenamelength = 7;
uchar flags = 0x00; // Force return previous value disabled
uchar clientIntelligence = 0x01; //No topology info neither cluster information needed
uchar keyMediaType = 0x00;
uchar valueMediaType = 0x00;
uchar defaultExpirationAndIdleTime = 0x77;

uchar PUT = 0x01;
uchar GET = 0x03;

int put(const uchar* key, unsigned int skey, const uchar* value, uint svalue, uchar* status);
int get(const uchar* key, unsigned int skey, uchar** pvalue, uint* psvalue, uchar* status);
int init();
int stop();

int write_request_header(uchar opcode);
int read_response_header();
void write_vint(uint val);
void write_vlong(ulong val);
uint read_vint();
ulong read_vlong();

void *ctx;

int init() {
    return sys_init(&ctx);
}

int stop(void* ctx) {
    return sys_stop(ctx);
}

int put(const uchar* key, unsigned int skey, const uchar* value, uint svalue, uchar* status) {
    void* ctx;
    sys_get_transport(&ctx);
    write_request_header(PUT);
    write_vint(skey);
    sys_send(0, key, skey);
    sys_send(0, &defaultExpirationAndIdleTime, 1);
    write_vint(svalue);
    sys_send(0, value, svalue);
    read_response_header();
    sys_rel_transport(&ctx);
    return 0;
}

int get(const uchar* key, unsigned int skey, uchar** pvalue, uint* psvalue, uchar* status) {
    void* ctx;
    sys_get_transport(&ctx);
    write_request_header(GET);
    write_vint(skey);
    sys_send(0, key, skey);
    read_response_header(status);
    if (status != 0) {      // TODO: a better check needed
        return 0;
    }
    *psvalue = read_vint();
    pvalue = malloc( *psvalue * sizeof(uchar));
    sys_recv(ctx, *pvalue, *psvalue);
    return 0;
}

ulong req_num = 0;

int write_request_header(uchar opcode) {
    uchar magic = 0xA0;
    uchar version = 0x1C;
    sys_send(0, &magic, 1);
    write_vlong(++req_num);
    sys_send(0, &version, 1);
    sys_send(0, &opcode, 1);
    write_vint(cachenamelength);
    sys_send(0, cachename, cachenamelength);
    sys_send(0, &flags, 1);
    sys_send(0, &clientIntelligence, 1);
    sys_send(0, &keyMediaType, 1); 
    sys_send(0, &valueMediaType, 1); 
    return 0;
}

int read_response_header(uchar* status) {
    uchar trash;
    sys_recv(0, &trash, 1); // magic
    read_vlong(); // message id
    sys_recv(0, &trash, 1); // opcode
    sys_recv(0, status, 1); 
    sys_recv(0, &trash, 1); // topology
    return 0;
}

void write_vint(uint val) {
    char b;
    while ((val & ~0x7F) != 0) {
        b = (val & 0x7f) | 0x80;
        sys_send(0, &b, 1);
        val >>= 7;
    }
    b = (val & 0x7f);
    sys_send(0, &b, 1);
}

void write_vlong(ulong val) {
    char b;
    while ((val & ~0x7F) != 0) {
        b = (val & 0x7f) | 0x80;
        sys_send(0, &b, 1);
        val >>= 7;
    }
    b = (val & 0x7f);
    sys_send(0, &b, 1);
}

uint read_vint() {
    uchar b;
    sys_recv(0, &b, 1);
    uint i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        sys_recv(0, &b, 1);
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

ulong read_vlong() {
    uchar b;
    sys_recv(0, &b, 1);
    ulong i = b & 0x7F;
    for (int shift = 7; (b & 0x80) != 0; shift += 7) {
        sys_recv(0, &b, 1);
        i |= (b & 0x7FL) << shift;
    }
    return i;
}

