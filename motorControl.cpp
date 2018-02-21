#include "motorControl.h"
#include "matrixFunctions.h"
#include <utilities.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "constants.h"
#include <time.h>

void composeDataFileName(char fileName[])
{       time_t t = time(NULL);
        tm* timePtr = localtime(&t);
        sprintf_s(
            fileName,
            200,
            "C:\\data\\noiseResponse_ST1BC_%4d_%02d_%02d_%02d_%02d_%02d.txt",
            timePtr->tm_year+1900,
            timePtr->tm_mon+1,
            timePtr->tm_mday,
            timePtr->tm_hour,
            timePtr->tm_min,
            timePtr->tm_sec
            );
}

motorControl::motorControl(double *param)
{
    RunningForces = false;
    I = 3;
    CurrentForce = 1;
    char        errBuff[2048]={'\0'};
    int32       error=0;
    loadCellOffset[0] = param[0];
    loadCellOffset[1] = param[1];
    loadCellOffset[2] = param[2];
    loadCellOffset[3] = param[3];
    loadCellOffset[4] = param[4];
    loadCellOffset[5] = param[5];
    loadCellOffset[6] = param[6];
    //For load cells & JR3
    for (int i=0; i < MUSCLE_NUM + NUM_JR3_CHANNELS; i++) {
        loadCellOffset[i] = param[i];
    }

    isEnable = FALSE;
    isWindUp = FALSE;
    isControlling = FALSE;
    live = FALSE;
	for(int i=0;i<MUSCLE_NUM;i++)
		motorRef[i] = 1; //initialize the forces before the experiment begins
	char subHeader[100]="";
    strcpy(header,"time, measured_M0, reference_M0, command_M0, measured_M1, reference_M1, command_M1, measured_M2, reference_M2, command_M2, measured_M3, reference_M3, command_M3, measured_M4, reference_M4, command_M4, measured_M5, reference_M5, command_M5, measured_M6, reference_M6, command_M6, angle_0, angle_1, angle_2, angle_3, angle_4, angle_5, angle_6, JR3_FX, JR3_FY, JR3_FZ, JR3_MX, JR3_MY, JR3_MZ, map_creation_id, unix_time_seconds \n");
    encoderData1[0] = 0;
    encoderData2[0] = 0;
    motorFlag = 0;
    //TODO DAQ_link_force(slot_string, slot_name) => 0 or 1 (1 if error); write a function which takes in the name of the PXI slot, and the load cell string, and runs this code upon it.
    //TODO DAQ_link_forces([(slot_string,slot_name),(slot_string,slot_name), ... ])  call DAQ_link_force by creating a list of tuples that represent those string-string relationships, and map the function to it. i.e. list_of_tuples_of_signalforce_identifiers.map i.e.


    // Inititalize voltage channel for motor loadcells
    /***************************************************************************************************************************************************************/
    DAQmxErrChk (DAQmxCreateTask("",&loadCelltaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai0","loadCell0",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai8","loadCell1",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
  	DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai1","loadCell2",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai9","loadCell3",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai2","loadCell4",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai10","loadCell5",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai11","loadCell6",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    /***************************************************************************************************************************************************************/

    // Initialize voltage Channel for JR3 loadcells
    /***************************************************************************************************************************************************************/
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai17","JR3-FX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai20","JR3-FY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai19","JR3-FZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai16","JR3-MX",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai18","JR3-MY",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(loadCelltaskHandle,"PXI1Slot5/ai21","JR3-MZ",DAQmx_Val_RSE,loadCellMinVoltage,loadCellMaxVoltage,DAQmx_Val_Volts,NULL));
    /***************************************************************************************************************************************************************/

	DAQmxErrChk (DAQmxCfgSampClkTiming(loadCelltaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,NULL));
    DAQmxErrChk (DAQmxSetRealTimeConvLateErrorsToWarnings(loadCelltaskHandle,1));
    // TODO create DAQ_link_motor(slot_string, slot_name, motorMinVoltage, motorMaxVoltage, DAQmx_Val_Volts)
    // TODO create DAQ_link_motors(list of params as above ^^)
    DAQmxErrChk (DAQmxCreateTask("",&motorTaskHandle));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao8","motor0",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao9","motor1",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao10","motor2",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao11","motor3",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao12","motor4",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao13","motor5",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCreateAOVoltageChan(motorTaskHandle,"PXI1Slot2/ao14","motor6",motorMinVoltage,motorMaxVoltage,DAQmx_Val_Volts,NULL));

    DAQmxErrChk (DAQmxCfgSampClkTiming(motorTaskHandle,"",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&motorEnableHandle));
    DAQmxErrChk (DAQmxCreateDOChan(motorEnableHandle,"PXI1Slot2/port0","motorEnable",DAQmx_Val_ChanForAllLines));
    //TODO rename enoder to encoder, and propagate spelling change across all dependent calls and files. test with data to ensure consistent output
    //TODO write DAQ_link_encoder(...) that creates the task, sets up the channel, and sets up the clock. each of these three tasks should be a small function call, passing only the relevant parameters to each.
    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[0]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[0],"PXI1Slot3/ctr0","Enoder 0",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[0],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[1]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[1],"PXI1Slot3/ctr1","Enoder 1",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[1],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[2]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[2],"PXI1Slot3/ctr2","Enoder 2",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[2],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[3]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[3],"PXI1Slot3/ctr3","Enoder 3",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[3],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[4]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[4],"PXI1Slot3/ctr4","Enoder 4",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[4],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[5]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[5],"PXI1Slot3/ctr5","Enoder 5",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[5],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));

    DAQmxErrChk (DAQmxCreateTask("",&encoderTaskHandle[6]));
    DAQmxErrChk (DAQmxCreateCIAngEncoderChan(encoderTaskHandle[6],"PXI1Slot3/ctr6","Enoder 6",DAQmx_Val_X4,0,0.0,DAQmx_Val_AHighBHigh,DAQmx_Val_Degrees,encoderPulsesPerRev,0.0,""));
    DAQmxErrChk (DAQmxCfgSampClkTiming(encoderTaskHandle[6],"/PXI1Slot5/ai/SampleClock",controlFreq,DAQmx_Val_Rising,DAQmx_Val_HWTimedSinglePoint,1));\

Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxClearTask(motorTaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
		DAQmxClearTask(motorEnableHandle);
        DAQmxClearTask(encoderTaskHandle[0]);
        DAQmxClearTask(encoderTaskHandle[1]);

		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor, load cell or encoder initialization error\n");
	}
}
motorControl::~motorControl()
{
    std::cout << "Deconstructing motor controller" << std::endl;
	live = FALSE;
	DAQmxStopTask(motorEnableHandle);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(loadCelltaskHandle);
    DAQmxStopTask(encoderTaskHandle[0]);
    DAQmxStopTask(encoderTaskHandle[1]);

    DAQmxClearTask(motorEnableHandle);
    DAQmxClearTask(motorTaskHandle);
    DAQmxClearTask(loadCelltaskHandle);
    DAQmxClearTask(encoderTaskHandle[0]);
    DAQmxClearTask(encoderTaskHandle[1]);

}

