#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#ifndef _RTP_H
#define _RTP_H
using namespace std;
#define MAX_PAYLOAD_LENGTH (1000)
#define DEBUG (1)
//here are different type of data
enum {Ram,DATA=1, LAST_DATA=2, ACK,LAST_ACK, FIN ,SYN,GETrequest,Request_ACK,Done,FIN_ACK};

//this is my data pakcet struct
typedef struct _PACKET {
    int seq_number;
    int type;
    int checksum;
    int payload_length;
    char payload[MAX_PAYLOAD_LENGTH];
    string message;
    char* filename;
    int portnumber;
    int window_size;
    struct sockaddr_in clntaddr;
} PACKET;
//this is my connect information struct
typedef struct Coninfo {
    int socket;
    struct sockaddr_in Servaddr;
    struct sockaddr_in clntaddr;
    struct sockaddr_in sin_f;
} Coninfo;

typedef struct _MESSAGE {
    int length;
    char* buffer;
} MESSAGE;



#endif
