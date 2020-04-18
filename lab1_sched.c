/*
*    DKU Operating System Lab
*        Lab1 (Scheduler Algorithm Simulator)
*        Student id : 32164959, 32162436
*        Student name : Heo Jeon Jin, Shin Chang Woo
*
*   lab1_sched.c :

*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
*
*/
///////Linux header//////////
// 리눅스 환경일 때 주석 풀면됨
// #include <aio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <asm/unistd.h>
// #include <sys/time.h>
/////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <math.h>
#include "lab1_sched_types.h"

int process_cnt;

void setProcess(PROCESS* pointer) {
	printf("\n▶ How many process (1 to 50) -> ");
	scanf("%d", &process_cnt);

	PROCESS* pc = (PROCESS*)malloc(sizeof(PROCESS) * process_cnt);

	for (int i = 0; i < process_cnt; i++) {
		pc[i].name = 'A' + i;
		pc[i].pid = i;
		pc[i].state = READY;
		printf("\n\n*--- Process %c ---*\n", pc[i].name);
		printf("▷ Arrival time -> "); scanf("%d", &pc[i].arriveT);
		printf("▷ Service time -> "); scanf("%d", &pc[i].serviceT);
		*(pointer + i) = pc[i];
	}

	free(pc);
}

int getSumST(PROCESS pc[]) {
	int i = 0;
	int j = 0;
	int sumT = 0;
	PROCESS* tmp_pc;
	PROCESS tmp_proc;

	tmp_pc = (PROCESS*)malloc(sizeof(PROCESS) * process_cnt);

	for (i = 0; i < process_cnt; i++) tmp_pc[i] = pc[i];

	// Sort process array by process arrival time
	for (i = 0; i < process_cnt; i++) {
		for (j = i; j < process_cnt; j++) {
			if (tmp_pc[i].arriveT > tmp_pc[j].arriveT) {
				tmp_proc = tmp_pc[i];
				tmp_pc[i] = tmp_pc[j];
				tmp_pc[j] = tmp_proc;
			}
		}
	}
	// Calculate total scheduling time   
	sumT = tmp_pc[0].arriveT;
	for (i = 0; i < process_cnt - 1; i++) {
		sumT += tmp_pc[i].serviceT;
		if (tmp_pc[i + 1].arriveT > sumT)
			sumT += tmp_pc[i + 1].arriveT - sumT;
	}

	sumT += tmp_pc[process_cnt - 1].serviceT;

	free(tmp_pc);
	return sumT;
}

int getQueueSize(PROC_QUEUE* queue) {
	PROCESS* pointer = queue->Q;
	int cnt = 0;
	int i = 0;
	for (; i < MAX_Q; i++) {
		if (pointer[i].serviceT > 0) cnt++;
	}
	return cnt;
}

void processRefresh(PROCESS* proc, int index) {
	proc[index].pid = -1;
	proc[index].arriveT = 0;
	proc[index].serviceT = 0;
	proc[index].name = ' ';
	proc[index].runT = 0;
	proc[index].state = READY;
	proc[index].rec = 0;
	proc[index].first_runT = 0;
	proc[index].final_endT = 0;
}

void draw(int** arr, PROCESS pc[]) {
	int i = 0;
	int k = 0;
	int height = process_cnt;
	int width = getSumST(pc);
	for (i = 0; i < height; i++) {
		printf("%c ", pc[i].name);
		for (int j = 0; j < width; j++) {
			if (arr[i][j] == 0) printf("□");
			else printf("■");
		}
		printf("\n");
	}
}

void insertQueue(PROCESS proc, PROC_QUEUE* queue) {
	int head = queue->head;
	int tail = queue->tail;

	if ((tail + 1) % MAX_Q == head) {
		printf("ERROR :: Queue is full\n"); return; // exception for queue full
	}

	proc.runT = 0;
	queue->Q[tail] = proc;
	queue->tail = (tail + 1) % MAX_Q;
}

