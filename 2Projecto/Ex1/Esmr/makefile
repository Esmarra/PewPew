###########################################################################
#                   makefile
###########################################################################

###########################################################################
#                   Macro definitions
###########################################################################

FLAGS  = -Wall -g 
CFLAGS  = -lrt -lpthread
CC     = gcc
PROG   = tpso1802
SEMLIB = semlib
OBJS   = ${SEMLIB}.o ${PROG}.o

###########################################################################
#                   Rules
###########################################################################

all:	${PROG}

clean:
	rm ${OBJS} ${PROG} *~
  
${PROG}:	${OBJS}
	${CC} ${FLAGS} ${OBJS} -o $@ ${CFLAGS}

.c.o:
	${CC} ${FLAGS} $< -c ${CFLAGS}

###########################################################################

${SEMLIB}.o: ${SEMLIB}.h ${SEMLIB}.c

${PROG}.o: ${SEMLIB}.h ${PROG}.c

${PROG}: ${PROG}.o ${SEMLIB}.o

###########################################################################

