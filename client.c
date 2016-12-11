#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

void shared_memory_operations(key_t key, char* buf, char* buf2) {
  int sd;
  sd = shmget(key, 1, 0);
  if (errno) {
    printf("Shmget error %s\n", strerror(errno));
  }

  int * seg = shmat(sd, 0, 0);
  if (errno) {
    printf("Shmat error %s\n", strerror(errno));
  }

  int fd = open("story.txt", O_RDWR, 0664);
  if (errno) {
    printf("Open error %s\n", strerror(errno));
  }
  printf("FD: %d\n", fd);

    off_t offset = lseek(fd, -(*seg), SEEK_END);
  if (errno) {
    printf("lseek error %s\n", strerror(errno));
  }
  printf("OFFSET FROM BEGINNING: %lu\n", offset);

  int status = read(fd, buf, 1024);
  if (errno) {
    printf("Read error %s\n", strerror(errno));
  }
  printf("READ STATUS: %d\n", status);
  if (status) {
    printf("Last line: %s\n", buf);
  }

  printf("Your update: ");
  fgets(buf2, 1024, stdin);
  write(fd,buf2,strlen(buf2));

  shmdt(seg);
  close(fd);
}

int main() {
  umask(113);

  key_t key = ftok("/usr", 23);
  if (errno) {
    printf("Ftok error: %s\n", strerror(errno));
  }

  int semd;
  semd = semget(key, 1, IPC_EXCL | 0644 );
  if (errno) {
    printf("Semget error: %s\n", strerror(errno));
  }

  // wait for semaphore to be released
  char buf[1024];
  char buf2[1024];
  shared_memory_operations(key, buf, buf2);

  struct sembuf sbuf;
  sbuf.sem_num = 0; // number of semaphores to access
  sbuf.sem_op = 1; // up: releases the semaphore
  sbuf.sem_flg = SEM_UNDO; // rollback in case program fails

  semop(semd, &sbuf, 1);
  if (errno) {
    printf("Semop error: %s\n", strerror(errno));
  }

  return 0;
}
