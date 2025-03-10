#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE]; // Shared buffer
int count = 0; // Number of items in the buffer

// Semaphores for synchronization
sem_t empty;   // Semaphore for empty slots in the buffer
sem_t full;    // Semaphore for filled slots in the buffer
pthread_mutex_t mutex; // Mutex for protecting the buffer

// Semaphores for Readers-Writers problem
sem_t write_lock;   // To ensure exclusive access for writers
sem_t read_count_lock; // To protect the read count
int read_count = 0; // Count of active readers

// Producer function (Write Access)
void* producer(void* arg) {
    int id = *(int*)arg;
    int item;

    while (1) {
        item = rand() % 100; // Generate a random item

        sem_wait(&empty); // Wait for an empty slot
        sem_wait(&write_lock); // Wait to gain exclusive access for writing

        // Add item to the buffer
        buffer[count] = item;
        count++;
        printf("Producer %d produced item: %d\n", id, item);

        sem_post(&write_lock); // Release write lock
        sem_post(&full); // Signal that a slot is filled

        sleep(1); // Simulate production time
    }
}

// Consumer function (Read Access)
void* consumer(void* arg) {
    int item;

    while (1) {
        sem_wait(&full); // Wait for a filled slot
        sem_wait(&read_count_lock); // Lock the read count

        // Increment the number of active readers
        read_count++;
        if (read_count == 1) {
            sem_wait(&write_lock); // Block the writer when first reader comes
        }

        sem_post(&read_count_lock); // Release the read count lock

        // Read item from the buffer
        item = buffer[count - 1];
        count--;
        printf("Consumer consumed item: %d\n", item);

        sem_wait(&read_count_lock); // Lock the read count again
        // Decrement the number of active readers
        read_count--;
        if (read_count == 0) {
            sem_post(&write_lock); // Allow writers if no readers are active
        }
        sem_post(&read_count_lock); // Release the read count lock

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
    sem_init(&write_lock, 0, 1);   // Initialize write_lock to 1 (writer can write initially)
    sem_init(&read_count_lock, 0, 1); // Initialize read_count_lock to 1
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
    sem_destroy(&write_lock);
    sem_destroy(&read_count_lock);
    pthread_mutex_destroy(&mutex);

    return 0;
}

