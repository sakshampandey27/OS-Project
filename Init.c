#include<stdio.h>
#include<stdlib.h>
#define numResources 5
int maxResources[numResources];

struct process
{
	int burst;
	int Res[NumResources];
}

struct queue
{
	
}

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