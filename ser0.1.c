#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KB 1024
#define MB 1048576
#define GB 1073741824

void *sender(void *data){
	char f_name[256], buf[256];
	int *ns = (int *)data;
	ssize_t nsize, f_size = 0;
	double size;
	int fd;

	strcpy(buf, "**Welcome to Server**");
	if(send(*ns, buf, strlen(buf), 0) == -1){
		perror("send");
		exit(1);
	}
	
	if(recv(*ns, f_name, sizeof(f_name), 0) == -1){
		perror("send");
		exit(1);
	}
	printf("**Requested File Name: %s **\n", f_name);
	
	if((fd = open(f_name, O_RDONLY)) == -1){
		perror("File Open");
		strcpy(buf, "**No files found**");
		send(*ns, buf, sizeof(buf), 0);
		exit(1);
	}
		strcpy(buf, "**Files found**");
		send(*ns, buf, sizeof(buf), 0);

	while((nsize = read(fd, buf, sizeof(buf))) > 0){
		if(send(*ns, buf, nsize, 0) == -1){
			perror("send");
			exit(1);
		}
		f_size += nsize;
	}
	
	printf("**Complete Sending Files %ld byte**\n", f_size);
	if(f_size >= KB && MB > f_size){
		size = (double)f_size/KB;
		printf("**Complete Sending Files %f KB**\n", size);
	}
	else if(f_size >= MB && f_size < GB){
		size = (double)f_size/MB;
		printf("**Complete Sending Files %f MB**\n", size);
	}
	else if(f_size >= GB){
		size = (double)f_size/GB;
		printf("**Complete Sending Files %f GB**\n", size);
	}

	close(fd);
	close(*ns);
	free(ns);
	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t tid;
	int pn = atoi(argv[1]);
	struct sockaddr_in sin, cli;
	socklen_t clilen = sizeof(cli);
	int sd,	*ns;
	int optvalue = 1;

	if(argc != 3){
		printf("%s <PORTNUM> <FILEDIRECTOY>\n", argv[0]);
		exit(1);
	}
	
	if(chdir(argv[2]) == -1){
			perror("move dir");
			exit(1);
	}

	preload("1.mkv");

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(pn);
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){
		perror("bind");
		exit(1);
	}

	if(listen(sd, 5)){
		perror("listen");
		exit(1);
	}

	while(1){
		printf(" *Waiting Client...**\n");

		ns = (int *)malloc(sizeof(int));
		if((*ns = accept(sd, (struct sockaddr *)&cli, &clilen)) == -1){
			perror("accept");
			exit(1);
		}
		pthread_create(&tid, NULL, sender, ns);
	}

	return 0;
}
