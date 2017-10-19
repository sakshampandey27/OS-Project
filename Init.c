#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#define numResources 5
#define quantum 3

/***************** global declarations *****************/
FILE *fp;
int maxResources[numResources];
int currentResources[numResources];
int processCount;
enum types {requestNew, releaseOld};
unsigned long long int timeElapsed;

/***************** structures *****************/
struct process
{
	int burstTime,arrivalTime,waitTime;
	int processID;
	int priority;
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

struct queue
{
	int front,rear,currentSize;
	struct process *Arr;
};

struct queue readyQueue;

/***************** basic functions *****************/
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

// Deleting entire process in case of abort //
void deleteLinkedList(struct requestLog *head)
{
	if(head->rightlink==NULL) 
		free(head);
	else
		deleteLinkedList(head->rightlink);
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

int getArrivalTime()
{
	return timeElapsed;
}
/***************** Initializing functions *****************/
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
	fp=fopen("sharedMemory.txt", "w");
}

/***************** Ready Queue Status *****************/
void currentStatus()
{
	if(readyQueue.front>=readyQueue.rear)
		printf("CPU is IDLE. \n");
	else
	{
		printf("READY QUEUE: \n");
		for (int i=readyQueue.front;i<readyQueue.rear;i++)
		{
			printf("Process ID - %d\t",readyQueue.Arr[i].processID);
		}
	}
	printf("Current Resources Available: \n");
	for (int i=0;i<numResources;i++)
		printf(" R%d - %d  ",i,currentResources[i]);
}

/***************** File Operations ******************/
void writeReadyQueue()
{
	int i;
	fprintf(fp, "ReadyQueue ");
	for(i=readyQueue.front;i<readyQueue.rear;i++)
	{
		fprintf(fp,"%d ",readyQueue.Arr[i].processID);
	}
	fprintf(fp,"\n");
}

void writeBurstTimes()
{
	int i;
	fprintf(fp, "BurstTimes ");
	for(i=readyQueue.front;i<readyQueue.rear;i++)
	{
		fprintf(fp,"%d ",readyQueue.Arr[i].burstTime);
	}
	fprintf(fp,"\n");
}

void writeResourcesAllocated()
{
	fprintf(fp, "ResourcesAllocated\n");
	for(int i=readyQueue.front;i<readyQueue.rear;i++)
	{
		fprintf(fp, "PID-%d ",readyQueue.Arr[i].processID);
		for(int j=0;j<numResources;j++)
		{
			fprintf(fp,"%d ",readyQueue.Arr[i].resourcesAllocated[j]);
		}
		fprintf(fp,"\n");
	}
}

void writeNumProcesses()
{
	fprintf(fp, "%d\n",(readyQueue.rear - readyQueue.front));
}

void writeToFile()
{
	writeNumProcesses();
	writeReadyQueue();
	writeBurstTimes();
	writeResourcesAllocated();
}

void resetFile()
{
	fclose(fopen("sharedMemory.txt", "w"));
	fflush(fp);
}


/***************** Pending Request Functions *****************/
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
}

void grantRequests()
{
	struct requestLog *curr=root,*req=curr;
	struct process copy;
	int j=0;
	while (curr->downlink!=NULL)
	{
		if (curr->requestType == readyQueue.Arr[readyQueue.front].processID)
		{
			copy = readyQueue.Arr[readyQueue.front];
			req = curr->rightlink;
			while (req!=NULL)
			{
				if (req->requestType == releaseOld)
				{
					copy.resourcesAllocated[req->resourceNumber] -= req->resourceInstances;
					readyQueue.Arr[readyQueue.front].resourcesAllocated[req->resourceNumber] -= req->resourceInstances;
					currentResources[req->resourceNumber] += req->resourceInstances;
				}
				else if (req->requestType == requestNew)
					copy.resourcesAllocated[req->resourceNumber] += req->resourceInstances;

				curr->rightlink = req->rightlink;
				req->rightlink=NULL;
				free(req);
			}
			curr = root;
			struct requestLog *prev = curr;
			while (j<numResources)
			{
				if (copy.resourcesAllocated[j] > maxResources[j])
				{
					deleteLinkedList(curr);
					break;
				}
				if (copy.resourcesAllocated[j] - readyQueue.Arr[readyQueue.front].resourcesAllocated[j] < currentResources[j]) 
					j++;
				else
					break;
			}
			if (j==numResources)
			{
				for (int k=0;k<numResources;k++)
					currentResources[k] -= copy.resourcesAllocated[k] - readyQueue.Arr[readyQueue.front].resourcesAllocated[k];		
				readyQueue.Arr[readyQueue.front] = copy;
				while (curr!=NULL && curr->requestType!=readyQueue.Arr[readyQueue.front].processID)
				{
					prev=curr;
					curr=curr->downlink;					
				}
				if (curr!=NULL && curr->requestType==readyQueue.Arr[readyQueue.front].processID)
				{
					prev->downlink = curr->downlink;
					curr->downlink=NULL;
					deleteLinkedList(curr);
				}
			}
			else
			{
				printf("All requests cannot be granted at this time. \n");
				push(pop());
			}
		}
		else
			curr = curr->downlink;
	}
	if (curr==NULL)
		printf("No pending requests for process ID - %d",readyQueue.Arr[readyQueue.front].processID);
}

/***************** User Options *****************/
void newProcess()
{
	printf("Enter new process details: \n");
	struct process *p;
	p=(struct process *)malloc(sizeof(struct process));
	p->arrivalTime=getArrivalTime();
	printf("Enter burst Time : ");
	scanf("%d",&(p->burstTime));
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
	for (int i=0;i<numResources;i++)
		currentResources[i] -= p->resourcesAllocated[i];
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
	currentStatus();
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
	printf("Are you sure you want to abort P%d?(Y/N)",found.processID);
	scanf("%c ", &ch);
	if (ch=='Y')
	{
		while (curr!=NULL && curr->requestType!=found.processID)
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
			printf("This process has no pending requests...\n");
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

/***************** Menu *****************/
void askUser()
{
	int opt=0,procID;
	//resetFile();
	printf("\n1.Enter a new process.\n2.Request a new resource.\n3.Release a resource.\n4.Abort a process\n5.Show process status\n6.Continue Execution\n");
	scanf("%d",&opt);
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
	//writeToFile();
}

/***************** Scheduling *****************/
void RoundRobin()
{
	struct process finishedProcess;
	int startTime,count;
	while (readyQueue.front!=readyQueue.rear)
	{
		startTime = timeElapsed;
		count=0;
		while(count<quantum && readyQueue.Arr[readyQueue.front].burstTime>timeElapsed-readyQueue.Arr[readyQueue.front].waitTime)
		{
			//grantRequests();
			count++;
			for (int i=readyQueue.front+1;i<readyQueue.rear;i++)
				readyQueue.Arr[i].waitTime++;
			askUser();
			timeElapsed++;
		}
		printf("Process %d runs from t = %d ms to t = %llu ms\n", readyQueue.Arr[readyQueue.front].processID, startTime, timeElapsed);
		if (readyQueue.Arr[readyQueue.front].burstTime==timeElapsed-readyQueue.Arr[readyQueue.front].waitTime)
		{
			for (int k=0;k<numResources;k++)
				currentResources[k] += finishedProcess.resourcesAllocated[k];
			finishedProcess = pop();
		}
		else
			push(pop());
	}
	currentStatus();
}

/***************** Main *****************/
int main()
{
	initAll();
	while(1)
	{
		askUser();
		RoundRobin();
	}
	return 0;
}