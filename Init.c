#include<stdio.h>
#include<stdlib.h>
#define numResources 5
#define queueSize 500
int maxResources[numResources];

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
	initMaxResources();
}

int main()
{
	initAll();
	return 0;
}