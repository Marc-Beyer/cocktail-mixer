#include <Arduino.h>
#include <stdio.h>
#include "MultiStepper.h"

struct Motor {
    int ports[4];
    int curStep;
    long nextStepTime;
    long stepsToGo;
};

byte motorSteps[4] = {0b1001, 0b0101, 0b0110, 0b1010};

struct Motor *motors;
// The number of motors in use
// This will be set by the init method
int motorCount = 0;

// Set the number of motor, you want to use
bool initStepper(int count){
    if(count <= 0){
        count = 1;
    }

    // Allocate the nessecery space for the number of motors
    motors = (struct Motor *) malloc(count * sizeof(struct Motor));

    // Check if allocation was successful
    if(motors){
        motorCount = count;
        return true;
    }else{
        return false;
    }
}

bool initMotor(int id, int *ports){
    // Check if the id is valid
    if(id < 0 || id >= motorCount){
        return false;
    }

    // Initilaze the ports
    for(int i = 0; i < 4; i++){
        motors[id].ports[i] = ports[i];
        pinMode(ports[i], OUTPUT);
    }
    motors[id].curStep = 0;
    motors[id].nextStepTime = millis();
    motors[id].stepsToGo = 0;

    return true;
}

char* getStats(int id){
  static char str[40];
  
  sprintf(str, "stepsToGo: %ld curStep: %d", motors[id].stepsToGo, motors[id].curStep);
  return str;
}

bool moveMotor(int id, long stepSpeed){
    if(id < 0 || id >= motorCount){
        return false;
    }

    if(motors[id].stepsToGo != 0 && millis() > motors[id].nextStepTime){
        motors[id].nextStepTime = millis() + stepSpeed;
        byte b = motorSteps[motors[id].curStep];

        for ( int i = 0; i < 4; i++) {
        digitalWrite(motors[id].ports[i], (b & 1) ? HIGH : LOW );
        b >>= 1;
        }

        if(motors[id].stepsToGo < 0){
            motors[id].curStep = (motors[id].curStep - 1 < 0) ? 3 : motors[id].curStep - 1;
            motors[id].stepsToGo++;
        }else if(motors[id].stepsToGo > 0){
            motors[id].curStep = (motors[id].curStep + 1) % 4;
            motors[id].stepsToGo--;
        }
    }
    
    return motors[id].stepsToGo == 0;
}

void setStepsToGo(int id, long steps){
    if(id < 0 || id >= motorCount){
        return;
    }

    motors[id].stepsToGo = steps;
}
