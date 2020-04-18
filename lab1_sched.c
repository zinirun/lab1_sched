/*
*    DKU Operating System Lab
*        Lab1 (Scheduler Algorithm Simulator)
*        Student id : 32164959, 32162436
*        Student name : Heo Jeon Jin, Shin Chang Woo
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scheduler algorithm function's definition.
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

int process_cnt;

//Set process by user (arrival time, service time)
void setProcess(PROCESS* pointer) {
	printf("\n¢º How many process (1 to 50) -> ");
	scanf("%d", &process_cnt);

	PROCESS* pc = (PROCESS*)malloc(sizeof(PROCESS) * process_cnt);

	for (int i = 0; i < process_cnt; i++) {
		pc[i].name = 'A' + i;
		pc[i].pid = i;
		pc[i].state = READY;
		printf("\n\n*--- Process %c ---*\n", pc[i].name);
		printf("¢¹ Arrival time -> "); scanf("%d", &pc[i].arriveT);
		printf("¢¹ Service time -> "); scanf("%d", &pc[i].serviceT);
		*(pointer + i) = pc[i];
	}

	free(pc);
}

//Get sum of process' service time
int getSumST(PROCESS pc[]) {
	int i = 0;
	int j = 0;
	int sumT = 0;
	PROCESS* tmp_pc;
	PROCESS tmp_proc;

	tmp_pc = (PROCESS*)malloc(sizeof(PROCESS) * process_cnt);

	for (i = 0; i < process_cnt; i++) tmp_pc[i] = pc[i];

	//Sort processes by arrival time
	for (i = 0; i < process_cnt; i++) {
		for (j = i; j < process_cnt; j++) {
			if (tmp_pc[i].arriveT > tmp_pc[j].arriveT) {
				tmp_proc = tmp_pc[i];
				tmp_pc[i] = tmp_pc[j];
				tmp_pc[j] = tmp_proc;
			}
		}
	}

	//Add to sumT for each process' service time
	sumT = tmp_pc[0].arriveT;
	for (i = 0; i < process_cnt - 1; i++) {
		sumT += tmp_pc[i].serviceT;
		if (tmp_pc[i + 1].arriveT > sumT)
			sumT += tmp_pc[i + 1].arriveT - sumT;
	}

	sumT += tmp_pc[process_cnt - 1].serviceT;

	//free malloc memory
	free(tmp_pc);
	return sumT;
}

//Get count of parameter queue
int getQueueSize(PROC_QUEUE* queue) {
	PROCESS* pointer = queue->Q;
	int cnt = 0;
	int i = 0;
	for (; i < MAX_Q; i++) {
		if (pointer[i].serviceT > 0) cnt++;
	}
	return cnt;
}

//Flush process by index
void processRefresh(PROCESS* proc, int index) {
	proc[index].pid = -1;
	proc[index].arriveT = 0;
	proc[index].serviceT = 0;
	proc[index].name = ' ';
	proc[index].runT = 0;
	proc[index].state = READY;
}

//show scheduled graph by parameter array (width: total service time, height: process' count
void draw(int** arr, PROCESS pc[]) {
	int i = 0;
	int k = 0;
	int height = process_cnt;
	int width = getSumST(pc);
	for (i = 0; i < height; i++) {
		printf("%c ", pc[i].name);
		for (int j = 0; j < width; j++) {
			if (arr[i][j] == 0) printf("¡à");
			else printf("¡á");
		}
		printf("\n");
	}
}

//push process to queue's tail
void insertQueue(PROCESS proc, PROC_QUEUE* queue) {
	int head = queue->head;
	int tail = queue->tail;

	if ((tail + 1) % MAX_Q == head) {
		printf("ERROR :: Queue is full\n"); return; //error exception (if queue is full)
	}

	proc.runT = 0;
	queue->Q[tail] = proc;
	queue->tail = (tail + 1) % MAX_Q;
}

//Do FIFO scheduling
void FIFO(PROCESS pc[]) {
	int i = 0;
	int j = 0;
	int time = 0;
	int height = process_cnt;
	int width = getSumST(pc);
	PROC_QUEUE Q_single = { 0, }; //FIFO uses one queue

	//create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++) {
		arr[i] = (int*)malloc(sizeof(int) * width);
	}

	//Init queue
	PROCESS* queue = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
	Q_single.head = 0;
	Q_single.tail = 0;
	Q_single.Q = queue;

	//Protect error of memory leaked
	for (i = 0; i < MAX_Q; i++) processRefresh(queue, i);

	//Init result array 
	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;

	//Init process before sched
	for (i = 0; i < process_cnt; i++) pc[i].state = READY;

	//Doing simulation
	for (; time < getSumST(pc); time++) {
		for (i = 0; i < process_cnt; i++) {
			if (pc[i].state == READY && pc[i].arriveT <= time) { //only insert by process arrival which is READY state
				insertQueue(pc[i], &Q_single);
				pc[i].state = RUN;
			}
		}
		if (getQueueSize(&Q_single)) FIFO_pop(&Q_single, pc, time, arr); //only pop when there is over one process
	}

	draw(arr, pc);

	for (i = 0; i < height; i++) free(arr[i]);
	free(queue);
}

//pop fifo's queue by param of singleQ pointer
void FIFO_pop(PROC_QUEUE* SQ_pointer, PROCESS* pc, int time, int** arr) {
	int i = 0;
	int head = SQ_pointer->head;
	int tail = SQ_pointer->tail;

	PROCESS* queue_pointer = SQ_pointer->Q;
	int pid = queue_pointer[head].pid;

	queue_pointer[head].serviceT--; //reduce service time which is running
	if (queue_pointer[head].serviceT == 0) { //Process end
		arr[pid][time] = 1;
		processRefresh(queue_pointer, head);
		SQ_pointer->head = (head + 1) % MAX_Q;
	}
	arr[pid][time] = 1;
}

//Do RR scheduling
void RR(PROCESS pc[], int tq)
{
	int height = process_cnt;
	int width = getSumST(pc);
	int i;
	int j;
	int time;
	int insert_flag = 0;

	//create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++) arr[i] = (int*)malloc(sizeof(int) * width);

	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;

	for (i = 0; i < process_cnt; i++) pc[i].state = READY;

	PROC_QUEUE Q_single = { 0, }; //RR uses one queue
	PROCESS* queue = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
	Q_single.head = 0;
	Q_single.tail = 0;
	Q_single.Q = queue;
	Q_single.TQ = tq;

	for (i = 0; i < MAX_Q; i++) processRefresh(queue, i);

	//Doing simulation
	for (time = 0; time < getSumST(pc); time++) {
		for (j = 0; j < process_cnt; j++) {
			if (pc[j].arriveT == time) { //check by process arrival time
				if(!insert_flag) insertQueue(pc[j], &Q_single);
			}
		}
		insert_flag = 0;
		if (getQueueSize(&Q_single)) RR_pop(&Q_single, pc, time, arr, &insert_flag);
		else continue;
	}

	draw(arr, pc);

	for (i = 0; i < height; i++) free(arr[i]);
	free(queue);
}

//pop rr's queue by param of singleQ pointer
void RR_pop(PROC_QUEUE* SQ_pointer, PROCESS pc[], int time, int** arr, int* flag) {
	int i = 0;
	int head = SQ_pointer->head;
	int tail = SQ_pointer->tail;
	int tq = SQ_pointer->TQ;
	PROCESS* queue_pointer = SQ_pointer->Q;
	int pid = queue_pointer[head].pid;
	int _runT = queue_pointer[head].runT;

	if (head == tail) printf("ERROR :: Queue is empty"); //error exception for empty queue

	queue_pointer[head].serviceT--; //if process run - reduce svt, plus runT
	queue_pointer[head].runT++;

	if (queue_pointer[head].serviceT == 0) { //process end
		arr[pid][time] = 1;
		processRefresh(queue_pointer, head);
		SQ_pointer->head = (head + 1) % MAX_Q;
	}
	else {
		if (_runT < tq - 1) { //if time quantum is enough
			arr[pid][time] = 1;
			return;
		}
		for (; i < process_cnt; i++) {
			if (pc[i].arriveT == time + 1) { //check by arrival time
				insertQueue(pc[i], SQ_pointer);
				*flag = 1;
			}
		}
		insertQueue(queue_pointer[head], SQ_pointer);
		arr[pid][time] = 1;
		processRefresh(queue_pointer, head);
		SQ_pointer->head = (head + 1) % MAX_Q;
	}
}

//Do MLFQ scheduling
void MLFQ(PROCESS pc[], int MLFQ_cnt) {
	int height = process_cnt;
	int width = getSumST(pc);
	int i = 0;
	int j = 0;
	int q_cnt = 0;
	int q_index;
	int proc_index;
	int time;
	int end_flag = 0;
	int continue_flag = 0;
	int pow_val;

	//create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i; i < height; i++) arr[i] = (int*)malloc(sizeof(int) * width);
	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;
	for (i = 0; i < process_cnt; i++) {
		pc[i].state = READY;
	}

	PROC_QUEUE* linked_Q = (PROC_QUEUE*)malloc(sizeof(PROC_QUEUE) * MLFQ_cnt); // create linked queue

	//if q_index is over 1 -> TQ is 2^i
	for (q_index = 0; q_index < MLFQ_cnt; q_index++) {
		pow_val = 1;
		linked_Q[q_index].head = 0;
		linked_Q[q_index].tail = 0;
		linked_Q[q_index].TQ = 1;
		for(j=0; j<q_index; j++){ //is same with pow(2, q_index);
			pow_val *= 2;
		}
		linked_Q[q_index].TQ = pow_val;
		linked_Q[q_index].Q = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
		for (i = 0; i < MAX_Q; i++)	processRefresh(linked_Q[q_index].Q, i);
	}

	//Doing simulation
	for (time = 0; time < width; time++) {
		continue_flag = 0;
		end_flag = 0;
		for (j = 0; j < process_cnt; j++) {
			if ((pc[j].state == READY) && (pc[j].arriveT <= time)) { //check by arrival time
				pc[j].state = RUN;
				insertQueue(pc[j], &linked_Q[0]);
			}
		}
		for (q_index = 0; q_index < MLFQ_cnt; q_index++) { //pop which is TOP priority process in queue
			for (proc_index = 0; proc_index < getQueueSize(&linked_Q[q_index]); ) {
				continue_flag = MLFQ_pop(linked_Q, pc, q_index, time, arr, MLFQ_cnt); //set which is continue or break

				if (continue_flag == 1) { //if continue
					time++;
					for (j = 0; j < process_cnt; j++) { //check new process' arrived while running sched
						if ((pc[j].state == READY) && (pc[j].arriveT <= time)) { //check by process arrival
							pc[j].state = RUN;
							insertQueue(pc[j], &linked_Q[0]);
						}
					}
					continue;
				}
				else {
					end_flag = 1;
					break; //If process - pop -> break in loop.
				}
			}
			if (end_flag) //check process end
				break;
		}
	}

	draw(arr, pc);

	for (i = 0; i < height; i++) free(arr[i]);
	free(linked_Q);
}

//pop mlfq's queue by param of Q pointer
int MLFQ_pop(PROC_QUEUE* Q_pointer, PROCESS pc[], int q_index, int time, int** arr, int MLFQ_cnt) {
	int i;
	int j;
	int p_cnt = 0;
	int flag = 0;
	int head = Q_pointer[q_index].head;
	int tail = Q_pointer[q_index].tail;
	int _runT = Q_pointer[q_index].Q[head].runT;
	int pid = Q_pointer[q_index].Q[head].pid;

	if (head == tail) {
		printf("ERROR :: Queue is empty\n"); return -1; //error exception for empty queue
	}

	if (_runT < Q_pointer[q_index].TQ - 1) { //stay in current queue
		Q_pointer[q_index].Q[head].serviceT--;
		if (Q_pointer[q_index].Q[head].serviceT == 0) { //if process end
			Q_pointer[q_index].head = (head + 1) % MAX_Q;
			processRefresh(Q_pointer[q_index].Q, head);
			arr[pid][time] = 1;
			return 0;
		}
		else {
			Q_pointer[q_index].Q[head].runT = _runT + 1;
			arr[pid][time] = 1;
			return 1;
		}
	}
	else { //move to lower priority of queue
		Q_pointer[q_index].Q[head].serviceT--;
		if (Q_pointer[q_index].Q[head].serviceT == 0) { //if process end
			Q_pointer[q_index].head = (head + 1) % MAX_Q;
			processRefresh(Q_pointer[q_index].Q, head);
			arr[pid][time] = 1;
			return 0;
		}
		else {
			for (i = 0; i < MLFQ_cnt; i++) p_cnt += getQueueSize(&Q_pointer[i]); //get count of process in linked_Q

			if (p_cnt == 1) { //if only one process exist in linked Q -> stay in current queue
				for (j = 0; j < process_cnt; j++) {
					if (pc[j].arriveT == time + 1) { // If there is new process on next time -> insert new process first
						flag = 1;
						insertQueue(pc[j], &Q_pointer[0]);
						pc[j].state = RUN;
					}
				}
				if (flag) {
					if (q_index == MLFQ_cnt - 1) {
						arr[pid][time] = 1;
						insertQueue(Q_pointer[q_index].Q[head], &Q_pointer[q_index]);
						processRefresh(Q_pointer[q_index].Q, head);
						Q_pointer[q_index].head = (head + 1) % MAX_Q;
						return 0;
					}
					insertQueue(Q_pointer[q_index].Q[head], &Q_pointer[q_index + 1]);
					Q_pointer[q_index].head = (head + 1) % MAX_Q;
					processRefresh(Q_pointer[q_index].Q, head);
					arr[pid][time] = 1;
					return 0;
				}
				else { //maintain process at same Q
					arr[pid][time] = 1;
					return 0;
				}
			}
			if (q_index == MLFQ_cnt - 1) { //exception for lowest priority queue
				for (j = 0; j < process_cnt; j++) {
					if (pc[j].arriveT == time + 1) { //if there is new process on next time -> insert new process first
						insertQueue(pc[j], &Q_pointer[0]);
						pc[j].state = RUN;
					}
				}
				insertQueue(Q_pointer[q_index].Q[head], &Q_pointer[q_index]); //do RR sched
			}
			else {
				insertQueue(Q_pointer[q_index].Q[head], &Q_pointer[q_index + 1]);
			}
			Q_pointer[q_index].head = (head + 1) % MAX_Q;
			processRefresh(Q_pointer[q_index].Q, head);
			arr[pid][time] = 1;
			return 0;
		}
	}
}

void STRIDE(PROCESS pc[]) {
	int height = process_cnt;
	int width = getSumST(pc);
	int i, j;
	int time = 0;
	int index = 0;
	int totalTickets;
	int schTarget = 0;
	int* schCandidate = (int*)malloc(sizeof(int) * process_cnt); //set sched candidate by each time

	for (i = 0; i < process_cnt; i++) schCandidate[i] = -1;

	//create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++) arr[i] = (int*)malloc(sizeof(int) * width);

	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0; //init array

	//init process before simulation
	for (i = 0; i < process_cnt; i++) {
		pc[i].state = READY;
		pc[i].runT = 0;
		pc[i].runStride = 0;
	}

	totalTickets = calcTotalTickets(pc);
	printf("\nEach process' stride -> ");

	for (i = 0; i < process_cnt; i++) { //set process stride by service time
		pc[i].stride = totalTickets / pc[i].serviceT;
		printf("[%c: %d] ", pc[i].name, pc[i].stride);
	} printf("\n");

	while (1) {
		index = 0;

		for (i = 0; i < process_cnt; i++) {
			if (pc[i].arriveT <= time && pc[i].runT <= pc[i].serviceT) schCandidate[index++] = pc[i].pid;
		}

		schTarget = getPidSmallStride(pc, schCandidate, index);

		if (schTarget >= 0) {
			if (pc[schTarget].runT <= pc[schTarget].serviceT) { //sched if serviceT remain
				arr[schTarget][time] = 1;
				pc[schTarget].runT++;
				pc[schTarget].runStride += pc[schTarget].stride;
			}
			else delCand(schCandidate, schTarget);
		}
		time++;
		if (time == width) break;
	}
	draw(arr, pc);
}

//delete candidate in array
void delCand(int* cand, int val) {
	int i, tmp;
	for (i = 0; i < process_cnt; i++) {
		if (cand[i] == val) {
			tmp = cand[i];
			cand[i] = cand[i + 1];
			cand[i + 1] = tmp;
		}
	} cand[i + 1] = -1;
}

//calculation by each service times' LCM
int calcTotalTickets(PROCESS pc[]) {
	int i;
	if (process_cnt > 1) {
		int l = getLCM(pc[0].serviceT, pc[1].serviceT);
		for (i = 2; i < process_cnt; i++) l = getLCM(l, pc[i].serviceT);
		return l;
	}
	else return pc[0].serviceT;
}

//get LCM after get GCD
int getGCD(int n, int m) {
	if (m == 0) return n;
	else return getGCD(m, n % m);
}
int getLCM(int n, int m) { return n * m / getGCD(n, m); }

//get process' pid if it's stride is smallest
int getPidSmallStride(PROCESS pc[], int schC[], int index) {
	int i;
	int value = pc[schC[0]].runStride;
	int pid = pc[schC[0]].pid;
	for (i = 1; i < index; i++) {
		if (value > pc[schC[i]].runStride) {
			value = pc[schC[i]].runStride;
			pid = pc[schC[i]].pid;
		}
	} return pid;
}
