#include "dataOneSample.h"

dataOneSample::dataOneSample()
{
        char errBuff[2048]={'\0'};
        int32 error=0;
        double loadCellData[MUSCLE_NUM + NUM_JR3_CHANNELS]= {0};
        DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));



        uInt32 dataDisable=0x00000000;
        int32 written;
        float64 zeroVoltages[MUSCLE_NUM]={0.0};

        DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
        DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));
        DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
        DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
        Sleep(500);
        DAQmxStopTask(motorEnableHandle);
        DAQmxClearTask(motorEnableHandle);



        DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao8","motor0",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao9","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor3",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao12","motor4",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao13","motor5",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao14","motor6",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));

        DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",1000,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
        DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
        float64 motorCommand[MUSCLE_NUM];

        for (int i = 0; i<MUSCLE_NUM; i++)
        {
                motorCommand[i] = 0;
        }
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
        Sleep(500);
        DAQmxStopTask(motorTaskHandle);
        DAQmxClearTask(motorTaskHandle);


        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai0","loadCell0",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai1","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai9","loadCell3",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai2","loadCell4",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai10","loadCell5",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai11","loadCell6",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));

        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai17","JR3-FX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai20","JR3-FY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai19","JR3-FZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai16","JR3-MX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai18","JR3-MY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai21","JR3-MZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
        DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,NULL,1000,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));





        DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,MUSCLE_NUM + NUM_JR3_CHANNELS,NULL,NULL));
        //For load cells
        for (int i=0; i<MUSCLE_NUM; i++) {
                loadCellOffset[i] = loadCellData[i] * loadCellScale[i];
        }
        //For JR3
        for (int i = MUSCLE_NUM; i<MUSCLE_NUM + NUM_JR3_CHANNELS; i++) {
                loadCellOffset[i] = loadCellData[i];
        }
Error:
        if( DAQmxFailed(error) ) {
                DAQmxGetExtendedErrorInfo(errBuff,2048);
                /*********************************************/
                // DAQmx Stop Code
                /*********************************************/
                printf("DAQmx Error: %s\n",errBuff);
                printf("Data one Sample Error: %s\n",errBuff);
        }
        if (loadCelltaskHandle)
        {
                DAQmxStopTask(loadCelltaskHandle);
                DAQmxClearTask(loadCelltaskHandle);
                loadCelltaskHandle = 0;
        }
}
