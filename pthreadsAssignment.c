#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 

#define NUM_THREADS 4

char string[100];
char stringToPrint[100];


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condOne = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mutexTwo = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condTwo = PTHREAD_COND_INITIALIZER;

int threadHasAcquiredMutex = 0;
int textChanged = 0;
int lastThreadAccessed = -1;

void *consumerThread(void *threadid) { 
	while (1) {
			pthread_mutex_lock(&mutex);
			if (string[0] != '\0') {
				printf("\t\tThread%d has mutex lock.\n\n", threadid);
				

				// char* prefix = "Thread";
				int lmao = (int) threadid;
				lastThreadAccessed = lmao;
				// char buffer[16];
				// sprintf(buffer, "%d", lmao);	
				// char* idString = strcat(&prefix, &buffer);

				//Send to printer Thread
				pthread_mutex_lock(&mutexTwo);
				int i = 0;
				char c = ' ';
				while (c != '\0') {
					c = string[i];
					stringToPrint[i] = c;
					i++;
				}

				printf("\t\t\t\tConsumer Thread is waiting...\n\n");
				pthread_cond_wait(&condTwo, &mutexTwo);
				printf("\t\t\t\tConsumer Thread is no longer waiting.\n\n");
				pthread_mutex_unlock(&mutexTwo);
				printf("\t\tThread %d released mutex lock.\n\n", threadid);


				string[0] = '\0';
			}
			pthread_cond_signal(&condOne);
			pthread_mutex_unlock(&mutex);
		//}
	} 
	pthread_exit(NULL);
}

void *printer(void *threadid) {
	while(1) {
		pthread_mutex_lock(&mutexTwo);
		if (stringToPrint[0] != '\0') {
			printf("\t\t\t\t\t\t\t\tThread %d: %s \n\n", lastThreadAccessed, stringToPrint);
			stringToPrint[0] = '\0';
		}
		pthread_cond_signal(&condTwo);
		pthread_mutex_unlock(&mutexTwo);
	}
	pthread_exit(NULL);
} 

int main (int argc, const char * argv[]) { 
	pthread_t threads[NUM_THREADS];
	int rc,cId, pId; 
	pId = 12;

	int rs;
	for (cId = 0; cId < NUM_THREADS; cId++) {
		if (cId == 0) {
			rs = pthread_create(&threads[cId],NULL,printer,(void *)cId); 
			if (rs) { 
				printf("ERROR return code from pthread_create(): %d\n",rs); 
				exit(-1); 
			} 
		}
		else { 
			printf("Creating thread %d\n",cId); 
			rs = pthread_create(&threads[cId],NULL,consumerThread,(void *)cId); 
			if (rs) { 
				printf("ERROR return code from pthread_create(): %d\n",rs); 
				exit(-1); 
			}
		} 
	} 

	pthread_mutex_lock(&mutex);
	while (1) {
			printf("Enter some input: \n"); 
			gets(string);
			printf("Input Entered: \n");
			puts(string);
			textChanged = 1;
			printf("\n");
			//wait
			printf("Main is waiting...\n\n");
			pthread_cond_wait(&condOne, &mutex);
			printf("Main is not waiting anymore.\n\n");
			// string[0] = '\0';

	}	
	pthread_mutex_unlock(&mutex);



	// wait for threads to exit 
	for(cId=0;cId<NUM_THREADS;cId++) { 
		pthread_join(threads[cId], NULL); 
	} 
	return 0;
}