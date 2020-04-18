/*
*    DKU Operating System Lab
*        Lab1 (Scheduler Algorithm Simulator)
*        Student id : 32164959, 32162436
*        Student name : Heo Jeon Jin, Shin Chang Woo
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm test code.
*
*/
#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>
#include "lab1_sched_types.h"

int main(int argc, char* argv[]) {
	PROCESS pc[MAX_PC];
	printf("\n¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á");
	printf("\n¡á                2020                ¡á");
	printf("\n¡á DKU OS LAB 1 - SCHEDULER SIMULATOR ¡á");
	printf("\n¡á             BY HJJ/SCW             ¡á");
	printf("\n¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á¡á\n");

	setProcess(pc);
	printf("\n------------------[ FIFO ]------------------\n");
	FIFO(pc);
	printf("\n-----------[ Round Robin - TQ 1]------------\n");
	RR(pc, 1);
	printf("\n-----------[ Round Robin - TQ 4]------------\n");
	RR(pc, 4);
	printf("\n---------------[ MLFQ - Q 1]----------------\n");
	MLFQ(pc, 1);
	printf("\n----------[ MLFQ - Q 4 - TQ 2^i ]-----------\n");
	MLFQ(pc, 4);
	printf("\n-------[ STRIDE - Set ticket by SVT ]-------\n");
	STRIDE(pc);

	printf("\n¢º Simulation finished.\n");
	return 0;
}
