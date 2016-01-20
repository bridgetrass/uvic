#include "rdpFunctions.h"
int rec_ack=0;
int total_syn=0;
int total_fin=0;
int total_data_bytes_sent=0;
int unique_data_bytes_sent=0;
int total_data=0;
int unique_data=0;
int RST_rec=0;

char* source_ip;
int source_port = 0;
char *dest_ip;
int dest_port = 0;



typedef struct socket_data{
	struct sockaddr_in *rec_addr;
	socklen_t rec_len;
	int sockfd;
	suseconds_t rtt_;
}socket_data;


pthread_mutex_t transmission_lock;

int receivePacket(int sockfd, socklen_t rec_len, struct sockaddr_in *rec_addr, char* buffer, int *numbytes){
	memset(buffer, '\0',MAXHEAD);
	pthread_mutex_lock(&transmission_lock);
	if ((*numbytes = recvfrom(sockfd, buffer, MAXBUFLEN, 0, (struct sockaddr *)rec_addr, &rec_len)) == -1) {
        	 perror("rdp: error on recvfrom()!");
	}
	format_time_stamp("ACK", "s", dest_ip, source_ip, source_port, dest_port,0);
	pthread_mutex_unlock(&transmission_lock);
	return(1);

}

void* check_for_received_packets(void* args){
	// if select returns 0, check if list is empty, otherwise go recieve the packet!!
	usleep(30000);
	int select_result;
	fd_set read_fds;


	socket_data *data = args;
	packetNode *node;
	RDP_head *ack = malloc(sizeof(RDP_head));
	int numbytes = MAXBUFLEN;
	char *receivebuffer=malloc(sizeof(char)*MAXBUFLEN);
	int empty =isEmpty();
	while(empty==0){
		memset(receivebuffer,'\0',MAXBUFLEN);
		struct timeval timeout;
		timeout.tv_usec = data->rtt_*10;
		timeout.tv_sec=0;
		FD_ZERO( &read_fds );
		FD_SET(data->sockfd,&read_fds);
		select_result=select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
		if(select_result ==0){
			empty= isEmpty();
		}
		else if(FD_ISSET(data->sockfd,&read_fds)){
			receivePacket(data->sockfd, data->rec_len, data->rec_addr,receivebuffer,&numbytes);
			build_packet_from_array(receivebuffer, ack);
			pthread_mutex_lock(&listlock);
			node = rem(ack->ackno);
			free_node(node);
			rec_ack++;
			pthread_mutex_unlock(&listlock);
		}
	}
	pthread_exit(NULL);


}

void buildsyn_(RDP_head *syn){
	strcpy(syn->magic,"CSC361");
	syn->type=3;
	syn->seqno =0;
	syn->ackno =0;
	syn->payload=0;
	syn->windowsize=0;
}


int sendPacket(int sockfd,socklen_t rec_len, struct sockaddr_in *rec_addr, char* buffer, int *numbytes, int size){
	pthread_mutex_lock(&transmission_lock);
	if ((*numbytes= sendto(sockfd, buffer, size, 0,(struct sockaddr *)rec_addr, rec_len)) == -1) {
            	perror("rdp: error in sendto()");
            		return(-1);
	}
	pthread_mutex_unlock(&transmission_lock);
	return(1);

}

RDP_head* connection_establish(int sockfd,socklen_t rec_len, struct sockaddr_in *rec_addr){
	char buffer[MAXHEAD];
	int numbytes;
	// build syn packet as a struct
	RDP_head *syn = malloc(sizeof(RDP_head));
	buildsyn_(syn);
	char* bufferpointer = buffer;

	// builds the synchronization packet that can be sent over the network as a char array
	bufferpointer=buildHeader(bufferpointer, syn);
	// send syn packet
	sendPacket(sockfd,rec_len,rec_addr,bufferpointer,&numbytes,strlen(bufferpointer));
	total_syn++;
	int select_result;
	fd_set read_fds;
	struct timeval timeout;
	timeout.tv_sec=1;
	timeout.tv_usec = 0;

	FD_ZERO( &read_fds );
	FD_SET(sockfd,&read_fds);
	select_result=select(FD_SETSIZE, &read_fds, NULL, NULL, &timeout);
	if(select_result<1){
		return(NULL);

	}
	else if(FD_ISSET(sockfd,&read_fds)){
		receivePacket(sockfd,rec_len,rec_addr,buffer,&numbytes);
		rec_ack++;
	}


	RDP_head *response = malloc(sizeof(RDP_head)); 
	build_packet_from_array(buffer, response);
	
	// if the response was an ack
	if(response->type==2){
		return(response);
	}
	return(NULL);

}