void FIFO(PROCESS pc[]) {
	int i = 0;
	int j = 0;
	int time = 0;
	int height = process_cnt;
	int width = getSumST(pc);
	PROC_QUEUE Q_single = { 0, };

	// Create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++) {
		arr[i] = (int*)malloc(sizeof(int) * width);
	}

	// Create queue
	PROCESS* queue = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
	Q_single.head = 0;
	Q_single.tail = 0;
	Q_single.Q = queue;

	// Prevent memory leaking error
	for (i = 0; i < MAX_Q; i++) processRefresh(queue, i);

	// Initiate result array 
	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;

	// Initiate process info before simulation
	for (i = 0; i < process_cnt; i++) {
		pc[i].state = READY;
		pc[i].rec = 0;
		pc[i].first_runT = 0;
		pc[i].final_endT = 0;
	}

	// Time - based simulation start 
	for (; time < getSumST(pc); time++) {
		for (i = 0; i < process_cnt; i++) {
			if (pc[i].state == READY && pc[i].arriveT <= time) { // Check for process arrival
				insertQueue(pc[i], &Q_single);
				pc[i].state = RUN;
			}
		}
		if (getQueueSize(&Q_single)) FIFO_pop(&Q_single, pc, time, arr); //Pop only if there's at least one process
	}

	draw(arr, pc); // Print out result

	for (i = 0; i < height; i++) free(arr[i]);
	free(queue);
}

void FIFO_pop(PROC_QUEUE* SQ_pointer, PROCESS* pc, int time, int** arr) {
	int i = 0;
	int head = SQ_pointer->head;
	int tail = SQ_pointer->tail;

	PROCESS* queue_pointer = SQ_pointer->Q;
	int pid = queue_pointer[head].pid;

	queue_pointer[head].serviceT--; // Run
	if (queue_pointer[head].serviceT == 0){ // Process end
		pc[pid].final_endT = time + 1;
		arr[pid][time] = 1;
		processRefresh(queue_pointer, head);
		SQ_pointer->head = (head + 1) % MAX_Q;
	}
	if (pc[pid].rec == 0){ // Record arrival time
		pc[pid].first_runT = time;
		pc[pid].rec = 1;
	}
	arr[pid][time] = 1;
}

