#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H
#include <math.h>
#include <NIDAQmx.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

const int controlFreq = 1000;
const double loadCellScale[13] = {(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56,(1/sqrt(2.0)) * 49.56, 1, 1 , 1 , 1, 1, 1}; //From calibration test with weights

// this information was ascertained from the JR3 Force-Moment Sensor Specification Sheet, available at the CHP basement lab. The calibration loads were 20-40 lbs. Brian Cohn March 17,2017 14h03.
const double calibrationMatrixJR3[6][6] =
{
    {13.669,0.229,0.105,-0.272,0.060,0.865},
    {0.160,13.237,-0.387,-0.027,-0.396,-0.477},
    {1.084,0.605,27.092,-2.88,-0.106,1.165},
    {-.007,-.003,-0.001,0.676,-0.002,-0.038},
    {0.004,-0.004,0.001,0.000,0.688,-0.012},
    {0.004,0.003,0.003,-0.006,0.013,0.665}
};

const double identityMatrixJR3[6][6] =
{
    {1.0,0.0,0.0,0.0,0.0,0.0},
    {0.0,1.0,0.0,0.0,0.0,0.0},
    {0.0,0.0,1.0,0.0,0.0,0.0},
    {0.0,0.0,0.0,1.0,0.0,0.0},
    {0.0,0.0,0.0,0.0,1.0,0.0},
    {0.0,0.0,0.0,0.0,0.0,1.0}
};



const int motorMinVoltage = 0;
const int motorMaxVoltage = 10;
const int loadCellMinVoltage = -10;
const int loadCellMaxVoltage = +10;
const int encoderPulsesPerRev = 500;


#define     MOTOR_STATE_INIT 0
#define     MOTOR_STATE_WINDING_UP 1
#define     MOTOR_STATE_CLOSED_LOOP 2
#define     MOTOR_STATE_RUN_EXPERIMENT 3
#define     MOTOR_STATE_SHUTTING_DOWN 4
#define     EXIT 5

int proceedState(int *);

#define SMARTA 0
#define SMARTB 1
#define FDS 2
#define PI 3
#define DI 4
#define LBR 5
#define EIP 6
#define MUSCLE_NUM 7
#define NUM_JR3_CHANNELS 6

#define MINIMUM_MUSCLE_FORCE 0
#define MAXIMUM_MUSCLE_FORCE 20


#define CIRCLE 0
#define LINE_FROM_FILE 1
#endif
