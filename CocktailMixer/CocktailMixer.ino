#include <ButtonDebouncedCalloc.h>

#include "MultiStepper.h"

// State definitions:
#define STATE_ERROR -1
#define STATE_TEST 0
#define STATE_CALIBRATE 1
#define STATE_READY 2
#define STATE_WORKING 3

#define STATE_CONTROLLED 42

// Micro switch buttons
#define LEFT_BTN_ID 0
#define LEFT_BTN_GPIO 2

#define RIGHT_BTN_ID 1
#define RIGHT_BTN_GPIO 3

#define TOP_BTN_ID 2
#define TOP_BTN_GPIO 4

#define BOTTOM_BTN_ID 3
#define BOTTOM_BTN_GPIO 5

// Stepper motors
#define X_MOTOR_ID 0
#define X_MOTOR_SPEED 6
#define Y_MOTOR_ID 1
#define Y_MOTOR_SPEED 6

/*
int xMotorPorts[4] = {39, 38, 41, 40};
int yMotorPorts[4] = {47, 46, 49, 48};
 */

int xMotorPorts[4] = {39, 38, 41, 40};
int yMotorPorts[4] = {47, 46, 49, 48};

// Drinks
typedef struct {
    int motorId;
    int steps;
    int speed;
    int interruptBtnId;
} Task;

Task drink_ginTonic[4] = {{X_MOTOR_ID, -400, 10, RIGHT_BTN_ID},
                          {Y_MOTOR_ID, 20, 10, TOP_BTN_ID},
                          {Y_MOTOR_ID, -20, 10, BOTTOM_BTN_ID},
                          {X_MOTOR_ID, -200, 10, RIGHT_BTN_ID}};

Task drink_water[4] = {{X_MOTOR_ID, -400, 10, RIGHT_BTN_ID},
                       {Y_MOTOR_ID, 20, 10, TOP_BTN_ID},
                       {Y_MOTOR_ID, -20, 10, BOTTOM_BTN_ID},
                       {X_MOTOR_ID, -400, 10, RIGHT_BTN_ID}};

Task *curTasks;
int curTask;
int taskLength;

void setup() {
    Serial.begin(115200);

    Serial.println("|================================================|");
    Serial.println("|  Cocktailmixer v1.0.1 by Marc, Marco and Alex  |");
    Serial.println("|================================================|");

    // Init buttons
    pinMode(LEFT_BTN_GPIO, INPUT_PULLUP);
    pinMode(RIGHT_BTN_GPIO, INPUT_PULLUP);
    pinMode(TOP_BTN_GPIO, INPUT_PULLUP);
    pinMode(BOTTOM_BTN_GPIO, INPUT_PULLUP);

    bd_init(4);
    bd_setButton(LEFT_BTN_ID, LEFT_BTN_GPIO, 20);
    bd_setButton(RIGHT_BTN_ID, RIGHT_BTN_GPIO, 20);
    bd_setButton(TOP_BTN_ID, TOP_BTN_GPIO, 20);
    bd_setButton(BOTTOM_BTN_ID, BOTTOM_BTN_GPIO, 20);

    // Init stepper-motors

    Serial.print("|    init lib     |");
    Serial.println(initStepper(2) ? "          successful        |"
                                  : "            error           |");
    Serial.println("|==============================================|");

    Serial.print("|  init stepper 1 |");
    Serial.println(initMotor(X_MOTOR_ID, xMotorPorts)
                       ? "          successful        |"
                       : "            error           |");
    Serial.println("|==============================================|");

    Serial.print("|  init stepper 2 |");
    Serial.println(initMotor(Y_MOTOR_ID, yMotorPorts)
                       ? "          successful        |"
                       : "            error           |");
    Serial.println("|==============================================|");
}

// State machine
// int state = STATE_TEST;
int state = STATE_READY;
bool changedState = true;

void loop() {
    switch (state) {
        case STATE_ERROR:
            stateErrorHandler();
            break;

        case STATE_CALIBRATE:
            stateCalibrateHandler();
            break;

        case STATE_READY:
            stateReadyHandler();
            break;

        case STATE_WORKING:
            stateWorkingHandler();
            break;

        case STATE_TEST:
            stateTestHandler();
            break;

        case STATE_CONTROLLED:
            stateControlledHandler();
            break;

        default:
            state = STATE_ERROR;
            break;
    }
}

void stateControlledHandler() {
    if (changedState) {
        changedState = false;
        Serial.println("1");
    }

    static int motorId = -1;
    static int steps = -1;
    static int speed = -1;
    static int interruptBtnId = -1;

    if (motorId >= 0) {
        int motorState = moveMotor(motorId, speed);

        if (bd_getButton(interruptBtnId) == LOW) {
        } else if (motorState == 0 || motorState == -1) {
        }
    } else if (Serial.available()) {
        char buffer[20];
        int n = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';

        sscanf(buffer, "%d:%d:%d:%d", &motorId, &steps, &speed,
               &interruptBtnId);

        Serial.print(motorId);
        Serial.print(steps);
        Serial.print(speed);
        Serial.print(interruptBtnId);

        setStepsToGo(motorId, steps);
    }
}