int motorControl::motorEnable()
{
    uInt32      dataEnable=0x000000FF; //this designates how many motors to enable. F is many ones.
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 zeroVoltages[MUSCLE_NUM]={0.0};
    //TODO identify whether this is still true with the new motor driver setup by putting all voltages to 0 and identifying whether they move.
    // Manual experiment performed 10h40 02-23-2017 by Cohn & Jalaleddini using NI Max shows that 0 voltage makes this motor move.
    // Setting to 0.075mv keeps it steady
    zeroVoltages[3] = 0.075;


    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataEnable,NULL,NULL));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isEnable = TRUE;
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor enable Error\n");
	}
    return 0;
}

int motorControl::motorWindUp()
{
    char        errBuff[2048]={'\0'};
    int32       error=0;
    float64 windingUpCmnd[MUSCLE_NUM];
	for (int i=0;i<MUSCLE_NUM;i++){
		windingUpCmnd[i] = 0.25; //this is an open loop
	}
    if (isEnable){
        DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,windingUpCmnd,NULL,NULL));
        Sleep(500);
        isWindUp = TRUE;


        DAQmxStopTask(motorTaskHandle);

        printf("Windup Pass.\n");
    }else  printf("Motors must be first enabled prior to winding up.\n");
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
        DAQmxStopTask(motorTaskHandle);
        DAQmxClearTask(motorTaskHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("winding up Error\n");
	}
     return 0;
}

