#include<stdio.h>
#include<stdlib.h>
#define numResources 5
#define queueSize 500
int maxResources[numResources];

unsigned long long int timeElapsed;

struct process
{
	int burst;
	int Res[NumResources];
};
/* Ready Queue Starts*/

struct queue
{
	int front,rear;
	int Arr[queueSize];
};

int isEmpty(struct queue *q)
{
	if(q.front==q.rear)
		return 1;
	return 0;
}

void push(struct queue *q,struct process p)
{
	q.Arr[q.rear++]=p;
}

struct process pop(struct queue *q)
{
	return q.Arr[q.front++];
}

/* Ready Queue Ends*/
void initMaxResources()
{
	for(int i=0;i<numResources;i++)
		maxResources[i]=10;
}

void initAll()
{
	timeElapsed=0;
	initMaxResources();
}

unsigned long long int getArrivalTime()
{
	return timeElapsed;
}

void newProcess()
{
	printf("Enter new process details : ");
	struct process p;
	p=(struct process *)malloc(sizeof(struct process));
	printf("Enter burst Time")
}

void requestResources(struct process processNumber)
{

}

void releaseResources(struct process processNumber)
{

}

void abortProcess(struct process processNumber)
{

}

int main()
{
	initAll();
	return 0;
}