void build_head(int ack,int payload, RDP_head *dat, int type){
	strcpy(dat->magic,"CSC361");
	dat->type=type;
	dat->seqno =ack;
	dat->ackno =0;
	dat->payload=payload;
	dat->windowsize=0;
}




int retransmit_data(int sockfd,socklen_t rec_len, struct sockaddr_in *rec_addr,suseconds_t rtt){
	sleep(1);
	useconds_t retrans = 10*rtt;
	pthread_mutex_lock(&listlock);
	int empty =isEmpty();
	pthread_mutex_unlock(&listlock);
	int numbytes;

	while(!empty){
		usleep(retrans);
		pthread_mutex_lock(&listlock);
		packetNode *node=front;
		int i=0;
		int resend = 0;
		int count=size();
		char *buffer[count];
		int data_size[count];
		while(i++< size()){
			char * resend_buffer = malloc(sizeof(char)*MAXBUFLEN);
			memset(resend_buffer,'\0',MAXBUFLEN);
			memcpy(resend_buffer, node->packet->full_buffer, MAXBUFLEN);
			buffer[resend] = resend_buffer;
			data_size[resend] = node->packet->head->payload;
			resend++;
			if(node->packet->counter==3){
				pthread_mutex_unlock(&listlock);
				RDP_head*RST_=malloc(sizeof(RDP_head));
				build_head(0,0,RST_,5);
				char*s=malloc(sizeof(char)*MAXHEAD);
				sendPacket(sockfd,rec_len,rec_addr,s,&numbytes,MAXHEAD);
				void build_packet_from_array(char* arr, RDP_head *head);
				return 0;
			}
			node->packet->counter++;
			node= node->next;
		}
	
		pthread_mutex_unlock(&listlock);
		int j;
		int z;
		for(j=0; j<resend; j++){
			sendPacket(sockfd,rec_len,rec_addr,buffer[j],&numbytes,MAXHEAD+ data_size[j]);
			
			z= atoi(buffer[j] + 7);
			if(z==1){
				total_data++;
				total_data_bytes_sent= total_data_bytes_sent+data_size[j];
				format_time_stamp("DAT", "S", dest_ip, source_ip, source_port, dest_port,data_size[j]);
			}
			if(z==4){
				total_fin++;
				format_time_stamp("FIN", "s", dest_ip, source_ip, source_port, dest_port,data_size[j]);
			}
				
		}
		empty = isEmpty();

		
		

	}


	return(0);



}

int data_transfer(RDP_head *ack, int sockfd,socklen_t rec_len, struct sockaddr_in *rec_addr, char *my_file){
	int fd;
	fd = open(my_file, O_RDONLY);
	if(fd==-1){
		printf("bad file descriptor\n");
	}
	
	struct stat file_stats;
	fstat(fd, &file_stats);
	int file_size = file_stats.st_size;
	char *file_to_send;	
	if((file_to_send = mmap((caddr_t)0, file_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED){
		perror("rdps:");
	}

	// file_to_send points to the place in mem where the file is
	// prepare packets to be sent over the network 
	// build header
	int size = file_size;
	int numbytes;

	// while loop goes here, sends data till all data is sent
	int d = ack->ackno;
	int windowsize = ack->windowsize;
	
	while(size>0){
		char sendbuffer[MAXHEAD +windowsize];
		RDP_head *dat = malloc(sizeof(RDP_head));
		if(size<windowsize){
			format_time_stamp("FIN", "s", dest_ip, source_ip, source_port, dest_port,windowsize);
			windowsize=size;
			build_head(d++,windowsize,dat,4);
			total_fin++;
		}else{
			format_time_stamp("DAT", "s", dest_ip, source_ip, source_port, dest_port,windowsize);
			build_head(d++,windowsize,dat, 1);
			total_data_bytes_sent= total_data_bytes_sent+windowsize;
			unique_data_bytes_sent=unique_data_bytes_sent+windowsize;
			total_data++;
			unique_data++;
			
		}
		//MAXHEAD+windowsize
		char *full_buffer= malloc( sizeof(char)* MAXBUFLEN);
		memset(sendbuffer,'\0',MAXBUFLEN);
		memset(full_buffer,'\0',MAXBUFLEN);
		buildHeader(sendbuffer,dat);
		memcpy(sendbuffer+MAXHEAD, file_to_send, windowsize);
		file_to_send= file_to_send + windowsize;


		sendPacket(sockfd,rec_len,rec_addr,sendbuffer,&numbytes,dat->payload+MAXHEAD);
		// build RDP_packet (to keep track of the packets that have been sent)
		RDP_packet *sent_packet =malloc(sizeof(RDP_packet));
		sent_packet->head = dat;
		
		memcpy(full_buffer, sendbuffer, MAXHEAD+windowsize);
		sent_packet->full_buffer = full_buffer;
		// insert into list 
		pthread_mutex_lock(&listlock);
		packetNode *node = malloc(sizeof(packetNode));
		node->packet = sent_packet;
		node->packet->counter=0;
		gettimeofday(node->timestamp, NULL);
		insert(node);
		pthread_mutex_unlock(&listlock);
		size=size-windowsize;
	}
	return(d);

	
}

int main(int argc, char *argv[]){
	// start time used for the final print out
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
	socklen_t rec_len;
	struct sockaddr_in sender_addr, rec_addr;	//we need these structures to store socket info
	// read input parameters
	if (argc != 6) {
		printf( "Usage:sender_ip sender_port receiver_ip receiver_port sender_file_name" );
		fprintf(stderr,"ERROR, improper syntax\n");
		return -1;;
	}

	char* sender_ip =argv[1];
	char* sender_port= argv[2];
	char* receiver_ip= argv[3];
	char* receiver_port= argv[4];
	char *my_file = argv[5];

	// checks if the file exists before we continue

	FILE * fp;
	fp = fopen (my_file, "r");
	if(fp== NULL){
		printf("File doe not exist\nUsage:sender_ip sender_port receiver_ip receiver_port sender_file_name");
		exit(1);
	}
	
	//create socket
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		perror("sws: error on socket()");
		return -1;
	}

	//prep socket info for the sender
	bzero((char *) &sender_addr, sizeof(sender_addr));
	portno = atoi(sender_port);
	sender_addr.sin_family = AF_INET;
	sender_addr.sin_addr.s_addr = inet_addr(sender_ip);
	sender_addr.sin_port = htons(portno);

	//prep socket info for the reciever
	bzero((char *) &rec_addr, sizeof(rec_addr));
	portno = atoi(receiver_port);
	rec_addr.sin_family = AF_INET;
	rec_addr.sin_addr.s_addr = inet_addr(receiver_ip);
	rec_addr.sin_port = htons(portno);



	//bind socket

	// to prevent blocking
	int socket_ops = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&socket_ops, sizeof(socket_ops)) < 0) {
		perror("Couldn't set socket.");
	}
	if (bind(sockfd, (struct sockaddr *) &sender_addr,
		sizeof(sender_addr)) < 0){
		close(sockfd);
		perror("sws: error on binding!");
		return -1;
	}
	rec_len = sizeof(rec_addr);

