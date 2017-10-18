#include<stdio.h>
#include<stdlib.h>
#define numResources 5
int maxResources[numResources];
int currentResources[numResources];
int processCount;
enum types {requestNew, releaseOld};

unsigned long long int timeElapsed;

struct process
{
	int burstTime,arrivalTime;
	int processID;
	int resourcesAllocated[numResources];
};

struct requestLog
{
	int requestType;
	int resourceNumber;
	int resourceInstances;
	struct requestLog *rightlink, *downlink;
};

struct requestLog *root = NULL;

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

void deleteLinkedList(struct requestLog *head)
{
	if(head->rightlink==NULL) free(head);
	else
	deleteLinkedList(head->rightlink);
}

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
	readyQueue.Arr=(struct process *)malloc((readyQueue.currentSize)*sizeof(struct process));
	readyQueue.currentSize=1;
}

void initAll()
{
	timeElapsed=0;
	processCount=1000;
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
	printf("READY QUEUE: \n");
	for (int i=readyQueue.front;i<readyQueue.rear;i++)
	{
		printf("Process ID - %d\t",readyQueue.Arr[i].processID);
	}
}

int printMenu()
{
	int opt=0;
	printf("\n1.Enter a new process.\n2.Request a new resource.\n3.Release a resource.\n4.Abort a process\n5.Show process status\n");
	scanf("%d",&opt);
	return opt;
}

void grantRequests()
{
	struct requestLog *curr = root, *currRequest;
	struct process running = readyQueue.Arr[readyQueue.front];
	while (curr->requestType!=running.processID)
		curr = curr->downlink;
	currRequest = curr->rightlink;	
	while (currRequest!=NULL)
	{
		if (currRequest->requestType == requestNew)
		{
			//insert code to check if request can be granted
			running.resourcesAllocated[currRequest->resourceNumber] += currRequest->resourceInstances;
			currentResources[currRequest->resourceNumber] -= currRequest->resourceInstances;
		}		
		else if (currRequest->requestType == releaseOld)
		{
			running.resourcesAllocated[currRequest->resourceNumber] -= currRequest->resourceInstances;
			currentResources[currRequest->resourceNumber] += currRequest->resourceInstances;
		}
		currRequest = currRequest->rightlink;
	}
	printf("All pending requests for process ID - %d granted!",running.processID);
}

