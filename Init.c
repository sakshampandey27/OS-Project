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
struct burstTimes
{
    int processID,burstTime;
    struct burstTimes *next;
};

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
struct burstTimes *allBurstTimes = NULL;

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
    struct requestLog *current=head,*next;
    current=current->rightlink;
    while (current != NULL)
    {        
        next = current->rightlink;
        current->rightlink = NULL;
        free(current);
        current = next;
    }
    head->rightlink=NULL;
    head=NULL;
}

struct process getProcessID(int id)
{
    int i;
    for(i=readyQueue.front;i<readyQueue.rear;i++)
    {
        if(readyQueue.Arr[i].processID==id)
            return readyQueue.Arr[i];
    }
    return readyQueue.Arr[0];
}

int getArrivalTime()
{
    return timeElapsed;
}
/***************** Burst Times ***********************/
void delBurstTime(int processID)
{
    struct burstTimes *curr,*prev;
    curr=allBurstTimes->next;
    prev=allBurstTimes;
    while(curr!=NULL)
    {
        if(curr->processID==processID)
        {
            prev->next=curr->next;
            curr->next = NULL;
            free(curr);
            return;
        }
        prev = curr;
        curr=curr->next;
    }
}

void addBurstTime(int processID,int burstTime)
{
    struct burstTimes *temp;
    struct burstTimes *curr;
    temp=(struct burstTimes *)malloc(sizeof(struct burstTimes));
    temp->processID=processID;
    temp->burstTime=burstTime;
    temp->next=NULL;
    curr=allBurstTimes;
    while(curr->next!=NULL)
        curr=curr->next;
    curr->next=temp;
}

/***************** Initializing functions *****************/
void initBurstTimes()
{
    allBurstTimes=(struct burstTimes *)malloc(sizeof(struct burstTimes));
    allBurstTimes->next=NULL;
    allBurstTimes->processID = allBurstTimes->burstTime = -1;
}

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
    readyQueue.front=readyQueue.rear=0;
    readyQueue.currentSize=1;
    readyQueue.Arr=(struct process *)malloc((readyQueue.currentSize)*sizeof(struct process));
}

void initAll()
{
    timeElapsed=0;
    processCount=1000;
    initResources();
    initBurstTimes();
    initReadyQueue();
    fp=fopen("sharedMemory.txt", "w");
}

/***************** Ready Queue Status *****************/
void currentStatus()
{
    if(readyQueue.front>=readyQueue.rear)
        printf("CPU is IDLE.\n");
    else
    {
        printf("READY QUEUE: \n");
        for (int i=readyQueue.front;i<readyQueue.rear;i++)
        {
            printf("Process ID - %d\t",readyQueue.Arr[i].processID);
        }
    }
    printf("\nCurrent Resources Available: \n");
    for (int i=0;i<numResources;i++)
        printf(" R%d - %d  ",i,currentResources[i]);
}

/***************** File Operations ******************/
void writeReadyQueue()
{
    int i;
    fprintf(fp, "ReadyQueue: ");
    for(i=readyQueue.front;i<readyQueue.rear;i++)
    {
        fprintf(fp,"%d ",readyQueue.Arr[i].processID);
    }
    fprintf(fp,"\n");
}

void writeBurstTimes()
{
    struct burstTimes *curr;
    fprintf(fp, "BurstTimes: ");
    curr=allBurstTimes->next;
    while(curr!=NULL)
    {
        fprintf(fp,"%d ",curr->burstTime);
        curr=curr->next;
    }
    fprintf(fp,"\n");
}

void writeResourcesAllocated()
{
    fprintf(fp, "ResourcesAllocated: \n");
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
    fclose(fp);
}

void resetFile()
{
    fp=fopen("sharedMemory.txt", "w");
    fflush(fp);
}

