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
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5 // threads
#define ROUNDS 800
#define MONITOR_INTERVAL 40      // in microseconds
#define TIME_UNTIL_DEAD 10000000  // in microseconds
#define SIMULATE_WORK 1 // 1 = TRUE, 0 = FALSE

pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_mutex_t data_lock[NUM_PHILOSOPHERS];
pthread_t philosophers[NUM_PHILOSOPHERS];
pthread_t monitor;
//pthread_barrier_t start_barrier;

struct PhilData { // Philosopher struct to track data
  int phil_id;
  struct timespec start_time;
  struct timespec end_time;
  int rounds;
  int is_dead;
};

struct PhilData phil_data[NUM_PHILOSOPHERS]; // Array for phil structs

atomic_int monitor_running = 1;

// Monitor checks if a thread is starved, based on treshold (TIME_UNTIL_DEAD)
// If so, monitor sets the state to dead
void *data_monitor() {
  while (monitor_running) {
    usleep(MONITOR_INTERVAL);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
      pthread_mutex_lock(&data_lock[i]);
      int elapsed_time =
          (phil_data[i].end_time.tv_sec - phil_data[i].start_time.tv_sec) *
              1e6 +
          (phil_data[i].end_time.tv_nsec - phil_data[i].start_time.tv_nsec) /
              1e3;

      if (elapsed_time > TIME_UNTIL_DEAD) {
        phil_data[i].is_dead = 1;
      }
      pthread_mutex_unlock(&data_lock[i]);
    }
  }
  return NULL;
}

void *philosopher(void *arg) {
//  pthread_barrier_wait(&start_barrier);
  int phil_id = *(int *)arg;
  free(arg);

  // Loops until total cycles equals set constant or until thread is starved.
  while (1) {

    pthread_mutex_lock(&data_lock[phil_id]);
    if (phil_data[phil_id].rounds >= ROUNDS || phil_data[phil_id].is_dead) {
      pthread_mutex_unlock(&data_lock[phil_id]);
      break;
    }
    clock_gettime(CLOCK_MONOTONIC, &phil_data[phil_id].start_time);
    pthread_mutex_unlock(&data_lock[phil_id]);

    if ((phil_id % 2) == 0) {
      // Grab left fork first
      pthread_mutex_lock(&forks[phil_id]);
      pthread_mutex_lock(&forks[(phil_id + 1) % NUM_PHILOSOPHERS]);
    } else {
      // Grab right fork first
      pthread_mutex_lock(&forks[(phil_id + 1) % NUM_PHILOSOPHERS]);
      pthread_mutex_lock(&forks[phil_id]);
    }
    if (SIMULATE_WORK){
    int sleep_time = rand() % 201 + 10;
    usleep(sleep_time);
    }

    pthread_mutex_unlock(&forks[phil_id]);
    pthread_mutex_unlock(&forks[(phil_id + 1) % NUM_PHILOSOPHERS]);

    pthread_mutex_lock(&data_lock[phil_id]);
    clock_gettime(CLOCK_MONOTONIC, &phil_data[phil_id].end_time);
    phil_data[phil_id].rounds++;
    pthread_mutex_unlock(&data_lock[phil_id]);
  }
  return NULL;
}

int main() {
  struct timespec prog_start_time, prog_end_time;
  clock_gettime(CLOCK_MONOTONIC, &prog_start_time);
  // pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;

  // initializing mutexes
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_init(&forks[i], NULL);     // initializing forks (mutexes)
    pthread_mutex_init(&data_lock[i], NULL); // mutexes for data
  }

  // initializing phils data
 // pthread_barrier_init(&start_barrier, NULL, NUM_PHILOSOPHERS + 1);

  // Creating the philosopher threads

  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_lock(&data_lock[i]);
    phil_data[i].phil_id = i;
    phil_data[i].rounds = 0;
    phil_data[i].is_dead = 0;
    pthread_mutex_unlock(&data_lock[i]);
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&philosophers[i], NULL, philosopher, id);
  }

  //pthread_barrier_wait(&start_barrier); // All philosophers wait here before starting

  // Creating the monitor thread
  pthread_create(&monitor, NULL, data_monitor, NULL);

  // Waiting for phil threads to finnish
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_join(philosophers[i], NULL);
  }

  // Finnish running the monitor
  atomic_store(&monitor_running, 0);
  pthread_join(monitor, NULL);

  // Destroying mutexes
  for (int i = 0; i <= NUM_PHILOSOPHERS; i++) {
    pthread_mutex_destroy(&data_lock[i]);
    pthread_mutex_destroy(&forks[i]);
  }
  // Destroying start_barrier
  //pthread_barrier_destroy(&start_barrier);

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
  int prog_time_elapsed =
      (prog_end_time.tv_sec - prog_start_time.tv_sec) * 1e6 +
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
