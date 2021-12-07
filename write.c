#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define KEY 49620
#define KEY2 453345

int main() {

	int semid = semget(KEY2, 1, IPC_CREAT | 0777);
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	semop(semid, &sb, 1);

	int shmid = shmget(KEY, 0, 0);
	int *data = shmat(shmid, 0, 0);
	int len = *data;

	int fd;
	if (len) {
		fd = open("story.txt", O_RDONLY, 0777);
		lseek(fd, -len, SEEK_END);
		char line[len+1];
		read(fd, line, len+1);
		line[len] = 0;
		printf("Last line: %s", line);
		close(fd);
	} else {
		printf("The story is just getting started!\n");
	}

	fd = open("story.txt", O_WRONLY | O_APPEND, 0777);
	char buf[1000];
	printf("Enter next line: ");
	fgets(buf, 1000, stdin);
	*data = strlen(buf);
	write(fd, buf, *data);
	close(fd);

	shmdt(data);

	sb.sem_op = 1;
	semop(semid, &sb, 1);
}
