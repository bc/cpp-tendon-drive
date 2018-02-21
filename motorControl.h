#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <NIDAQmx.h>
#include "utilities.h"
#include <iostream>
#include <windows.h>
#include <process.h>
#include <ctime>
#include "constants.h"
#include <string>

void composeDataFileName(char*);

class TimeData
{
      LARGE_INTEGER initialTick, currentTick, frequency;
public:

    double actualTime;
    TimeData(void);
    ~TimeData(void);
    int resetTimer();
    double getCurrentTime(void);
};

class motorControl
{
    TaskHandle  motorTaskHandle, motorEnableHandle,loadCelltaskHandle,encoderTaskHandle[MUSCLE_NUM], forceTaskHandle;
    double loadCellOffset[MUSCLE_NUM+NUM_JR3_CHANNELS],I;
    TimeData timeData;
    static void motorControlLoop(void*);
    void controlLoop(void);
    HANDLE hIOMutex;
    bool live;
    char header[500];
public:
    motorControl(double*);
    ~motorControl(void);
    bool RunningForces;
    bool resetMuscleLength;
    float64 loadCellData[MUSCLE_NUM+6], motorRef[MUSCLE_NUM],encoderData0[1],encoderData1[1],encoderData2[1],encoderData3[1],encoderData4[1],encoderData5[1],encoderData6[1];
    float64 ReferenceForces[MUSCLE_NUM][NUM_FORCES];
    std::string MapIds[NUM_FORCES + 1];
    bool isEnable, isWindUp, isControlling;
    int motorEnable();
    int motorWindUp();
    int motorDisable();
    int motorControllerStart();
    int motorControllerEnd();
	int newCommand;
    int CurrentForce;
    /*****
     * 0 - Initialized
     * 1 - Trasitioning
     * 2 - Steady
     */
    int motorFlag;

    bool loadMAPandIDS(char[]);
    void assignReferenceForces(float[]);
};

#endif
