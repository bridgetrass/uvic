#include "rdpFunctions.h"

int acks=0;
int syns=0;
int total_fin=0;
int total_data_bytes_rec=0;
int unique_data_bytes_rec=0;
int data_packets=0;
int unique_data_packets=0;
int RSTs=0;


pthread_mutex_t transmission_lock;

void write_to_file(char* my_file, int fin){
	packetNode *iter=front;
	int first_seq_num=fin+25;
	FILE * fp;
	fp = fopen (my_file, "w");
	int i=1;
	while(i<=fin){
		iter=front;
		while(iter!=NULL){
			if(iter->packet->counter==i){
				char *f = malloc(sizeof(char)*(iter->packet->head->payload +MAXHEAD));
				memset(f,'\0',iter->packet->head->payload + MAXHEAD);
				f = iter->packet->full_buffer+MAXHEAD;
				fprintf(fp,"%s",f);
				packetNode *remove;
				remove = rem_rec(i);
				
				i++;
			}
			else{	
				iter=iter->next;
			}

		}
		
		
		
		
	}

	fclose(fp);

}

void buildack (RDP_head *ack, RDP_head *syn){
	acks++;
	strcpy(ack->magic,"CSC361");
	ack->type=2;
	ack->seqno =0;
	ack->ackno =syn->seqno+1;
	ack->payload=0;
	ack->windowsize=MAXBUFLEN -MAXHEAD;
}

int sendPacket(int sockfd,socklen_t sender_len, struct sockaddr_in *sender_addr, char* buffer, int *numbytes){
	pthread_mutex_lock(&transmission_lock);
	if ((*numbytes = sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)sender_addr, sender_len)) == -1) {
   		perror("rdp: error in sendto()");
    		return -1;
	}
	pthread_mutex_unlock(&transmission_lock);
	return(1);

}

int receivePacket(int sockfd,socklen_t sender_len, struct sockaddr_in *sender_addr, char* buffer, int *numbytes){
	pthread_mutex_lock(&transmission_lock);
	if ((*numbytes =recvfrom(sockfd, buffer, MAXBUFLEN, 0,(struct sockaddr *)sender_addr, &sender_len)) == -1) {
		 perror("rdp: error on recvfrom()!");
		 return -1;
	}
	pthread_mutex_unlock(&transmission_lock);
	int ret_bytes = *numbytes;
	return(ret_bytes);
}

int connection(int sockfd,socklen_t sender_len, struct sockaddr_in *sender_addr){
	char buffer[MAXBUFLEN];
	int numbytes;
	memset(buffer,'\0',MAXBUFLEN);
	int select_result;
	fd_set read_fds;
	struct timeval timeout;
	timeout.tv_sec=2;
	timeout.tv_usec = 0;

	FD_ZERO( &read_fds );
	FD_SET(sockfd,&read_fds);
	select_result=select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
	if(select_result<1){
		return(-1);

	}
	if(FD_ISSET(sockfd,&read_fds)){
		receivePacket(sockfd,sender_len,sender_addr,buffer,&numbytes);
		
	}
	// make RDP_head from the char array recieved from the sender, build ack to be sent
	RDP_head *ack = malloc(sizeof(RDP_head));
	RDP_head *syn = malloc(sizeof(RDP_head));
	build_packet_from_array(buffer, syn);
	// if packet was a syn packet, send the ack to establish connection :)
	
	if(syn->type ==3 && strcmp(syn->magic, "CSC361") ==0){
		syns++;
		// this is where the print log statement will go

		
		buildack(ack,syn);

		// build the ack packet and use sendto to send to the sender

		char* ackbuffer= malloc( sizeof(char)* MAXHEAD);
		ackbuffer=buildHeader(ackbuffer, ack);

		sendPacket(sockfd,sender_len,sender_addr,ackbuffer,&numbytes);
		return(1);
	}
	sleep(1);
	return(-1);
}


