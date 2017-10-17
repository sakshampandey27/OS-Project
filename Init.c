#include<stdio.h>
#include<stdlib.h>
#define numResources 5
int maxResources[numResources];
int currentResources[numResources];
enum types {requestNew, releaseOld};

unsigned long long int timeElapsed;

struct process
{
	int burstTime,arrivalTime;
	int ResourcesAllocated[numResources];
};

struct requestLog
{
	int requestType;
	int resourceNumber;
	int resourceInstances;
	struct list *rightlink, *downlink;
};

struct resourceLog *root = NULL;

/* Ready Queue Starts*/
struct queue
{
	int front,rear,currentSize;
	struct process *Arr;
};

//Ready Queue
struct queue readyQueue;

int isEmpty()
{
	if(readyQueue.front==readyQueue.rear)
		return 1;
	return 0;
}

void push(struct process p)
{
	if(readyQueue.rear>=readyQueue.currentSize)
	{
		readyQueue.Arr=realloc(readyQueue.Arr,2*readyQueue.currentSize);
		readyQueue.currentSize *=2;
	}
	readyQueue.Arr[readyQueue.rear++]=p;
}

struct process pop()
{
	return readyQueue.Arr[readyQueue.front++];
}

/* Ready Queue Ends*/

/* Init Functions Start */
void initResources()
{
	for(int i=0;i<numResources;i++)
	{
		maxResources[i]=10;
		currentResources[i]=10;
	}
}

void initReadyQueue()
{
	readyQueue.Arr=(int *)malloc(readyQueue.currentSize*sizeof(int));
	readyQueue.currentSize=1;
}

void initAll()
{
	timeElapsed=0;
	initResources();
	initReadyQueue();
}
/* Init Functions End */

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