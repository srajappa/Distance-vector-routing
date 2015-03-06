/**
 * @srajappa_assignment3
 * @author  Srinivasan Rajappa <srajappa@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../include/global.h"
#include "../include/logger.h"
//#define inf 0xFFFFFFFFFFFFFFFF
#define STDIN 0
#define zero 0x0000000000000000


uint16_t inf = 0xFFFF;
int input;
int sockfd,newfd,selRet,param;
static int rcvStatus[6],neigh_false[5],packets;
static int totalNeigh,totalSvrs,totalEntry,hostPort,hostSvr;
static char input_string[30];
uint16_t my_id;
FILE *fptr;
struct addrinfo hints,neigh,*servinfo,*n0,*n1,*n2,*n3,*n4,*n5;


static char fileInput[20],hostIP[20];
struct update_pkt
{
	 uint16_t numof;
	 uint16_t srvport;
	 uint32_t srvip;
	
	 uint32_t srvip_n[5];
	 uint16_t srvportn[5];
	 uint16_t padn[5];
	 uint16_t idn[5];	
	 uint16_t costn[5];
}up;

struct interim_pkt
{
	 uint16_t numof;
	 uint16_t srvport;
	 uint32_t srvip;
	
	 uint32_t srvip_n[5];
	 uint16_t srvportn[5];
	 uint16_t padn[5];
	 uint16_t idn[5];	
	 uint16_t costn[5];
}interPkt;

struct routerTable
{
	int destn;
	int nextHop;
	uint16_t cost;
}rt[5];

struct svrinfo
{
	int id;
	char ipAddr[20];
	int port;
	
}si[5];
struct neighinfo
{
	int hostID;
	int nID;
	int cost;
	char nIP[20];
	char port[10];
	int flag; 
}ni[6];

struct colCmd															
{	
	char cmd[20]; 
}obCmd[5];


void enterSvrInfo(char f[],char s[],char t[],int k)
{
	si[k].id = atoi(f);
	//printf("fi- %s\n",f);
	strcpy(si[k].ipAddr,s);
	si[k].port = atoi(t);
	//printf("Svr Info: %d IP %s \n",si[k].id,si[k].ipAddr);
}
void enterNeighInfo(char f[],char s[],char t[],int l)
{	
	//printf("enterNeigh: value of l %d\n",l);
	int i;
	ni[l].hostID = atoi(f);
	//printf("first %s\n",f);
	ni[l].nID = atoi(s);
	ni[l].cost = atoi(t);
	ni[l].flag = 1; 
}
/*
void bzeroer(char b[], int n)
{
	int i ; 
	for ( i = 0 ; i < n ; i++)
	{
		bzero(b,1);
		//b++;
	}
}
*/
/*//http://www.programmingsimplified.com/c-program-reverse-string
void reverse(char *string) 
{
   int length, c;
   char *begin, *end, temp;
 
   length = strlen(string);
 
   begin = string;
   end = string;
 
   for ( c = 0 ; c < ( length - 1 ) ; c++ )
      end++;
 
   for ( c = 0 ; c < length/2 ; c++ ) 
   {        
      temp = *end;
      *end = *begin;
      *begin = temp;
 
      begin++;
      end--;
   }
   *end = '\0';
}*/


void inttochar(char cp[],int port)
{
	sprintf(cp,"%d",port);
}



int topology_reader()
{	
	int lineNum = 1,i,space = 0,k =0,l = 0,ip_entry =1,j = 0 ;
	static char fileInfo[30], temp[30], first[2], second[20] , third[2],newport[16];
	//printf("Inside topology_reader\n");
	//printf("linenum 174 %d\n",lineNum);
	while(1)
	{
		fgets(fileInfo,30,fptr);
		tokener(fileInfo);
		if(lineNum == 1)
		{	totalSvrs = atoi(obCmd[0].cmd);
			lineNum++;
			//printf("linenum 182 %d\n",totalSvrs);
			continue;
		}
		else if (lineNum == 2)
		{
			totalNeigh = atoi(obCmd[0].cmd);
			//printf("total neigh %d\n",totalNeigh);
			lineNum++;
			continue;
		}
		else if (l==totalNeigh)
		{
			break;
		}
		else
		{
			ip_entry = (strlen(obCmd[1].cmd) > 2) ? 1: 0 ;
				
			if ( ip_entry == 1)
			{
				enterSvrInfo(obCmd[0].cmd,obCmd[1].cmd,obCmd[2].cmd,k);
				k++;
				totalEntry++;
				//printf("%s IP: %s PORT %s\n",obCmd[0].cmd,obCmd[1].cmd,obCmd[2].cmd);
				continue;
			}
			else if (ip_entry == 0)
			{
				my_id = atoi(obCmd[0].cmd);
				enterNeighInfo(obCmd[0].cmd,obCmd[1].cmd,obCmd[2].cmd,atoi(obCmd[1].cmd));
				l++;
				continue;
			}
		}
	}
	// Pad the strings with nulls
	for ( i = 0 ; i < 5; i++)
	{
		if(ni[i].flag==1)
		{
		bzero(ni[i].nIP,20);
		bzero(ni[i].port,10);
		}
		
	}
	
	for(i = 1 ; i < 6; i++)
	{
		for ( j = 0 ; j < totalSvrs ; j++)
		{
			if (ni[i].nID == si[j].id)
			{
				//printf("SI: IP: %s Port %d\n",si[j].ipAddr,si[j].port);
				strcpy(ni[i].nIP,si[j].ipAddr);
				
				inttochar(newport,si[j].port);
				strcpy(ni[i].port,newport);
				//printf("Neigh: IP: %s Port %s\n",ni[i].nIP,ni[i].port);
				break;
			}
			
		}
	}	
	//printf("linenum 364 %d\n",lineNum);
	//printf("Exiting topology reader\n");
	fclose(fptr);
}

