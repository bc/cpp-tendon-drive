// experimentUserInterface.cpp : Defines the entry point for the console application.
//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <string>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include "utilities.h"

/*
int HelloZMQ(){
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind ("tcp://*:5555");

    while (true) {
        zmq::message_t request;

        //  Wait for next request from client
        socket.recv (&request);
        std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
        sleep(1);

        //  Send reply back to client
        zmq::message_t reply (5);
        memcpy (reply.data (), "World", 5);
        socket.send (reply);
    }
    return 0;
}

*/

int main()
{
        printf("Press Spc to move forward\n\n");
        printf("ENSURE RESILIO, TEAMVIEWER, CHROME, and ALL APPS ARE CLOSED\n\n");
        printf("Press Esc to terminate\n");
        printf("Initialization; Next stage is Motors Winding up\n");
        int gExperimentState = MOTOR_STATE_INIT;

        //HelloZMQ();

        bool stayInTheLoop = TRUE;
        while(stayInTheLoop)
        {
                char key = 0;
                if (kbhit()!=0) {
                        key = getch();
                        switch ( key )
                        {
                        case 27: // Terminate Anytime when Escape Is Pressed...
                                stayInTheLoop = FALSE;
                                gExperimentState = MOTOR_STATE_SHUTTING_DOWN;
                                proceedState(&gExperimentState);
                                std::cout << "REMEMBER TO RESTART THE PROGRAM AND DO A SAFE EXIT" << "\n";
                                break;
                        case ' ': // Move forward in the state machine
                                proceedState(&gExperimentState);
                                break;
                        }
                }
        }
        return 0;

}