void motorControl::motorControlLoop(void* a)
{
	((motorControl*)a)->controlLoop();
}

void motorControl::controlLoop(void)
{

    // ST no_spaces_noise_lo_0_hi_10_nmaps_500_replicates_1__dec20BC1.csv //500FORCES
   //replicates no_spaces_noise_lo_0_hi_10_nmaps_500_replicates_1__dec3BC1.csv
 
    // ST validation: scaling_and_horizontal: 
   //hand1 all_task_validations_concatenated_Ascaling50_Ahorizontal50_LRM_scaling5_LRM_horizontal5_NN_scaling_5_NN_scaling_5_total_CHECKNUMFORCES_withaliforces
    //hand2 all_task_validations_concatenated_Ascaling50_Ahorizontal50_LRM_scaling5_LRM_horizontal5_NN_scaling_5_NN_scaling_5_total_CHECKNUMFORCES_HAND2_with_aliforces
    // PD_EXTMECH big_jumbo_set_for_posture_dependence_and_extmech_914_NFORCES.csv //NFORCES914
    //no_spaces_noise_lo_0_hi_10_nmaps_500_replicates_5_shuffled_then_nmaps_5000_replicates_1.csv // 7500 forces for grand challenge
    //char rFileName[290] = "no_spaces_noise_lo_0_hi_10_nmaps_500_replicates_5_shuffled_then_nmaps_5000_replicates_1_PART2_last5700.csv"; //X NUM_FORCES this file doesn't really change.

    char rFileName[290] = "big_jumbo_set_for_posture_dependence_and_extmech_914_NFORCES.csv"; //X NUM_FORCES this file doesn't really change.


    if(!loadMAPandIDS(rFileName))
    {
        std::cout << "Could not open CSV file." << std::endl;
    }
    else
    {
        for(int i = 0; i < NUM_FORCES + 1; i++)
        {
            std::cout << MapIds[i] << std::endl;
        }
    }
    matrixFunctions matrixFunctionsInstance;
    int32       error=0;
    bool keepReading=TRUE;
    bool32 isLate = {0};
    double tick=0.0,tock=0.0;
    float64 motorCommand[MUSCLE_NUM]={0.0},errorForce[MUSCLE_NUM]= {0.0},integral[MUSCLE_NUM]={0.0};
    char        errBuff[2048]={'\0'};
    FILE* dataFile;
    float64 voltageVectorJR3[6],calibratedVectorJR3[6] = {0.0};
    char dataSample[600]="";
    char dataTemp[100]="";
    //TODO write function initialize_data_file(directory_to_save_file,localtime) that sprintf
    //TODO write function add_header_to_new_file(file_name, header_string)
    char fileName[200];

    composeDataFileName(fileName);

    dataFile = fopen(fileName,"w");
    fprintf(dataFile,header);
    DAQmxErrChk (DAQmxStartTask(loadCelltaskHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    timeData.resetTimer();
    tick = timeData.getCurrentTime();
    //TODO figure out what goffsetLoadCell is, rename variable to be more informative, and add comment above to describe what it is.
    float64 goffsetLoadCell[2]={0};
    while(live)
    {
        WaitForSingleObject(hIOMutex, INFINITE);
        //desire Forced, muscle Length, muscle Velocity PIPES should be read here
        //TODO idenfify what these 7 lines are doing, define new function to call all 7 separately. Only pass the encoderdata0, encoderData1, etc.
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[0],1,10.0,encoderData0,1,NULL,0));// low level function to read encoder data / angular position
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[1],1,10.0,encoderData1,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[2],1,10.0,encoderData2,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[3],1,10.0,encoderData3,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[4],1,10.0,encoderData4,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[5],1,10.0,encoderData5,1,NULL,0));
        DAQmxErrChk (DAQmxReadCounterF64(encoderTaskHandle[6],1,10.0,encoderData6,1,NULL,0));
        //TODO describe what these two lines do
        encoderData2[0] =  -(encoderData2[0] );
        //End

        DAQmxErrChk(DAQmxWaitForNextSampleClock(loadCelltaskHandle,10, &isLate));
        DAQmxErrChk (DAQmxReadAnalogF64(loadCelltaskHandle,-1,10.0,DAQmx_Val_GroupByScanNumber,loadCellData,MUSCLE_NUM + NUM_JR3_CHANNELS,NULL,NULL));//low level force read function
        DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));//low level motor command function
        tock = timeData.getCurrentTime();
        //TODO move all of these PI controller code into new function that takes the current state of the system, and outputs the recommended change of state.
        //TODO write dispatch function, that takes in a new desired state of the system( I.e. new desired magnitude of amps across the motors) and applies that change to the motors.
        //i.e. new_motor_state = PI_controller(old_state),
        //     dispatch(new_motor_state), which can be composed as dispatch(PI_controller(old_state)) because the output of PI_controller should be the new state object/variables.
		//target force adjustment
        for (int i=0;i<MUSCLE_NUM;i++)
        {
			loadCellData[i] = (loadCellData[i] * loadCellScale[i]) - loadCellOffset[i];//converts raw data to forces
			errorForce[i] = motorRef[i] - loadCellData[i];
			integral[i] = integral[i] + errorForce[i] * (tock - tick);
			motorCommand[i] = integral[i] * I;
      // Cap motor commands at the motorMax and motorMin voltage levels
			if (motorCommand[i] > motorMaxVoltage)
				motorCommand[i] = motorMaxVoltage;
			if (motorCommand[i] < motorMinVoltage)
				motorCommand[i] = motorMinVoltage;
		}
        //TODO identify what this section does, create new function for it that dispatches a new state. see the dispatch function above for guidance, it will be very similar.
        for (int i = MUSCLE_NUM; i < MUSCLE_NUM+6; i++)//converts raw JR3
        {
            loadCellData[i] = loadCellData[i] - loadCellOffset[i];
            voltageVectorJR3[i - MUSCLE_NUM] = loadCellData[i];
        }

        matrixFunctionsInstance.matrixMultiply(identityMatrixJR3, voltageVectorJR3,calibratedVectorJR3);
        //std::cout << printf("FX: %+4.2f; FY: %+4.2f; FZ: %+4.2f; MX: %+4.2f; MY: %+4.2f, MZ: %+4.2f, \r",calibratedVectorJR3[0],calibratedVectorJR3[1],calibratedVectorJR3[2],calibratedVectorJR3[3], calibratedVectorJR3[4], calibratedVectorJR3[5]) << std::endl;

        ReleaseMutex(hIOMutex);
        sprintf(dataSample,"%.3f",tock);
		for(int i = 0;i<MUSCLE_NUM;i++)
		{
			sprintf(dataTemp,",%.6f,%.6f,%.6f",loadCellData[i],motorRef[i], motorCommand[i]);
			strcat (dataSample, dataTemp);
		}

        //Added code to save data to disk in log file [VE]
        sprintf(dataTemp,",%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f",encoderData0[0],encoderData1[0],encoderData2[0],encoderData3[0],encoderData4[0],encoderData5[0],encoderData6[0]);
        strcat (dataSample, dataTemp);

        //test  write  JR3 sensor data to file.
        for(int i=0;i<NUM_JR3_CHANNELS;i++){
            sprintf(dataTemp,",%f",calibratedVectorJR3[i]);
			strcat (dataSample, dataTemp);
        }

        if(RunningForces)
        {
            sprintf(dataTemp, ",%s", MapIds[CurrentForce].c_str());
        }
        else
        {
            sprintf(dataTemp, ",%s", "0");
        }
        strcat(dataSample,dataTemp);

        sprintf(dataTemp, ",%d", time(NULL));

        strcat(dataSample,dataTemp);
        strcat (dataSample, "\n");
        fprintf(dataFile,dataSample);
        tick = timeData.getCurrentTime();
    }
    std::cout << "Experiment completed in " << tock << "s. Press space to terminate program." << std::endl;
    //shutting down power to motor
    for (int i=0;i<MUSCLE_NUM;i++)
        motorCommand[i] = 0;
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,motorCommand,NULL,NULL));
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isControlling = FALSE;
    fclose(dataFile);
    std::cout << "Motors Shut down. Closed loop lasted for "<< tock << "s." << std::endl;