int get_server_index(){
	int i;
	for ( i = 0; i < totalSvrs; ++i)
	{
		if (si[i].id == my_id)
		{
			return i;
		}
	}
	return inf;
}

//[PA3] Routing Table Start
void routerUpdate(int mode, int hop, int node)
{	
	int i,j ;
	if (mode == 1)
	{	// Initialize all the values
		for( i = 0; i < 5; i++)
		{
			rt[i].destn = i+1;
			rt[i].nextHop = -1;
			rt[i].cost = inf;
		}
		//Fill the routing table using the update packet
		for( i = 0; i < 5; i++)
		{
			for ( j = 0 ; j<5; j++)
			{
				//printf("RU: i-%d (up)j-%d\n",rt[i].destn,up.idn[j]); 
				if(rt[i].destn==up.idn[j])
				{	//printf("destn%d(i) and idn%d(j) cost-%d\n",i,j,up.costn[j]); 
					rt[i].cost = up.costn[j];
					if(up.costn[j] == 999)
					{
						rt[i].nextHop = -1;
					}
					//printf("Cost update %d-%d\n",rt[i].destn,rt[i].cost);
					break;
				}
			}
		}
		for(i = 0 ; i < 5; i++)
		{
			for ( j = 0 ;j < 5 ; j++)
			{
				if(ni[j+1].nID == rt[i].destn)
				{
					if(rt[i].cost !=999){
					rt[i].nextHop = ni[j+1].nID;
					break;}
					
					//printf("id %d next hop %d\n",rt[i].nextHop,ni[j+1].nID);
					//printf("next hop update %d-%d\n",ni[j].nID,rt[i].destn);
					break;
				}
			}
		}
		for(i=1;i<=5;i++)
		{	//printf("host value %d\n",ni[0].hostID);
			//printf("ni[2] %d\n",ni[1].hostID);
			for(j=1;j<=5;j++)
			{	if(ni[j].hostID == i){
			rt[i-1].nextHop = i;
			//printf("ni[2] %d\n",ni[2].hostID);
			rt[i-1].cost = 0;
			break;
			}
			}
		}
	}
	if(mode == 2)	//calculation made after change in the update packet
	{
		if(hop == 999)	//For disable link scenario
		{	
			//Update the next next hop and cost
			for(i=0; i<5;i++)
			{
				if(rt[i].destn==node)
				{
					rt[i].nextHop = -1;
					rt[i].cost = inf; 
					break;
				}
			}
		}
		else
		{
			for(i=0; i < 5; i++)
			{
				if(rt[i].destn == node)
				{
					rt[i].nextHop = node;
					rt[i].cost = hop;
				}
			}
		}
		
	}
	if(mode == 3)
	{
		for(i=0; i < 5; i++)
		{
			for(j=0;j<5;j++)
			{
				if(rt[i].destn==up.idn[j])
				{
					if(rt[i].destn == node)
					{	rt[i].cost = up.costn[j];
						rt[i].nextHop = hop;
					}
				}
			}
		}
	}	
}
//[PA3] Routing Table End

