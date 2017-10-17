#include<stdio.h>
#include<stdlib.h>
#define numResources 5
#define queueSize 500
int maxResources[numResources];
int currentResources[numResources];
enum types {requestNew, releaseOld};
unsigned long long int timeElapsed;

struct process
{
	int burst;
	int resourcesAllocated[NumResources];
};

struct requestLog
{
	int requestType;
	int resourceNumber;
	int resourceInstances;
	struct list *rightlink, *downlink;
};
/* Ready Queue Starts*/
struct resourceLog *root = NULL;
struct queue
{
	int front,rear;
	struct process Arr[queueSize];
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
	{
		maxResources[i]=10;
		currentResources[i]=10;
	}
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

void currentStatus()
{
	for (int i=q->front;i<q->rear;i++)
	{
		printf("P%d\t",q->Arr[i]->processID);
	}
}

void log(int id, int type, int resNum, int resInstances)
{
	struct resourceLog *curr=root;
	struct resourceLog newRequest;
	newRequest = (struct resourceLog *)malloc(sizeof(struct resourceLog));
	newRequest->requestType = type;
	newRequest->resourceNumber = resNum;
	newRequest->resourceInstances = resInstances;	
	if (curr==NULL)
	{
		curr=(struct resourceLog *)malloc(sizeof(struct resourceLog));
		curr->rightlink=NULL;
		curr->requestType = id;
		curr->resourceNumber = curr->resourceInstances = -1;
		curr->downlink = -1;		
	}
	while (curr->downlink!=NULL)
	{
		if (curr->requestType == id)
		{
			while (curr->rightlink!=NULL)
				curr = curr->rightlink;
			curr->rightlink = newRequest;
		}
		else
			curr = curr->downlink;
	}
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
	printf("Which resource do you want to release? \n");
	scanf("%d",&ch);
	printf("How many instances do you want to release? \n")
	scanf("%d",&num);
	log(processNumber->processID, releaseOld, ch, num);
	/*
	if (num > processNumber->resourcesAllocated[ch])
		printf("Insufficent resources!\nCurrent allocation = %d",processNumber->resourcesAllocated[ch]);
	else if (num < processNumber->resourcesAllocated[ch])
	{
		currentResources[ch] += num;
		processNumber->resourcesAllocated[ch++] -= num;		
	}
	else
	{
		currentResources[ch] += num;
		processNumber->resourcesAllocated[ch]=0;
	}
	*/
	currentStatus();
}

void abortProcess(struct process processNumber)
{
	int k=0;
	struct process processNum;
	char ch;
	printf("Are you sure you want to abort this process?(Y/N)");
	scanf("%c", &ch);
	if (ch=='Y')
	{
		currentResources[k] += processNumber->resourcesAllocated[k];
		processNumber->resourcesAllocated[k++] = 0;
	}
	processNum = pop(q);
	if (processNum->processID!=-1)
		printf("Process has been successfully aborted!");
	else
		printf("Abortion unsuccessful");
	currentStatus();
}

int main()
{
	initAll();
	return 0;
}