#include <stdio.h>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
#define SIZE (100)
typedef pair <int, int> pii;

struct processInfo {
    int pid;
    int curQueue;
    int arrivalTime;
    int waitingTime;
    vector <int> burstTime;
    int curBurstTimeIdx;
    // even index : CPU burst
    // odd index : IO burst
    bool timeQuantumExist;
    int timeQuantum;
    bool SRTN;
    bool inProcessorQueue;
};

bool cmp(const processInfo &pi1, const processInfo &pi2) {
    return pi1.arrivalTime < pi2.arrivalTime;
};

processInfo processInfoArray[SIZE];
queue <int> processorQueue_0;
queue <int> processorQueue_1;
// <remainingTime, processInfoIdx>
priority_queue < pii, vector <pii>, greater <pii> > processorQueue_2;
queue <int> processorQueue_3;
queue <int> IOQueue;

int ganttChart[SIZE * 100];
int waitingTimeArray[SIZE];
int turnAroundTimeArray[SIZE];

int main() {
    int processNum;
    scanf("%d", &processNum);
    for (int i = 0; i < processNum; i++) {
        int pid, initQueue, arrivalTime, cycleNum;
        scanf(" %d %d %d %d", &pid, &initQueue, &arrivalTime, &cycleNum);
        vector <int> burstTime;
        int j = cycleNum * 2 - 1;
        while (j--) {
            int t;
            scanf(" %d", &t);
            burstTime.push_back(t);
        }
        processInfo pi;
        pi.pid = pid;
        pi.curQueue = initQueue;
        pi.arrivalTime = arrivalTime;
        pi.waitingTime = 0;
        pi.burstTime = burstTime;
        pi.curBurstTimeIdx = 0;
        pi.inProcessorQueue = false;
        processInfoArray[i] = pi;
    }
    sort(processInfoArray, processInfoArray + processNum, cmp);
    int curProcessInfoIdx = 0;
    bool processorBurst = false;
    bool IOBurst = false;
    int curBurstProcessIdx;
    int processExitNum = 0;
    int time = 0;
    for (;; time++) {
        while (curProcessInfoIdx < processNum) {
            if (processInfoArray[curProcessInfoIdx].arrivalTime == time) {
                processInfo curProcess = processInfoArray[curProcessInfoIdx];
                int initQueue = curProcess.curQueue;
                processInfoArray[curBurstProcessIdx].inProcessorQueue = true;
                switch (initQueue) {
                    case 0:
                        processInfoArray[curProcessInfoIdx].timeQuantumExist = true;
                        processInfoArray[curProcessInfoIdx].timeQuantum = 2;
                        processorQueue_0.push(curProcessInfoIdx);
                        break;
                    case 1:
                        processInfoArray[curProcessInfoIdx].timeQuantumExist = true;
                        processInfoArray[curProcessInfoIdx].timeQuantum = 6;
                        processorQueue_1.push(curProcessInfoIdx);
                        break;
                    case 2:
                        processInfoArray[curProcessInfoIdx].timeQuantumExist = false;
                        processInfoArray[curBurstProcessIdx].SRTN = true;
                        processorQueue_2.push(make_pair(curProcess.burstTime[0], curProcessInfoIdx));
                        break;
                    case 3:
                        processInfoArray[curProcessInfoIdx].timeQuantumExist = false;
                        processInfoArray[curBurstProcessIdx].SRTN = false;
                        processorQueue_3.push(curProcessInfoIdx);
                        break;
                }
                curProcessInfoIdx++;
            }
            else break;
        }


        if (!processorBurst) {
            if (!processorQueue_0.empty()) {
                curBurstProcessIdx = processorQueue_0.front();
                processorQueue_0.pop();
                processInfoArray[curBurstProcessIdx].inProcessorQueue = false;
                processorBurst = true;
            }
            else if (!processorQueue_1.empty()) {
                curBurstProcessIdx = processorQueue_1.front();
                processorQueue_1.pop();
                processInfoArray[curBurstProcessIdx].inProcessorQueue = false;
                processorBurst = true;
            }
            else if (!processorQueue_2.empty()) {
                curBurstProcessIdx = processorQueue_2.top().second;
                processorQueue_2.pop();
                processInfoArray[curBurstProcessIdx].inProcessorQueue = false;
                processorBurst = true;
            }
            else if (!processorQueue_3.empty()) {
                curBurstProcessIdx = processorQueue_3.front();
                processorQueue_3.pop();
                processInfoArray[curBurstProcessIdx].inProcessorQueue = false;
                processorBurst = true;
            }
        }

        // SRTN Scheduling
        if (processorBurst) {
            processInfo curProcess = processInfoArray[curBurstProcessIdx];
            if ( (curProcess.SRTN) && (processorQueue_0.empty())
            && (processorQueue_1.empty()) && (!processorQueue_2.empty())
            && (processorQueue_2.top().first < curProcess.burstTime[curProcess.curBurstTimeIdx]) ) {
                processInfoArray[curBurstProcessIdx].SRTN = false;
                processInfoArray[curBurstProcessIdx].inProcessorQueue = true;
                processorQueue_3.push(curBurstProcessIdx);
                curBurstProcessIdx = processorQueue_2.top().first;
                processorQueue_2.pop();
                processInfoArray[curBurstProcessIdx].inProcessorQueue = false;
            }
        }

        for (int i = 0; i < processNum; i++) {
            if (processInfoArray[i].inProcessorQueue) {
                processInfoArray[i].waitingTime++;
            }
        }

        if (processorBurst) {
            processInfo curProcess = processInfoArray[curBurstProcessIdx];
            ganttChart[time] = curProcess.pid;
            if (curProcess.timeQuantumExist) {
                int remainingBurstTime =
                --(processInfoArray[curBurstProcessIdx].burstTime[curProcess.curBurstTimeIdx]);
                if (remainingBurstTime == 0) {
                    processorBurst = false;
                    if ((processInfoArray[curBurstProcessIdx].curBurstTimeIdx)++
                    == processInfoArray[curBurstProcessIdx].burstTime.size()) {
                        processExitNum++;
                        waitingTimeArray[curBurstProcessIdx] = processInfoArray[curBurstProcessIdx].waitingTime;
                        turnAroundTimeArray[curBurstProcessIdx] = time - processInfoArray[curProcessInfoIdx].arrivalTime;
                    }
                    else {
                        IOQueue.push(curBurstProcessIdx);
                    }
                }
                else {
                    int timeQuantum = --(processInfoArray[curBurstProcessIdx].timeQuantum);
                    if (timeQuantum == 0) {
                        processorBurst = false;
                        int curQueue = ++(processInfoArray[curBurstProcessIdx].curQueue);
                        if (curQueue == 1) {
                            processInfoArray[curBurstProcessIdx].timeQuantum = 6;
                            processInfoArray[curBurstProcessIdx].inProcessorQueue = true;
                            processorQueue_1.push(curBurstProcessIdx);
                        }
                        else {
                            processInfoArray[curBurstProcessIdx].timeQuantumExist = false;
                            processInfoArray[curBurstProcessIdx].SRTN = true;
                            processInfoArray[curBurstProcessIdx].inProcessorQueue = true;
                            processInfo curProcess = processInfoArray[curBurstProcessIdx];
                            processorQueue_2.push(make_pair(curProcess.burstTime[curProcess.curBurstTimeIdx],
                            curBurstProcessIdx));
                        }
                    }
                }
            }
            else if (curProcess.SRTN) {
                int remainingBurstTime =
                --(processInfoArray[curBurstProcessIdx].burstTime[curProcess.curBurstTimeIdx]);
                if (remainingBurstTime == 0) {
                    processorBurst = false;
                    if ((processInfoArray[curBurstProcessIdx].curBurstTimeIdx)++
                    == processInfoArray[curBurstProcessIdx].burstTime.size()) {
                        processExitNum++;
                        waitingTimeArray[curBurstProcessIdx] = processInfoArray[curBurstProcessIdx].waitingTime;
                        turnAroundTimeArray[curBurstProcessIdx] = time - processInfoArray[curProcessInfoIdx].arrivalTime;
                    }
                    else {
                        IOQueue.push(curBurstProcessIdx);
                    }
                }

            }
            else {
                int remainingBurstTime =
                --(processInfoArray[curBurstProcessIdx].burstTime[curProcess.curBurstTimeIdx]);
                if (remainingBurstTime == 0) {
                    processorBurst = false;
                    if ((processInfoArray[curBurstProcessIdx].curBurstTimeIdx)++
                    == processInfoArray[curBurstProcessIdx].burstTime.size()) {
                        processExitNum++;
                        waitingTimeArray[curBurstProcessIdx] = processInfoArray[curBurstProcessIdx].waitingTime;
                        turnAroundTimeArray[curBurstProcessIdx] = time - processInfoArray[curProcessInfoIdx].arrivalTime;
                    }
                    else {
                        IOQueue.push(curBurstProcessIdx);
                    }
                }
            }
        }
        if (processExitNum == processNum) break;


        if (IOBurst) {

        }
        else {
            if (!IOQueue.empty()) {

            }
        }


    }

    double waitingTimeSum = 0;
    double turnAroundTimeSum = 0;
    for (int i = 0; i < SIZE; i++) {
        processInfo curProcess = processInfoArray[i];
        waitingTimeSum += waitingTimeArray[i];
        turnAroundTimeSum += turnAroundTimeArray[i];
        printf("PID : %d Waiting Time : %d Turnaround Time : %d\n",
        curProcess.pid, waitingTimeArray[i], turnAroundTimeArray[i]);
    }
    printf("\nAverage Waiting Time : %lf\nAverage Turnaround Time : %lf\n\nGantt Chart\n",
    waitingTimeSum / processNum, turnAroundTimeSum / processNum);
    for (int i = 0; i <= time; i++) {
        printf("%d ", ganttChart[i]);
    }



    return 0;
}