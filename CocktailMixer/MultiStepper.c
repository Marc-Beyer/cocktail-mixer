#include <Arduino.h>
#include <stdio.h>

#include "MultiStepper.h"

struct Motor {
    int ports[4];
    int curStep;
    long nextStepTime;
    long stepsToGo;
    long position;
};

byte motorSteps[4] = {0b1001, 0b0101, 0b0110, 0b1010};

struct Motor *motors;
// The number of motors in use
// This will be set by the init method
int motorCount = 0;

// Set the number of motor, you want to use
bool initStepper(int count) {
    if (count <= 0) {
        count = 1;
    }

    // Allocate the necessary space for the number of motors
    motors = (struct Motor *)malloc(count * sizeof(struct Motor));

    // Check if allocation was successful
    if (motors) {
        motorCount = count;
        return true;
    } else {
        return false;
    }
}

// Create a Motor struct, add it to the motors array and init the ports
bool initMotor(int id, int *ports) {
    // Check if the id is valid
    if (id < 0 || id >= motorCount) {
        return false;
    }

    // Initialize the ports
    for (int i = 0; i < 4; i++) {
        if(ports[i] <= 0){
            return false;
        }
        motors[id].ports[i] = ports[i];
        pinMode(ports[i], OUTPUT);
    }
    motors[id].curStep = 0;
    motors[id].nextStepTime = millis();
    motors[id].stepsToGo = 0;
    motors[id].position = 0;

    return true;
}

// Moves the stepper motor if stepsToGo is greater 0
// and nextStepTime is reached.
// The nextStepTime is created with stepSpeed
int moveMotor(int id, long stepSpeed) {
    if (id < 0 || id >= motorCount) {
        return -1;
    }

    if (motors[id].stepsToGo != 0 && millis() > motors[id].nextStepTime) {
        motors[id].nextStepTime = millis() + stepSpeed;
        byte b = motorSteps[motors[id].curStep];

        for (int i = 0; i < 4; i++) {
            digitalWrite(motors[id].ports[i], (b & 1) ? HIGH : LOW);
            b >>= 1;
        }

        if (motors[id].stepsToGo < 0) {
            motors[id].curStep =
                (motors[id].curStep - 1 < 0) ? 3 : motors[id].curStep - 1;
            motors[id].stepsToGo++;
            motors[id].position++;
        } else if (motors[id].stepsToGo > 0) {
            motors[id].curStep = (motors[id].curStep + 1) % 4;
            motors[id].stepsToGo--;
            motors[id].position--;
        }
    }

    return (motors[id].stepsToGo == 0) ? 1 : 0;
}

/*
 *  GETTER AND SETTER
 */

long getPosition(int id) {
    if (id < 0 || id >= motorCount) {
        return -1;
    }

    return motors[id].position;
}

void setPosition(int id, long pos) {
    if (id < 0 || id >= motorCount) {
        return;
    }

    motors[id].position = pos;
}

long getStepsToGo(int id) {
    if (id < 0 || id >= motorCount) {
        return;
    }

    return motors[id].stepsToGo;
}

void setStepsToGo(int id, long steps) {
    if (id < 0 || id >= motorCount) {
        return;
    }

    motors[id].stepsToGo = steps;
}