Error:
	if( DAQmxFailed(error) ) {
		DAQmxGetExtendedErrorInfo(errBuff,2048);
		DAQmxStopTask(loadCelltaskHandle);
		DAQmxClearTask(loadCelltaskHandle);
        DAQmxStopTask(motorTaskHandle);
		DAQmxClearTask(motorTaskHandle);
        DAQmxStopTask(motorEnableHandle);
		DAQmxClearTask(motorEnableHandle);
		printf("DAQmx Error: %s\n",errBuff);
        printf("Motor control Error\n");
	}
}

int motorControl::motorControllerStart()
{
    if ((isEnable) && (isWindUp))
    {
        isControlling = TRUE;
        live = TRUE;
        hIOMutex = CreateMutex(NULL, FALSE, NULL);
		isControlling = TRUE;
		_beginthread(motorControl::motorControlLoop,0,this);
    }else
    {
        isControlling = FALSE;
        printf("Motors must be first enabled or wind up before closed-loop control.\n");
    }
    return 0;
}
int motorControl::motorControllerEnd()
{
    live = FALSE;
    motorControl::motorDisable();
    isControlling = FALSE;
    DAQmxStopTask(motorTaskHandle);
	DAQmxClearTask(motorTaskHandle);
    DAQmxStopTask(loadCelltaskHandle);
	DAQmxClearTask(loadCelltaskHandle);
    return 0;
}

