#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtp.h"
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fcntl.h>
#include <ctime>
using namespace std;
// this is my rtp


// this is my client struct
typedef struct CLIENT {
    PACKET* DATAarray;
    int portnumber;
    struct sockaddr_in clntaddr;
    int filelocation;
    int sent_window_Size;
    int empty;
    int tempseq1;
    int tempseq2;
    MESSAGE* file_get;
    clock_t start_time;
    int sentnumber;
    int sentamount;
    int getavaible;
    int close;
    int expect_number;
} CLIENT;

//this is my bind function, it will setup connect information stuff for my server
Coninfo* bindd(int port)
{
    int sent_bytes;
    int servSock;
    struct sockaddr_in servaddr;
    struct sockaddr_in clntaddr;
    unsigned short servport;
    unsigned int clntLen;


    servport = port;
    //set up the socket here
    servSock = socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
    fcntl(servSock, F_SETFL, O_NONBLOCK);
    if (servSock<0) {
        perror("socket() failed");

    }
    //setup some value here
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr  = htonl(INADDR_ANY);
    servaddr.sin_port = htons(servport);
    //bind to the clicent
    int bindd = bind(servSock, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(bindd < 0) {
        perror("bind failed");
    }

    Coninfo *info = (Coninfo*)malloc(sizeof(Coninfo));
    info->socket=servSock;
    info->clntaddr = clntaddr;
    return info;
}

//this function is to setup the socket for my client
Coninfo* setup_socket_client(int port, char* ip)
{
    Coninfo *info = (Coninfo*)malloc(sizeof(Coninfo));

    struct sockaddr_in Servaddr;
    struct sockaddr_in sin_f;
    int sock;
    PACKET*gettt = (PACKET*)malloc(sizeof(PACKET));
    

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    fcntl(sock, F_SETFL, O_NONBLOCK);
    if (sock<0) {
        perror("sock failed");
    }
    clock_t start;
    start=clock();
    memset(&sin_f, 0, sizeof(sin_f));
    sin_f.sin_family=AF_INET;
    sin_f.sin_addr.s_addr = inet_addr(ip);
    sin_f.sin_port = htons(port);
//sent SYN to client

    printf("sent syn\n");
    PACKET *syn=(PACKET*)malloc(sizeof(PACKET));
    syn->type=SYN;
    syn->seq_number = 997;
    int sent_bytes = sendto(sock,syn,sizeof(PACKET),0,(struct sockaddr *) &sin_f, sizeof(sin_f));

    char buffer[100];   //setup buffer here
    int recesize=100;       //set the buffer size
    unsigned int lenss = sizeof(Servaddr);
    //receive message here
    // wait for the ACK from server
    int read_byte=0;
    do {
        read_byte = recvfrom(sock,gettt,sizeof(PACKET),0,(struct sockaddr *) &Servaddr, &lenss );
        if (((clock()-start)/(double)CLOCKS_PER_SEC)>0.2) {
            sendto(sock,syn,sizeof(PACKET),0,(struct sockaddr *) &sin_f, sizeof(sin_f));
            start = clock();
        }

    } while (read_byte==-1);
    //cout<<"here??"<<endl;
    if (gettt->type==ACK) {
        printf("get ack\n");
        char *data2 = (char*)malloc(100);
        int data2_len = 100;
        printf("sent ack\n");
        strcat(data2,"ack");
        int sent_bytes = sendto(sock, data2,data2_len,0,(struct sockaddr *) &sin_f, sizeof(sin_f));
    }

    info->socket=sock;
    info->Servaddr = Servaddr;
    info->sin_f=sin_f;
    return info;

}
//this is the accpet function
void accept(Coninfo *connection)
{
    PACKET*response = (PACKET*)malloc(sizeof(PACKET));
    response->type=ACK;
    sendto(connection->socket,response,sizeof(PACKET),0,(struct sockaddr *)&connection->clntaddr, sizeof(connection->clntaddr));
   
}


// this is my checksum function
int checksum(char *buffer, int length)
{

    int sum = 0;
    for (int i = 0; i < length; i++) {
        /* code */
        sum = sum+(int)buffer[i];

    }
    return sum;

}
//this is my packetize function, pass in file, will return all the packet of the file
PACKET* packetize(char *buffer, int length, int *count)
{

    int packagenum;
    if (length% MAX_PAYLOAD_LENGTH == 0) {
        packagenum = length/MAX_PAYLOAD_LENGTH;
    } else {
        packagenum = length/MAX_PAYLOAD_LENGTH + 1;
    }

    PACKET *packarray = (PACKET*)malloc(packagenum * sizeof(PACKET));

    for (int i = 0; i < packagenum; i++) {
        packarray[i].seq_number = i;

        if (MAX_PAYLOAD_LENGTH * (i + 1) < length) {
            packarray[i].payload_length = MAX_PAYLOAD_LENGTH;
            packarray[i].type = DATA;
        } else {
            packarray[i].payload_length = length - MAX_PAYLOAD_LENGTH*i;
            packarray[i].type = LAST_DATA;
        }



        for (int j = 0; j < packarray[i].payload_length; j++) {
            packarray[i].payload[j]=buffer[MAX_PAYLOAD_LENGTH*i+j];
            /* code */
        }

        packarray[i].checksum = checksum(packarray[i].payload,packarray[i].payload_length);
        //cout<<"checksum: "<<packarray[i].checksum<<endl;

    }

    *count = packagenum;

    return packarray;


}
// this is the send function we need to pass in the file name, and how many pack need to sent;
int sendd(Coninfo *connection,char* buffer, int len,int window_size,int from_where, bool clientornot)
{
    int* count = (int*)malloc(sizeof(int));
    PACKET*pack= packetize(buffer,len,count);
   // cout<<"from where"<<from_where<<endl;
    //cout<<"total packet"<<*count<<endl;
    PACKET *response=(PACKET*)malloc(sizeof(PACKET));
    unsigned int ee = sizeof(connection->clntaddr);

    for (int j = from_where; j < window_size+from_where; ++j) {
        if (j<*count) {
          //  cout<<pack[j].seq_number<<endl;

            if (clientornot==false) {
                sendto(connection->socket, &(pack[j]),sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));
               // cout<<"my port number: "<<connection->clntaddr.sin_port<<endl;    /* code */
            } else {
                sendto(connection->socket, &(pack[j]),sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));

            }


        } else {
            return 10;

        }
    }


    free(response);

    return 1;
}

