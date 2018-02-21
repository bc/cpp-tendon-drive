#include "PostureDependencyExperimentControl.h"

#include "utilities.h"
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include "constants.h"

PostureDependencyExperimentControl::PostureDependencyExperimentControl(motorControl* param)
{
        this->forceScaling = FALSE;
        this->motor = param;
        this->numberOfForces = NUM_FORCES;
        srand((unsigned int)time(NULL));
        generator = std::default_random_engine(rand());
        forceDistribution = std::uniform_real_distribution<double>(MINIMUM_MUSCLE_FORCE, MAXIMUM_MUSCLE_FORCE);
}
PostureDependencyExperimentControl::~PostureDependencyExperimentControl() {
  homeMuscleForce();
  std::cout << "Desconstructing PDEC()" << std::endl;
}

void PostureDependencyExperimentControl::runExperiment()
{
    std::cout << "homeMuscleForce" << "\n";
    homeMuscleForce();

    motor->RunningForces = true;


    /************************************************************/

    //RUN NETWORKING CODE HERE, ASSIGN TO REFERENCE FORCES


    /*
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) 
    {
        string buffer;

        //  Wait for next request from client
        socket.recv (&buffer);
        //std::cout << "Received Hello" << std::endl;
        std::getline(, temp, ',');

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        //zmq::message_t reply (5);
        //memcpy (reply.data (), "World", 5);
        //socket.send (reply);
    }

for(int i = 0; i < MUSCLE_NUM; i++)
    {
        motor->motorRef[i] = atof();
    }

    */

    /************************************************************/


    for(int j = 0; j < numberOfForces; j++)
    {
        std::cout << "Force:" << j + 1  << "/" << numberOfForces << "\r";
        assignReferenceForces();// motor.assignForce(j);
        motor->CurrentForce++;
    }
/*
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) 
    {
        std::string buffer;

        //  Wait for next request from client
        socket.recv (&buffer);
        //std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
        Sleep(1);

        //  Send reply back to client
        //zmq::message_t reply (5);
        //memcpy (reply.data (), "World", 5);
        //socket.send (reply);
    }
*/
    motor->RunningForces = false;
      std::cout << "Completed forces of interest" << std::endl;
        homeMuscleForce();
        std::cout << "Placed all muscle reference forces to home" << std::endl;
        std::cout << "starting 16m  JR3 Null recording period. deload the tendons!" << std::endl;
        Sleep(1000000);
        std::cout << "Completed 16m second JR3 Null recording period. If you are seeing this message you need to rerun and manually shutoff the emergency stop" << std::endl;
    motor->motorControllerEnd();
      std::cout << "Detached motor controller" << std::endl;
}

void PostureDependencyExperimentControl::homeMuscleForce()
{
        for (int i = 0; i<MUSCLE_NUM; i++)
        {
        motor->motorRef[i] = MINIMUM_MUSCLE_FORCE;
        }
        // sleep while the motors transition
        Sleep(800);
 
}

void PostureDependencyExperimentControl::assignReferenceForces()
{
    for(int i = 0; i < MUSCLE_NUM; i++)
    {
        motor->motorRef[i] = motor->ReferenceForces[i][motor->CurrentForce];
    }
    Sleep(800);
}

void PostureDependencyExperimentControl::pickRandomForces()
{
    // generate MUSCLE_NUM random forces
    for (int i = 0; i < MUSCLE_NUM; i++) 
    {
        motor->motorRef[i] = forceDistribution(generator);
    }
    Sleep(800);
}
int PostureDependencyExperimentControl::setNumberForces(int numberOfForces)
{
        this->numberOfForces = numberOfForces;
        return 0;
}
