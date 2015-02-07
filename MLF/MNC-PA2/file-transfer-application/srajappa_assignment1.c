/**
 * @srajappa_assignment1
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
/**
 * @srajappa_assignment1
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define STDIN 0
#define MAXSIZE 100

char input_string[30];
char myHName[50],myipAdd[INET6_ADDRSTRLEN],svripAdd[INET6_ADDRSTRLEN];
char service[10];
char prompt[10] = "[PA-1]$";

struct colCmd
{	char cmd[10]; //individual token string
}obCmd[10];

void inputStr();
int tokener(char []);
int cmdClas(int a);
void my_reg();
void fMyip();
void fCreator();
void fMyport(char []);
void fHelp();

int main(int argc, char *argv[])
{	//printf("dummy\n");
	if(argc!=3)
	{	
		if((argv[1]!="s")||(argv[1]!="c"))
			printf("The mode must be either \nSERVER--\"S\" OR CLIENT--\"C\"\n");
		if (atoi(argv[2])<1024 ||atoi(argv[2])>65000)
			{printf("\nWrong PORT input: The PORT number should lie\n in the range 1025-65535\n");

			printf("\nInsufficient arguments. Please Enter again\n");}
		
		exit(0);
	}
	else
{	//printf("sind\n");
	int fdSock,fdASock,fdMax;
	int i,j,nbr,addrlen,param = 0,len,indx;
	char storage[50],myHName[40];

	struct addrinfo ret,*key,*t;
	const struct addrinfo *pa;
	struct sockaddr_storage remoteaddr;
	struct sockaddr_in	resv;
	fd_set dlList,dlCpy;

	printf("[PA-1]$  ");
	fflush(stdout);
	memset(&ret,0,sizeof(ret));
	ret.ai_family = AF_UNSPEC;
	ret.ai_socktype = SOCK_STREAM;
	ret.ai_flags = AI_PASSIVE;
	
	
	if(getaddrinfo(NULL,argv[2],&ret,&key)==-1)
	{	perror("Getaddrinfo:");
		exit(1);
	}
		
	for(t=key; t!=NULL; t=key->ai_next)
	{
		fdSock = socket(t->ai_family,t->ai_socktype,0);
		if (fdSock < 0)
		{	continue;
		}
		if( bind(fdSock,t->ai_addr,t->ai_addrlen) < 0)
		{ 	close(fdSock);
			continue;
		}
		break;
	}
	if (t == NULL)
	{	printf("Unable to Bind");
		exit(2);
	}

	len = sizeof(resv);
	//fetch the ipaddress
	//getsockname(fdSock,(struct sockaddr*)&resv,&len);	

	
	
	
	freeaddrinfo(key);
	if(listen(fdSock,6)<0)
	{	perror("Listen: ");
		exit(3);
	}
	FD_ZERO(&dlList);
	FD_ZERO(&dlCpy);
	
	FD_SET (STDIN,&dlList);
	FD_SET (fdSock,&dlList);
	fdMax = fdSock+1;
	
	//printf("All listening done here\n");
	while(1)
	{	//printf("Inside while loop\n");
		//printf("[PA-1]$  ");
		dlCpy = dlList;
		if (select(fdMax+1,&dlList,NULL,NULL,NULL)==-1)
		{	perror("SELECT: ");
			continue;
		}
		//printf("Select is done\n");
		for( i = 0 ; i <fdMax ; i++)
		{	if (FD_ISSET(STDIN,&dlList))
			{	//If someone typed in something.
				
				while(i==STDIN)
				{		
						inputStr();
						//printf("Taken in the input\n");
						param = tokener(input_string);
						//printf("Parsed the input\n");
						indx = cmdClas(param);
						//printf("after index: %d\n",indx);
						if(indx==1)
						{	//fCreator();
							//i=0;
							//inputStr();
							printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity\n");
							fflush(stdout);
							continue;
						}
						if(indx==2)
						{	fHelp();
							//inputStr();
							continue;
						}
						if(indx==3)
						{	fMyip();
							//inputStr();
							continue;
						}
						if(indx==4)
						{	fMyport(argv[2]);
							//inputStr(argv[2]);
							continue;
						}
						if(indx==5)
						{	my_reg(myHName,myipAdd,argv[2]);
						}
				
				}
			}
			else if(FD_ISSET(i,&dlList))
			{	//Indicates that there is a connection
				if (i == fdSock)
				{	
					addrlen = sizeof (remoteaddr);
					fdASock = accept(i,(struct sockaddr*)&remoteaddr,&addrlen);
					if (fdASock == -1)
					{	perror("Accept: ");
						continue;
					}
					else
					{	FD_SET(fdASock,&dlList);
						if(fdASock>fdMax)
							fdMax = fdASock;
					}
					
					//after performing an accept
				}
			}
			else
			{	//perform a recv()
				if((nbr = recv(i,storage, sizeof(storage),0))<=0)
				{	if(nbr == 0)
					{	printf("There is a connection problem\n");
					}
					else
						perror("Receive: ");
					close(i);
					FD_CLR(i,&dlList);
						
				}
				else
				{ //This is section where we have some data with us
					printf("Here is where things go for inspection\n");
				}
			}
		}
	}
	return(0);
}
}

void inputStr()
{ 	
	printf("%s",prompt);
	fflush(stdout);
    fgets(input_string,MAXSIZE,stdin);
    if((strlen(input_string)>0)&&(input_string[strlen(input_string)-1]=='\n'))
        input_string[strlen(input_string)-1] = '\0';
    else
    	{	perror("Input error: ");
			exit(6);
		}
}

int tokener(char is[])
{ 
	int i=0;
	char *ptr;
    ptr = strtok(is," ");
    while (ptr!=NULL)
    { 	strcpy(obCmd[i].cmd,ptr);
        i++;
        ptr =strtok(NULL," ");
    }
	//printf("Tokener: Oprn done\n");
	//printf("The input: %s and I: %d\n",obCmd[0].cmd,i);
	return (i);
}

int cmdClas(int a)
{
	if((strcmp(obCmd[0].cmd,"CREATOR")==0) && a==1)
	{	
		return (1);
	
	}
	
	if((strcmp(obCmd[0].cmd,"HELP")==0) && a==1)
	{	return (2);
	}
	
	if((strcmp(obCmd[0].cmd,"MYIP")==0) && a==1)
	{	return (3);
	}
	
	if((strcmp(obCmd[0].cmd,"MYPORT")==0) && a==1)
	{	return (4);
	}
	
	if((strcmp(obCmd[0].cmd,"REGISTER")==0) && a==3)
	{	return (5);
	}
	
	
	else
	{	printf("Wrong Command Input\n");
	}
	//else if(obCmd[1].cmd == "
}

/*void fCreator()
{	printf("I have read and understood the course academic integrity policy located at http://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/index.html#integrity");

}*/

