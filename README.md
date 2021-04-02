# Scheduler Simulator
(FIFO, RR, MLFQ, STRIDE)

운영체제 강의 과제에서 구현한 스케줄러 시뮬레이터이다. C로 작성되었으며 FIFO(FCFS), RR, MLFQ, Stride에 대한 스케줄링 알고리즘을 눈으로 볼 수 있다.

## Structure
- include: 스케줄러에 필요한 헤더가 포함된 directory
- lab1_sched.c: 스케줄러 알고리즘을 실질적으로 구현한 c file
- lab1_sched_test.c: 결과를 출력할 main 함수가 포함된 c file
- Makefile: make 커맨드를 사용하기 위해 gcc가 설정된 Makefile


## Automation - Using Makefile
```
CC = gcc
INC = -I${CURDIR}/include/
CFLAGS = -g $(INC)
OBJS_LAB1 = lab1_sched.o lab1_sched_test.o 
SRCS = $(OBJS_LAB1:.o=.c)
TARGET_LAB1 = lab1_sched
.SUFFIXES : .c .o
.c.o:
    @echo "Compiling lab1 scheduler simulator $< ..."
    $(CC) -c $(CFLAGS) -o $@ $<
$(TARGET_LAB1) : $(OBJS_LAB1)
    $(CC) -o $(TARGET_LAB1) $(OBJS_LAB1)
all : $(TARGET_LAB1)
dep : 
    gccmaedep $(INC) $(SRCS)
clean :
    @echo "Cleaning lab1 scheduler simulator $< ..."
    rm -rf $(OBJS_LAB1) $(TARGET_LAB1) 
new :
    $(MAKE) clean
    $(MAKE)
```

## Scheduling Algorithm
### FIFO
FIFO(FCFS)는 먼저 도착하는 순서대로 프로세스를 스케줄링하는 기법이다. 도착한 순서(Arrival Time)대로 single queue에 삽입되고, 스케줄링되는 프로세스의 수행 시간(Service time)이 0이 되면, 다음으로 도착한 프로세스를 수행시킨다.

구현된 모든 스케줄링 알고리즘 (FIFO, RR, MLFQ, STRIDE)는 Queue와 Array(구현 결과가 저장될 이차원 배열)을 초기화시킨 후 시작한다. 프로세스가 도착하면 Queue에 넣고 state(flag)를 RUN 상태로 바꾼다. 시간이 1초씩 증가함에 따라 queue가 비어있지 않으면 FIFO_pop 함수를 호출하여 queue의 가장 앞(head)에 있는 프로세스의 service time의 1초를 수행한다. 이 과정을 반복하다가 해당 프로세스의 service time이 0이 되면 queue의 맨 앞을 플러시(processEnd 함수)하고 queue를 pop한다.

### RR(Round Robin)
RR은 도착한 순서대로 single queue에 삽입되고, 시간적 할당(Time slice)를 주어서 time slice만큼 프로세스를 수행하고 service time이 남아있다면 다시 queue에 들어가는 방식이다. queue와 array를 초기화시키고 프로세스의 arrival time에 따라 insertQueue 함수를 통해 queue에 삽입한다. 그 후에 queue에 프로세스가 하나라도 있다면 RR_pop을 호출한다. 또한 프로세스의 상태를 알기 위해 insert_flag를 사용한다. time slice가 지나고 service time이 남아 있는 경우 queue의 가장 뒤(tail)에 삽입한다.


### MLFQ
MLFQ 본연의 정책에 어긋나지 않으려 노력했다. queue의 개수에 따라 타임 퀀텀()을 자동으로 할당한다. 시간을 증가시키며 도착하는 프로세스를 queue의 Top에 삽입한다. 그리고 queue를 탐색하여 스케줄링 대상을 결정한다. MLFQ의 기본적인 정책은 우선 순위가 가장 높은 queue부터 아래로 내려가며 제일 먼저 탐색된 프로세스를 수행시키는 것이다. 프로세스가 스케줄링되면 수행 완료 후 다음 queue의 위치를 선택해야 하며, queue에 설정된 time slice를 모두 사용하면 lower priority queue에 삽입하고, 그렇지 않다면 priority는 그대로 유지한다. 그리고 lowest priority queue에 도달하면 해당 queue의 time slice만큼 Round Robin의 방식으로 수행하게 된다.


위에서 설명한 알고리즘이 기본이고, 다른 예외 처리를 많이 했다. 모든 queue에서 프로세스가 하나라면 큐의 time slice를 모두 사용해도 lower priority로 내리지 않는다. 또한, 수행한 프로세스가 다시 queue에 삽입됨과 동시에 새롭게 도착한 프로세스가 있다면 새로운 프로세스에 우선 순위를 주었다.


### Stride
STRIDE 스케줄링은 프로세스마다 Ticket을 부여하여 Stride를 계산하고, time slice마다 가장 낮은 stride의 프로세스를 수행하고 해당 프로세스의 stride를 누적하는 방식이다. ticket은 프로세스의 service time에 따라 자동으로 할당하고, 각 ticket의 LCM(최소공배수)를 구하여 각 프로세스의 stride를 정한다. 1초마다 스케줄링 대상이 되는 프로세스(schCandidate) 중 수행할 프로세스(schTarget)를 결정하기 위해 각 프로세스의 stride를 탐색하고 가장 낮은 stride를 가진 프로세스를 수행한다. 수행한 프로세스의 service time이 0이 되면 스케줄링 후보에서 삭제한다. 이 과정을 반복하다가 time이 width(프로세스 service time의 합)와 같아지면 구현을 종료한다.