//[PA3] Update Packet  Start
void packetUpdate(int mode, int node, int cost_change)
{	
	static int i,j,k,f,hID,senderID,senderIDcost,senderIndex,nodes,left_over[5];
	static int host_index;
	uint32_t x; 
	//printf("the value of mode: %d %d %d\n",mode,node, cost_change);
	for(i = 0 ; i < totalSvrs ; i++)
	{
		if(ni[1].hostID == si[i].id)
		{
			host_index = i; 
			break;
		}
		
	}
	if ( mode == 1)
	{
		//Update Packet from SI
		up.numof = 5;
		inet_pton(AF_INET,si[host_index].ipAddr,&(up.srvip));
		up.srvport = si[host_index].port;
		
		//Initialize
		for(i = 0 ; i < 5; i++)
		{
			//inet_pton(AF_INET,"0.0.0.0",&(up.srvip_n[i]));
			up.srvportn[i] = zero;
			up.padn[i] = zero;
			up.idn[i] = zero;
			up.costn[i] = 999;
	 	}
		
		//Fill the details corresponding through Srv info
		for(i = 0 ; i < totalSvrs; i++)
		{
			inet_pton(AF_INET,si[i].ipAddr,&(up.srvip_n[i]));
			up.srvportn[i] = si[i].port;
			up.idn[i] = si[i].id;	
		}
		for(i=1;i<6;i++)
		{
			for(j=0; j<5;j++)
			{
				if(i == up.idn[j])
				{
					f = 1;
					break;
				}
			}
			if(f!=1)
			{
				left_over[k] = i;
				k++;
			}
			else
			f=0; 
		}
		k=0;
		for(i = 0 ; i < 5; i++)
		{
			if(up.idn[i] == 0)
			{
				up.idn[i] = left_over[k]; 
				k++;
			}
		}
		//Default cost to self is 0
		up.costn[host_index] = zero;
		//Fill in the cost in the packet
		for(i=0; i < 5; i++)
		{
			for(j=0; j < totalSvrs; j++)
			{
				if(ni[i].nID == up.idn[j])
				{
					up.costn[j] = ni[i].cost;
					
				}
			}
		}
		routerUpdate(1,1,1);
	}
	
	if( mode == 2 )	//Just updating the cost of the link
	{
		for ( i = 0 ; i < 5; i++)
		{
			if(node == up.idn[i])
			{
				up.costn[i] = cost_change;
				if (cost_change == 999)
				{
					routerUpdate(2,999,node);
				}
				else
				{
					routerUpdate(2,cost_change,node);
				}
				break;
			}
		}
	}
	
	if( mode == 3)
	{
		for(i= 0 ; i < 5; i++)	//Go through the entire packet of interPkt
		{
			if(interPkt.costn[i] == 0)
			{
				// Identify the sender Index and Sender ID
				senderID = interPkt.idn[i];
				senderIndex = i; 
			}
		}
		for(i = 0 ; i < 5; i++)			// Capture the cost to that node
		{
			if(up.idn[i] == senderID)
			{
				senderIDcost = up.costn[i];
			}
		}
		for(i= 0 ; i < 5; i++)			// Perform the check 
		{
			for(j = 0 ; j<5; j++)
			{
				if (up.idn[i] == interPkt.idn[j] && interPkt.idn[j] != interPkt.idn[senderIndex])
				{
					
					if(senderIDcost+interPkt.idn[j] < up.costn[i])
					{
						// Update the cost details and mention change in router table
						up.costn[i] = senderIDcost+interPkt.idn[j];
						up.srvip_n[i] = interPkt.srvip_n[j];
						up.srvportn[i] = interPkt.srvportn[j];
						up.idn[i] = interPkt.idn[j];
						routerUpdate(3,senderID,up.idn[i]);
					}
				}
			}
		}
		
	}
	
}
//[PA3] Update Packet  End

/**
 * isNumber function
 *
 * @param  input time interval
 * @return 1 EXIT_SUCCESS
 */
int isNumber(char*input)
{
        while(*input){
                if(!isdigit(*input))
                        return 0;
                else
                        input++;
        }
	return 1;
}
/**
 * isPath function
 *
 * @param  input path string
 * @return 1 EXIT_SUCCESS
 */

int isPath(char input[])
{
	
		if((fptr = fopen(input,"r"))==NULL)
			return 0;
		else
		{
	return 1;}
}

void inputStr()
{ 	
	
	fflush(stdout);
    fgets(input_string,30,stdin);
	input_string[strlen(input_string)-1] = '\0';
	//printf("The last enlement: %c\n",input_string[strlen(input_string)-2]);
    if(strlen(input_string)==0)
    {	perror("Input error: ");
			exit(6);
	}
}

int tokener(char is[])
{ 
	int i=0,j;
	char *ptr;
    for (j = 0 ; j<5;j++)
	{
		bzero(obCmd[j].cmd,20);
	}
	
	ptr = strtok(is," ");
	
    while (ptr!=NULL)
    { 	strcpy(obCmd[i].cmd,ptr);
		//printf("\t%s\n",obCmd[i].cmd);
        i++;
        ptr =strtok(NULL," ");
    }
	//printf("toke: Last char %c\n",obCmd[1].cmd[strlen(obCmd[1].cmd)-1]);
	return (i);
}

