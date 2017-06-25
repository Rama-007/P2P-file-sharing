#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
int fd[2];
int closetime=0;
int childpid;


void client(int cport)
{
	int connected;
	struct sockaddr_in server_address;
	struct hostent *host;
	char send_data[1024],recv_data[1024];
	int byte_recv;
	host=gethostbyname("127.0.0.1");
	char *cutcommand;
	int addr_len;
	time_t seconds;
	if((connected=socket(AF_INET, SOCK_STREAM ,0))==-1)
	{
		perror("socket");
		exit(1);
	}
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(cport);
	server_address.sin_addr=*((struct in_addr *)host->h_addr);
	bzero(&(server_address.sin_zero),8);
	while(connect(connected, (struct sockaddr*)&server_address, sizeof(struct sockaddr))==-1);
	char copy[1024];
	addr_len=sizeof(struct sockaddr);
	char output[100000];
	seconds=time(NULL);
	long long flagt=0;
	while(1)
	{
		system("touch server.log");
		printf("%d\n",flagt);
		if(flagt<2)
		{
		printf("prompt>");
		gets(send_data);
		strcpy(copy,send_data);
		char log[1024];
		sprintf(log,"echo %s >> server.log",send_data);
		system(log);
		if ((strcmp(send_data , "q") == 0 || strcmp(send_data , "Q") == 0) || closetime ==1)
		{
			send(connected, send_data,strlen(send_data), 0); 
			fflush(stdout);
			close(connected);
			kill(childpid, SIGTERM);
			system("rm downloaded");
			exit(0);
			break;
		}
		cutcommand=strtok(copy," ");
		if(cutcommand!=NULL)
		{
			if(strcmp(cutcommand,"hash")==0)
			{
				cutcommand=strtok(NULL," ");
				if(cutcommand!=NULL)
				{
					if(strcmp(cutcommand,"verify")==0)
					{
						cutcommand=strtok(NULL," ");
						if(cutcommand!=NULL)
						{
							strcpy(send_data,"HV");
							strcat(send_data,cutcommand);
							send(connected,send_data ,strlen(send_data),0);
							while(1)
							{
								byte_recv=recv(connected,recv_data,1024,0);
								recv_data[byte_recv]='\0';
								if(strcmp(recv_data,"The End")==0)
									break;
								fwrite(recv_data,1,byte_recv,stdout);
								//printf("%s\n",output);
							}
							continue;
							flagt++;
						}
					}
					else if(strcmp(cutcommand,"checkall")==0)
					{
						strcpy(send_data,"HC");
						send(connected,send_data ,strlen(send_data),0);
						while(1)
						{
							byte_recv=recv(connected,recv_data,1024,0);
							recv_data[byte_recv]='\0';
							if(strcmp(recv_data,"The End")==0)
								break;
							fwrite(recv_data,1,byte_recv,stdout);
						}
						continue;
						flagt++;
					}
					else
					{
						perror("wrong use of hash");
					}
				}
			}
			else if(strcmp(cutcommand,"index")==0)
			{
				cutcommand=strtok(NULL," ");
				if(cutcommand!=NULL)
				{
					printf("%s\n",cutcommand );
					if(strcmp(cutcommand,"shortlist")==0)
					{
						cutcommand=strtok(NULL," ");
						if(cutcommand!=NULL)
						{
							strcpy(send_data,"IS");
							strcat(send_data,cutcommand);
							strcat(send_data," ");
							cutcommand=strtok(NULL," ");
							if(cutcommand!=NULL)
							{
								strcat(send_data,cutcommand);
								send(connected,send_data,strlen(send_data),0);
								while(1)
								{
									byte_recv=recv(connected,recv_data,1024,0);
									recv_data[byte_recv]='\0';
									if(strcmp(recv_data,"The End")==0)
										break;
									fwrite(recv_data,1,byte_recv,stdout);
								}
								continue;
								flagt++;
							}
						}
						else
						{
							perror("Wrong usage. Type index shortlist <filename> \n");
						}
					}
					else if(strcmp(cutcommand,"longlist")==0)
					{
						
							strcpy(send_data,"IL");							
							send(connected,send_data,strlen(send_data),0);
							while(1)
							{
								byte_recv=recv(connected,recv_data,1024,0);
								recv_data[byte_recv]='\0';
								if(strcmp(recv_data,"The End")==0)
									break;
								fwrite(recv_data,1,byte_recv,stdout);
							}
							continue;	
					}
					else if(strcmp(cutcommand,"regex")==0)
					{
						cutcommand=strtok(NULL," ");
						if(cutcommand!=NULL)
						{
							strcpy(send_data,"IR");
							strcat(send_data,cutcommand);
							send(connected,send_data,strlen(send_data),0);
							while(1)
							{
								byte_recv=recv(connected,recv_data,1024,0);
								recv_data[byte_recv]='\0';
								if(strcmp(recv_data,"The End")==0)
									break;
								fwrite(recv_data,1,byte_recv,stdout);
							}
							continue;
						}
						else
						{
							perror("regex not given");
							continue;
						}
					}
				}
			}
			else if(strcmp(cutcommand,"download")==0)
			{
				cutcommand=strtok(NULL," ");
				if(cutcommand!=NULL)
				{
resend:				strcpy(send_data,"D");
					if(cutcommand!=NULL)
					{
						if (strcmp(cutcommand,"UDP")==0)
						{
							strcat(send_data,"U");
						}
						else if(strcmp(cutcommand,"TCP")==0)
						{
							strcat(send_data,"T");
						}
						else
						{
							perror("Enter either TCP or UDP");
							continue;
						}
					}
					else
					{
						perror("Enter either TCP or UDP");
							continue;
					}
					cutcommand=strtok(NULL," ");
					char tokka[1024];
					printf("cutcommand-%s\n",cutcommand );
					fflush(stdout);
					sprintf(tokka,"echo %s >> downloaded",cutcommand);
					system(tokka);
				//	system("sort downloaded | uniq > downloaded");
					strcat(send_data,cutcommand);	
					if(1)
					{
						if(send_data[1]=='T')
							write(connected,send_data,1024);
						else
							sendto(connected,send_data,1024,0,(struct sockaddr *)&server_address, sizeof(struct sockaddr));
						FILE *fileptr1=fopen(cutcommand,"w");
						memset(recv_data,0,1024);
						while(1)
						{
							if(send_data[1]=='T')
								byte_recv=read(connected,recv_data,1024);
							else
								byte_recv=recvfrom(connected,recv_data,1024,0,(struct sockaddr *)&server_address, sizeof(struct sockaddr));
							//recv_data[byte_recv]='\0';
							printf("%s\n",recv_data);
							fflush(stdout);	
							if(strcmp(recv_data,"The End")==0)
								break;
							fwrite(recv_data,1,byte_recv,fileptr1);
						}
						fclose(fileptr1);
						if(send_data[1]=='T')
						{
						byte_recv=read(connected,recv_data,1024);
						char delimt[1024];	
						strcpy(delimt,recv_data);
						char *hash=strtok(delimt," ");
						if(hash==NULL)
						{
							hash=strtok(NULL," ");
						}				
						if(strcmp(hash,cutcommand)==0)
						{
							hash=strtok(NULL," ");
							char filname[1024],command[1024],packet[1024],temp[1024];
							strcpy(filname,hash);
							sprintf(command,"mdsum %s | awk '{print $1}' > t1",filname);
							system(command);
							FILE *f1=fopen("t1","r");
							fgets(packet,1024,f1);
							packet[strlen(packet)-1]='\0';
							fclose(f1);
							system("rm t1");
							if(strcmp(packet,hash)==0)
							{
								perror("Tranfer complete\n");
							}
							else
							{
								perror("Error");
								goto resend;
							}
						}
						}
					}
					/*else if(send_data[1]=="U")
					{
						sendto(connected,send_data,1024,0,(struct sockaddr *)&server_address, &addr_len);
						FILE *fileptr1=fopen(cutcommand,"w");
						memset(recv_data,0,1024)
					}*/
				}
			}
			else
			{
				send(connected,send_data,strlen(send_data),0);
				system("touch downloaded");
			}
			flagt++;
		}
		}
		else{
		char buffer2[1024],buffer3[1024],command1[1024];
		system("sort downloaded | uniq > edokati");
		system("cat edokati > downloaded");
		system("rm edokati");
		FILE *fp2=fopen("downloaded","r");

		while(fgets(buffer2,1024,fp2) !=NULL)
		{
			
			buffer2[strlen(buffer2)-1]='\0';
			strcpy(buffer3,"ls -l --time-style=+%Y%m%d%H%M%S -t ");
			strcat(buffer3,buffer2);
			strcat(buffer3, " > tempapa ");
			system(buffer3);
			system(" awk '{print $6}' tempapa > tempapa1");
			FILE *fp3=fopen("tempapa1","r");
			fgets(command1,1024,fp3);
			command1[strlen(command1)-1]='\0';
			fclose(fp3);
			system("rm tempapa tempapa1");
		
			strcpy(send_data,"PP");
			strcat(send_data,buffer2);
			write(connected,send_data,1024);
			
				byte_recv=recv(connected,recv_data,1024,0);
				recv_data[byte_recv-1]='\0';
				while(recv_data[0]!='2')
				{
					byte_recv=recv(connected,recv_data,1024,0);
					recv_data[byte_recv]='\0';
				}
				
				char *hash1=strtok(recv_data," ");
				hash1=strtok(NULL," ");
				char rama[1024],rama1[3];
				
				int lengthe=strlen(recv_data);
				strncpy(rama,recv_data,lengthe-6);
				rama1[0]=recv_data[lengthe-3];
				rama1[1]=recv_data[lengthe-2];
				rama1[2]=recv_data[lengthe-1];
				int perm=atoi(rama1);
				sprintf(buffer3,"chmod %d ",perm);
				strcat(buffer3,buffer2);
				system(buffer3);
				printf("%s %ddad\n",rama,perm);
				fflush(stdout);
				
				if(strcmp(rama,command1)>0)
				{
					char deletes[1024];
					sprintf(deletes,"rm %s",buffer2);
					system(deletes);
					strcpy(send_data,"DT");
					strcat(send_data,buffer2);
					write(connected,send_data,1024);
					FILE *fileptr1=fopen(buffer2,"w");
					memset(recv_data,0,1024);
					while(1)
					{
						byte_recv=read(connected,recv_data,1024);
						//printf("%s\n",recv_data);
						//fflush(stdout);	
						if(strcmp(recv_data,"The End")==0)
						{
							break;
						}
						fwrite(recv_data,1,byte_recv,fileptr1);
					}
					fclose(fileptr1);
					byte_recv=read(connected,recv_data,1024);
					char delimt[1024];	
					strcpy(delimt,recv_data);
					char *hash=strtok(delimt," ");
					if(hash==NULL) 
					{
						hash=strtok(NULL," ");
					}				
					if(strcmp(hash,buffer2)==0)
					{
						hash=strtok(NULL," ");
						char filname[1024],command[1024],packet[1024],temp[1024];
						strcpy(filname,hash);
						strcpy(command,"mdsum ");
						strcat(command,filname);
						strcat(command," | awk `{print $1}' > t1");
						system(command);
						FILE *f1=fopen("t1","r");
						fgets(packet,1024,f1);
						packet[strlen(packet)-1]='\0';
						fclose(f1);
						system("rm t1");
						if(strcmp(packet,hash)==0)
						{
							perror("Tranfer complete\n");
						}
						else
						{
							perror("Error");
						}
					}
				}
		}
		fclose(fp2);
		flagt=0;
	}
		/*char direct[1024];
		getcwd(direct,1024);
		memset(recv_data,0,1024);
		write(connected,"DR",1024);
		char commander[1024];
		byte_recv=read(connected,recv_data,1024);
		sprintf(commander,"rsync -avz --delete \"%s\" \"%s\" ",direct,recv_data);
		printf("%s\n",commander );
		system(commander);*/
	}
}




