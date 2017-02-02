
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */

#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include "rtp.h"
#include "rtp.cc"

//this is my FTA client

// this function will sent the file name to server and return the window size of my server
int sent_request(Coninfo* connect,PACKET* packett)
{
    int getsth;
    clock_t start;
    start=clock();
    PACKET* get=(PACKET*)malloc(sizeof(PACKET));
    unsigned int eee = sizeof(connect->Servaddr);

    do {
        getsth= recvfrom(connect->socket, get, sizeof(PACKET), 0,(struct sockaddr *)&(connect->Servaddr), &eee);
        if (((clock()-start)/(double)CLOCKS_PER_SEC)>0.5) {
            sendto(connect->socket,packett,sizeof(PACKET),0,(struct sockaddr *)&(connect->sin_f),sizeof(connect->sin_f));
            start=clock();
        }
    } while(get->type!=Request_ACK);
    return get->window_size;

}


int main(int argc, char *argv[])
{
    char filename[100];
    unsigned short portnumber;
    char *pro;
    pro = strstr(argv[1],":");
    if (pro[0] == ':') {
        pro++;
    }
    int stop=0;
    //my port number here
    portnumber=atoi(pro);
    char *ipadr=argv[1];
    int window_size = atoi(argv[2])/1000;
    ipadr=strtok(ipadr,":");
    //setup the connection here, do the two way hand shake
    Coninfo* connect = setup_socket_client(portnumber,ipadr);
    unsigned int eee = sizeof(connect->Servaddr);
    printf("File transfer begin-> please input commend: \n");
    int server_windowsize;

// begin the while loop here
    while(1) {
        int dotaseq=-1;
        MESSAGE *get = (MESSAGE*)malloc(sizeof(MESSAGE));
        get->length = 0;
        string conmend;
        getline(cin,conmend);
        stop=0;

        // different input here.
        //if the input is get 
        if (((int)(conmend.find("post")))<0&&((int)(conmend.find("disconnect")))<0) {
            int* fromwhere = (int*)malloc(sizeof(int));
            // get the file name;
            string filename = conmend.substr(4,conmend.length());
            char* downloadname = new char[filename.length()+1];
            strcpy (downloadname, filename.c_str());
            PACKET *file = (PACKET*)malloc(sizeof(PACKET));
            file->type=GETrequest;
            for (int i = 0; i < filename.length()+1; ++i) {
                file->payload[i]=downloadname[i];
            }
            file->window_size = window_size;
            //sent the file name to server;
            server_windowsize=sent_request(connect,file);


            //begin get the file
            while(stop==0) {
                //receive pack here, each time i get the packet number equal to my window size;
                PACKET* arraybuffer=recvv(connect,window_size,dotaseq,0,fromwhere);
                for (int i = 0; i < window_size; ++i) {
                    //if the packet is data packet;
                    if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {
                        if (arraybuffer[i].seq_number==dotaseq+1&& arraybuffer[i].checksum==checksum(arraybuffer[i].payload,arraybuffer[i].payload_length)) {
                            cout<<"the data seq number: "<<arraybuffer[i].seq_number<<endl;
                            //put the data packet together;
                            char* part = (char*)malloc((arraybuffer[i].payload_length +get->length)*sizeof(char));
                            for (int j = 0; j < get->length; j++) {
                                part[j] = get->buffer[j];
                            }
                            for (int k = 0; k < arraybuffer[i].payload_length; k++) {
                                part[k + get->length] = arraybuffer[i].payload[k];

                            }
                            get->buffer = part;
                            get->length = get->length + arraybuffer[i].payload_length;
                            if (arraybuffer[i].type==LAST_DATA) {
                                stop=1;
                                break;

                            }
                           // cout<<dotaseq<<endl;
                            if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {
                                dotaseq = arraybuffer[i].seq_number;

                            }


                        }
                    }
                }


               // cout<<stop<< "  stop is here" <<endl;

            }
            //write my file 
            cout<<"get is done"<<endl;
            FILE *myfile=fopen("get_F","wb");
            fwrite(get->buffer,get->length,1,myfile);
            fclose(myfile);
            printf("input next commend:\n");
            free(get);
            // the second input, if this is a get-post
        } else if(((int)(conmend.find("get-post")))>-1) {
            //start the timer.
            clock_t start;
            start=clock();
            string filename_part = conmend.substr(9,conmend.length());
            cout<<filename_part<<endl;
            cout<<filename_part.substr(filename_part.find(" ")+1,filename_part.length())<<endl;
            int* fromwhere = (int*)malloc(sizeof(int));

            int sentamount=5;
            //this is the filebuff
            MESSAGE *get =(MESSAGE*) malloc(sizeof(MESSAGE));
            get->buffer=NULL;
            get->length = 0;
            int someint;
            int* acounter = (int*)malloc(sizeof(int));
            // the file name of my upload file
            string uploadfilename = filename_part.substr(filename_part.find(" ")+1,filename_part.length());
            cout<<"post filename: "<<uploadfilename<<endl;
            char* postname = new char[uploadfilename.length()+1];
            strcpy (postname, uploadfilename.c_str());
            //read the uoload file
            FILE *ptr_myfile=fopen(postname,"rb");
            char *buffer;
            long len;
            int done1=0;
            int done2=0;
            int sentnumber=0;
            
            fseek(ptr_myfile,0,SEEK_END);
            len = ftell(ptr_myfile);
            rewind(ptr_myfile);
            buffer = (char *)malloc((len+1)*sizeof(char));
            fread(buffer,len,1,ptr_myfile);
            int goldnumber=-1;
            // get the download filename
            string filename = filename_part.substr(0,filename_part.find(" "));
            cout<<"get filename: "<<filename<<endl;
            char* downloadname = new char[filename.length()+1];
            strcpy (downloadname, filename.c_str());
            //this is the file i want to download
            PACKET *file = (PACKET*)malloc(sizeof(PACKET));
            file->type=GETrequest;
            for (int i = 0; i < filename.length()+1; ++i) {
                file->payload[i]=downloadname[i];
            }
            file->window_size = window_size;
          //  cout<<connect->Servaddr.sin_port<<endl;
            // sent the file name to server;
            server_windowsize=sent_request(connect,file);

            someint=sendd(connect,buffer,len,5,sentnumber,true);
            *fromwhere=0;
            goldnumber=goldnumber+sentamount;
            sentamount = server_windowsize;
            //the loop begin here
            while(stop==0) {
                // the packet receive begin here;
                PACKET* arraybuffer=recvv(connect,window_size,dotaseq,sentamount,fromwhere);
                for (int i = 0; i < window_size; ++i) {
                    if (arraybuffer[i].type==DATA||arraybuffer[i].type==LAST_DATA) {

                       // cout<<"this is the dataseq number: "<<dotaseq<<endl;
                        if (arraybuffer[i].seq_number==dotaseq+1&& arraybuffer[i].checksum==checksum(arraybuffer[i].payload,arraybuffer[i].payload_length)) {
                            cout<<"my data seq number is: "<< arraybuffer[i].seq_number<<endl;
                          // add all data together
                            char* part = (char*)malloc((arraybuffer[i].payload_length +get->length)*sizeof(char));
                            for (int j = 0; j < get->length; j++) {
                                part[j] = get->buffer[j];
                            }
                            for (int k = 0; k < arraybuffer[i].payload_length; k++) {
                                part[k + get->length] = arraybuffer[i].payload[k];

                            }
                            get->buffer = part;
                            get->length = get->length + arraybuffer[i].payload_length;
                            //if the client receive the last data packet from server
                            //then we know the get is done/
                            if (arraybuffer[i].type==LAST_DATA) {
                                cout<<"get is done"<<endl;
                                done1=1;

                            }
                            //updata the data sequence number;
                            dotaseq = arraybuffer[i].seq_number;
                        } 
                    }


                    if (done2==0)
                    {
                    // if the post is not done yet
                    if (*fromwhere==goldnumber||((clock()-start)/(double)CLOCKS_PER_SEC)>0.5) {

                        someint=sendd(connect,buffer,len,sentamount,*fromwhere+1,true);
                        goldnumber=goldnumber+sentamount;
                        start = clock();
                    }
                    }
                    // if the post is done here;
                    if (arraybuffer[i].type==LAST_ACK) {
                        cout<<"post is done"<<endl;
                        done2=1;
                        someint=0;
                    }

                    if (done2==1&&done1==1) {
                        // if post and get both done here;
                        stop=1;
                        break;
                    }

                }
            }
            //write the file
            FILE *myfile=fopen("get_F","wb");
            fwrite(get->buffer,get->length,1,myfile);
            fclose(myfile);
            file->type=Done;
            file->seq_number = 13238;
            sendto(connect->socket, file,sizeof(PACKET),0,(struct sockaddr *)&(connect->sin_f),sizeof(connect->sin_f));

            printf("input next commend:\n");
            free(get);

                // if the input is disconnect
        } else if (((int)(conmend.find("disconnect")))>=0) {
            cout<<"disconnect"<<endl;
            shutdown_socket(connect);
            break;
        }

    }
}
