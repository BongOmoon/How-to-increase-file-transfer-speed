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

char *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p;
off_t f_byte[7], *f_b;

void sel(char *f_name){		//compare server file to requested file
	if(strcmp(f_name, "1.mkv")==0){
		p = p1;
		f_b = &f_byte[0];
	}
	
	else if(strcmp(f_name, "2.fna")==0){
		p = p2;
		f_b = &f_byte[1];
	}
	else if(strcmp(f_name, "a.mp3")==0){
		p = p3;
		f_b = &f_byte[2];
	}
	else if(strcmp(f_name, "b.mp3")==0){
		p = p4;
		f_b = &f_byte[3];
	}
	else if(strcmp(f_name, "c.mp3")==0){
		p = p5;
		f_b = &f_byte[4];
	}
	else if(strcmp(f_name, "d.mp3") == 0){
		p = p6;
		f_b = &f_byte[5];
	}
	else{
		p = p7;
		f_b = &f_byte[6];
	}
}

void *sender(void *cli){	//send file to client(thread)
	int fd;
	char f_name[128];
	char buf[256];
	int *ns = (int *)cli; //client
	
	if(recv(*ns, f_name, sizeof(f_name), 0) == -1){	//recv f_name
		perror("recv");
		exit(1);
	}

	if((fd = open(f_name, O_RDONLY)) == -1){	//open requested file
		perror("open");
		exit(1);
	}
	sel(f_name);	
	sprintf(buf, "%ld", (*f_b));

	if(send(*ns, buf, sizeof(buf), 0) == -1){	//send fbyte
		perror("send");
		exit(1);
	}

	//send to client
	ssize_t cal_byte = 0;
	ssize_t f_mod;
	f_mod = (*f_b)%1446;
	int len = 1446;
	int nbyte;

	while(1){
		nbyte = send(*ns, p, len, 0);
		cal_byte += nbyte;
		p = &p[nbyte];

		if((*f_b) - cal_byte == f_mod){
			nbyte = send(*ns, p, f_mod, 0);
			cal_byte += nbyte;
			break;
		}
	}

	printf("%ld\n", cal_byte);
	printf("Finish Send\n");

	close(fd);
	close(*ns);
	free(ns);
	return NULL;
}

int main(int argc, char *argv[]){
	pthread_t tid;
	int port_num = atoi(argv[1]);
	struct sockaddr_in sin, cli;
	int sd, *ns;
	socklen_t clilen = sizeof(cli);
	int optvalue = 1;

	//check argument number
	if(argc != 3){
		printf("%s <PORTNUM> <SERVICE_DIRECTORY>\n", argv[0]);
		exit(1);
	}

	//move to SERVICE_DIRECTORY
	if(chdir(argv[2]) == -1){
		perror("move dir");
		exit(1);
	}

	//init socket
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	//preloading files
	int rfd[7];
	//1.mkv
	if((rfd[0] = open("1.mkv", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[0] = lseek(rfd[0], 0, SEEK_END);
	lseek(rfd[0], 0, SEEK_SET);
	p1 = (char *)malloc(f_byte[0]);
	read(rfd[0], p1, f_byte[0]);

	//2.fna
	if((rfd[1] = open("2.fna", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[1] = lseek(rfd[1], 0, SEEK_END);
	lseek(rfd[1], 0, SEEK_SET);
	p2 = (char *)malloc(f_byte[1]);
	read(rfd[1], p2, f_byte[1]);

	//a.mp3
	if((rfd[2] = open("a.mp3", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[2] = lseek(rfd[2], 0, SEEK_END);
	lseek(rfd[2], 0, SEEK_SET);
	p3 = (char *)malloc(f_byte[2]);
	read(rfd[2], p3, f_byte[2]);

	//b.mp3
	if((rfd[3] = open("b.mp3", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[3] = lseek(rfd[3], 0, SEEK_END);
	lseek(rfd[3], 0, SEEK_SET);
	p4 = (char *)malloc(f_byte[3]);
	read(rfd[3], p4, f_byte[3]);

	//c.mp3
	if((rfd[4] = open("c.mp3", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[4] = lseek(rfd[4], 0, SEEK_END);
	lseek(rfd[4], 0, SEEK_SET);
	p5 = (char *)malloc(f_byte[4]);
	read(rfd[4], p5, f_byte[4]);

	//d.mp3
	if((rfd[5] = open("d.mp3", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[5] = lseek(rfd[5], 0, SEEK_END);
	lseek(rfd[5], 0, SEEK_SET);
	p6 = (char *)malloc(f_byte[5]);
	read(rfd[5], p6, f_byte[5]);

	//kernel.tar
	if((rfd[6] = open("kernel.tar", O_RDONLY)) == -1){
		perror("open");
		exit(1);
	}

	f_byte[6] = lseek(rfd[6], 0, SEEK_END);
	lseek(rfd[6], 0, SEEK_SET);
	p7 = (char *)malloc(f_byte[6]);
	read(rfd[6], p7, f_byte[6]);

	printf("Finish Preload, Try Connect Client\n");

	//set socket
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue));
	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port_num);
	sin.sin_addr.s_addr = inet_addr("0.0.0.0");

	if(bind(sd, (struct sockaddr *)&sin, sizeof(sin))){
		perror("bind");
		exit(1);
	}
	
	if(listen(sd, 5)){
		perror("listen");
		exit(1);
	}

	//accept client
	while(1){
		ns = (int *)malloc(sizeof(int));
		if((*ns = accept(sd, (struct sockaddr *)&cli, &clilen)) == -1){
			perror("accept");
			exit(1);
		}
		pthread_create(&tid, NULL, sender, ns);
	}

	return 0;
}
