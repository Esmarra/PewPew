#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>


int main(){
	int fd, retval;
	char buffer[8]="teste";
	fflush(stdin);

	retval=mkfifo("/tmp/myfifo", 0666);

	fd=open("/tmp/myfifo",O_WRONLY);

	write(fd,buffer,sizeof(buffer));
	printf("\n");

	close(fd);
	return(0);
}
