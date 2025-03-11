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

#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define ROUNDS 200

pthread_mutex_t forks[NUM_PHILOSOPHERS]; // semaphore array

void *philosopher(void *arg) {
  int id = *(int *)arg;
  int phil_rounds = 0;

  while (phil_rounds < ROUNDS) {
    // Grab a fork (left)
    pthread_mutex_lock(&forks[id]);
    // Grab a fork (right)
    pthread_mutex_lock(&forks[(id + 1)%NUM_PHILOSOPHERS]);

    phil_rounds++;
  }


}

// creating the threads "philosophers"
void init_phils() {
  pthread_t philosophers[NUM_PHILOSOPHERS];
  int ids[NUM_PHILOSOPHERS];

  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    ids[i] = i;
    pthread_create(&philosophers[i], NULL, philosopher, &ids[i]);
  }
}

// creating the mutexes "forks"
void init_forks() {
  for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_mutex_init(&forks[i], NULL);
  }
}

int main() {
  init_phils();
  init_forks();
}