void fHelp()
{ printf("DEBUG");
}

void fMyport(char a[])
{ 	printf("%s",a);
	fflush(stdout);
}


void fMyip(char mode[])
{	int sockfd;
	//set the variables of the structure
	struct addrinfo hints,*res;
	struct sockaddr_in resv;
	char ref[2]="S";
	int val;
	struct socklen_t len;
	memset(&hints,0,sizeof hints);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	//__________________________________	

	//Get address for socket
	if((val=getaddrinfo("8.8.8.8","4545",&hints,&res))==-1)
	{	printf("Getaddrinfo\n");
	}
	//__________________________________

	//Create a socket
	sockfd = socket(res->ai_family,res->ai_socktype,0);
	if (sockfd == -1)
	{	printf("Socket problem\n");
	}
	//__________________________________

	//bind the socket
	/*if ((val=bind(sockfd,res->ai_addr,res->ai_addrlen))==-1)
	{	printf("Bind problem\n");
	}*/
	//__________________________________

	//connect()
	if((val = connect(sockfd,res->ai_addr,res->ai_addrlen))==-1)
	{	printf("Connect error");
	}
	//__________________________________
	len = sizeof(resv);
	//fetch the ipaddress
	printf("\nconnect done\n");
		
	if((getsockname(sockfd,(struct sockaddr*)&resv,&len))<0)
		printf("Problem!!\n");
	//getnameinfo((struct sockaddr*)&resv, sizeof(resv),myHName,sizeof(myHName),service,sizeof(service),0);
	if (strcmp(mode,ref)==0)
		{	strcpy(svripAdd,inet_ntoa(resv.sin_addr));
			strcpy(myipAdd,inet_ntoa(resv.sin_addr));
		}
	else
		strcpy(myipAdd,inet_ntoa(resv.sin_addr));
	printf("%s\n",myipAdd);
	
	//__________________________________
}
void my_reg()
{	printf("###");
}