#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // usleep
#include <semaphore.h>

using namespace std;

pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int v = 0;

void *producer(void *arg) {
  pthread_mutex_lock(&mylock);
  v++;
  printf("PRODUCER #%0d v = %d\n", arg, v);
  pthread_mutex_unlock(&mylock);
  pthread_cond_signal(&cv);
  return NULL;
}

void *consumer(void *arg) {
  printf("CONSUMER %d\n, arg");

  while(1) {
    pthread_mutex_lock(&mylock);
    pthread_cond_wait(&cv, &mylock);
    if (v > 0)
      v--;
    printf("CONSUMER #%d v = %d\n", (long int)arg, v);
    pthread_mutex_unlock(&mylock);
  }
  return NULL;
}

int main(int argc, char **argv)
{
  if (argc != 5) {
    //stderr
    perror("usage: ./hw1 num_prod num_cons buf_size num_items");
    exit(1);
  }

  int num_prod = atoi(argv[1]);
  int num_cons = atoi(argv[2]);
  int buf_size = atoi(argv[3]);
  int num_items = atoi(argv[4]);

  cout << num_prod << '\n' << num_cons << '\n' << buf_size << '\n' << num_items << '\n';

  pthread_t producers[20];
  pthread_t consumer[2];

  for (int i = 0; i < 2; i++) {
    pthread_create(&consumer[i], NULL, consumer, i);
  }

  for (int i = 0; i < 20; i++) {
    pthread_create(&producers[i], NULL, producers, i);
  }

  for (int i = 0; i < 20; i++) {
    pthread_join(producers[i], NULL);
  }

  for (int i = 0; i < 2; i++) {
    pthread_join(consumer[i], NULL);
  }
}