// LETS BEGIN 
	// establish connection
	RDP_head *ack;
	struct timeval init;
	struct timeval fin;
	while(ack==NULL){
		gettimeofday(&init, NULL);
		ack = connection_establish(sockfd,rec_len, &rec_addr);
		gettimeofday(&fin, NULL);
	}

	source_ip = receiver_ip;
	source_port=atoi(argv[2]);
	dest_ip=malloc(sizeof(char)*20);

	inet_ntop(AF_INET, &(sender_addr.sin_addr), dest_ip, 20);
	dest_port =ntohs(sender_addr.sin_port);
	
	suseconds_t rtt = fin.tv_usec - init.tv_usec;

	// this is the information to pass into the recieving thread
	socket_data *info= malloc(sizeof(struct socket_data));
	info->rec_addr = &rec_addr;
	info->rec_len = rec_len;
	info->sockfd=sockfd;
	info->rtt_ = rtt;

	// we have recieved the first ack, meaning we can begin to send data
	// pass the ack(the first response from the reciever into the send data function

	pthread_t rec_from_thread;
	if(pthread_create(&rec_from_thread,NULL,&check_for_received_packets,info)){
		printf("Error creating thread\n");
	}
	data_transfer(ack,sockfd,rec_len, &rec_addr, my_file);
	retransmit_data(sockfd,rec_len, &rec_addr,rtt);
	if(pthread_join(rec_from_thread, NULL)) {
		printf("Error joining thread\n");
	}




	pthread_mutex_destroy(&transmission_lock); 
	pthread_mutex_destroy(&listlock);
	// end of data transfer
	gettimeofday(&fin_time, NULL);
	long seconds = fin_time.tv_sec -start_time.tv_sec;
	long micro =(fin_time.tv_usec- start_time.tv_sec)%1000000;


	printf("total data bytes sent: %d \n",total_data_bytes_sent);
	printf("unique data bytes sent: %d \n",unique_data_bytes_sent);
	printf("total data packets sent: %d \n",total_data);
	printf("unique data packets sent: %d \n", unique_data);
	printf("SYN packets sent: %d \n",total_syn);
	printf("FIN packets sent: %d \n", total_fin);
	printf("RST packets received: %d \n", RST_rec);
	printf("ACK packets received: %d \n", rec_ack);
	printf("total time duration: %lu.%lu \n", seconds, micro);
	

	close(sockfd);
	return(1);
}

