#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS];

void *philosopher(void *arg) {
  int id = *(int *)arg;

  while (1) {
    printf("Philosopher %d is thinking...\n", id);
    sleep(rand() % 3); // Simulate thinking time

    //::: DEADLOCK CAUSING CODE ::::
    // Instead of every other philosophers picking hand first, all start with
    // the same hand.
    pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]); // Pick left fork
    pthread_mutex_lock(&forks[id]);                          // Pick right fork

    // Pick forks (odd philosophers pick left first, even pick right first)
    /*    if (id % 2 == 0) {
          pthread_mutex_lock(&forks[id]); // Pick right fork
          pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]); // Pick left
       fork } else { pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]); //
       Pick left fork pthread_mutex_lock(&forks[id]); // Pick right fork
        }
    */

    // Eating
    printf("Philosopher %d is eating...\n", id);
    sleep(rand() % 2 + 1); // Simulate eating time

    // Put down forks
    pthread_mutex_unlock(&forks[id]);
    pthread_mutex_unlock(&forks[(id + 1) % NUM_PHILOSOPHERS]);

    printf("Philosopher %d finished eating and is thinking again.\n", id);
    sleep(rand() % 3); // Simulate post-eating thinking time
  }

  return NULL;
}

int main() {
  pthread_t philosophers[NUM_PHILOSOPHERS];
  int ids[NUM_PHILOSOPHERS];

  // Initialize forks (mutexes)
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_init(&forks[i], NULL);
  }

  // Create philosopher threads
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    ids[i] = i;
    pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
  }

  // Join threads (never actually reached in this infinite loop example)
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_join(philosophers[i], NULL);
  }

  // Destroy mutexes
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_destroy(&forks[i]);
  }

  return 0;
}
