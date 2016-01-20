#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>


#define MAXBUFLEN 1024
#define MAXHEAD 48

#define header_format "%s %d %d %d %d %d\n", head->magic,head->type, head->seqno, head->ackno, head->payload,head->windowsize

pthread_mutex_t listlock;

// TYPE: DAT: 1 ACK: 2 SYN: 3 FIN: 4 RST: 5
typedef struct RDP_head {
	char magic[7];
	int type; 
	int seqno;
	int ackno;
	int payload;
	int windowsize;
   
} RDP_head;

typedef struct RDP_packet{
	RDP_head* head;
	char* full_buffer;
	int counter;
}RDP_packet;

typedef struct packetNode{
	struct packetNode* next;
	RDP_packet* packet;
	struct timeval *timestamp;
}packetNode;


void format_time_stamp(char *type, char *unique, char *destip, char *sourceip, int source_port, int dest_port,int length){
	time_t t = time(NULL);
	struct tm *info = localtime(&t);
	char *timestamp=malloc(sizeof(char)*50);


	struct timeval  tv;
	gettimeofday(&tv, NULL);
	long micro =tv.tv_usec%1000000;
	strftime(timestamp, 64, "%H:%M:%S ", info);
	printf("%s.%lu %s %d %s %d %s %d %s\n", timestamp,micro, sourceip, source_port, destip, dest_port, type, length, unique); 
    
}

char* buildHeader(char* arr,RDP_head *head){
	snprintf(arr, MAXHEAD, header_format);
	return arr;
}

void build_packet_from_array(char* arr, RDP_head *head){
	// copy first token to magic in RDP_head struct
	char* token= malloc(sizeof(char)*sizeof(arr));
	token = strtok(arr, " ");
	strcpy(head->magic, token);

	token = strtok(NULL, " ");
	head->type = atoi(token);

	token = strtok(NULL, " ");
	head->seqno = atoi(token);

	token = strtok(NULL, " ");
	head->ackno = atoi(token);

	token = strtok(NULL, " ");
	head->payload = atoi(token);

	token = strtok(NULL, " ");
	head->windowsize = atoi(token);

}



packetNode* front;
packetNode* back;



//insert
void insert(packetNode* inserted_packet){
	struct packetNode* temp;
	if(front==NULL && back==NULL){
		front=inserted_packet;
		back=inserted_packet;
	}
	else if(front==back){
		front->next=inserted_packet;
		back=inserted_packet;
	}
	else{
		temp=back;
		back=inserted_packet;
		temp->next=inserted_packet;
	}
}


int size(){
	packetNode *iter = front;
	int count=0; 
	while(iter!=NULL){
		count++;
		iter=iter->next;
	}
	return(count);
	

}

int isEmpty(){
	if(size() == 0){
		return 1;
	}
	return 0;
}

//delete
/*
void printlist(){
	packetNode *t = front;
	while(t != NULL){
		printf("%d", t->test);
		t=t->next;
	}
	printf("\n");
}
*/

packetNode* rem(int ack){
	packetNode *iter = front; 
	packetNode *follow = front; 
	while(iter!=NULL){
		if(iter->packet->head->seqno == ack-1){
 			if(iter == front){ 
				if(front ==back){ 
					front =NULL; 
					back = NULL; 
					
				}
				else{ 
					front = front->next; 
				} 
			}
			else if(iter == back){ 
				follow->next = NULL; 
				back = follow;
			}
			else{ 
				follow->next = iter->next; 
			}
				iter->next = NULL;
				return iter;
		}
		else if(iter ==back){ 
			return NULL; 
		}
		else{ 
			follow = iter; 
			iter = iter->next; 
		} 
	}
	return(NULL);

}

packetNode* rem_rec(int seq){
	packetNode *iter = front; 
	packetNode *follow = front; 
	while(iter!=NULL){
		if(iter->packet->counter == seq){
 			if(iter == front){ 
				if(front ==back){ 
					front =NULL; 
					back = NULL; 
					
				}
				else{ 
					front = front->next; 
				} 
			}
			else if(iter == back){ 
				follow->next = NULL; 
				back = follow;
			}
			else{ 
				follow->next = iter->next; 
			}
				iter->next = NULL;
				return iter;
		}
		else if(iter ==back){ 
			return NULL; 
		}
		else{ 
			follow = iter; 
			iter = iter->next; 
		} 
	}
	return(NULL);

}


void free_node(packetNode* node){
	free(node);
}


































































//free packnode
