#ifndef DATA_ONE_SAMPLE_H
#define DATA_ONE_SAMPLE_H

#include <NIDAQmx.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <utilities.h>

class dataOneSample
{
TaskHandle loadCelltaskHandle,motorTaskHandle,motorEnableHandle;
public:
dataOneSample();
double loadCellOffset[MUSCLE_NUM + NUM_JR3_CHANNELS];
};
#endif
