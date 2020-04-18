#
#	DKU Operating System Lab
#	    Lab1 (Scheduler Algorithm Simulator)
#	    Student id : 32164959, 32162436
#	    Student name : Heo Jeon Jin, Shin Chang Woo
#
#   Makfeile :
#       - Makefile for lab1 compilation.
#

CC = gcc
INC = -I${CURDIR}/include/
CFLAGS = -g $(INC)

OBJS_LAB1 = lab1_sched.o lab1_sched_test.o 

SRCS = $(OBJS_LAB1:.o=.c)

TARGET_LAB1 = lab1_sched

.SUFFIXES : .c .o

.c.o:
	@echo "Compiling lab1 scheduler simulator $< ..."
	$(CC) -c $(CFLAGS) -o $@ $< -lm

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
