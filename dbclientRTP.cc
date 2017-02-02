
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */

#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include "rtp.h"
#include "rtp.cc"
#include <iostream>
int main(int argc, char *argv[])
{

    int read_byte;
    char *studentID=argv[2];
    char *info=argv[3];
    char *info1=argv[4];
    char *info2=argv[5];
    char *info3=argv[6];
    char *info4=argv[7];
    int *sth=(int*)malloc(sizeof(int));
    *sth=-1;
    unsigned short portnumber;
    PACKET* mess = (PACKET*)malloc(sizeof(PACKET));
    PACKET* messs = (PACKET*)malloc(2*sizeof(PACKET));
   
    char *pro;
    pro = strstr(argv[1],":");
    if (pro[0] == ':') {
        pro++;
    }
    //
    portnumber=atoi(pro);
    char *ipadr=argv[1];
    ipadr=strtok(ipadr,":");
    Coninfo* connect = setup_socket_client(portnumber,ipadr);
    string data1 ="";

    //prepare the data which will sent to server
    //understand what is the user input.
    if (argc>2) {
        (data1=data1+ studentID);
    } else {
        printf("Please input student number!\n");
        exit(0);
    }
    if (argc>3) {
        (data1=data1+ info);
    }
    if (argc>4) {
        (data1=data1+ info1);
    }
    if (argc>5) {
        (data1=data1+ info2);
    }
    if (argc>6) {
        (data1=data1+ info3);
    }
    if (argc>7) {
        (data1=data1+ info4);
    }
    //put the data into packet
    char* buffer = new char[data1.length()+1];
    strcpy (buffer, (data1.c_str()) );
    for (int i = 0; i < data1.length(); ++i) {
        mess->payload[i]=buffer[i];
    }
    mess->seq_number=1;

    unsigned int lenss = sizeof(connect->Servaddr);
    //sent the packet to server;
    sendd(connect,buffer,800,1,0,true);

//get the student information from server
    while(1) {
        messs = recvv(connect,1,-1,0,sth);

        if (messs[0].type==LAST_DATA) {
            cout<<messs[0].type<<endl;
            break;
        }
        sendd(connect,buffer,800,1,0,true);
    }
    //print out the student information I get;
    cout<<messs->payload<<endl;
    //just close the connection here;
    close(connect->socket);
    free(buffer);

    return 0;
}

