bool motorControl::loadMAPandIDS(char rFileName[])
{
    // TODO convert to N muscles

    std::ifstream csvFile;
    csvFile.open(rFileName);
    if(!csvFile.is_open())
    {
         std::cout << "CSV not opened correctly" << std::endl;
       return(false);
    }
    std::string temp;
    for(int i = 0; i < NUM_FORCES + 1; i++)// 1 index because of formatting line
    {
        std::getline(csvFile, temp, ',');
        MapIds[i] = temp;
        std::getline(csvFile, temp, ',');
        ReferenceForces[0][i] = atof(temp.c_str());
        std::getline(csvFile, temp, ',');
        ReferenceForces[1][i] = atof(temp.c_str());
        std::getline(csvFile, temp, ',');
        ReferenceForces[2][i] = atof(temp.c_str());
        std::getline(csvFile, temp, ',');
        ReferenceForces[3][i] = atof(temp.c_str());
        std::getline(csvFile, temp, ',');
        ReferenceForces[4][i] = atof(temp.c_str());
        std::getline(csvFile, temp, ',');
        ReferenceForces[5][i] = atof(temp.c_str());
         std::getline(csvFile, temp);
        ReferenceForces[6][i] = atof(temp.c_str());
    }

    csvFile.close();
    return(true);
}

int motorControl::motorDisable()
{
	int32       error=0;
	char        errBuff[2048] = {'\0'};
    uInt32      dataDisable=0x00000000;
    int32		written;
    float64 zeroVoltages[MUSCLE_NUM]={0.0};
    while(isControlling){
    }
    DAQmxErrChk (DAQmxStartTask(motorEnableHandle));
    DAQmxErrChk (DAQmxStartTask(motorTaskHandle));
    DAQmxErrChk (DAQmxWriteDigitalU32(motorEnableHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&dataDisable,NULL,NULL));
    DAQmxErrChk (DAQmxWriteAnalogF64(motorTaskHandle,1,FALSE,10,DAQmx_Val_GroupByChannel,zeroVoltages,NULL,NULL));
    Sleep(500);
    DAQmxStopTask(motorTaskHandle);
    DAQmxStopTask(motorEnableHandle);
    isControlling = FALSE;
    isWindUp = FALSE;
    live = FALSE;
    isEnable = FALSE;

Error:
	if( DAQmxFailed(error) ){
		printf("DisableMotor Error: %s\n",errBuff);
        DAQmxGetExtendedErrorInfo(errBuff,2048);
        printf("DAQmx Error: %s\n",errBuff);
        printf("Motor Disable Error\n");
		DAQmxStopTask(motorEnableHandle);
        DAQmxStopTask(motorTaskHandle);

        DAQmxClearTask(motorEnableHandle);
        DAQmxClearTask(motorTaskHandle);
    }
	return 0;
}

TimeData::TimeData(void)
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&initialTick);
}


TimeData::~TimeData(void)
{
}

// Reset the timer
int TimeData::resetTimer(void)
{
    QueryPerformanceCounter(&initialTick);
    return 0;
}


// Get current time in seconds
double TimeData::getCurrentTime(void)
{
    QueryPerformanceCounter(&currentTick);
    actualTime = (double)(currentTick.QuadPart - initialTick.QuadPart);
    actualTime /= (double)frequency.QuadPart;
    return actualTime;
}
