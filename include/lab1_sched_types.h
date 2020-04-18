#pragma once
/*
*    DKU Operating System Lab
*        Lab1 (Scheduler Algorithm Simulator)
*        Student id : 32164959, 32162436
*        Student name : Heo Jeon Jin, Shin Chang Woo
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H


#define RUN 1 // Flag for control process
#define READY 0
#define MAX_Q 10 // Each queue size
#define MAX_PC 50 // Max number of process

typedef struct Process {
	char name;
	int pid;
	int arriveT; // Arrival time
	int serviceT; // Service time
	int runT; // Accumulated time
	int stride;
	int runStride;
	int state;
	int rec;
	int first_runT;
	int final_endT;
} PROCESS;

typedef struct ProcQueue {
	int head;
	int tail;
	int TQ; // Time quantum
	PROCESS *Q;
} PROC_QUEUE;

/// General functions
void setProcess(PROCESS* pointer);
int getSumST(PROCESS pc[]);
int getQueueSize(PROC_QUEUE* queue);
void processRefresh(PROCESS* proc, int index);
void draw(int** arr, PROCESS pc[]);
void insertQueue(PROCESS proc, PROC_QUEUE* queue);
int calcTotalTickets(PROCESS pc[]);
int getGCD(int n, int m);
int getLCM(int n, int m);

/// Scheduler algorithm functions
void FIFO(PROCESS pc[]);
void FIFO_pop(PROC_QUEUE* SQ_pointer, PROCESS* pc, int time, int** arr);
void RR(PROCESS pc[], int tq);
void RR_pop(PROC_QUEUE* SQ_pointer, PROCESS pc[], int time, int** arr, int* flag);
void MLFQ(PROCESS pc[], int MLFQ_cnt);
int MLFQ_pop(PROC_QUEUE* MLFQ_ptr, PROCESS processArr[], int q_index, int time, int** arr, int MLFQ_cnt);
void STRIDE(PROCESS pc[]);
int getPidSmallStride(PROCESS pc[], int SchC[], int index);
void delCand(int* cand, int val);

#endif /* LAB1_HEADER_H*/ 