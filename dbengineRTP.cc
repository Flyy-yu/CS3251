
#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */

#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include "rtp.h"
#include "rtp.cc"
#include <iostream>
int main(int argc, char const *argv[])
{
    int temp_seq=0;
    int first_namec;
    int last_namec;
    int quality_pointsc;
    int gpac;
    int gpa_hoursc;
    int wronginfo;
    int IDwrong;
    string buffer;
    int x;
    int *sth=(int*)malloc(sizeof(int));
    *sth=-1;
    int servport = atoi(argv[1]);
    Coninfo* lisent = bindd(servport);
    CLIENT* asd;
    while(1) {

        PACKET* resp= (PACKET*)malloc(sizeof(PACKET));
        PACKET* respp= (PACKET*)malloc(sizeof(PACKET));

        //set up the connection with my client
        while(1) {
            resp = recvvv(lisent,1,asd,1,2);
            if (resp[0].type==SYN) {
                lisent->clntaddr=resp[0].clntaddr;

                accept(lisent);
                cout<<"listen"<<endl;

            }
            if (resp[0].type==LAST_DATA) {
                break;
            }


        }


        if (1) {
            /* code */
            printf("hererere\n");
            temp_seq =resp->seq_number;
            buffer.clear();
            buffer = resp[0].payload;
            cout<<buffer<<endl;
            //check what inside the message
            if (( (int)buffer.find("last_name"))>0) {
                last_namec=1;           //if need last name
            }
            if (( (int)buffer.find("first_name"))>0) {
                first_namec=1;          //if need first name
            }
            if (( (int)buffer.find("gpa_hours"))>0) {
                gpa_hoursc=1;           //if need gpa hours
            }
            if (( (int)buffer.find("quality_points"))>0) {
                quality_pointsc=1;          //if need quality_points
            }
            if (( (int)buffer.find("gpa"))>0) {
                gpac=1;                     //if need gpa
            }

            //if the message is wrong
            if (last_namec!=1&& first_namec!=1&& gpac!=1&&quality_pointsc!=1&&gpa_hoursc!=1) {
                wronginfo = 1;
            }
            //take out the ID number
            string ID = buffer.substr(0,9);
            string responmsm="";
            responmsm=responmsm+"From server: ";

            // see which student is here
            if (  ((int)ID.find("903076259"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Anthony ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Peterson ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_pointsc: 231 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hoursc: 63 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.666667 ";
                }
            } else if (  ((int)ID.find("903084074"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Richard ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Harris ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 236 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 66 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.575758 ";
                }
            } else if (  ((int)ID.find("903077650"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Joe ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Miller ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 224 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 65 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.446154 ";
                }
            } else if (  ((int)ID.find("903083691"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Todd ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Collins ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 218 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 56 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.892857 ";
                }
            } else if (  ((int)ID.find("903082265"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Laura ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Stewart ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 207 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 64 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.234375 ";
                }
            } else if (  ((int)ID.find("903075951"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Marie ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Cox ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 246 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 63 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.904762 ";
                }
            } else if (  ((int)ID.find("903084336"))>-1) {

                if (first_namec==1) {
                    responmsm=responmsm+"first_name: Stephen ";
                }
                if (last_namec==1) {
                    responmsm=responmsm+"last_name: Baker ";
                }
                if (quality_pointsc==1) {
                    responmsm=responmsm+"quality_points: 234 ";
                }
                if (gpa_hoursc==1) {
                    responmsm=responmsm+"gpa_hours: 66 ";
                }
                if (gpac==1) {
                    responmsm=responmsm+"gpa: 3.545455 ";
                }
            } else {

                responmsm=responmsm+"Student not in system!";
                IDwrong=1;
            }



            //if the student is not in our system
            if (wronginfo == 1&&IDwrong!=1) {
                responmsm=responmsm+" please input correct information: first_name,last_name,gpa,quality_points, gpa_hours";

            }

            char* rebuffer = new char[responmsm.length()+1];
            strcpy (rebuffer, (responmsm.c_str()));

            cout<<responmsm<<endl;
            // sent message back to client
            sendd(lisent,rebuffer,800,1,0,false);
            //reset some value to 0 here, for next loop
            first_namec=0;
            last_namec=0;
            quality_pointsc=0;
            gpa_hoursc=0;
            gpac=0;
            responmsm="";
            IDwrong=0;
            wronginfo = 0;

            free(resp);
            free(respp);
            continue;

        }

    }
    return 0;

}