int cmdClas(int a)
{
	static char toTemp[30];
	int i; 
	//printf("Called cmdClas\n");
	
	for ( i = 0 ; i < strlen(obCmd[0].cmd); i++)
		{
			obCmd[0].cmd[i] = toupper(obCmd[0].cmd[i]) ;
			
		}

		//printf("The input first %s\n",obCmd[0].cmd);
		//printf("The comparison yields %d\n",strcmp(obCmd[0].cmd,"ACADEMIC"));
	for ( i = 0 ; i < strlen(obCmd[1].cmd); i++)
		{
			obCmd[1].cmd[i] = toupper(obCmd[1].cmd[i]) ;
			
		}
		//obCmd[1].cmd[i] = '\0';
		//printf("The input second %s\n",obCmd[1].cmd);
		//printf("The comparison yields %d ~~~%s \n",obCmd[1].cmd,strcmp(obCmd[1].cmd,"INTEGRITY"));
	if((strcmp(obCmd[0].cmd,"DUMP")==0) && a==1)
	{	
		return (1);
	}
	if((strcmp(obCmd[0].cmd,"ACADEMIC_INTEGRITY")==0))
	{
		return(2);
	}
	if((strcmp(obCmd[0].cmd,"STEP")==0) && a==1)
	{
		return(3);
	}
	if((strcmp(obCmd[0].cmd,"PACKETS")==0) && a==1)
	{
		return(4);
	}
	if((strcmp(obCmd[0].cmd,"DISPLAY")==0) && a==1)
	{
		return(5);
	}
	if((strcmp(obCmd[0].cmd,"UPDATE")==0) && a==4)
	{
		return(6);
	}
	if((strcmp(obCmd[0].cmd,"DISABLE")==0) && a==2)
	{
		return(7);
	}
	if((strcmp(obCmd[0].cmd,"CRASH")==0) && a==1)
	{
		return(8);
	}
	else
	{	printf("Incorrect command\n");
		return(0);
	}
}
/*
void updatecost(int node, int cost)			// Updates the cost in the Update packet
{
	int i;
	for ( i = 0  ; i < 5; i++)
	{
		if (node == up.idn[i])
		{
			if ( cost == 999)
			{
				up.costn[i] = inf;
			}
			else
			{
				up.costn[i] = cost;
			}
			break;
		}
	}
	routerUpdate(node, 0); 
}*/
void * serializer(char * bx)
{	
	uint16_t x;
	uint32_t y;
	size_t a,b;
	int i;
	char *bb = (char *)calloc(68,sizeof(char));
	char * tmp = bb ; 
	a = sizeof(uint16_t);
	b = sizeof(uint32_t);
	
	x = htons(up.numof);
	memcpy(bb,&x,a);
	bb = bb+a;
	
	x = htons(up.srvport);
	memcpy(bb,&x,a);
	bb = bb+a;
	
	memcpy(bb,&up.srvip,b);
	bb = bb+b;
	for(i=0;i<5;i++)
	{
		memcpy(bb,&up.srvip_n[i],b);
		bb = bb+b;
		
		x = htons(up.srvportn[i]);
		memcpy(bb,&x,a);
		bb = bb+a;
		
		
		x = htons(up.padn[i]);
		memcpy(bb,&x,a);
		bb = bb+a;
		
		x = htons(up.idn[i]);
		memcpy(bb,&x,a);
		bb = bb+a;
		
		if(up.costn[i] >=999)
		{
			up.costn[i] = inf;
		}
		
		x = htons(up.costn[i]);
		memcpy(bb,&x,a);
		bb = bb+a;
		
	}
	bb = bb-68;
	return tmp;

	/*
	uint16_t x;
	uint32_t y;
	//char * c; 
	//c = ba;
	char *ba = calloc(sizeof(char),68);
	int i; 
	x = htons(up.numof);
	memcpy(ba,&x,sizeof(up.numof));
	ba = ba+sizeof(up.numof);
	
	x = htons(up.srvport);
	memcpy(ba,&x,sizeof(up.srvport));
	ba = ba+sizeof(up.srvport);
	
	y = htonl(up.srvip);
	memcpy(ba,&up.srvip,sizeof(up.srvip));
	ba = ba+sizeof(up.srvip);
	
	for(i = 0 ; i<5 ; i++)
	{
		y = htonl(up.srvip_n[i]);
		memcpy(ba,&up.srvip_n[i],sizeof(up.srvip_n[i]));
		ba = ba+sizeof(up.srvip_n[i]);
		
		x = htons(up.srvportn[i]);
		memcpy(ba,&x,sizeof(up.srvportn[i]));
		ba = ba+sizeof(up.srvportn[i]);
		
		x = htons(up.padn[i]);
		memcpy(ba,&x,sizeof(up.padn[i]));
		ba = ba+sizeof(up.padn[i]);
		
		x = htons(up.idn[i]);
		memcpy(ba,&x,sizeof(up.idn[i]));
		ba = ba+sizeof(up.idn[i]);
		
		if(up.costn[i] >=999)
		{
			up.costn[i] = inf;
		}
		
		x = htons(up.costn[i]);
		memcpy(ba,&x,sizeof(up.costn[i]));
		ba = ba+sizeof(up.costn[i]);
		
	}
	ba = ba-68;
	return ba;
	*/
}
void deserialize(char buffer[],char *c)
{
	int i,id,j;
	memcpy(&interPkt.numof,c,sizeof(interPkt.numof));
	c = c+sizeof(interPkt.numof);
	interPkt.numof = ntohs(interPkt.numof);
	
	memcpy(&interPkt.srvport,c,sizeof(interPkt.srvport));
	c = c+sizeof(interPkt.srvport);
	interPkt.srvport = ntohs(interPkt.srvport);
	
	memcpy(&interPkt.srvip,c,sizeof(interPkt.srvip));
	c = c+sizeof(interPkt.srvip);
	
	for(i = 0 ; i<5 ; i++)
	{
		memcpy(&interPkt.srvip_n[i],c,sizeof(interPkt.srvip_n[i]));
		c = c+sizeof(interPkt.srvip_n[i]);
		
		memcpy(&interPkt.srvportn[i],c,sizeof(up.srvportn[i]));
		c = c+sizeof(interPkt.srvportn[i]);
		interPkt.srvportn[i] = ntohs(interPkt.srvportn[i]);
		
		memcpy(&interPkt.padn[i],c,sizeof(up.padn[i]));
		c = c+sizeof(interPkt.padn[i]);
		interPkt.padn[i] = ntohs(interPkt.padn[i]);
		
		memcpy(&interPkt.idn[i],c,sizeof(up.idn[i]));
		c = c+sizeof(interPkt.idn[i]);
		interPkt.idn[i] = ntohs(interPkt.idn[i]);
		
		memcpy(&interPkt.costn[i],c,sizeof(up.costn[i]));
		c = c+sizeof(interPkt.costn[i]);
		interPkt.costn[i] = ntohs(interPkt.costn[i]);
		if(interPkt.costn[i] == 0)
			id = interPkt.idn[i];
		if(interPkt.costn[i] ==inf)
		{
			interPkt.costn[i] = 999;
		}
	}
	
	cse4589_print_and_log((char *)"RECEIVED A MESSAGE FROM SERVER %d\n",id);
	for(i=1 ; i <6; i++)
	{
		for(j=1; j<6; j++)
		{
			if(interPkt.idn[j]== i)
			{
					cse4589_print_and_log((char *)"%-15d%-15d\n",interPkt.idn[j],interPkt.costn[j]);
			}
		}
	}
	
}

