/*
 * =====================================================================================
 *
 *       Filename:  signal.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/12/2012 03:54:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  dycforever (), dycforever@126.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

# define MAX_LEN 100
int gfd;

void input_handler(int num)
{
		char data[MAX_LEN];
		int len;

//		len = read(STDIN_FILENO, &data, MAX_LEN);
//		len = read(gfd, &data, MAX_LEN);
//		data[len] = 0;
//		printf("input available:%s\n",data);
		printf ( "received a signal,signalNum:%d\n",num );
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
		int
main ( int argc, char *argv[] )
{
		int oflags;
		int fd;
		fd = open("/dev/globalmem",O_RDWR,S_IRUSR | S_IWUSR);
		if(fd < 0){
				printf("open file failed");
				perror("file :");
				exit(1);
		}else if(fd > 0)
				printf ( "open successed, fd = %d\n",fd );
		gfd = fd;
		signal(SIGIO,input_handler);
		fcntl(fd,F_SETOWN,getpid());
		oflags = fcntl(fd, F_GETFL);
		fcntl(fd, F_SETFL, oflags | FASYNC);
//		fcntl(STDIN_FILENO,F_SETOWN,getpid());
//		oflags = fcntl(STDIN_FILENO, F_GETFL);
//		fcntl(STDIN_FILENO, F_SETFL, oflags | FASYNC);
		while(1){
				printf ( "sleeping\n" );
				sleep(100);
		};
}				/* ----------  end of function main  ---------- */