// Test the buttons and motors
// If the left and right button are pressed simultaneously,
// the calibrate-state is triggered
void stateTestHandler() {
    if (changedState) {
        changedState = false;
    }

    if (bd_getButton(LEFT_BTN_ID) == LOW) {
        setStepsToGo(X_MOTOR_ID, -12);
        moveMotor(X_MOTOR_ID, X_MOTOR_SPEED);
    }

    if (bd_getButton(RIGHT_BTN_ID) == LOW) {
        setStepsToGo(X_MOTOR_ID, 12);
        moveMotor(X_MOTOR_ID, X_MOTOR_SPEED);
    }

    if (bd_getButton(BOTTOM_BTN_ID) == LOW) {
        setStepsToGo(Y_MOTOR_ID, 12);
        moveMotor(Y_MOTOR_ID, Y_MOTOR_SPEED);
    }

    if (bd_getButton(TOP_BTN_ID) == LOW) {
        setStepsToGo(Y_MOTOR_ID, -12);
        moveMotor(Y_MOTOR_ID, Y_MOTOR_SPEED);
    }

    if (bd_getButton(LEFT_BTN_ID) == LOW && bd_getButton(RIGHT_BTN_ID) == LOW) {
        changedState = false;
        state = STATE_CALIBRATE;
        delay(1000);
    }
}

// Handle a unknown state
void stateErrorHandler() {
    Serial.println("Unknown error!");
    state = STATE_CALIBRATE;
    changedState = true;
}

// Move the motors in their initial positions
void stateCalibrateHandler() {
    if (changedState) {
        changedState = false;
    }

    // setStepsToGo(X_MOTOR_ID, 1400);
    // setStepsToGo(Y_MOTOR_ID, 1100);

    /*
    if (bd_getButton(BOTTOM_BTN_GPIO) == 0) {
        setStepsToGo(Y_MOTOR_ID, 10);
        moveMotor(Y_MOTOR_ID, X_MOTOR_SPEED);
    } else
    */

    if (bd_getButton(LEFT_BTN_ID) == HIGH) {
        // Stop the y motor
        setStepsToGo(Y_MOTOR_ID, 0);

        // Move the motor
        setStepsToGo(X_MOTOR_ID, 10);
        moveMotor(X_MOTOR_ID, X_MOTOR_SPEED);
    } else {
        // Stop the motor and set it's position to zero
        setStepsToGo(X_MOTOR_ID, 0);
        setPosition(X_MOTOR_ID, 0);
        setPosition(Y_MOTOR_ID, 0);

        state = STATE_READY;
        changedState = true;
    }
}

// Wait for user input
void stateReadyHandler() {
    if (changedState) {
        changedState = false;

        Serial.println("");
        Serial.println("|=========================|");
        Serial.println("|  Wähle ein Getränk aus  |");
        Serial.println("|=========================|");
        Serial.println("|  1  |  Gin Tonic        |");
        Serial.println("|  2  |  Rum Cola         |");
        Serial.println("|  3  |  Pina Colada      |");
        Serial.println("|  4  |  Wasser           |");
        Serial.println("|=========================|");
    }

    if (Serial.available()) {
        int nr = -1;

        char buffer[4];
        int n = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';

        sscanf(buffer, "%d", &nr);

        switch (nr) {
            case 1:
                Serial.println("|=========================|");
                Serial.println("|        Gin Tonic        |");
                Serial.println("|=========================|");
                break;
            case 2:
                Serial.println("|=========================|");
                Serial.println("|        Rum Cola         |");
                Serial.println("|=========================|");
                break;
            case 3:
                Serial.println("|=========================|");
                Serial.println("|       Pina Colada       |");
                Serial.println("|=========================|");
                break;
            case 4:
                Serial.println("|=========================|");
                Serial.println("|         Wasser          |");
                Serial.println("|=========================|");

                curTasks = drink_ginTonic;
                curTask = 0;
                taskLength = sizeof(drink_ginTonic) / sizeof(drink_ginTonic[0]);

                changedState = true;
                state = STATE_WORKING;
                break;

            case 42:  // Arduino will be controlled by the serial port
                changedState = true;
                state = STATE_CONTROLLED;
                break;

            default:
                Serial.println("FEHLER: Getränk konnte nicht gefunden werden!");
                break;
        }
    }
}

// Complete an order
void stateWorkingHandler() {
    if (changedState) {
        changedState = false;
        Serial.println("|=========================|");
        Serial.println("|         WORKING         |");
        Serial.println("|=========================|");

        setStepsToGo(curTasks[0].motorId, curTasks[0].steps);
        curTask = 0;
    }

    Serial.println(sizeof(curTasks));
    Serial.println(sizeof(curTasks[0]));
    Serial.println(sizeof(curTasks) / sizeof(curTasks[0]));
    Serial.println(curTask);

    int motorState =
        moveMotor(curTasks[curTask].motorId, curTasks[curTask].speed);

    if (motorState == 0 || motorState == -1 ||
        bd_getButton(curTasks[curTask].interruptBtnId) == LOW) {
        curTask++;
        if (curTask >= taskLength) {
            changedState = true;
            state = STATE_CALIBRATE;
        } else {
            setStepsToGo(curTasks[curTask].motorId, curTasks[0].steps);
        }
    }
}
