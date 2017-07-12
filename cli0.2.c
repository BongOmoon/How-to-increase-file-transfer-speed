#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KB 1024
#define MB 1048576
#define GB 1073741824

int main(int argc, char *argv[]){
	int pn = atoi(argv[1]);			//PORTNUM
	char f_name[256], buf[256];
	int fd;	
	ssize_t nsize = 0, f_size = 0;
	double size;
	struct timeval t1, t2;
	double sec;
	struct sockaddr_in sin;
	int sd;
	
	if(argc != 3){
		printf("%s <PORTNUM> <FILENAME>\n", argv[0]);
		exit(1);
	}

	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}

	memset((char *)&sin, '\0', sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(pn);
	sin.sin_addr.s_addr = inet_addr("210.117.184.164");

	if(connect(sd, (struct sockaddr *)&sin, sizeof(sin)) != 0){
		perror("connect");
		exit(1);
	}
	if(recv(sd, buf, sizeof(buf), 0) == -1){
		perror("recv");
		exit(1);
	}
	printf("%s\n", buf);

	gettimeofday(&t1, NULL);
	strcpy(f_name, argv[2]);
	if(send(sd, f_name, sizeof(f_name), 0) == -1){
		perror("send");
		exit(1);
	}
	printf("**Requsting File Name**\n");

	if(recv(sd, buf, sizeof(buf), 0) == -1){
		perror("recv");
		exit(1);
	}

	long long len = atoi(buf);
	char *src;
	long long a = len/256;
	src = (char *)malloc(256*(a+15));
	recv(sd, src, len, 0);
	
	if((fd = open(f_name, O_WRONLY|O_CREAT, 0664)) == -1){
		perror("open");
		exit(1);
	}
	
	if((write(fd, src, len)) == -1){
		perror("write");
		exit(1);
	}
	FILE *fp;
	fp = fopen(f_name, "rb");
	fseek(fp, 0, SEEK_END);
	size_t llen = ftell(fp);
	fseek(fp,0,SEEK_SET);
	printf("%ld\n", llen);
/*	if(recv(sd, buf ,sizeof(src), 0) == -1){
		perror("recv");
		exit(1);
	} */
	
/*
	if(recv(sd, buf, sizeof(buf), 0) == -1){
		perror("recv");
		exit(1);
	}
	if(strcmp(buf, "No files found") == 0){
		printf("**No files found(Sever)**\n");
		exit(1);
	}
	printf("%s\n", buf);
	
	strcpy(f_name, argv[2]);
	if((fd = open(f_name, O_WRONLY|O_CREAT, 0664)) == -1){
		perror("open");
		exit(1);
	}
	
	while((nsize = recv(sd, buf, sizeof(buf), 0)) > 0){
		if((write(fd, buf, nsize)) == -1){
				perror("write");
				break;
		}
		f_size += nsize;
	}
*/
	gettimeofday(&t2, NULL);

	printf("**Complete Downloading Files %ld byte**\n", f_size);
	if(f_size >= KB && MB > f_size){
		size = (double)f_size/KB;
		printf("**Complete Downloading Files %f KB**\n", size);
	}
	else if(f_size >= MB && f_size < GB){
		size = (double)f_size/MB;
		printf("**Complete Downloading Files %f MB**\n", size);
	}
	else if(f_size >= GB){
		size = (double)f_size/GB;
		printf("**Complete Downloading Files %f GB**\n", size);
	}
	sec = (t2.tv_sec + t2.tv_usec*0.000001)-(t1.tv_sec + t1.tv_usec*0.000001);
	printf("Download Speed %f byte/sec\n", f_size/sec);
	printf("Download Time %f\n", sec);
	close(fd);
	close(sd);
	
	return 0;
}
