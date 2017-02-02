#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtp.h"
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "rtp.cc"
#include <string>
#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <ctime>
//this is my fta server

//this is my the struct to store all file information
typedef struct File_shop {
    int length;
    char* buffer;
    PACKET* filearray;
    int *count;

} File_shop;

int main(int argc, char *argv[])
{

    clock_t start;
    int getsum=0;
    File_shop* filesystem=(File_shop*)malloc(100*sizeof(File_shop));
    CLIENT* clientarray = (CLIENT*)malloc(100*sizeof(CLIENT));
    int clientnum=0;
    //get the post number
    int servport = atoi(argv[1]);
    int ori_window_size = atoi(argv[2])/1000;
    int window_size = atoi(argv[2])/1000;
    int client_amount=0;
    //bind to the socket
    Coninfo* conninfo = bindd(servport);
    PACKET* packarray=(PACKET*)malloc(window_size*sizeof(PACKET));
    MESSAGE *get = (MESSAGE*)malloc(sizeof(MESSAGE));
    unsigned int ee = sizeof(conninfo->clntaddr);
    int tempport=0;

    //the server begin here.
    while(1) {
        //receive packhere
        packarray = recvvv(conninfo,window_size,clientarray,clientnum,ori_window_size);
        for (int i = 0; i < window_size; ++i) {
            // if the pack is a SYN from client
            if (packarray[i].type==SYN) {

                if (tempport!=packarray[i].portnumber) {
                    client_amount++;
                    window_size=ori_window_size*client_amount;
                    conninfo->clntaddr=packarray[i].clntaddr;
                    //accept the connetion from client.
                    accept(conninfo);
                    //setup some initial value for each client
                    clientarray[clientnum].getavaible=1;
                    //the client's port number
                    clientarray[clientnum].portnumber = packarray[i].portnumber;
                    clientarray[clientnum].clntaddr = packarray[i].clntaddr;
                    clientarray[clientnum].tempseq1=0;
                    clientarray[clientnum].tempseq2=-100;
                    clientarray[clientnum].file_get = (MESSAGE*)malloc(sizeof(MESSAGE));
                    clientarray[clientnum].file_get->length=0;
                    clientarray[clientnum].sentnumber=0;
                    clientarray[clientnum].empty=1;
                    clientarray[clientnum].expect_number=-1;
                    clientarray[clientnum].sentamount=1;
                    clientarray[clientnum].close=0;
                    clientnum = clientnum+1;
                    tempport =packarray[i].portnumber;
                }
            }

            //if the packet I get is a data packet
            else if (packarray[i].type==DATA||packarray[i].type==LAST_DATA) {

                for (int j = 0; j < clientnum; ++j) {
                    // put all the data from data packet into my file buffer
                    if (packarray[i].portnumber==clientarray[j].portnumber) {
                        cout<<"my data seq number is: "<< packarray[i].seq_number<<endl;
                        char* part = (char*)malloc((packarray[i].payload_length+clientarray[j].file_get->length)*sizeof(char));
                        for (int k = 0; k < clientarray[j].file_get->length; k++) {
                            part[k] = clientarray[j].file_get->buffer[k];
                        }
                        for (int l = 0; l < packarray[i].payload_length; l++) {
                            part[l + clientarray[j].file_get->length] = packarray[i].payload[l];

                        }


                        clientarray[j].file_get->buffer = part;
                        clientarray[j].file_get->length = clientarray[j].file_get->length + packarray[i].payload_length;
                        if (packarray[i].type==LAST_DATA) {
                            cout<<"endhere"<<endl;
                            FILE *myfile=fopen("post_G","wb");
                            fwrite(clientarray[j].file_get->buffer,clientarray[j].file_get->length,1,myfile);
                            fclose(myfile);
                            clientarray[j].file_get->buffer=NULL;
                            clientarray[j].file_get->length=0;
                        }
                        clientarray[j].expect_number=packarray[i].seq_number;
                        if (packarray[i].type==LAST_DATA) {
                            // the file done here
                            clientarray[j].expect_number=-1;
                        }

                    }
                }

            }   else if (packarray[i].type==GETrequest) {
                //if the packet is a file name from client
               // cout<<"here i get a request"<<endl;
                FILE *ptr_myfile;
                char *buffer;
                long len;
                //read the file into buffer
                char* filename = (char*)malloc(100*sizeof(char));
                filename = packarray[i].payload;
                ptr_myfile=fopen(filename,"rb");
                fseek(ptr_myfile,0,SEEK_END);
                len = ftell(ptr_myfile);
                rewind(ptr_myfile);
                buffer = (char *)malloc((len+1)*sizeof(char));
                fread(buffer,len,1,ptr_myfile);
                conninfo->clntaddr=packarray[i].clntaddr;
                filesystem[getsum].buffer=buffer;
                filesystem[getsum].length=len;
              //  cout<<"the file length"<<len<<endl;
                sendd(conninfo,filesystem[getsum].buffer,filesystem[getsum].length,1,0,false);

                // find the correct client
                for (int j = 0; j < clientnum; ++j) {
                    if (packarray[i].portnumber==clientarray[j].portnumber) {
                        //prepare to sent the file
                        clientarray[j].getavaible=1;
                        clientarray[j].tempseq2=1;
                        clientarray[j].empty=0;
                        clientarray[j].filelocation = getsum;
                        clientarray[j].sent_window_Size = packarray[i].window_size;
                        clientarray[i].start_time=clock();

                    }
                }

                getsum++;
                if (getsum==99) {
                    getsum=0;
                }
                
                //if the file is done;
            } else if (packarray[i].type==LAST_ACK) {
                for (int j = 0; j < clientnum; ++j) {
                    if (clientarray[j].portnumber==packarray[i].portnumber) {

                        if (clientarray[j].empty==0) {
                            filesystem[clientarray[j].filelocation].buffer=NULL;
                            filesystem[clientarray[j].filelocation].filearray=NULL;
                            clientarray[j].empty=1;
                        }
                    }
                }
                //if I get a fIn packet shows that a client want to disconnect from the server.
            } else if (packarray[i].type==FIN) {
                for (int j = 0; j < clientnum; ++j) {
                    //findout the correct client, then sent ACK back to my client
                    if (clientarray[j].portnumber==packarray[i].portnumber) {
                        if (client_amount>1) {
                            client_amount=client_amount;
                            cout<<client_amount<<endl;
                            window_size = ori_window_size*client_amount;
                            cout<<window_size<<endl;
                        } else if (client_amount==1) {
                            window_size = ori_window_size;
                            client_amount=0;
                        }

                    }
                }
            }


        }



        for (int j = 0; j < clientnum; ++j) {
            // here is sent data part
            if (clientarray[j].tempseq2!=-100) {
               // cout<<"my tempseq1: "<<clientarray[j].tempseq1<<clientarray[j].sentnumber<<endl;
                if (clientarray[j].sentnumber==clientarray[j].tempseq1||((clock()-clientarray[j].start_time)/(double)CLOCKS_PER_SEC)>0.7) {

                    if (((clock()-clientarray[j].start_time)/(double)CLOCKS_PER_SEC)<0.7) {
                        if (clientarray[j].sentamount<clientarray[j].sent_window_Size) {
                            clientarray[j].sentamount++;
                        }

                    } else {
                        if (clientarray[j].sentamount>1) {
                          
                           
                            clientarray[j].sentamount--;
                        }
                    }

                    conninfo->clntaddr=clientarray[j].clntaddr;
                    //cout<<packarray[i].seq_number<<"ack number"<<endl;
                    sendd(conninfo,filesystem[clientarray[j].filelocation].buffer,filesystem[clientarray[j].filelocation].length,clientarray[j].sentamount,clientarray[j].sentnumber+1,false);
                    clientarray[j].start_time=clock();
                    clientarray[j].tempseq1=clientarray[j].sentnumber+clientarray[j].sentamount;
                }



            } else {


                clientarray[j].tempseq1=0;
                clientarray[j].sentnumber=-1;
            }

        }

    }
}