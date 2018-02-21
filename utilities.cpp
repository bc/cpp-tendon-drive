#include <utilities.h>
#include <conio.h>
#include <dataOneSample.h>
#include <motorControl.h>
#include <PostureDependencyExperimentControl.h>
#include "constants.h"
#include <stdio.h>

int proceedState(int *state)
{
    int numForces = NUM_FORCES;
    int X = -514.747, Y = 175.634, Z= 284.342, y = -111.537, p = 92.566, r = -144.265;
    static dataOneSample loadCellOffsets;
    static motorControl motors(loadCellOffsets.loadCellOffset);
    static PostureDependencyExperimentControl pdec(& motors);
    static int connectionPass;
    switch(*state)
    {
    case MOTOR_STATE_INIT:
        printf("Motors Winding Up; Next stage is Closed-Loop\n");
        motors.motorEnable();
        motors.motorWindUp();
        *state = MOTOR_STATE_WINDING_UP;
        break;
    case MOTOR_STATE_WINDING_UP:
        //Start controlling muscle force
        motors.motorControllerStart();
        printf("Motors are now in closed loop. Collecting 2s of data");
        Sleep(2000);
        printf("SpaceBar to start experiment\n");
        *state = MOTOR_STATE_CLOSED_LOOP;
        break;
    case MOTOR_STATE_CLOSED_LOOP:
        Sleep(500);
        printf("Running experiment\n");
        *state = MOTOR_STATE_RUN_EXPERIMENT;
        break;
    case MOTOR_STATE_RUN_EXPERIMENT:
        std::cout << "All elements enabled, Experiment Running" << std::endl;
        pdec.setNumberForces(numForces);
        pdec.runExperiment();
        Sleep(5000);
        exit(0);
        //std::terminate();
        break;
    }
    return 0;
}
