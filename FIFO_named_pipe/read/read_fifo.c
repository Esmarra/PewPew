#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(){
	int fd, retval;
	char buffer[8];
	fd=open("/tmp/myfifo",O_RDONLY);
	retval=read(fd,buffer,sizeof(buffer));
	fflush(stdin);
	write(1,buffer,sizeof(buffer));
	printf("\n");
	close(fd);
	return(0);
}