/***************** Pending Request Functions *****************/
void logRequests(int id, int type, int resNum, int resInstances)
{
    struct requestLog *curr=root, *temp, *prev = curr;
    struct requestLog *newRequest;
    newRequest = (struct requestLog *)malloc(sizeof(struct requestLog));
    newRequest->requestType = type;
    newRequest->resourceNumber = resNum;
    newRequest->resourceInstances = resInstances;
    newRequest->downlink = newRequest->rightlink = NULL;
    if (root==NULL)
    {
        curr=(struct requestLog *)malloc(sizeof(struct requestLog));
        curr->requestType = id;
        curr->resourceNumber = curr->resourceInstances = -1;
        curr->downlink = NULL;
        curr->rightlink = newRequest;
        root=curr;
        return;
    }
    else
    {
        while (curr!=NULL)
        {
            if (curr->requestType == id)
            {
                temp = curr->rightlink;
                while (temp->rightlink!=NULL)
                    temp = temp->rightlink;
                temp->rightlink = newRequest;
                break;
            }
            else
            {
                prev=curr;
                curr = curr->downlink;
            }
        }
    }
    if (curr==NULL)
    {
        temp=(struct requestLog *)malloc(sizeof(struct requestLog));
        temp->downlink=NULL;
        temp->requestType = id;
        temp->resourceNumber = temp->resourceInstances = -1;
        prev->downlink = temp;
        temp->rightlink=newRequest;
    }
}

void printLog()
{
    struct requestLog *curr=root,*temp;
    printf("\n******************************************\n");
    while(curr!=NULL)
    {
        printf("%d -> ",curr->requestType);
        temp=curr->rightlink;
        
        while(temp!=NULL)
        {
            printf("%d %d %d -> ",temp->requestType,temp->resourceNumber,temp->resourceInstances);
            temp=temp->rightlink;
        }
        printf("\n");
        curr=curr->downlink;
    }
    printf("\n******************************************\n");
}

