#include<stdio.h>
#include<stdlib.h>
#define numResources 5
int maxResources[numResources];
int currentResources[numResources];

unsigned long long int timeElapsed;

struct process
{
	int burstTime,arrivalTime;
	int ResourcesAllocated[numResources];
};
/* Ready Queue Starts*/
struct queue
{
	int front,rear,currentSize;
	struct process Arr[queueSize];
};

//Ready Queue
struct queue readyQueue;

int isEmpty()
{
	if(readyQueue->front==readyQueue->rear)
		return 1;
	return 0;
}

void push(struct process p)
{
	if(readyQueue->rear>=readyQueue->currentSize)
	{
		readyQueue=realloc(readyQueue,2*readyQueue->currentSize);
		readyQueue->currentSize *=2;
	}
	readyQueue->Arr[readyQueue->rear++]=p;
}

struct process pop()
{
	return readyQueue->Arr[readyQueue->front++];
}

/* Ready Queue Ends*/

/* Init Functions Start */
void initMaxResources()
{
	for(int i=0;i<numResources;i++){
		maxResources[i]=10;
		currentResources[i]=10;
	}
}

void initReadyQueue()
{
	readyQueue=(struct queue *)malloc(1*sizeof(struct queue));
	readyQueue->currentSize=1;
}

void initAll()
{
	timeElapsed=0;
	initMaxResources();
	initReadyQueue();
}
/* Init Functions End */

unsigned long long int getArrivalTime()
{
	return timeElapsed;
}

void newProcess()
{
	printf("Enter new process details : ");
	struct process p;
	p=(struct process *)malloc(sizeof(struct process));
	p->arrivalTime=getArrivalTime();
	printf("Enter burst Time : ");
	scanf("%d",&p->burstTime);
	printf("Enter initial resources required : ");
	for(int i=0;i<numResources;i++)
	{
		printf("Enter request for resource %d",i);
		scanf("%d",&p->ResourcesAllocated[i]);
	}
	//Push process to end of queue
	push(p);
}

int getProcessIndex()
{
	int ID;
	printf("Enter process ID : ");
	scanf("%d",&ID);
	for(i=readyQueue->front;i<readyQueue->rear;i++)
	{
		if(readyQueue->Arr[i].processID==ID)
			break;
	}
	return i;
}

void requestResources() //
{
	int ID=getProcessIndex();
	int resourceNumber,numberOfInstances;
	printf("Process ID is : %d \n",ID);
	printf("Enter resource number for request");
	scanf("%d",&resourceNumber);
	printf("Enter number of instances to request");
	scanf("%d",&numberOfInstances);
	log(ID,requestNew,resourceNumber,numberOfInstances);
	//Request kept as a log
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