//this is the recvive function
PACKET* recvvv(Coninfo *connection,int window_size,CLIENT* clientarray,int clientnumber,int ori_window_size)
{
    clock_t start;
    start=clock();
    int gett;
    int temp_seq=-1;
    int temp_port_number=-1;
    PACKET* mypackarray = (PACKET*)malloc(window_size*sizeof(PACKET));
    PACKET*response = (PACKET*)malloc(sizeof(PACKET));
    unsigned int ee = sizeof(connection->clntaddr);

    for (int i = 0; i < window_size; ++i) {
        //get packet here
        gett = recvfrom(connection->socket,&(mypackarray[i]),sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),&ee );

        if (gett!= -1) {
           // cout<<"my server recv array number"<<mypackarray[i].type<<"   "<<mypackarray[i].seq_number<<endl;
            //if this is a data packet.
            if (mypackarray[i].type==DATA||mypackarray[i].type==LAST_DATA) {
                for (int j = 0; j < clientnumber; ++j) {

                    if (clientarray[j].portnumber==connection->clntaddr.sin_port) {
                        //cout<<"my server expect number: "<<clientarray[j].expect_number<<endl;
                        if (mypackarray[i].seq_number==clientarray[j].expect_number+1) {
                            if (checksum(mypackarray[i].payload,mypackarray[i].payload_length)==mypackarray[i].checksum) {
                                //sent ack back to sender
                                response->type=ACK;
                                response->seq_number = mypackarray[i].seq_number;
                                //if this is the lack packet, I will sent a different Ack call last ACK
                                if (mypackarray[i].type==LAST_DATA) {
                                    response->type=LAST_ACK;
                                    response->seq_number = mypackarray[i].seq_number;
                                   // cout<<"my out ack number"<< response->seq_number <<endl;
                                    sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));
                                    sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));
                                }
                                // sent the ack back to my client
                                sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));
                               // cout<<"my sent out ack number: "<< response->seq_number <<endl;

                                clientarray[j].expect_number=mypackarray[i].seq_number;

                            } else {
                                mypackarray[i].type=Ram;
                              //  cout<<"packet corrupt here"<<endl;
                            }
                        } else {

                            if (mypackarray[i].seq_number<clientarray[j].expect_number+1)
                            {
                                response->type=ACK;
                                response->seq_number = mypackarray[i].seq_number;
                                          sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));
                            //    cout<<"my sent out ack number: "<< response->seq_number <<endl;

                            }
                            mypackarray[i].type=Ram;
                        }
                    }
                }

            }
            mypackarray[i].portnumber=connection->clntaddr.sin_port;
            mypackarray[i].clntaddr=connection->clntaddr;
            temp_seq = mypackarray[i].seq_number;

            temp_port_number=mypackarray[i].portnumber;
            //if I get a ackm I will updata the ack sequence number
            if (mypackarray[i].type==ACK||mypackarray[i].type==LAST_ACK) {
                for (int j = 0; j < clientnumber; ++j) {
                    if (connection->clntaddr.sin_port==clientarray[j].portnumber) {
                        if (mypackarray[i].seq_number>clientarray[j].sentnumber) {
                            clientarray[j].sentnumber=mypackarray[i].seq_number;
                        }
                    }
                }
            }
            //if this is a last ack packet, i will tell my applicaiton this is my data is done
            if (mypackarray[i].type==LAST_ACK) {
                for (int j = 0; j < clientnumber; ++j) {
                    if (connection->clntaddr.sin_port==clientarray[j].portnumber) {
                        
                        clientarray[j].tempseq2=-100;

                    }
                }
            }
            // if I get a download request from client
            if (mypackarray[i].type==GETrequest) {
                for (int j = 0; j < clientnumber; ++j) {
                    if (connection->clntaddr.sin_port==clientarray[j].portnumber) {
                        if (clientarray[j].getavaible==1) {

                            clientarray[j].getavaible=0;
                            response->type=Request_ACK;
                            response->window_size=ori_window_size;
                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));

                        } else {
                            mypackarray[i].type=Ram;
                            response->type=Request_ACK;

                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));

                        }
                    }
                }
            }
            // if the packet if FIn, i will sent finack to client
            if (mypackarray[i].type==FIN) {
                for (int j = 0; j < clientnumber; ++j) {
                    if (clientarray[j].portnumber==mypackarray[i].portnumber) {
                        if (clientarray[j].close==0) {
                            response->type=FIN_ACK;
                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));
                            clientarray[j].close=1;
                        } else {
                            mypackarray[i].type=Ram;
                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->clntaddr),sizeof(connection->clntaddr));

                        }
                    }
                }
            }


        } else {

            if (((clock()-start)/(double)CLOCKS_PER_SEC)>0.2) {

                break;
            }
            i=i-1;

        }


    }
    return mypackarray;
}