int grantRequests()
{
    struct requestLog *curr=root,*req,*prev=curr,*prevreq;
    struct process copy;
    while (curr!=NULL)
    {
        int j=0,flagStatus=0;
        printf("%d - %d \n",curr->requestType,(readyQueue.Arr[readyQueue.front]).processID);
        if (curr->requestType == (readyQueue.Arr[readyQueue.front]).processID)
        {
            copy = readyQueue.Arr[readyQueue.front];
            req = curr->rightlink;
            prevreq=curr;
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
                if(req==NULL)
                    break;
                prevreq=req;
                req = req->rightlink;
            }
            while (j<numResources)
            {
                if (copy.resourcesAllocated[j] > maxResources[j])
                {
                    printf("\nProcess cannot be served, has been deleted.\n");
                    if (curr==root)
                    {
                        root = curr->downlink;
                        deleteLinkedList(curr);
                        curr = root;
                    }
                    else
                    {
                        prev->downlink = curr->downlink;
                        curr->downlink = NULL;
                        deleteLinkedList(curr);
                        curr=NULL;
                        if(prev==NULL)
                            root=NULL;
                    }
                    flagStatus = 1;
                    break;
                }
                if (copy.resourcesAllocated[j] - readyQueue.Arr[readyQueue.front].resourcesAllocated[j] < currentResources[j])
                    j++;
                else
                {
                    printf("\nResources currently not available...process has to wait.\n");
                    flagStatus = 2;
                    break;
                }
            }            
            if (j==numResources && flagStatus==0)
            {
                for (int k=0;k<numResources;k++)
                    currentResources[k] -= copy.resourcesAllocated[k] - readyQueue.Arr[readyQueue.front].resourcesAllocated[k];
                readyQueue.Arr[readyQueue.front] = copy;
                req = curr->rightlink;
                while (req!=NULL)
                {
                	curr->rightlink = req->rightlink;
                	req->rightlink = NULL;
                	free(req);
                	req = curr->rightlink;
                }
                curr->rightlink = NULL;
                prev->downlink = curr->downlink;
                curr->downlink=NULL;                
                if(prev==NULL)
                    root=NULL;
                else
                	curr=prev->downlink;                
            }
            else
            {
                if (flagStatus == 2)
                {
                    push(pop());
                    flagStatus=0;
                }
                else if (flagStatus==1)
                    flagStatus=0;
                return 0;
            }
            currentStatus();
        }
        else
        {
            prev = curr;
            curr = curr->downlink;
        }
    }
    return 1;
}
/***************** User Options *****************/
void newProcess()
{
    printf("Enter new process details: \n");
    struct process *p;
    p=(struct process *)malloc(sizeof(struct process));
    p->arrivalTime=getArrivalTime();
    p->waitTime=0;
    printf("Enter burst Time : ");
    scanf("%d",&(p->burstTime));
    printf("Enter initial resources required : ");
    for(int i=0;i<numResources;i++)
    {
        printf("Enter request for resource %d",i);
        scanf("%d",&p->resourcesAllocated[i]);
        if (p->resourcesAllocated[i] > currentResources[i])
        {
            printf("Please enter a number between 0 and %d\n",currentResources[i]);
            i--;
        }
    }
    p->processID = processCount++;
    printf("Process ID is %d\n",p->processID);
    push(*p);
    for (int i=0;i<numResources;i++)
        currentResources[i] -= p->resourcesAllocated[i];
    addBurstTime(p->processID,p->burstTime);
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
    numberOfInstances=11; //1 greater than max
    while (numberOfInstances > found.resourcesAllocated[resourceNumber])
    {
        printf("Enter number of instances to be released: ");
        scanf("%d",&numberOfInstances);
        if (numberOfInstances > found.resourcesAllocated[resourceNumber])
            printf("Please enter a number between 0 and %d\n",found.resourcesAllocated[resourceNumber]);
    }
    logRequests(found.processID,releaseOld,resourceNumber,numberOfInstances);
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
    scanf("%c", &ch);
    scanf("%c", &ch);
    if (ch=='Y' || ch=='y')
    {
        while (curr!=NULL && curr->requestType!=found.processID)
        {
            prev = curr;
            curr= curr->downlink;
        }
        if (curr!=NULL && curr->requestType==found.processID)
        {
            prev->downlink = curr->downlink;
            curr->downlink = NULL;
            free(curr);
            if(prev==NULL)
                root=NULL;
            else
            	curr = prev->downlink;
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
        readyQueue.rear--;
        printf("Process aborted successfully!\n");
        delBurstTime(procID); //Delete
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
    printf("\n1.Enter a new process.\n2.Request a new resource.\n3.Release a resource.\n4.Abort a process\n5.Show process status\n6.Print Log\n7.Continue Execution\n8.Exit");
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
        case 6:	printLog();
            break;
        case 8: exit(1);
        default: break;
    }
    resetFile();
    writeToFile();
}

/***************** Scheduling *****************/
void RoundRobin()
{
    struct process finishedProcess;
    int startTime,count;
    while (readyQueue.front<readyQueue.rear)
    {
        startTime = timeElapsed;
        count=0;
        while(!grantRequests());
        while(count<quantum && readyQueue.Arr[readyQueue.front].burstTime>0)
        {
            readyQueue.Arr[readyQueue.front].burstTime--;
            count++;
            for (int i=readyQueue.front+1;i<readyQueue.rear;i++)
                readyQueue.Arr[i].waitTime++;
            askUser();
            timeElapsed++;
        }
        printf("Remaining Burst Times: \n");
		for (int i=readyQueue.front; i<readyQueue.rear;i++)
			printf("Process ID - %d \t Remaining time - %d\n",readyQueue.Arr[i].processID,readyQueue.Arr[i].burstTime);
        if (readyQueue.front<readyQueue.rear && readyQueue.Arr[readyQueue.front].burstTime==0)
        {
            finishedProcess = pop();
            for (int k=0;k<numResources;k++)
                currentResources[k] += finishedProcess.resourcesAllocated[k];
            //delBurstTime(finishedProcess.processID); //Delete
        }
        else //if(readyQueue.front<readyQueue.rear)
            push(pop());
    }
    currentStatus();
    //exit(1);
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