int main(int argc, char *argv[]){
	struct timeval start_time;
	struct timeval fin_time;

	gettimeofday(&start_time, NULL);

	if (pthread_mutex_init(&transmission_lock, NULL) != 0) { 
		printf("transmission mutex failed\n"); 
		return -1; 
	} 
	if (pthread_mutex_init(&listlock, NULL) != 0) { 
		printf("list mutex failed\n"); 
		return -1; 
	}

	int sockfd, portno;
	socklen_t sender_len;
	struct sockaddr_in rec_addr, sender_addr;	//we need these structures to store socket info
	// read input parameters
	if (argc != 4) {
		printf( "Usage: receiver_ip receiver_port rec_file_name" );
		fprintf(stderr,"ERROR, improper syntax\n");
		return -1;
	}

	char *receiver_ip= argv[1];
	char *receiver_port= argv[2];
	char *my_file = argv[3];

	
	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("sws: error on socket()");
		return -1;
	}
	//prep socket info for the reciever
	memset((char *) &rec_addr,0, sizeof(rec_addr));
	portno = atoi(receiver_port);
	rec_addr.sin_family = AF_INET;
	rec_addr.sin_addr.s_addr = inet_addr(receiver_ip);
	rec_addr.sin_port = htons(portno);

	int socket_ops = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&socket_ops, sizeof(socket_ops)) < 0) {
		perror("Couldn't set socket.");
	}

	//bind socket
	if (bind(sockfd, (struct sockaddr *) &rec_addr,sizeof(rec_addr)) < 0){
		close(sockfd);
		perror("sws: error on binding!");
		return -1;
	}
	sender_len = sizeof(sender_addr);
	int connect =0;
	while(connect!=1){
		connect=connection(sockfd,sender_len, &sender_addr);
	}

		char *buffer = malloc(sizeof(char)* MAXBUFLEN);
		char head_buffer[MAXHEAD];
		int numbytes;
		int received_bytes;
		char* ackbuffer= malloc( sizeof(char)* MAXHEAD);
		int fin=-1;
		int select_result;
		fd_set read_fds;
		int done=-1;
		while(1){
			struct timeval timeout;
			timeout.tv_sec=4;
			timeout.tv_usec = 0;
			memset(buffer, '\0',MAXBUFLEN);
			memset(head_buffer, '\0',MAXHEAD);
			memset(ackbuffer, '\0',MAXHEAD);
			
			FD_ZERO( &read_fds );
			FD_SET(sockfd,&read_fds);
			select_result=select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
			if(select_result==0){
				if(fin>0){
					int CHECK_ALL[fin+1];
					packetNode *check=front;
					while(check!=NULL){
						CHECK_ALL[check->packet->counter]=1;
						check=check->next;
						
					}
					int c;
					for(c=1;c<fin;c++){
						if (CHECK_ALL[c] !=1){
							done=0;
							printf("%d\n",CHECK_ALL[c]);
						}
						else{
							done=1;
						}
					}
					if(done){
						write_to_file(my_file,fin);
						break;
					}
					
					
				}
				continue;
			}
			if(FD_ISSET(sockfd,&read_fds)){
				received_bytes =receivePacket(sockfd,sender_len,&sender_addr,buffer,&numbytes);
			}

			
			memcpy(head_buffer, buffer, MAXHEAD);
			RDP_head *header = malloc(sizeof(RDP_head));
			build_packet_from_array(head_buffer, header);

			if(header->type==1){
				data_packets++;
				total_data_bytes_rec=total_data_bytes_rec+received_bytes;
				RDP_head *ack = malloc(sizeof(RDP_head));
				buildack(ack,header);
				ackbuffer=buildHeader(ackbuffer, ack);
				sendPacket(sockfd,sender_len,&sender_addr,ackbuffer,&numbytes);

				//insert data into a list if seqno is unique
				packetNode *node = malloc(sizeof(packetNode));
				RDP_packet *rec_data =malloc(sizeof(RDP_packet));
				char *full_buffer= malloc( sizeof(char)* MAXBUFLEN);
				memcpy(full_buffer, buffer, MAXBUFLEN);
				rec_data->head = malloc(sizeof(RDP_head));
				memcpy(rec_data->head, header, sizeof(RDP_head));
				rec_data->full_buffer = full_buffer;
				node->packet=rec_data;
				rec_data->counter =  rec_data->head->seqno;
				gettimeofday(node->timestamp, NULL);
				int unique=1;
				packetNode *iter = front;
				while(iter!=NULL){
					if(iter->packet->counter==header->seqno){
						unique=0;
						total_data_bytes_rec=total_data_bytes_rec+received_bytes;
						
					}
					iter=iter->next;
				}
				if(unique){
					unique_data_bytes_rec=unique_data_bytes_rec+received_bytes;
					unique_data_packets++;
					insert(node);
				}	
			}
			//fin
			if(header->type==4){
				total_fin++;
				RDP_head *fin_ack = malloc(sizeof(RDP_head));
				buildack(fin_ack,header);
				ackbuffer=buildHeader(ackbuffer, fin_ack);
				sendPacket(sockfd,sender_len,&sender_addr,ackbuffer,&numbytes);

				fin=header->seqno;
				packetNode *node = malloc(sizeof(packetNode));
				RDP_packet *rec_data =malloc(sizeof(RDP_packet));
				char *full_buffer= malloc( sizeof(char)* MAXBUFLEN);
				memcpy(full_buffer, buffer, MAXBUFLEN);
				rec_data->head = malloc(sizeof(RDP_head));
				memcpy(rec_data->head, header, sizeof(RDP_head));
				
				rec_data->full_buffer = full_buffer;
				rec_data->counter =  rec_data->head->seqno;

				node->packet=rec_data;
				gettimeofday(node->timestamp, NULL);
				int unique=1;
				packetNode *iter = front;
				while(iter!=NULL){
					if(iter->packet->counter==header->seqno){
						unique=0;	
					}
					iter=iter->next;
				}
				if(unique){
					insert(node);
				}


			}
			if(header->type==5){
				printf(" rst packet recieved\n");
				close(sockfd);

			}

			free(header);
		}		
	
	pthread_mutex_destroy(&transmission_lock); 
	pthread_mutex_destroy(&listlock);

	gettimeofday(&fin_time, NULL);
	long seconds = fin_time.tv_sec -start_time.tv_sec;
	long micro =(fin_time.tv_usec- start_time.tv_sec)%1000000;

	printf("total data bytes received: %d \n",total_data_bytes_rec);
	printf("unique data bytes received: %d \n",unique_data_bytes_rec);
	printf("total data packets received: %d \n",data_packets);
	printf("unique data packets received: %d \n", unique_data_packets);
	printf("SYN packets received: %d \n",syns);
	printf("FIN packets sent: %d \n", total_fin);
	printf("RST packets sent: %d \n", RSTs);
	printf("ACK packets sent: %d \n", acks);
	printf("total time duration: %lu.%lu \n", seconds, micro);


	close(sockfd);
	return(1);
}
