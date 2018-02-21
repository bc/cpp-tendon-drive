#pragma once

#ifndef experimentcontrol
#define experimentcontrol

#include <iostream>
#include <random>
#include <motorControl.h>
#include <windows.h>

class PostureDependencyExperimentControl
{
    public:
        PostureDependencyExperimentControl(motorControl* param);
        ~PostureDependencyExperimentControl();
        void runExperiment();
        int setNumberForces(int);
        void assignReferenceForces();
        void homeMuscleForce();
        void pickRandomForces();
    private:
        BOOL forceScaling;
        motorControl* motor;
        int numberOfPostures,postureType;
        int numberOfForces;
        double randomlyChosenAngle;
        double postureRadius;
        // to generate random angles and forces
        std::default_random_engine generator;
        std::uniform_real_distribution<double> forceDistribution;
};
#endif
