#include <pthread.h>
#include <time.h> // usleep
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

int total_haircuts = 0;
int total_left = 0;
float time_slept = 0; // note to self: time slept and time waiting is in micro seconds
float wait_time = 0;

int num_barbers = 0;
int num_clients = 0;
int num_chairs = 0;
int arrival_t = 0;
int haircut_t = 0;

pthread_mutex_t barberLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t clientLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t leavingLock = PTHREAD_MUTEX_INITIALIZER;

sem_t barberSem; // counts down until empty
sem_t chairSem; // counts up until full

struct Barber {
	int id;
};

struct Client {
	int id;
};

void *barber(void *arg) {

	struct Barber *barber = (struct Barber*)malloc(sizeof(struct Barber));;
	barber->id = (int)arg;
	
	while (1) {

			printf("barber %d: sleeping...\n", barber->id);
			struct timeval tvalBefore;
			gettimeofday (&tvalBefore, NULL);
			
			sem_wait(&chairSem); // wait for chairs to be filled

				struct timeval tvalAfter; 
				gettimeofday (&tvalAfter, NULL);  		
				pthread_mutex_lock(&barberLock);
					time_slept += tvalAfter.tv_usec - tvalBefore.tv_usec; // add to barber time slept
					total_haircuts++;					
				pthread_mutex_unlock(&barberLock);	

				printf("barber %d: haircut...\n", barber->id);	
				usleep(haircut_t); // duration of the haircut
			sem_post(&barberSem); // haircut done, free the barber
	}
 	return NULL;
}

void *client(void *arg) {

	struct Client *client = (struct Client*)malloc(sizeof(struct Client));
	client->id = (int)arg;
	printf("client %d: arriving...\n", client->id);

	int filledChairs;
   	sem_getvalue(&chairSem, &filledChairs);
	if (filledChairs < num_chairs) { // there is a chair available

		struct timeval tvalBefore;
		gettimeofday (&tvalBefore, NULL);

		printf("client %d: waiting...\n", client->id); // waiting

		sem_post(&chairSem); // inc chair sem

		sem_wait(&barberSem); // we have wait for a barber	

		struct timeval tvalAfter; // done waiting now
		gettimeofday (&tvalAfter, NULL);  
		pthread_mutex_lock(&clientLock);		
    		wait_time += tvalAfter.tv_usec - tvalBefore.tv_usec; // add to wait time	
		pthread_mutex_unlock(&clientLock);		

		printf("client %d: haircut...\n", client->id);	
		usleep(haircut_t); // duration of the haircut (both processes need to sleep)			
	}
	else {
		pthread_mutex_lock(&leavingLock);
			total_left++; // inc total left when client leaves
		pthread_mutex_unlock(&leavingLock);
		printf("client %d: leaving...\n", client->id);
	}	
  	return NULL;
}

int main(int argc, char **argv)
{
	
	if (argc != 6) {
		perror("usage: ./hw1 num_prod num_cons buf_size num_items");
		exit(1);
	}

	num_barbers = atoi(argv[1]);
	num_clients = atoi(argv[2]);
	num_chairs = atoi(argv[3]);
	arrival_t = atoi(argv[4]);
	haircut_t = atoi(argv[5]);

	if (!(num_barbers > 0) || !(num_clients > 0) || !(num_chairs > 0) || !(arrival_t > 0) || !(haircut_t > 0)) {
		perror("pram must be greater than 1");
		exit(-1);
	}

	pthread_t barbers[num_barbers];
	pthread_t clients[num_clients];	

	sem_init(&barberSem, 0, num_barbers); // initilize to num barbers
	sem_init(&chairSem, 0, 0); // init with 0 chairs filled
	
	int i;
	for (i = 0; i < num_barbers; i++) {
	 	int success = pthread_create(&barbers[i], NULL, barber, (void *)i);
	}

	for (i = 0; i < num_clients; i++) {
		int next_client_time =	random() %	arrival_t +	1;
		usleep(next_client_time); // clients arrive at random times
		int success = pthread_create(&clients[i], NULL, client, (void *)i);
	}

	for (i = 0; i < num_clients; i++) {
		pthread_join(clients[i], NULL); // join clients
  	}
  	// we don't join barbers bcs they never stop

 	fprintf(stderr, "TOTALS:\n");
	fprintf(stderr, "Total haircuts: %d\n", total_haircuts);
	fprintf(stderr, "Avg Barber sleep time: %f\n", time_slept / num_barbers); // divide that
	fprintf(stderr, "Number clients that left: %d\n", total_left);
	fprintf(stderr, "Avg Client wait time: %f\n", wait_time / num_clients); // divide that too
}
