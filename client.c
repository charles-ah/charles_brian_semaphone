#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

void shared_memory_operations(key_t key) {
  int sd;
  sd = shmget(key, 10000, IPC_CREAT | IPC_EXCL | 0664);
  void * seg = shmat(sd, 0, 0);

  struct shmid_ds data;
  data.shm_segsz = 10;
  shmctl(sd, IPC_SET, &data);

  struct shmid_ds data2;
  shmctl(sd, IPC_STAT, &data2);

  printf("Size: %lu\n", data2.shm_segsz);

  shmdt(seg);
  shmctl(sd, IPC_RMID, NULL);
}

int main() {
  umask(113);

  key_t key = ftok("/usr", 23);
  if (errno) {
    printf("Ftok error: %s\n", strerror(errno));
  }

  int semd;
  semd = semget(key, 1, IPC_CREAT | IPC_EXCL | 0664 );
  if (errno) {
    printf("Semget error: %s\n", strerror(errno));
  }

  shared_memory_operations(key);
  
  struct sembuf buf;
  buf.sem_num = 0; // number of semaphores to access
  buf.sem_op = 1; // up: releases the semaphore
  buf.sem_flg = SEM_UNDO; // rollback in case program fails
  
  semop(semd, &buf, 1);
  if (errno) {
    printf("Semop error\n");
  }
  semctl(semd, 0, IPC_RMID);
  if (errno) {
    printf("Semctl error\n");
  }
  
  return 0;
}