void RR(PROCESS pc[], int tq)
{
	int height = process_cnt;
	int width = getSumST(pc);
	int i = 0;
	int j = 0;
	int time = 0;
	int insert_flag = 0;

	// Create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i; i < height; i++) arr[i] = (int*)malloc(sizeof(int) * width);

	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;

	for (i = 0; i < process_cnt; i++)
	{
		pc[i].state = READY;
		pc[i].rec = 0;
		pc[i].first_runT = 0;
		pc[i].final_endT = 0;
	}

	PROC_QUEUE Q_single = { 0, };
	PROCESS* queue = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
	Q_single.head = 0;
	Q_single.tail = 0;
	Q_single.Q = queue;
	Q_single.TQ = tq;
	
	// Prevent memory leaking error
	for (i = 0; i < MAX_Q; i++) processRefresh(queue, i);

	// Time - based simulation start
	for (; time < getSumST(pc); time++) {
		for (j = 0; j < process_cnt; j++) {
			if (pc[j].arriveT == time){ // Check for process arrival
				if (insert_flag){
					// Do nothing
				}
				else{
					insertQueue(pc[j], &Q_single);
				}
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

void RR_pop(PROC_QUEUE* SQ_pointer, PROCESS pc[], int time, int** arr, int* flag) {
	int i = 0;
	int head = SQ_pointer->head;
	int tail = SQ_pointer->tail;
	int tq = SQ_pointer->TQ;
	PROCESS* queue_pointer = SQ_pointer->Q;
	int pid = queue_pointer[head].pid;
	int _runT = queue_pointer[head].runT;

	if (head == tail) printf("ERROR :: Queue is empty");

	queue_pointer[head].serviceT--; // Run
	queue_pointer[head].runT++;

	if (queue_pointer[head].serviceT == 0) { // Process end
		pc[pid].final_endT = time + 1;
		arr[pid][time] = 1;
		processRefresh(queue_pointer, head);
		SQ_pointer->head = (head + 1) % MAX_Q;
	}
	else {
		if (pc[pid].rec == 0) { // Record arrival time
			pc[pid].first_runT = time;
			pc[pid].rec = 1;
		}
		if (_runT < tq - 1) { // Enough time quantum
			arr[pid][time] = 1;
			return;
		}
		for (; i < process_cnt; i++) {// Pre-check process arrival 
			if (pc[i].arriveT == time + 1) {
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


void MLFQ(PROCESS pc[], int MLFQ_cnt)
{
	int height = process_cnt;
	int width = getSumST(pc);
	int i = 0;
	int j = 0;
	int q_cnt = 0;
	int q_index;
	int proc_index;
	int time = 0;
	int end_flag = 0;
	int continue_flag = 0;

	// Create result array
	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i; i < height; i++) arr[i] = (int*)malloc(sizeof(int) * width);
	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;
	for (i = 0; i < process_cnt; i++) {
		pc[i].state = READY;
		pc[i].rec = 0;
		pc[i].first_runT = 0;
		pc[i].final_endT = 0;
	}

	PROC_QUEUE* linked_Q = (PROC_QUEUE*)malloc(sizeof(PROC_QUEUE) * MLFQ_cnt); // create linked queue

	for (q_index = 0; q_index < MLFQ_cnt; q_index++) {
		linked_Q[q_index].head = 0;
		linked_Q[q_index].tail = 0;
		linked_Q[q_index].TQ = pow(2, q_index);
		linked_Q[q_index].Q = (PROCESS*)malloc(sizeof(PROCESS) * MAX_Q);
		// prevent memory leaking error
		for (i = 0; i < MAX_Q; i++)	processRefresh(linked_Q[q_index].Q, i);
	}

	// Time - based simulation start
	for (; time < width; time++) {
		continue_flag = 0;
		end_flag = 0;
		for (j = 0; j < process_cnt; j++) {
			if ((pc[j].state == READY) && (pc[j].arriveT <= time)) { // Check for process arrival
				pc[j].state = RUN;
				insertQueue(pc[j], &linked_Q[0]);
			}
		}
		for (q_index = 0; q_index < MLFQ_cnt; q_index++) { // Pop top priority process from the queue
			for (proc_index = 0; proc_index < getQueueSize(&linked_Q[q_index]); ) {
				continue_flag = MLFQ_pop(linked_Q, pc, q_index, time, arr, MLFQ_cnt); // Pop process and determine whether continue or break

				if (continue_flag == 1) { // Continue process
					time++;
					for (j = 0; j < process_cnt; j++) { // Must check arrival of new process while running a job
						if ((pc[j].state == READY) && (pc[j].arriveT <= time)) { // Check for process arrival
							pc[j].state = RUN;
							insertQueue(pc[j], &linked_Q[0]);
						}
					}
					continue;
				}
				else {
					end_flag = 1;
					break; // Once the process has popped, then exit the loop.
				}
			}
			if (end_flag)
				break;
		}
	}

	draw(arr, pc);

	for (i = 0; i < height; i++) free(arr[i]);
	free(linked_Q);
}

int MLFQ_pop(PROC_QUEUE* Q_pointer, PROCESS pc[], int q_index, int time, int** arr, int MLFQ_cnt) {
	int i;
	int j;
	int p_cnt = 0;
	int flag = 0;
	int head = Q_pointer[q_index].head;
	int tail = Q_pointer[q_index].tail;
	int _runT = Q_pointer[q_index].Q[head].runT;
	int pid = Q_pointer[q_index].Q[head].pid;

	// exception for queue empty
	if (head == tail) {
		printf("ERROR :: Queue is empty\n"); return -1;
	}

	if (_runT < Q_pointer[q_index].TQ - 1) { // Stay in current queue
		Q_pointer[q_index].Q[head].serviceT--;
		if (pc[pid].rec == 0) {
			pc[pid].first_runT = time;
			pc[pid].rec = 1;
		}
		if (Q_pointer[q_index].Q[head].serviceT == 0) { // Process end
			Q_pointer[q_index].head = (head + 1) % MAX_Q;
			processRefresh(Q_pointer[q_index].Q, head);
			arr[pid][time] = 1;
			pc[pid].final_endT = time + 1;
			return 0;
		}
		else {
			Q_pointer[q_index].Q[head].runT = _runT + 1;
			arr[pid][time] = 1;
			return 1;
		}
	}
	else { // Move to lower priority queue
		Q_pointer[q_index].Q[head].serviceT--;
		if (pc[pid].rec == 0) {
			pc[pid].first_runT = time;
			pc[pid].rec = 1;
		}
		if (Q_pointer[q_index].Q[head].serviceT == 0) { // Process end
			Q_pointer[q_index].head = (head + 1) % MAX_Q;
			processRefresh(Q_pointer[q_index].Q, head);
			arr[pid][time] = 1;
			pc[pid].final_endT = time + 1;
			return 0;
		}
		else {
			for (i = 0; i < MLFQ_cnt; i++) p_cnt += getQueueSize(&Q_pointer[i]); // Get total number of process in linked_Q

			if (p_cnt == 1) { // If only one process exist in linked queue, stay in current queue
				for (j = 0; j < process_cnt; j++) {
					if (pc[j].arriveT == time + 1) { // If there's incoming process on next time insert incoming process first
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
				else { // Retain the process at the same queue
					arr[pid][time] = 1;
					return 0;
				}
			}
			if (q_index == MLFQ_cnt - 1) { // Exception for lowest queue
				for (j = 0; j < process_cnt; j++) {
					if (pc[j].arriveT == time + 1) { // If there's incoming process on next time insert incoming process first
						insertQueue(pc[j], &Q_pointer[0]);
						pc[j].state = RUN;
					}
				}
				insertQueue(Q_pointer[q_index].Q[head], &Q_pointer[q_index]); // Round robin
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
	int* schCandidate = (int*)malloc(sizeof(int) * process_cnt);

	for (i = 0; i < process_cnt; i++) {
		schCandidate[i] = -1;
	}

	int** arr = (int**)malloc(sizeof(int*) * height);
	for (i = 0; i < height; i++) {
		arr[i] = (int*)malloc(sizeof(int) * width);
	}
	for (i = 0; i < height; i++) for (j = 0; j < width; j++) arr[i][j] = 0;

	for (i = 0; i < process_cnt; i++) // Initiate process info before simulation
	{
		pc[i].state = READY;
		pc[i].runT = 0;
		pc[i].runStride = 0;
		pc[i].first_runT = 0;
		pc[i].final_endT = 0;
	}

	totalTickets = calcTotalTickets(pc);

	//set process stride by service time
	printf("\nEach process' stride -> ");
	for (i = 0; i < process_cnt; i++) {
		pc[i].stride = totalTickets / pc[i].serviceT;
		printf("[%c: %d] ", pc[i].name, pc[i].stride);
	}
	printf("\n");

	while (1) {
		index = 0;

		for (i = 0; i < process_cnt; i++) {
			if (pc[i].arriveT <= time && pc[i].runT <= pc[i].serviceT) schCandidate[index++] = pc[i].pid;
		}

		schTarget = getPidSmallStride(pc, schCandidate, index);

		if (schTarget >= 0) {
			if (pc[schTarget].runT <= pc[schTarget].serviceT) {
				arr[schTarget][time] = 1;
				pc[schTarget].runT++;
				pc[schTarget].runStride += pc[schTarget].stride;
			}
			else {
				pc[schTarget].final_endT = time + 1;
				delCand(schCandidate, schTarget);
			}
		}

		time++;

		if (time == width) break;
	}

	draw(arr, pc);
}

void delCand(int* cand, int val) {
	int i, tmp;
	for (i = 0; i < process_cnt; i++) {
		if (cand[i] == val) {
			tmp = cand[i];
			cand[i] = cand[i + 1];
			cand[i + 1] = tmp;
		}
	} cand[i + 1] = -1;
	return;
}

int calcTotalTickets(PROCESS pc[]) {
	int i;
	if (process_cnt > 1) {
		int l = getLCM(pc[0].serviceT, pc[1].serviceT);
		for (i = 2; i < process_cnt; i++) {
			l = getLCM(l, pc[i].serviceT);
		}
		return l;
	}
	else return pc[0].serviceT;
}

int getGCD(int n, int m) {
	if (m == 0) return n;
	else return getGCD(m, n % m);
}

int getLCM(int n, int m) {
	return n * m / getGCD(n, m);
}

int getPidSmallStride(PROCESS pc[], int schC[], int index) {
	int i;
	int value = pc[schC[0]].runStride;
	int pid = pc[schC[0]].pid;
	for (i = 1; i < index; i++) {
		if (value > pc[schC[i]].runStride) {
			value = pc[schC[i]].runStride;
			pid = pc[schC[i]].pid;
		}
	}
	return pid;
}