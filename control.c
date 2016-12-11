#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

//*
union semun {
  int val; /* Value for SETVAL */
  struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo *__buf; /* Buffer for IPC_INFO
			    (Linux specific) */
};
//*/

int main(int argc, char* argv[])
{
  if (argc < 2) {
    return 0;
  }

  umask(0133);

  int shmDES;
  int semDES;
  int fd;

  key_t sem_key = ftok("/usr", 23);
  if (errno) {
    printf("Ftok error #%d: %s\n", errno, strerror(errno));
  }
  key_t shm_key = ftok("makefile", 60);
  if (errno) {
    printf("Ftok error #%d: %s\n", errno, strerror(errno));
  }

  if (strcmp(argv[1],"-c")==0 ) {
    semDES = semget(sem_key, 1, IPC_CREAT |  0644);
    shmDES = shmget(shm_key, sizeof(int), IPC_CREAT | 0644);

    union semun semUN;
    semUN.val = 1;
    semctl(semDES, 0, SETVAL, semUN);

    fd = open("story.txt", O_TRUNC | O_CREAT, 0644);
    /* printf("shmDES: %d\n",shmDES); */
    /* printf("semDES: %d\n",semDES); */
    /* printf("file FD: %d\n",fd); */

    close(fd);
  }

  if(strcmp(argv[1],"-r")==0) {
    semDES = semget(sem_key, 1, 0);
    shmDES = shmget(shm_key, 0, 0);

    /* printf("shmDES: %d\n",shmDES); */
    /* printf("semDES: %d\n",semDES); */
    /* printf("removing shared memory, should be 0: %d\n",shmctl(shmDES,IPC_RMID,0)); */
    /* printf("removing semaphore, should be 0: %d\n",semctl(semDES,0,IPC_RMID,0)); */

    shmctl(shmDES, IPC_RMID, 0);
    semctl(semDES, 0, IPC_RMID, 0);

    char* args[3] = {"cat","story.txt", NULL};
    execvp(args[0],args);
  }

  if(strcmp(argv[1],"-v")==0) {
    char* args[3] = {"cat", "story.txt", NULL};
    execvp(args[0],args);
  }

  return 0;
}
