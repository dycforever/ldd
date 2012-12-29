/*
 * =====================================================================================
 *
 *       Filename:  second_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/24/2012 04:28:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  dycforever (), dycforever@126.com
 *   Organization:  
 *
 * =====================================================================================
 */


#include	<stdlib.h>
#include <fcntl.h>
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
		int
main ( int argc, char *argv[] )
{
		int fd;
		int counter=0;
		int old_counter=0;

		fd=open("/dev/second",O_RDONLY);
		if(fd != -1)
				for(;;){
						read(fd,&counter,sizeof(unsigned int));
						if(counter!=old_counter){
								printf("now is %d\n",counter);
								old_counter=counter;
						}
				}
		else
				printf("Device open faaailure\n");
		return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
