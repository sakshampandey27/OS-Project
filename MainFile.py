from subprocess import call
import numpy as np
import os
import time
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
args = ("asd","qwe")
#N = 10  #could change

newPID=os.fork()
if(newPID==0):
	os.execvp("./a.out",args)
	os._exit(0)
else:
	plt.cla()
	#wait()
	def makeResourceGraph():
		fig, axs = plt.subplots(ncols=2,figsize=(8,7))
		plt.ion()
		fig.subplots_adjust(bottom=0.4, wspace=0.4,hspace=0.4)
		fi=open('sharedMemory.txt','r')
		f=fi.readlines()
		try:
			x=int(f[0])
		except:
			x=0
		A=np.array([])
		B=np.array([])
		ind=np.array([])
		C=np.array([])
		D=np.array([])
		E=np.array([])
		try:
			PIDs=np.array(list(map(int,f[1].split()[1:])))
		except:
			PIDs=[]
		for i in range(x):
			temp=list(map(int,f[i+5].split()[1:]))
			A=np.append(A,[temp[0]])
			B=np.append(B,[temp[1]])
			C=np.append(C,[temp[2]])
			D=np.append(D,[temp[3]])
			E=np.append(E,[temp[4]])
		#ind = np.arange(x)
		try:
			ind = np.array(list(map(int,f[1].split()[1:])))
		except:
			ind= np.arange(x)
		if len(ind)==0:
			ind=np.arange(x)
		colors=['#5882b7','#b25852','#a1b864','#7b669e','#63aac2']
		p1 = axs[0].bar(ind, A,width, color=colors[0])
		p2 = axs[0].bar(ind, B, width, color=colors[1], bottom=A)
		p3 = axs[0].bar(ind, C, width, color=colors[2], bottom=A+B)
		p4 = axs[0].bar(ind, D,width, color=colors[3], bottom=A+B+C)
		p5 = axs[0].bar(ind, E, width, color=colors[4], bottom=A+B+C+D)
		axs[0].set_ylabel('Resources')
		axs[0].set_xlabel('ProcessID')
		axs[1].set_ylabel('Wait Time')
		axs[1].set_xlabel('ProcessID')
		axs[1].set_title('Wait Time Graph')
		axs[0].set_title('Resource Allocation Graph')
		waittimeColor='#87c0ec'
		try:
			waitTimeValues=np.array(list(map(int,f[3].split()[2:])))
			burstTimeValues=np.array(list(map(int,f[2].split()[1:])))
		except:
			waitTimeValues=np.array([])
			burstTimeValues=np.array([])
		axs[0].set_xticks(PIDs)#dynamic - fed
		axs[1].set_xticks(PIDs)
		r1 = mpatches.Patch(color=colors[1], label='Resource 1')
		r2 = mpatches.Patch(color=colors[2], label='Resource 2')
		r3 = mpatches.Patch(color=colors[3], label='Resource 3')
		r4 = mpatches.Patch(color=colors[4], label='Resource 4')
		r0 = mpatches.Patch(color=colors[0], label='Resource 0')
		waitTime=mpatches.Patch(color=waittimeColor, label='Wait Time')
		plt.legend(handles=[r0,r1,r2,r3,r4,waitTime], bbox_to_anchor=(0.5, -0.2))
		axs[1].bar(ind, waitTimeValues,width, color=waittimeColor)
		#axs[1, 0].imshow(np.random.random((100, 100)))
		#axs[1, 1].imshow(np.random.random((100, 100)))
		#plt.grid(True)
		plt.draw()
		plt.pause(0.1)
		plt.close()
		    # the x locations for the groups
	width = 0.5       # the width of the bars: can also be len(x) sequence

	while True:
		makeResourceGraph()