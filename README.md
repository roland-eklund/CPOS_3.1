## Cpos - 3.1
This is a repo for the coding exercises in assignment 3.1

### Included files:
 1. README
 2. producer_consumer.c 
 3. readers_writers_problem.c
 4. dining_philosophers/dining_philosophers.c

 | file     | content   | author    | comment   |
 | ------| ------- | ------ | -------|
 | README.md | Contents and instructions | Roland Eklund | |
 | producer_consumer.c | sample code | MD Hosen   | |
 | readers_writers_problem.c | Readers writers problem written in c | Roland | Written with AI and articles regading the problem. Geeks for geeks.com |
 | file4 | comment |    | |


### 2. producer_cousumer
Here you find the implementation of the producer-Consumer problem with multiple producers and a single consumer (Variation A):
Explanation 

1. Producer: 

   - Generates random items and adds them to the buffer. 

   - Uses `sem_wait(&empty)` to wait for an empty slot and `pthread_mutex_lock(&mutex)` to protect the buffer. 

2. Consumer: 

   - Retrieves items from the buffer and prints them. 

   - Uses `sem_wait(&full)` to wait for a filled slot and `pthread_mutex_lock(&mutex)` to protect the buffer. 

3.Synchronization: 

   - Semaphores (`empty` and `full`) manage buffer slots. 

   - Mutex ensures mutual exclusion for buffer access. 

 
#### Testing and Debugging  
#####Console Output: 
```
  Producer 1 produced item: 42 
  Producer 2 produced item: 17 
  Consumer consumed item: 17 
  Producer 3 produced item: 56 
  Consumer consumed item: 56 
  ...
```

#####Debugging: 

  - Use ThreadSanitizer to detect race conditions: 

``` 
    clang -fsanitize=thread -o producer_consumer producer_consumer.c -pthread 
``` 
``` 
    
./producer_consumer
``` 


=======
| file 5 | comment | | |
>>>>>>> origin/main
