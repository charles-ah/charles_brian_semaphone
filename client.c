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

void shared_memory_operations(key_t key, char* write_buffer) {
  // Memory operations
  int sd = shmget(key, sizeof(int), 0); // get shared memory
  int* seg = shmat(sd, 0, 0); // attach memory to a variable

  // File operations
  int fd = open("story.txt", O_RDWR, 0644); // open file
  off_t offset = lseek(fd, -(*seg), SEEK_END); // set offset

  // Get last line
  char* buf = (char*) malloc(*seg + 1);
  int bytes_read = read(fd, buf, *seg);
  *(buf + *seg) = '\0';
  if (bytes_read > 0) {
    printf("Last line: %s\n", buf);
  } else {
    printf("No recently added lines found. Create a new story.\n");
  }
  free(buf);

  // Get input
  printf("Your update: ");
  fgets(write_buffer, 1024, stdin);

  // Store data in file and shared memory
  int status = write(fd, write_buffer, strlen(write_buffer));
  *seg = strlen(write_buffer);

  // Detach memory segment and close file
  shmdt(seg);
  close(fd);
}

int main() {
  umask(0133);

  // Get keys
  key_t sem_key = ftok("/usr", 23);
  if (errno) {
    printf("Ftok error #%d: %s\n", errno, strerror(errno));
  }
  key_t shm_key = ftok("makefile", 60);
  if (errno) {
    printf("Ftok error #%d: %s\n", errno, strerror(errno));
  }

  // Get semaphore
  int semid;
  semid = semget(sem_key, 1, 0);

  // wait for semaphore to be released
  struct sembuf sbuf = {0, -1, SEM_UNDO};
  semop(semid, &sbuf, 1);

  // Add to story
  char write_buffer[1024];
  shared_memory_operations(shm_key, write_buffer);

  // Release semaphore
  sbuf.sem_op = 1; // up: releases the semaphore
  semop(semid, &sbuf, 1);

  return 0;
}
