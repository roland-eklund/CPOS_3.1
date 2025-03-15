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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_PHILOSOPHERS 1000 // threads
#define ROUNDS 800
#define MONITOR_INTERVAL 40 // in microseconds
#define TIME_UNTIL_DEAD 10000000 // in microseconds

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_mutex_t monitor_lock = PTHREAD_MUTEX_INITIALIZER;

struct PhilData { // Philosopher struct to track data
  int phil_id;
  struct timespec start_time;
  struct timespec end_time;
  int rounds;
  int is_dead;
  pthread_mutex_t lock;
};

struct PhilData phil_data[NUM_PHILOSOPHERS]; // Array for phil structs 

atomic_int monitor_running = 1;

void *data_monitor() {
  while (monitor_running) {
    usleep(MONITOR_INTERVAL);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_lock(phil_data[i]->lock);
      int elapsed_time =
          (phil_data[i].end_time.tv_sec - phil_data[i].start_time.tv_sec) * 1e6 
            +
          (phil_data[i].end_time.tv_nsec - phil_data[i].start_time.tv_nsec) / 1e3;

      if (elapsed_time > TIME_UNTIL_DEAD) {
        phil_data[i].is_dead = 1;
      }
    }
    pthread_mutex_unlock(&monitor_lock);
  }
  return NULL;
}

void *philosopher(void *arg) {
  struct PhilData *data = (struct PhilData *)arg;
  data->rounds = 0;
  data->is_dead = 0;
  int id = data->phil_id;
  int dead = 0;
  int rounds = 0;
  
  while (rounds < ROUNDS && !(dead)) {
    //usleep(100);
    // printf("Philosopher %d is waiting..", data->phil_id);
    pthread_mutex_lock(&monitor_lock);
    clock_gettime(CLOCK_MONOTONIC, &data->start_time);
    pthread_mutex_unlock(&monitor_lock);
    if ((id % 2) == 0) {
      // Grab left fork first
      pthread_mutex_lock(&forks[id]);
      pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
    } else {
      // Grab right fork first
      pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
      pthread_mutex_lock(&forks[id]);
    }
    pthread_mutex_lock(&monitor_lock);
    clock_gettime(CLOCK_MONOTONIC, &data->end_time);
    pthread_mutex_unlock(&monitor_lock);
    // printf("philosopher %d is eating", id);
    // Drop fork (left)
    pthread_mutex_unlock(&forks[id]);
    pthread_mutex_unlock(&forks[(id + 1) % NUM_PHILOSOPHERS]);

    rounds++;
    pthread_mutex_lock(&monitor_lock);
    data->rounds++;
    dead = data->is_dead;
    pthread_mutex_unlock(&monitor_lock);
  }
  return NULL;
}

int main() {
  struct timespec prog_start_time;
  struct timespec prog_end_time;

  clock_gettime(CLOCK_MONOTONIC, &prog_start_time);
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
    pthread_mutex_init(&phil_data[i].lock, NULL);
    pthread_create(&philosophers[i], NULL, philosopher, &phil_data[i]);
  }

  pthread_create(&monitor, NULL, data_monitor, NULL);

  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_join(philosophers[i], NULL);
  }
  monitor_running = 0;
  pthread_join(monitor, NULL);
  clock_gettime(CLOCK_MONOTONIC, &prog_end_time);

  // :::: STATS ::::: //
  // Philosophers stats:
  printf("\n\n::: Philosophers stats :::");
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    printf("\nPhilosopher %d ", phil_data[i].phil_id);
    printf("ate %d times ", phil_data[i].rounds);
    if (phil_data[i].is_dead == 0) {
      printf("and did not die.");
    } else {
      printf("but died.");
    }
  }

  // Print values:
  printf("\n\n::: Input values :::\n");
  printf("Number of threads: %d \n", NUM_PHILOSOPHERS); 
  printf("Target number of cycles: %d \n", ROUNDS); 
  printf("Monitor interval: %d microseconds\n", MONITOR_INTERVAL); 
  printf("Time until dead: %d microseconds\n", TIME_UNTIL_DEAD); 

  // Summarized stats:
  int rounds = 0;
  int deaths = 0;
  int prog_time_elapsed = (prog_end_time.tv_sec - prog_start_time.tv_sec) * 1e6 +
    (prog_end_time.tv_nsec - prog_start_time.tv_nsec) / 1e3;
          
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    rounds += phil_data[i].rounds;
    deaths += phil_data[i].is_dead;
  }
  printf("\n::: Summarized stats :::");
  printf("\nProgram run time: %d nanoseconds", prog_time_elapsed);
  printf("\nAverage cycles: %d", (rounds / NUM_PHILOSOPHERS));
  printf("\nTotal deaths: %d\n\n", deaths);
}
