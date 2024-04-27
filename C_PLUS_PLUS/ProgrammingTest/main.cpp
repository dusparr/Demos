// QueueListBased.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include "QueueListbased.h"
#include <cstdlib>
#include <time.h>

void playingaround()
{
	srand(time(NULL));
	int numberOfQ = (rand() % 20)+1;
	Q ** Queues = new Q*[numberOfQ];
	int * QueueNum = new int[numberOfQ];
	for (int i = 0; i < numberOfQ; i++)
	{
		Queues[i] = create_queue();
		QueueNum[i] = 0;
	}
	unsigned char i = 255;
	unsigned int numOfBytesAlloced = 0;
	unsigned int totalLoops = 0;
	while (1)
	{
		totalLoops++;
		int num = rand() % numberOfQ;
		int r = rand() % 255;
		i--;
		printf("Queue # %d enq %d\n", num, r);
		enqueue_byte(Queues[num], r);
		numOfBytesAlloced++;
		QueueNum[num]++;
		if (i % 3)
		{
			int num = rand() % numberOfQ;
			int r = rand() % 255;
			printf("Queue # %d enq %d\n", num, r);
			enqueue_byte(Queues[num],r);
			QueueNum[num]++;
			numOfBytesAlloced++;
		}
		if (i > 100)
		{
			int num = rand() % numberOfQ;
			int r = rand() % 255;
			printf("Queue # %d enq %d\n", num, r);
			enqueue_byte(Queues[num], r);
			QueueNum[num]++;
			numOfBytesAlloced++;
		}
		if (i % 6 && totalLoops > 50)
		{
			int num = rand() % numberOfQ;
			printf("Queue # %d dec %d\n", num, dequeue_byte(Queues[num]));
			numOfBytesAlloced--;
			QueueNum[num]--;
			num = rand() % numberOfQ;
			printf("Queue # %d dec %d\n", num, dequeue_byte(Queues[num]));
			numOfBytesAlloced--;
			QueueNum[num]--;
		}
	}
}

int main()
{
	int i = sizeof(Q);
	// Must call this FIRST
	InitializeQueueSpace();
	//playingaround();
	Q * q0 = create_queue();
	enqueue_byte(q0, 0);
	enqueue_byte(q0, 1);
	Q * q1 = create_queue();
	enqueue_byte(q1, 3);
	enqueue_byte(q0, 2);
	enqueue_byte(q1, 4);
	printf("%d", dequeue_byte(q0));
	printf("%d\n", dequeue_byte(q0));
	enqueue_byte(q0, 5);
	for(int i = 0; i < 109*15; i++)
		enqueue_byte(q1, 6);
	printf("%d", dequeue_byte(q0));
	printf("%d\n", dequeue_byte(q0));
	
	printf("%d", dequeue_byte(q1));
	printf("%d", dequeue_byte(q1));
	printf("%d\n", dequeue_byte(q1));
	
	destroy_queue(q1);
	destroy_queue(q0);
	
	return 0;
}



void on_out_of_memory() { printf("Out Of Memory INPUT BEFORE CLOSING\n"); char  c; scanf_s(&c); exit(-1); };
void on_illegal_operation() { printf("Illegal opINPUT BEFORE CLOSING\n"); char  c; scanf_s(&c); exit(-1); };