void server(int sport)
{
	int sock;
	struct sockaddr_in server_address,client_address;
	int server_in_size;
	char regex[1024];
	char filename[1024];
	char copy[1024];
	int connected;
	char *cutcommand;
	char direct1[1024];
	char send_data[1024],recv_data[1024];
	int byte_recv;
	if((sock=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		perror("socket");
		exit(1);
	}
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(sport);
	server_address.sin_addr.s_addr=INADDR_ANY;
	bzero(&(server_address.sin_zero),8);//puts zeros

	if(bind(sock, (struct sockaddr *)&server_address,sizeof(struct sockaddr))==-1)
	{
		perror("unable to bind");
		exit(1);
	}

	if(listen(sock,5)==-1)
	{
		perror("Error");
		exit(1);
	}
	printf("\n Server waiting for client on port %d\n",sport);
	fflush(stdout);
	server_in_size=sizeof(struct sockaddr_in);
	connected=accept(sock,(struct sockaddr *) &client_address, &server_in_size);
	printf("\n Connection from %s,%d\n", inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));
	
	char command[1024];

	getcwd(direct1,1024);

	while(1)
	{
		byte_recv=recv(connected,recv_data,1024,0);
		recv_data[byte_recv]='\0';
		//printf("%d\n",byte_recv );
		if(strcmp(recv_data,"q")==0 || strcmp(recv_data,"Q")==0)
		{
			closetime=1;
			close(connected);
			break;
		}
		else if(recv_data[0]=='H' && recv_data[1]=='V')
		{
			strncpy(filename,(char*)recv_data+2,1024);
			printf("Verifying %s\n",filename);
			strcpy(command, "openssl md5 ");
			strcat(command,filename);
			strcat(command, " | cut -d\" \" -f2 > md5 ");
			system(command);
			strcpy(command,"date -r ./");
			strcat(command,filename);
			strcat(command, " > date");
			system(command);
			strcpy(command, "paste md5 date  > ver");
			system(command);
			system("rm md5 date ");

			FILE *fp=fopen("ver","r");
			if(fp==NULL)
			{
				perror("wrong File\n");
				char end[]= "The End";
				strcpy(send_data,end);
				write(connected,send_data,1024);
				continue;
			}
			memset(send_data,0,1024);
			int bytesent;
			while(!feof(fp))
			{
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				write(connected,send_data,1024);
			}
			system("rm ver");
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			write(connected,send_data,1024);
			fclose(fp);
			continue;
		}
		else if(recv_data[0]=='P' && recv_data[1]=='P')
		{
			strncpy(filename,(char*)recv_data+2,1024);
			char buffer3[1024],buffer4[1024];
			strcpy(buffer3,"ls -l --time-style=+%Y%m%d%H%M%S -t ");
			strcat(buffer3,filename);
			strcat(buffer3, " > tempapa ");
			system(buffer3);
			system(" awk '{print $6}' tempapa > tempapa1");
			strcpy(buffer3,"stat -c \"%a\"");
			sprintf(buffer4," %s > permission",filename);
			strcat(buffer3,buffer4);
			system(buffer3);
			system("paste tempapa1 permission > tempapa");
			FILE *ponier=fopen("tempapa","r");
			int bytesent;
			memset(send_data,0,1024);
			while(!feof(ponier))
			{
				bytesent=fread(send_data,1,1024,ponier);
				send_data[bytesent-1]='\0';
				write(connected,send_data,1024);
			}
			memset(send_data,0,1024);
			fclose(ponier);
			system("rm tempapa tempapa1 permission");
			continue;
		}
		else if(recv_data[0]=='H' && recv_data[1]=='C')
		{
			system("find . -type f -exec sh -c 'printf \"%s %s \n\" \"$(ls -l --time-style=+%Y%m%d%H%M%S $1 )\" \"$(md5sum $1 | cut -d\" \" -f1)\"' '' '{}' '{}' \\; | awk '{print $7, $6, $8}' > allcheck");
			FILE *fp = fopen("allcheck","r");
			if(fp==NULL)
			{
				perror("wrong file");
				continue;
			}
			memset(send_data,0,1024);
			int bytesent;
			while(!feof(fp))
			{
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				write(connected,send_data,1024);
			}
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			write(connected,send_data,1024);
			fclose(fp);
			continue;
		}
		else if(recv_data[0]=='D' && recv_data[1]=='R' )
		{
			getcwd(direct1,1024);
			write(connected,direct1,1024);
			continue;
		}
		else if(recv_data[0]=='I' && recv_data[1]=='L' )
		{
			printf("hie\n");
			fflush(stdout);
			system("ls -l --time-style=+%Y%m%d%H%M%S -t > sl");
			system("awk {'if ($7 !=\"sl\" && $7 !=\"slw\" && $7 !=\"slw2\") print \"  \" $7 \"  \" $5 \"  \" $6 \"  \" $1'} sl > slw");
			system("awk {' if(substr ($1,0,1)==\"d\") grade=\"directory\"; else if(substr ($1,0,1)==\"-\") grade=\"file\"; print grade'} sl > slw1");
			system("paste slw1 slw > sl");
			FILE *fp=fopen("sl","r");
			if(fp==NULL)
			{
			//	perror("wrong file");
				continue;
			}
			int bytesent,flag=0;
			memset(send_data,0,1024);
			while(!feof(fp))
			{
				if(flag<=1)
				{
					flag++;
					continue;
				}
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				write(connected,send_data,1024);
			}
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			write(connected,send_data,1024);
			fclose(fp);
			system("rm sl slw slw1");
			continue;	
		}
		else if(recv_data[0]=='I' && recv_data[1]=='S' )
		{
			strncpy(filename,(char*)recv_data+2,1024);
			cutcommand=strtok(filename," ");
			char uptime[1024],lowtime[1024];
			strcpy(uptime,cutcommand);
			cutcommand=strtok(NULL," ");
			strcpy(lowtime,cutcommand);
			system("ls -l --time-style=+%Y%m%d%H%M%S -t > sl");
			system("awk {' if(substr ($1,0,1)==\"-\") print \"file\"; else print \"Directory\"; print \"  \" $7 \"  \" $5 \"  \" $6 \"  \" $1'} sl > slww");
			FILE *fileptr1=fopen("slw","w");
			FILE *fileptr=fopen("sl","r");
			char buffer[1024];
			int counter=0;
			int place=0;
			while(fgets(buffer,1024,fileptr) !=NULL)
			{
				if(counter!=0)
				{
					if(counter!=1)
					{
						strcpy(copy,buffer);
						place=0;
						cutcommand=strtok(buffer," ");
						while(cutcommand!=NULL)
						{
							if(place==5)
							{
								if(strcmp(cutcommand,lowtime)<0 && strcmp(cutcommand,uptime)>0)
								{
									fprintf(fileptr1, "%s",copy );
								}
							}
							cutcommand=strtok(NULL," ");
							place++;
						}
					}
				}
				counter++;
			}
			fclose(fileptr1);
			fclose(fileptr);
			FILE *fp=fopen("slw","r");
			if(fp==NULL)
			{
				perror("wrong file");
				continue;
			}
			int bytesent;
			memset(send_data,0,1024);
			while(!feof(fp))
			{
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				write(connected,send_data,1024);
			}
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			write(connected,send_data,1024);
			fclose(fp);
			system("rm sl slw slww");
			continue;
		}
		else if(recv_data[0]=='I' && recv_data[1]=='R' )
		{
			strncpy(filename,(char*)recv_data+2,1024);
			cutcommand=strtok(filename," ");
			sprintf(command,"ls -lR | grep \"%s\" > ir",cutcommand);
			system(command);
			FILE *fp=fopen("ir","r");
			if(fp==NULL)
			{
				perror("wrong file");
				continue;
			}
			int bytesent;
			memset(send_data,0,1024);
			while(!feof(fp))
			{
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				write(connected,send_data,1024);
			}
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			write(connected,send_data,1024);
			fclose(fp);
			system("rm ir");
			continue;
		}
		else if(recv_data[0]=='D')
		{
			char array[1024];
			printf("%s\n",recv_data+2);
			fflush(stdout);
			strcpy(array,recv_data+2);
			char tokka[1024];
			sprintf(tokka,"echo %s >> downloaded",recv_data+2);
			system(tokka);
			//system("sort downloaded | uniq > downloaded");
			system("cat downloaded");
			FILE *fp=fopen(array,"r");
			if(fp==NULL)
			{
				perror("wrong file");
				char end[]= "The End";
				strcpy(send_data,end);
				write(connected,send_data,1024);
				continue;
			}
			int bytesent;
			memset(send_data,0,1024);
			while(!feof(fp))
			{
				bytesent=fread(send_data,1,1024,fp);
				send_data[bytesent]='\0';
				if(recv_data[1]=='T')
					write(connected,send_data,1024);
				else
					sendto(connected,send_data,1024,0,(struct sockaddr*)&client_address, sizeof(struct sockaddr));
			}
			memset(send_data,0,1024);
			char end[]= "The End";
			strcpy(send_data,end);
			if(recv_data[1]=='T')
			{
				write(connected,send_data,1024);
			}
			else
				sendto(connected,send_data,1024,0,(struct sockaddr*)&client_address, sizeof(struct sockaddr));
			if(recv_data[1]=='T')
			{
			write(connected,send_data,1024);
			fclose(fp);
			char name[1024];
			strcpy(name,array);
			char packet[1024];
			sprintf(command,"md5sum %s | awk '{print $2, $1}' > t1", name);
			system(command);
			strcpy(command,"stat -c%s ");
			strcat(command,name);
			strcat(command,"> t2");
			system(command);
			system("paste t1 t2 | awk '{print $1, $2, $3}' > md5; rm t1 t2 ");
			FILE *f1=fopen("md5","r");
			fgets(packet,1024,f1);
			packet[strlen(packet)-1]='\0';
			printf("length of packet now is %d\n",strlen(packet));
			printf("packet header : %s\n",packet);
			fclose(f1);
			system("rm md5");
			memset(send_data,0,1024);
			strcpy(send_data,packet);
			write(connected,send_data,1024);
			}
			else
			{
				fclose(fp);
			}
		}
		else
		{
			printf("%s\n",recv_data );
			fflush(stdout);
		}
	}
	
	close(sock);
}


int main()
{
	int pid,sport,choice=0;
	printf("Enter server port\n");
	scanf("%d",&sport);
	closetime=0;
	pipe(fd);
	childpid=getpid();
	pid=fork();
	if(!pid)
	{
		int cport;
		printf("Enter cport\n");
		scanf("%d",&cport);
		if(choice==0)
			client(cport);
	}
	else
	{
		if(choice==0)
			server(sport);
	}
	return 0;
}