int sendPacket(int rcvStatus[])
{
	int i,bytes,s0,s1,s2,s3,s4,s5,flag = 0;
	static char bx[200];
	void *pkt_buf;
	//printf("Called here\n");
	for(i = 1; i<= 5; i++)
	{	
		if (rcvStatus[ni[i].nID] < 3 )
		{
		
			 rcvStatus[ni[i].nID]++;		//
			// send update packet
			if(ni[i].flag == 1)
			{
				//send packet
				pkt_buf = serializer(bx);
					//printf("Into the machine\n");
				if(i==0)
					{	if ((s0 = socket(n0->ai_family, n0->ai_socktype,
								n0->ai_protocol)) == -1) {
										perror("ID 1: socket");
							//continue;
						}
						
						if((bytes = sendto(sockfd,pkt_buf,68,0,n0-> ai_addr,n0->ai_addrlen))< 0)
						{
							perror("couldn't send n0\n");
						}
						//printf("n0: %d bytes\n",bytes);
						close(s0);
						flag = 1;
					}
				if(i==1)
					{
						if ((s1 = socket(n1->ai_family, n1->ai_socktype,
								n1->ai_protocol)) == -1) {
										perror("ID 1: socket");
							//continue;
						}
						if((bytes=sendto(sockfd,pkt_buf,68,0,n1-> ai_addr,n1->ai_addrlen))< 0)
							perror("couldn't send n1\n");
							//printf("n1: %d bytes\n",bytes);
						close(s1);
						flag = 1;
					}
				if(i==2)
					{
						if ((s2 = socket(n2->ai_family, n2->ai_socktype,
								n2->ai_protocol)) == -1) {
										perror("ID 2: socket");
							//continue;
						}
						if((bytes=sendto(sockfd,pkt_buf,68,0,n2-> ai_addr,n2->ai_addrlen))< 0)
						perror("couldn't send n2\n");
						//printf("n2: %d bytes\n",bytes);
						close(s2);
						flag = 1;
					}
				if(i==3)
				{
						if ((s3 = socket(n3->ai_family, n3->ai_socktype,
								n3->ai_protocol)) == -1) {
										perror("ID 3: socket");
							//continue;
						}
					if((bytes=sendto(sockfd,pkt_buf,68,0,n3-> ai_addr,n3->ai_addrlen))< 0)
						perror("couldn't send n3\n");
						//printf("n3: %d bytes\n",bytes);
						close(s3);
						flag = 1;
				}
				if(i==4)
				{
						if ((s4 = socket(n4->ai_family, n4->ai_socktype,
								n4->ai_protocol)) == -1) {
										perror("ID 4: socket");
							//continue;
						}
					if((bytes=sendto(sockfd,pkt_buf,68,0,n4-> ai_addr,n4->ai_addrlen))< 0)
					{
						perror("couldn't send n4\n");
						
					}
					//printf("n4: %d bytes\n",bytes);
					close(s4);
						flag = 1;
				}
				if(i==5)
				{
						if ((s5 = socket(n5->ai_family, n5->ai_socktype,
								n5->ai_protocol)) == -1) {
										perror("ID 5: socket");
							//continue;
						}
					if((bytes=sendto(sockfd,pkt_buf,68,0,n5-> ai_addr,n5->ai_addrlen))<0)
						perror("couldn't send n5\n");
						//printf("n5: %d bytes\n",bytes);
					close(s5);
						flag = 1;
				}

			}
		}
		else		//time to change entries to inf
		{
			rcvStatus[ni[i].nID] = 0; 
			ni[i].flag = 0;
			// update the packet entry to inf
			// updatecost(ni[i].nID,999);
			packetUpdate(3,ni[i].nID,999);
		}
	}
	return flag;
}

