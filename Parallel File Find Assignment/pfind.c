#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int num_of_found_files = 0;
int num_of_live_threads;
pthread_mutex_t lock;
pthread_cond_t not_empty;
char* term;
int num_threads;
pthread_t* thread;
struct queue
{
	struct queue *next;
	char *data;
};
typedef struct queue queue_t;

queue_t *tail =NULL;
queue_t *head =NULL;

void sigint_handler_kill(){
	for(int t = 0; t<num_threads;t++){
		if(thread[t]==pthread_self() ){
			continue;
		}
		pthread_cancel(thread[t]);
    }
	printf("Search stopped, found %d files\n", num_of_found_files);
	pthread_exit(NULL);
}

void enqueue(char *new)
{
	pthread_mutex_lock(&lock);
	queue_t *new_item = (queue_t *)malloc(1 * sizeof(queue_t));
	if (head ==NULL){
		head = new_item;
		tail = new_item;
	}
	else{
		(head)->next = new_item;
		head = new_item;
	}
	char* temp = (char*)malloc((strlen(new)+1)*sizeof(char));
	strcpy(temp, new);
	(head)->data = temp;
	(head)->next = NULL;
	pthread_cond_signal(&not_empty);
	pthread_mutex_unlock(&lock);
}


char *dequeue()
{
	pthread_mutex_lock(&lock);
	while (tail == NULL){
		if(num_of_live_threads ==1){
			pthread_cond_signal(&not_empty);
			num_of_live_threads--;
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL);
		}
		num_of_live_threads--;
		pthread_cond_wait(&not_empty,&lock);
		num_of_live_threads++;
	}

char *a;
	a = (tail)->data;
	queue_t *temp = (tail)->next;
	free(tail);
	if(temp == NULL){
		head = NULL;
	}
	tail = temp;
	pthread_mutex_unlock(&lock);
	return a;
}


void* search_for_term()
{
	while (1)
	{
		char *path = dequeue();
		if(path == NULL){
			return NULL;
		}
		DIR *dir = opendir(path);
		if (!dir)
		{
			perror(path);
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL);
		}
		struct dirent* entry;
		while((entry=readdir(dir)) != NULL){
			if(strcmp(entry->d_name,"..") != 0 && strcmp(entry->d_name,".") != 0){
				char buff[strlen(path)+strlen(entry->d_name)+2];
        		sprintf(buff,"%s/%s",path,entry->d_name);
				if(entry->d_type == DT_DIR){
					enqueue(buff);
				}
				else if(strstr(entry->d_name, term)){
					__sync_fetch_and_add(&num_of_found_files, 1);
					printf("%s\n", buff);
				}
			}
		}
		closedir(dir);
	}
}

int main(int argc, char *argv[])
{
    signal(SIGINT, &sigint_handler_kill);   
	int rc;
	int t;
	void* status;
	if(argc<4){
		printf("Need 3 main arguments\n");
		return 1;
	}
	num_threads = atoi(argv[3]);
  	rc = pthread_mutex_init( &lock, NULL );
  	if( rc ) 
  	{
    printf("ERROR in pthread_mutex_init(): "
        "%s\n", strerror(rc));
    exit( -1 );
  	}
	pthread_cond_init (&not_empty, NULL);
	pthread_t thread_array[num_threads];
	thread = thread_array;
	queue_t *new_item = (queue_t *)malloc(1 * sizeof(queue_t));
	head = new_item;
	tail = new_item;
	char* temp = (char*)malloc((strlen(argv[1])+1)*sizeof(char));
	strcpy(temp, argv[1]);
	head->data = temp;
	head->next = NULL;
	head->data = argv[1];
	head->next = NULL;
	term = argv[2];
	for(t=0; t<num_threads; t++)
	{
		rc = pthread_create(&thread[t], NULL, search_for_term,NULL ); 
		if (rc)
		{
			printf("ERROR in pthread_create(): %s\n", strerror(rc));
			exit(-1);
		}
		num_of_live_threads++;
	}
	for(t=0; t<num_threads; t++)
{
		rc = pthread_join(thread[t], &status);
		if (rc)
		{
			printf("ERROR in pthread_join(): %s\n", strerror(rc));
			exit(-1);
		}
}
	printf("Done searching, found %d files\n", num_of_found_files);
	pthread_exit(  NULL);
	return 0;
}