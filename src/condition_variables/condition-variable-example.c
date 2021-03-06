#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define NUM_READERS 3
#define NUM_WRITERS 3

pthread_mutex_t mutex;
pthread_cond_t cond;

int num_readers = 0;
int num_writers = 0;
int num_readers_waiting = 0;
int num_writers_waiting = 0;
int writer_ticket_number = 0;
int next_writer_ticket_to_be_served = 0;
int read_condition() {
  // (num_writers - num_writers_waiting) is the number of _active_ writers.
  return num_writers_waiting == 0;
}

int write_condition(int writer_ticket_number) {
	return num_writers_waiting == 0 && writer_ticket_number == next_writer_ticket_to_be_served; // This thread is the 1 writer
}

void *reader(void *notused) {
  while (1) {
    // acquire resource
    pthread_mutex_lock(&mutex);
    num_readers++;
    while (! read_condition()) {
      num_readers_waiting++;
      pthread_cond_wait(&cond, &mutex); // wait on cond
      num_readers_waiting--;
    }
    pthread_mutex_unlock(&mutex);
    // use resource (we fake this by sleeping)
    int thread_id = syscall(__NR_gettid);  // Similar to getpid().
    printf("reader %d is reading\n", thread_id);
    sleep(1);
    // release resource
    pthread_mutex_lock(&mutex);
    num_readers--;
    pthread_cond_broadcast(&cond); // wake up everyone and let them try again
    // NOTE: For efficiency, once could consider multiple calls to
    //   pthread_cond_signal above, and wake up only as many threads as needed.
    //   However, carefully check your code for correctness if you try that.
    pthread_mutex_unlock(&mutex);
  }
}

void *writer(void *notused) {
  while (1) {
    // acquire resource
    pthread_mutex_lock(&mutex);
    num_writers++; 
    int local = writer_ticket_number;
    writer_ticket_number++;
    while (! write_condition(writer_ticket_number)) {
      num_writers_waiting++;
      pthread_cond_wait(&cond, &mutex); // wait on cond
      num_writers_waiting--;
    }
    next_writer_ticket_to_be_served++;     
    pthread_mutex_unlock(&mutex);
    // use resource (we fake this by sleeping)
    int thread_id = syscall(__NR_gettid);  // Similar to getpid().
    printf("writer %d is writing\n", thread_id);
    sleep(5);
    // release resource
    pthread_mutex_lock(&mutex);
    num_writers--;
    pthread_cond_broadcast(&cond); // wake up everyone and let them try again
    pthread_mutex_unlock(&mutex);
  }
}

int main() {
  pthread_t read_thread[NUM_READERS];
  pthread_t write_thread[NUM_WRITERS];
  int i;

  for (i = 0; i < NUM_READERS; i++) {
    pthread_create(&read_thread[i], NULL, reader, NULL);
  }
  for (i = 0; i < NUM_WRITERS; i++) {
    pthread_create(&write_thread[i], NULL, writer, NULL);
  }

  for (i = 0; i < NUM_READERS; i++) {
    pthread_join(read_thread[i], NULL);
  }
  for (i = 0; i < NUM_WRITERS; i++) {
    pthread_join(write_thread[i], NULL);
  }
  return 0;
}