void display_routing()
{
	int i, j;
	for( i = 1 ; i < 6 ;i++)
	{
		for ( j = 0 ; j < 5 ; j++)
		{
			if(rt[j].destn == i)
			{	
				if(rt[j].cost >= 999)
				{	
					cse4589_print_and_log((char *)"%-15d%-15d%-15d\n",rt[j].destn,rt[j].nextHop,inf);
				}
				else
				{	cse4589_print_and_log((char *)"%-15d%-15d%-15d\n",rt[j].destn,rt[j].nextHop,rt[j].cost);
				}
			}
		}
	}
	
}
/*
char * serial_dump(char*buffer)
{	int i; 
	for(i=0; i < 5; i++)
	{	printf("----For %d----\n",i);
		
		memcpy(buffer,&(htons(rt[i].destn)),sizeof(rt[i].destn));
		printf("destn: %d\n",rt[i].destn);
		buffer = buffer+sizeof(rt[i].destn);
		memcpy(buffer,&(htons(rt[i].nextHop)),sizeof(rt[i].nextHop));
		printf("nextHop: %d\n",rt[i].nextHop);
		buffer = buffer+sizeof(sizeof(rt[i].nextHop));
		memcpy(buffer,&(htons(rt[i].cost)),sizeof(rt[i].cost));
		printf("cost: %d\n",rt[i].cost);
		buffer = buffer+sizeof(rt[i].cost);
	}
	return buffer;
}*/
/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log();	
	/*Clear LOGFILE and DUMPFILE*/
	fclose(fopen(LOGFILE, "w"));
	fclose(fopen(DUMPFILE, "wb"));

	/*Start Here*/
	int opt,lineNum,timeout,i,j,k,v,flg = 0,pq; 
	char topo_input[20][20];
	static char prompt[10],brk[2],charPort[16];
	static int disable_neigh;
	uint32_t their_ipadd;
	strcpy(prompt,"PA-3 > ");
	if (argc!=5)
	{
		printf("\nInsufficient parameters: \n");
		printf("Number of Parameters %d\n",argc);
		printf("Usage -t (path to topology file) -i (time interval)\n");
		return -1;
	}
	if ( strcmp(argv[1],"-t") == 0 && strcmp(argv[3],"-i") == 0 || strcmp(argv[1],"?") == 0)
	{
		if (strcmp(argv[1],"?") == 0)
		{
			printf("Usage -t (path to topology file) -i (time interval)\n");
			return -1;
		}
		else{
		if (!isPath(argv[2]))
		{
			fprintf(stderr,"Invalid value for -t\n");
			return -1;
		}
		if (!isNumber(argv[4]))
		{
			fprintf(stderr,"Invalid value for -i\n");
			return -1;
		}
		}
	}
	else
	{
		printf("Unidentified parameters");
	}
	timeout = atoi(argv[4]);
	
	//**************************//
	//	File read	   //			
	topology_reader();
	//**************************//
	//	Create Update Packet    //
	packetUpdate(1,1,1);
	
	//*************************//
	//	Sockets and Binds      //

	int rv;
	int numbytes,indx;
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
	char *buffer, *ptr, *p,*c;
	char *q;
	static char rcvBuffer[200];
	static char their_ip[20],router_packet[30];
	struct sockaddr_in *new_their;
	
	struct sockaddr_storage their_addr;
	struct interim_pkt interimIP;
	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	socklen_t addr_len;
	//char s[INET6_ADDRSTRLEN];
	
	FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	
	inttochar(charPort,hostPort);
	
	//neigh.ai_family = AF_INET;
	//neigh.ai_socktype = SOCK_DGRAM;
	
	for(i = 0 ; i<= 5;i++)
	{
		//printf("%d---> flag %d\n",ni[i].nID,ni[i].flag);
		if( ni[i].flag != 1)
		{
			continue;
		}
		printf("\n%s %s socket",ni[i].nIP,ni[i].port);
		if(i==0)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n0)) != 0) {
				fprintf(stderr, "1019:getaddrinfo: %s\n", gai_strerror(rv));
				//printf("IP %s, PORT %s\n",ni[i].nIP, ni[i].port);
				return 1;
				}
		}
		if(i==1)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n1)) != 0) {
				fprintf(stderr, "1026:getaddrinfo: %s\n", gai_strerror(rv));
				//printf("IP %s, PORT %s\n",ni[i].nIP, ni[i].port);
				return 1;
				}
		}
		if(i==2)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n2)) != 0) {
				//printf("IP %s, PORT3 %c\n",ni[i].nIP, ni[i].port[strlen(ni[i].port)-3]);
				fprintf(stderr, "1033:getaddrinfo: %s\n", gai_strerror(rv));
				
				return 1;
				}
		}
		if(i==3)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n3)) != 0) {
				//printf("printf IP %s\n",ni[i].nIP);
				fprintf(stderr, "1040:getaddrinfo: %s\n", gai_strerror(rv));
				//printf("IP %s, PORT %s\n",ni[i].nIP, ni[i].port);
				return 1;
				}
		}
		if(i==4)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n4)) != 0) {
				fprintf(stderr, "1047:getaddrinfo: %s\n", gai_strerror(rv));
				//printf("IP %s, PORT %s\n",ni[i].nIP, ni[i].port);
				return 1;
				}
		}
		if(i==5)
		{	if ((rv = getaddrinfo(ni[i].nIP, ni[i].port, &hints, &n5)) != 0) {
				fprintf(stderr, "1054:getaddrinfo: %s\n", gai_strerror(rv));
				//printf("IP %s, PORT %s\n",ni[i].nIP, ni[i].port);
				return 1;
				}
		}	
	}
	
	int my_index = get_server_index();
	//printf("\nlistening on %d",si[my_index].port);
	char port_str[5];
	sprintf(port_str,"%d",si[my_index].port);
	if ((rv = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	
	
		if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
				servinfo->ai_protocol)) == -1) {
			perror("Server: socket");
			//continue;
		}

		if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
			close(sockfd);
			perror("Server: bind");
			
		}

	freeaddrinfo(servinfo);
	
	FD_SET(0,&master);
	FD_SET(sockfd, &master);
	
    // keep track of the biggest file descriptor
    fdmax = sockfd; // so far, it's this one
	//printf("Value of fdmax : %d\n",fdmax);
	//Select begins
	
	printf("%s",prompt);
	fflush(stdout);
	while(1)
	{
		fflush(stdout);
		read_fds = master; // copy it
        if ((selRet = select(fdmax+1, &read_fds, NULL, NULL, &tv )) == -1) {
            perror("select");
            exit(1);
        }
		else if (selRet == 0)
		{
			//Timeout Handling is required
			//printf("The timeout has occured\n");
			tv.tv_sec = timeout;
			sendPacket(rcvStatus);
			printf("%s",prompt);
			fflush(stdout);
		}
		else 				//When it is non-zero and positive 
		{
			for ( pq = 0 ; pq < fdmax ; pq++)
			{	//printf("Value of i: %d\n",pq);
				if (FD_ISSET(STDIN,&read_fds))
				{
					//User Inputs
					fflush(stdout);
					inputStr();
					//printf("Taken in the input\n");
					param = tokener(input_string);
					//printf("main: first %s\n",obCmd[0].cmd);
					//printf("main: second %s\n",obCmd[1].cmd);
					//printf("Parsed the input\n");
					indx = cmdClas(param);
					//printf("after index: %d\n",indx);
					if(indx == 1)	//dump
					{ 
						q = serializer(router_packet);
						if(cse4589_dump_packet(q,68) <=-1)
						{
							cse4589_print_and_log((char *)"%s:%s\n","DUMP","Unable to write into file");
						}
						cse4589_print_and_log((char *)"%s:SUCCESS\n","DUMP");
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 2)	//Academic 
					{
						cse4589_print_and_log((char *)"I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 3)		//STEP
					{
						if(sendPacket(rcvStatus)==1){
						cse4589_print_and_log((char *)"%s:SUCCESS\n","STEP");
						}
						else{
						cse4589_print_and_log((char *)"%s:%s\n","STEP","Unable to send packets");							
						}
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 4)	//packets
					{
						cse4589_print_and_log((char *)"%-15d\n",packets);
						cse4589_print_and_log((char *)"%s:SUCCESS\n","PACKETS");
						packets = 0 ;
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 5)		//Display
					{
						display_routing();
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 6)			//Update
					{
						if(atoi(obCmd[1].cmd) == ni[1].hostID)
						{
							if(ni[atoi(obCmd[2].cmd)].flag == 1)
							{
								if((obCmd[3].cmd,"inf")||(obCmd[3].cmd,"INF"))
								{
									packetUpdate(3,atoi(obCmd[2].cmd),999);
								}
								else
								packetUpdate(3,atoi(obCmd[2].cmd),atoi(obCmd[3].cmd));
							}
							else
							{
								cse4589_print_and_log((char *)"%s:%s\n","DUMP","Entered node value is not neighbor of the server");
							}
						}
						else
						{
							cse4589_print_and_log((char *)"%s:%s\n","DUMP","Entered node is not the server");
						}
						cse4589_print_and_log((char *)"%s:SUCCESS\n","UPDATE");
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 7)		//Disable
					{	
						if(neigh_false[atoi(obCmd[1].cmd)]==0)
						{
							//Update routertable
							//update neigh table
							for(v=0;v<5;v++)
							{
								if(up.idn[v] == atoi(obCmd[1].cmd))
								{
									disable_neigh = up.idn[v];
								}
							}
							neigh_false[atoi(obCmd[1].cmd)] = 1;
							packetUpdate(2,disable_neigh,999);
						}
						else
						{
							cse4589_print_and_log((char *)"%s:%s\n","DUMP","Entered value is not neighbor of the server");
						}
						cse4589_print_and_log((char *)"%s:SUCCESS\n","DISABLE");
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 8)				//simulate crash
					{
						while(1)
						{
							fflush(stdout);
							fgets(brk,30,stdin);
							if(strlen(brk)> 0)
								break;
						}
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
					if(indx == 0)
					{
						//printf("The available commands are as follows: \n");
						printf("%s",prompt);
						fflush(stdout);
						continue;
					}
				}
				else if(FD_ISSET(pq,&read_fds))
				{
					packets++;
					//printf("Received data\n");
					// We receive data
					if(numbytes = recvfrom(pq,rcvBuffer,200-1,0,(struct sockaddr *) &their_addr, &addr_len) < 0)
					{
						printf("Error in recv:\n");
						break;
					}
					// We check the sender 
					new_their = (struct sockaddr_in*)&their_addr;
					
					their_ipadd = ntohl(new_their->sin_addr.s_addr);
					
					for(k=0; k<5;k++)
					{
						//printf("%d their- %d srvip %d\n",k,their_ipadd,up.srvip_n[k]);
						if(their_ipadd == up.srvip_n[k])
						{
							//authorize deserialize
							//printf("Receiving into deserialize\n");
							c = rcvBuffer;
							deserialize(rcvBuffer,c);
							for(v=0;v<5;v++)
							{
								if(up.srvip_n[k] == interPkt.srvip_n[v])
								{
									if(interPkt.costn[v]!=999)
									{										
										packetUpdate(3,1,1);
									}
									
								}
							}
							break;
						}
					}
					
					/*
					inet_ntop(AF_INET,&their_ipadd,their_ip,20);

					for ( k = 0 ; k < totalNeigh; k++)
					{	
						if (strcpy(ni[k].nIP,their_ip)==0)
						{
							if(neigh_false[ni[k].nID] ==  0)
							{	
								//accept and validate
								// We receive the data in interim_pkt 
								c = rcvBuffer;
								deserialize(rcvBuffer,c);
								
							}
							
						}
					}
					*/
					
					// and update the packet
				}
			}
		}
	}
	return 0;
}
