/*  ::::: BASIC PROBLEM ::::::
 *   n number of philosophers (thread)
 *   n number of forks (mutex)
 *   every phil needs 2 forks to eat
 *         phil can take fork
 *         phil can put fork
 *         phil states: eat, think (wait), hungry, dead
 *
 *   rules:left or right phil cannot eat if current wants to eat
 *   ::::::::::::::::::::::::::*/

//#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define ROUNDS 200
#define MONITOR_INTERVAL 20 // in milliseconds
#define TIME_UNTIL_DEAD 20

pthread_mutex_t forks[NUM_PHILOSOPHERS]; // semaphore array

struct PhilData {
  int phil_id;
  struct timespec start_time;
  struct timespec end_time;
  int rounds;
  int is_dead;
};

struct PhilData phil_data[NUM_PHILOSOPHERS]; // Each phil gets its own struct

void *data_monitor(){
  while (1) {
    usleep(MONITOR_INTERVAL * 1e3);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++){
      int elapsed_time = (phil_data[i].end_time.tv_nsec - phil_data[i].start_time.tv_nsec)/1e3;
      if (elapsed_time > TIME_UNTIL_DEAD){
        phil_data[i].is_dead = 1;
      }
    }
  }
  
}
                                             // and each struct is stored here
void *philosopher(void *arg) {
  struct PhilData *data = (struct PhilData *)arg;
  data->rounds = 0;
  data->is_dead = 0;

  while (data->rounds < ROUNDS && data->is_dead != 1) {
    usleep(1);
    // printf("Philosopher %d is waiting..", data->phil_id);
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
    if (data->phil_id % 2 == 0){
    // Grab a fork (left)
    pthread_mutex_lock(&forks[data->phil_id]);
    // Grab a fork (right)
    pthread_mutex_lock(&forks[(data->phil_id + 1) % NUM_PHILOSOPHERS]);
    }else {
    // Grab a fork (right)
    pthread_mutex_lock(&forks[(data->phil_id + 1) % NUM_PHILOSOPHERS]);
    // Grab a fork (left)
    pthread_mutex_lock(&forks[data->phil_id]);
    }
    clock_gettime(CLOCK_MONOTONIC, &data->end_time);
    // printf("philosopher %d is eating", id);
    // Drop fork (left)
    pthread_mutex_unlock(&forks[data->phil_id]);
    pthread_mutex_unlock(&forks[(data->phil_id + 1) % NUM_PHILOSOPHERS]);

    data->rounds++;
  }
  return NULL;
}

int main() {
  // creating the threads "philosophers"
  pthread_t philosophers[NUM_PHILOSOPHERS];
  pthread_t monitor;

  // creating the mutexes "forks"
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_init(&forks[i], NULL);
  }


  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    phil_data[i].phil_id = i;
    phil_data[i].rounds = 0;
    pthread_create(&philosophers[i], NULL, philosopher, &phil_data[i]);
  }

  pthread_create(&monitor, NULL, data_monitor, NULL);

  for (int i = 0; i < NUM_PHILOSOPHERS; i++){

  pthread_join(philosophers[i], NULL);
  }


  // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
  // Philosophers stats:
  printf("\n\n::: Philosophers stats :::");
  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
    printf("\nPhilosopher id: %d, ",phil_data[i].phil_id);
    printf("did %d rounds and ",phil_data[i].rounds);
    if (phil_data[i].is_dead == 0){
    printf("did not die.");
    }else{
    printf("died.");
    }}

  // Summarized stats:
  int rounds = 0;
  int deaths;
  for (int i = 0; i < NUM_PHILOSOPHERS; i++){
      rounds += phil_data[i].rounds;
      deaths += phil_data[i].is_dead;
    }
  printf("\n\n::: Summarized stats :::");
  printf("\nAverage rounds: %d", (rounds / NUM_PHILOSOPHERS));
  printf("\nTotal deaths: %d\n\n", deaths);
}
