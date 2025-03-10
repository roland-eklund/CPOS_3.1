#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>


#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0; // Number of items in the buffer
sem_t empty; // Semaphore for empty slots in the buffer
sem_t full; // Semaphore for filled slots in the buffer

pthread_mutex_t mutex; // Mutex for protecting the buffer

void *producer(void *arg) {
  int id = *(int *)arg;
  int item;

  while (1) {
    item = rand() % 100; // Generate a random item
    sem_wait(&empty); // Wait for an empty slot
    pthread_mutex_lock(&mutex); // Lock the buffer

    // Add item to the buffer
    buffer[count] = item;
    count++;
    printf("Producer %d produced item: %d\n", id, item);
    pthread_mutex_unlock(&mutex); // Unlock the buffer
    sem_post(&full); // Signal that a slot is filled
    sleep(1); // Simulate production time
  }
}

void *consumer(void *arg) {
  int item;

  while (1) {
    sem_wait(&full); // Wait for a filled slot
    pthread_mutex_lock(&mutex); // Lock the buffer

    // Remove item from the buffer
    item = buffer[count - 1];
    count--;
    printf("Consumer consumed item: %d\n", item);
    pthread_mutex_unlock(&mutex); // Unlock the buffer
    sem_post(&empty); // Signal that a slot is empty
    sleep(1); // Simulate consumption time
  }
}

int main() {

  pthread_t prod_threads[3], cons_thread;
  int prod_ids[3] = {1, 2, 3};

  // Initialize semaphores and mutex
  sem_init(&empty, 0, BUFFER_SIZE);
  sem_init(&full, 0, 0);
  pthread_mutex_init(&mutex, NULL);

  // Create producer threads
  for (int i = 0; i < 3; i++) {
    pthread_create(&prod_threads[i], NULL, producer, &prod_ids[i]);
  }

  // Create consumer thread
  pthread_create(&cons_thread, NULL, consumer, NULL);

  // Wait for threads to finish (they won't in this example)
  for (int i = 0; i < 3; i++) {
    pthread_join(prod_threads[i], NULL);
  }
  pthread_join(cons_thread, NULL);

  // Clean up
  sem_destroy(&empty);
  sem_destroy(&full);
  pthread_mutex_destroy(&mutex);

  return 0;
}
