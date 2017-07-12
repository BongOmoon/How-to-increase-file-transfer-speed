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

int main(int argc, char *argv[]){
	int pn = atoi(argv[1]);			//PORTNUM
	char f_name[128], buf[1446];
	int fd;	
	ssize_t fbyte = 0;
	struct timeval t1, t2;
	double sec;
	struct sockaddr_in sin;
	int sd;
	int n;

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

	strcpy(f_name, argv[2]);

	gettimeofday(&t1, NULL);
	
	if(connect(sd, (struct sockaddr *)&sin, sizeof(sin)) != 0){
		perror("connect");
		exit(1);
	}

	if(send(sd, f_name, sizeof(f_name), 0) == -1){	//send file name
		perror("send");
		exit(1);
	}
	
	if((fd = open(f_name, O_WRONLY|O_CREAT, 0664)) == -1){
		perror("open");
		exit(1);
	}

	if(recv(sd, buf, sizeof(buf), 0) == -1){	//recv f_byte
		perror("recv");
		exit(1);
	}
	fbyte = atoi(buf);
	printf("%ld\n", fbyte);
	//int i = 0;
	
	while((n = recv(sd, buf, sizeof(buf), 0))>0){	//recv file
		//if(i >= fbyte)
		//	return;
		if((write(fd, buf, n)) == -1){
			perror("write");
			exit(1);
		}
	//	i += n;
	}
	
	gettimeofday(&t2, NULL);

	sec = (t2.tv_sec + t2.tv_usec*0.000001)-(t1.tv_sec + t1.tv_usec*0.000001);
	//printf("Download Speed %f byte/sec\n", f_size/sec);
	printf("Download Time %f\n", sec);
	close(fd);
	close(sd);
	
	return 0;
}