void logRequests(int id, int type, int resNum, int resInstances)
{
	struct requestLog *curr=root, *temp;
	struct requestLog *newRequest;
	newRequest = (struct requestLog *)malloc(sizeof(struct requestLog));
	newRequest->requestType = type;
	newRequest->resourceNumber = resNum;
	newRequest->resourceInstances = resInstances;	
	if (curr==NULL)
	{
		curr=(struct requestLog *)malloc(sizeof(struct requestLog));
		curr->rightlink=NULL;
		curr->requestType = id;
		curr->resourceNumber = curr->resourceInstances = -1;
		curr->downlink = NULL;		
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
	if (curr->downlink==NULL)
	{
		temp=(struct requestLog *)malloc(sizeof(struct requestLog));
		temp->rightlink=NULL;
		temp->requestType = id;
		temp->resourceNumber = temp->resourceInstances = -1;
		temp->downlink = NULL;		
		temp = curr->downlink;
	}
	//grantRequests();
}

struct process getProcessID(int id)
{
	int i;
	for(i=readyQueue.front;i<readyQueue.rear;i++)
	{
		if(readyQueue.Arr[i].processID==id)
			return readyQueue.Arr[i];
	}
}

void newProcess()
{
	printf("Enter new process details: \n");
	struct process *p;
	p=(struct process *)malloc(sizeof(struct process));
	p->arrivalTime=getArrivalTime();
	printf("Enter burst Time : ");
	scanf("%d",&p->burstTime);
	printf("Enter initial resources required : ");
	for(int i=0;i<numResources;i++)
	{
		printf("Enter request for resource %d",i);
		scanf("%d",&p->resourcesAllocated[i]);
		if (p->resourcesAllocated[i] > maxResources[i])
		{
			printf("Please enter a number between 0 and %d\n",maxResources[i]);
			i--;
		}
	}
	p->processID = processCount++;
	printf("Process ID is %d\n",p->processID);
	push(*p);
	currentStatus();
}

void requestResources(int procID) //
{
	struct process found=getProcessID(procID);
	if(procID!=found.processID)
	{
		printf("There is no running process with the process ID %d\n",procID);
		return;
	}
	int resourceNumber,numberOfInstances;
	printf("Enter resource number for request: ");
	scanf("%d",&resourceNumber);
	printf("Enter number of instances for request: ");
	scanf("%d",&numberOfInstances);
	if(found.resourcesAllocated[resourceNumber]+numberOfInstances <= maxResources[resourceNumber])
	{
		logRequests(found.processID,requestNew,resourceNumber,numberOfInstances);
	}
	else
	{
		printf("Total request by the process exceeds the max resources the system can provide!!!\n");
	}
	//Request kept as a log
	//currentStatus();
}

void releaseResources(int procID)
{
	struct process found=getProcessID(procID);
	if(procID!=found.processID)
	{
		printf("There is no running process with the process ID %d\n",procID);
		return;
	}
	int resourceNumber,numberOfInstances;
	printf("Process ID is : %d \n",found.processID);
	printf("Enter resource number to be released: ");
	scanf("%d",&resourceNumber);
	while (numberOfInstances < found.resourcesAllocated[resourceNumber])
	{
		printf("Enter number of instances to be released: ");
		scanf("%d",&numberOfInstances);
		if (numberOfInstances > found.resourcesAllocated[resourceNumber])
			printf("Please enter a number between 0 and %d\n",found.resourcesAllocated[resourceNumber]);
	}
	logRequests(found.processID,requestNew,resourceNumber,numberOfInstances);
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

void abortProcess(int procID)
{
	int k=0,flag=0,i;
	struct process found;
	struct requestLog *curr = root, *prev = curr;
	char ch;
	found = getProcessID(procID);
	if(procID!=found.processID)
	{
		printf("There is no running process with the process ID %d\n",procID);
		return;
	}
	//insert code to check if ID exists
	printf("Are you sure you want to abort P%d?(Y/N)",found.processID);
	scanf("%c", &ch);
	scanf("%c", &ch);
	if (ch=='Y')
	{
		while (curr!=NULL && curr->requestType!=found.processID && curr->downlink!=NULL)
		{
			prev = curr;
			curr= curr->downlink;
		}
		if (curr!=NULL && curr->requestType==found.processID)
		{
			prev->downlink = curr->downlink;
			deleteLinkedList(curr);	
		}
		else
		{
			printf("This process has no pending requests...\n");
		}
		for(i=readyQueue.front;i<readyQueue.rear;i++)
		{
			if (readyQueue.Arr[i].processID == found.processID)
			{
				for(k=0;k<numResources;k++)
				{
					currentResources[k] += found.resourcesAllocated[k];
				}
				readyQueue.Arr[i]=readyQueue.Arr[i+1];
				break;
			}
		}
		i++;
		while(i<readyQueue.rear-1)
		{
			readyQueue.Arr[i]=readyQueue.Arr[i+1];
			i++;
		}
		readyQueue.rear--;
		printf("Process aborted successfully!\n");
	}
	else
		printf("Process not aborted!\n");
	currentStatus();
}


void printProcessStatus()
{
	printf("Process Status: \n");
	if(readyQueue.front>=readyQueue.rear)
	{
		printf("CPU is IDLE.\n");
		return;
	}
	printf("Running process -> Process ID %d\n",readyQueue.Arr[readyQueue.front].processID);
	for(int i=readyQueue.front;i<readyQueue.rear;i++)
	{
		printf("\nID - %d\n",readyQueue.Arr[i].processID);
		for (int j=0;j<numResources;j++)
		{
			printf("  Resource %d - %d\n",j,readyQueue.Arr[i].resourcesAllocated[j]);
		}
	}
}


int main()
{
	int count=0,opt,procID;
	initAll();
	while(1)
	{
		if (count%10==0)
			opt = printMenu();
		switch(opt)
		{
			case 1:	newProcess();
					break;
			case 2: printf("Enter process ID of the process: ");
					scanf("%d",&procID);
					requestResources(procID);
					break;
			case 3: printf("Enter process ID of the process: ");
					scanf("%d",&procID);
					releaseResources(procID);
					break;
			case 4: printf("Enter process ID of the process: ");
					scanf("%d",&procID);
					abortProcess(procID);
					break;
			case 5: printProcessStatus();
					break;
			default: break;
		}
		count++;
		timeElapsed++;
		opt=0;
	}
	return 0;
}