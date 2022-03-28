#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include <string>
#include <vector>

using namespace std;

//Defines process struct and its' members
struct process{
	string processID;
	double arrivalTime;
	double executionTime; //This is the same as burst time
	double startTime;
	double responseTime;
	double turnaroundTime;
	double stopTime;
	double burstRemaining;
	bool isInVect;
};

//Comparison functions for the sort algorithm
//----------------------------------------------------------------
bool compArrival(process p1, process p2){
	//sorting by arrival time
	return p1.arrivalTime <= p2.arrivalTime;
}

bool compExecution(process p1, process p2){
	//sorting by execution time (burst time)
	return p1.executionTime <= p2.executionTime;
}
//----------------------------------------------------------------

void nonpreFCFS(process pro[], int processCount){
	//Sorts process array by arrival time
	sort(pro, pro+processCount, compArrival);

	//Calculates the start and stop times
	//----------------------------------------------------------------
	double totalTurnaround = 0;
	double totalResponse = 0;
	double throughput = 0;
	int totalTime = 0;
	int firstArrival = 100;

	for(int i = 0; i < processCount; i++){
		if(pro[i].arrivalTime < firstArrival){
			firstArrival = pro[i].arrivalTime;
			totalTime = firstArrival;
		}
	}

	for(int i = 0; i < processCount; i++){
		if(i == 0){
			pro[i].startTime = firstArrival;
			pro[i].responseTime = 0;
			totalTime += pro[i].executionTime;
			pro[i].stopTime = totalTime;
			pro[i].turnaroundTime = pro[i].stopTime - pro[i].arrivalTime;
		}else{
			pro[i].startTime = totalTime;
			pro[i].responseTime = pro[i].startTime - pro[i].arrivalTime;
			totalTime += pro[i].executionTime;
			pro[i].stopTime = totalTime;
			pro[i].turnaroundTime = pro[i].stopTime - pro[i].arrivalTime;
		}
		totalTurnaround += pro[i].turnaroundTime;
		totalResponse += pro[i].startTime - pro[i].arrivalTime;
	}
	//----------------------------------------------------------------

	//FCFS Outputs
	//----------------------------------------------------------------
	cout << "FCFS SCHEDULER:" << endl;

	for(int i = 0; i < processCount; i++){
		cout << "==============================" << endl;
		cout << "Schedule for process " << pro[i].processID << endl;
		cout << "Start time \t Stop time" << endl;
		cout << pro[i].startTime << "\t\t" << pro[i].stopTime << endl;
	}

	cout << "==============================" << endl;
	cout << "Throughput: " << (double)processCount / ((double)totalTime - firstArrival) << endl;
	cout << "Average Turnaround Time: " << totalTurnaround / processCount << endl;
	cout << "Average Response Time: " << totalResponse / processCount << endl;
	//----------------------------------------------------------------
}

void preemptiveSJF(process pro[], int processCount){
	double totalTurnaround = 0;
	double totalResponse = 0;
	double throughput = 0;
	int totalTime = 0;
	int usedTime = 0;
	double maxArrival = 0;
	int numInVect = 0;
	int firstArrival = 100;

	vector<process> temp;
	vector<process> final;

	//Sorts by arrival
	sort(pro, pro+processCount, compArrival);

	for(int i = 0; i < processCount; i++){
		totalTime += pro[i].executionTime;

		pro[i].startTime = -1;

		if(pro[i].arrivalTime > maxArrival){
			maxArrival = pro[i].arrivalTime;
		}

		if(pro[i].arrivalTime < firstArrival){
			firstArrival = pro[i].arrivalTime;
		}

		pro[i].isInVect = false;

		pro[i].burstRemaining = pro[i].executionTime;
	}

	//Scheduler Code
	//----------------------------------------------------------------

	while(usedTime <= totalTime + firstArrival){
		//Sorts by arrival
		sort(pro, pro+processCount, compArrival);

		//Ticks until the first arrival
		if(usedTime < firstArrival){
			usedTime = firstArrival;
		}

		//Adds to the process vector on arrival
		for(int i = 0; i < processCount; i++){
			if(pro[numInVect].arrivalTime <= maxArrival && pro[numInVect].arrivalTime == usedTime){
				if(pro[numInVect].isInVect == false){
					temp.push_back(pro[numInVect]);
					pro[numInVect].isInVect = true; 
					numInVect++;
				}
			}
		}
		

		//Sorts process vector by execution time every tick
		sort(temp.begin(), temp.end(), compExecution);
		
		//If execution time left is 0, removes from temp vector and adds to final vector
		if(temp[0].burstRemaining == 0){
			temp[0].stopTime = usedTime;
			temp[0].turnaroundTime = temp[0].stopTime - temp[0].arrivalTime;
			final.push_back(temp.at(0));
			temp.erase(temp.begin());
		}

		//If process hasn't started yet, it sets the start time to current runtime
		if(temp[0].startTime == -1){
			temp[0].startTime = usedTime;
			temp[0].burstRemaining--;
			temp[0].responseTime = usedTime;
		}else{
			temp[0].burstRemaining--;
		}

		usedTime++;
	}

	//Response Time and Turnaround Time calculations
	for(int i = 0; i < processCount; i++){
		totalResponse += final[i].startTime - final[i].arrivalTime;
		totalTurnaround += final[i].stopTime - final[i].arrivalTime;
	}
	//----------------------------------------------------------------

	//SJF Outputs
	//----------------------------------------------------------------
	cout << "SJF SCHEDULER:" << endl;

	for(int i = 0; i < processCount; i++){
		cout << "==============================" << endl;
		cout << "Schedule for process " << final[i].processID << endl;
		cout << "Start time \t Stop time" << endl;
		cout << final[i].startTime << "\t\t" << final[i].stopTime << endl;
	}

	cout << "==============================" << endl;
	cout << "Throughput: " << (double)processCount / (double)totalTime << endl;
	cout << "Average Turnaround Time: " << totalTurnaround / processCount << endl;
	cout << "Average Response Time: " << totalResponse / processCount << endl;
	//----------------------------------------------------------------
}

int main(int argc, char* argv[]){
	
	//Takes the command line arguments and stores them into an array of
	// processes.
	//----------------------------------------------------------------
	int processCount = (argc - 2) / 3;
	struct process processes[processCount];
	int i = 1;
	int processNum = 0;
	while(i < argc - 1){
		string pid = argv[i];
		processes[processNum].processID = pid;
		i++;
		processes[processNum].arrivalTime = stod(argv[i]);
		i++;
		processes[processNum].executionTime = stod(argv[i]);
		i++;
		processNum++; 
	}
	//----------------------------------------------------------------

	//Determines which algorithm to use
	//FCFS or SJF
	//----------------------------------------------------------------
	string algorithm = argv[argc - 1];
	if(algorithm == "FCFS"){
		nonpreFCFS(processes, processCount);
	}
	if(algorithm == "SJF"){
		preemptiveSJF(processes, processCount);
	}
	//----------------------------------------------------------------


return 0;
}