// this is also a receive function

PACKET* recvv(Coninfo *connection,int window_size,int dotaseq,int needacknumber,int* fromwhere)
{

    int count = 0;
    int xx=1;
    int data_temp_number=dotaseq;
    int ack_temp_number=*fromwhere;
    int getsth=-1;
    int expect_number=dotaseq;
    unsigned int eee = sizeof(connection->Servaddr);

    PACKET *package = (PACKET *)malloc(sizeof(PACKET));
    PACKET *response = (PACKET *)malloc(sizeof(PACKET));
    PACKET *arraybuffer = (PACKET *)malloc(window_size*sizeof(PACKET));

    clock_t start;
    start=clock();
    for (int i = 0; i < window_size; ++i) {


        getsth= recvfrom(connection->socket, &(arraybuffer[i]), sizeof(PACKET), 0,(struct sockaddr *)&(connection->Servaddr), &eee);
        
        if (getsth==-1) {
            i=i-1;
            //cout<<((clock()-start)/(double)CLOCKS_PER_SEC)<<endl;
            if (((clock()-start)/(double)CLOCKS_PER_SEC)>0.3) {

                //cout<<"hello this is timer"<<endl;

                break;
            }
        } else {

         //   cout<<"my client recv array number"<<arraybuffer[i].type<<"   "<<arraybuffer[i].seq_number<<endl;

            if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {
                //cout<<"this is my expect_number number: "<<expect_number<<endl;
                if (arraybuffer[i].seq_number==expect_number||arraybuffer[i].seq_number==expect_number+1) {

                    if (arraybuffer[i].checksum==checksum(arraybuffer[i].payload,arraybuffer[i].payload_length)) {
                     //   cout<<"this is the packet checksum: "<<arraybuffer[i].checksum<<endl;
                     //   cout<<"this is my current checksum: "<<checksum(arraybuffer[i].payload,arraybuffer[i].payload_length)<<endl;

                        expect_number = arraybuffer[i].seq_number;
                        response->type=ACK;
                        if (arraybuffer[i].type==LAST_DATA) {
                            response->type=LAST_ACK;
                            response->seq_number = arraybuffer[i].seq_number;
                           // cout<<"my out ack number"<< response->seq_number <<endl;
                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));
                            sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));


                        }
                        response->seq_number = arraybuffer[i].seq_number;
                       // cout<<"my out ack number"<< response->seq_number <<endl;
                        int xx= sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));


                    } else {
                       
                        arraybuffer[i].type = Ram;
                    }


                } else {

                if (arraybuffer[i].seq_number<expect_number)
                      {
                        response->type=ACK;
                        response->seq_number = arraybuffer[i].seq_number;
                       // cout<<"my out ack number"<< response->seq_number <<endl;
                        int xx= sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));

                          
                      }      
                  //  cout<<"type change"<<endl;
                    arraybuffer[i].type = Ram;
                }
            }
            if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {
                if (data_temp_number==arraybuffer[i].seq_number) {
                    i=i-1;
                }
            } else if (arraybuffer[i].type==ACK) {
                if (ack_temp_number==arraybuffer[i].seq_number) {
                    i=i-1;
                }
            }

            if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {
                if (arraybuffer[i].seq_number==data_temp_number+1) {
                    data_temp_number=arraybuffer[i].seq_number;
                }

            } else if (arraybuffer[i].type==ACK) {
                if (arraybuffer[i].seq_number>=ack_temp_number) {
                    ack_temp_number=arraybuffer[i].seq_number;
                }
            }

        }
    }

    *fromwhere=ack_temp_number;
    return arraybuffer;
}
// this function will help to disconnect client from server
void shutdown_socket(Coninfo *connection)
{
    clock_t start;
    start=clock();
    unsigned int eee = sizeof(connection->Servaddr);
    int recv=-1;
    PACKET *response = (PACKET *)malloc(sizeof(PACKET));
    PACKET *fin = (PACKET *)malloc(sizeof(PACKET));
    response->type=FIN;
    cout<<"sent FIN" <<endl;
    sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));
    do {
        recv = recvfrom(connection->socket, fin, sizeof(PACKET), 0,(struct sockaddr *)&(connection->Servaddr), &eee);
        
        if (((clock()-start)/(double)CLOCKS_PER_SEC)>0.8)
        {
        sendto(connection->socket, response, sizeof(PACKET),0,(struct sockaddr *)&(connection->sin_f),sizeof(connection->sin_f));

        }

        
    } while (fin->type!=FIN_ACK);

    if(connection)
        close(connection->socket);
}
