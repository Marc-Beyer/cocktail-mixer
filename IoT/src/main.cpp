#include <Arduino.h>
#include <ButtonDebouncedCalloc.h>
#include <LedControl.h>
#include <MultiStepper.h>

// stepper distances
#define X_DIST_1 200
#define X_DIST_2 100
#define X_DIST_3 100

#define Y_DIST_1 100
#define Y_DIST_2 100

// State definitions:
#define STATE_ERROR -1
#define STATE_TEST 0
#define STATE_CALIBRATE 1
#define STATE_READY 2
#define STATE_WORKING 3

#define STATE_CONTROLLED 42

// Micro switch buttons
#define LEFT_BTN_ID 0
#define RIGHT_BTN_ID 1
#define BOTTOM_BTN_ID 2
#define TOP_BTN_ID 3

#define LEFT_BTN_GPIO 13
#define RIGHT_BTN_GPIO 4
#define BOTTOM_BTN_GPIO 21
#define TOP_BTN_GPIO 22

// Stepper motors
#define X_MOTOR_ID 0
#define X_MOTOR_SPEED 1
#define X_MOTOR_CONF_SPEED 1
#define Y_MOTOR_ID 1
#define Y_MOTOR_SPEED 1
#define Y_MOTOR_CONF_SPEED 6

// Stepper GPIOs 32 33 25 26
int xMotorPorts[4] = {33, 32, 26, 25};
// Stepper GPIOs 19 18 17 16
int yMotorPorts[4] = {18, 19, 16, 17};

// Drinks
typedef struct {
    int motorId;
    int steps;
    int speed;
    int interruptBtnId;
    int delayAfter;
} Task;

#define X_DIV_1 145
#define X_DIV_2 500

#define Y_DIV 675
#define Y_DIV_1 950

Task drink_ginTonic[12] = {
    {X_MOTOR_ID, X_DIV_1, X_MOTOR_SPEED, RIGHT_BTN_ID, 2000},
    {Y_MOTOR_ID, -Y_DIV_1, Y_MOTOR_SPEED, RIGHT_BTN_ID, 6000},
    {Y_MOTOR_ID, Y_DIV_1, Y_MOTOR_SPEED, BOTTOM_BTN_ID, 2000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 2000},
    {Y_MOTOR_ID, -Y_DIV, Y_MOTOR_SPEED, RIGHT_BTN_ID, 5000},
    {Y_MOTOR_ID, Y_DIV, Y_MOTOR_SPEED, BOTTOM_BTN_ID, 2000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 2000},
    {Y_MOTOR_ID, -Y_DIV, Y_MOTOR_SPEED, RIGHT_BTN_ID, 5000},
    {Y_MOTOR_ID, Y_DIV, Y_MOTOR_SPEED, BOTTOM_BTN_ID, 2000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 2000},
    {Y_MOTOR_ID, -Y_DIV, Y_MOTOR_SPEED, RIGHT_BTN_ID, 5000},
    {Y_MOTOR_ID, Y_DIV, Y_MOTOR_SPEED, BOTTOM_BTN_ID, 2000}};

Task drink_water[4] = {{X_MOTOR_ID, X_DIV_1, X_MOTOR_SPEED, RIGHT_BTN_ID, 0},
                       {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 2000},
                       {Y_MOTOR_ID, -Y_DIV, Y_MOTOR_SPEED, RIGHT_BTN_ID, 6000},
                       {Y_MOTOR_ID, Y_DIV, Y_MOTOR_SPEED, BOTTOM_BTN_ID, 4000}};

Task drink_abstandTest[4] = {
    {X_MOTOR_ID, X_DIV_1, X_MOTOR_SPEED, RIGHT_BTN_ID, 4000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 4000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 4000},
    {X_MOTOR_ID, X_DIV_2, X_MOTOR_SPEED, RIGHT_BTN_ID, 4000}};

Task *curTasks;
int curTask;
int taskLength;

// State machine
int state = STATE_TEST;
// int state = STATE_READY;
bool changedState = true;

void setup() {
    Serial.begin(115200);
    setupLeds();

    Serial.println("|================================================|");
    Serial.println("|  Cocktailmixer v1.1.0 by Marc, Marco and Alex  |");
    Serial.println("|================================================|");

    // Init buttons
    pinMode(LEFT_BTN_GPIO, INPUT_PULLUP);
    pinMode(RIGHT_BTN_GPIO, INPUT_PULLUP);
    pinMode(BOTTOM_BTN_GPIO, INPUT_PULLUP);
    pinMode(TOP_BTN_GPIO, INPUT_PULLUP);

    // Currently only 3
    bd_init(4);
    bd_setButton(LEFT_BTN_ID, LEFT_BTN_GPIO, 20);
    bd_setButton(RIGHT_BTN_ID, RIGHT_BTN_GPIO, 20);
    bd_setButton(BOTTOM_BTN_ID, BOTTOM_BTN_GPIO, 20);
    bd_setButton(TOP_BTN_ID, TOP_BTN_GPIO, 20);

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

// The arduino is listening on thr serial port and
// gets controlled by it
void stateControlledHandler() {
    if (changedState) {
        changedState = false;
        Serial.println("42");
    }

    static int motorId = -1;
    static int steps = -1;
    static int speed = -1;
    static int interruptBtnId = -1;
    static int delayAfter = 0;

    if (motorId >= 0) {
        int motorState = moveMotor(motorId, speed);

        if (bd_getButton(interruptBtnId) == LOW) {
            setStepsToGo(motorId, 0);
            shutdownMotor(motorId);

            delay(delayAfter);
            Serial.println(getPosition(motorId));

            motorId = -1;
        } else if (motorState == 1 || motorState == -1) {
            shutdownMotor(motorId);

            delay(delayAfter);
            Serial.println(getPosition(motorId));

            motorId = -1;
        }
    } else if (Serial.available()) {
        char buffer[40];
        int n = Serial.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
        buffer[n] = '\0';

        sscanf(buffer, "%d:%d:%d:%d:%d", &motorId, &steps, &speed,
               &interruptBtnId, &delayAfter);

        Serial.print("id: ");
        Serial.print(motorId);
        Serial.print(" inter: ");
        Serial.print(interruptBtnId);
        Serial.print(" speed: ");
        Serial.print(speed);
        Serial.print(" steps: ");
        Serial.print(steps);

        if (motorId == -1 && steps >= 0) {
            changedState = false;
            state = STATE_CALIBRATE;
            shutdownMotor(X_MOTOR_ID);
            shutdownMotor(Y_MOTOR_ID);
            delay(steps);
        } else {
            setStepsToGo(motorId, steps);
        }
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
        setStepsToGo(X_MOTOR_ID, 12);
        moveMotor(X_MOTOR_ID, X_MOTOR_SPEED);
    } else if (bd_getButton(RIGHT_BTN_ID) == LOW) {
        setStepsToGo(X_MOTOR_ID, -12);
        moveMotor(X_MOTOR_ID, X_MOTOR_SPEED);
    } else {
        shutdownMotor(X_MOTOR_ID);
    }

    if (bd_getButton(BOTTOM_BTN_ID) == LOW) {
        setStepsToGo(Y_MOTOR_ID, -12);
        moveMotor(Y_MOTOR_ID, Y_MOTOR_SPEED);
    } else {
        shutdownMotor(Y_MOTOR_ID);
    }

    // Currently not in use
    /*
    if (bd_getButton(TOP_BTN_ID) == LOW) {
        setStepsToGo(Y_MOTOR_ID, 12);
        moveMotor(Y_MOTOR_ID, Y_MOTOR_SPEED);
    }
    */

    if (bd_getButton(LEFT_BTN_ID) == LOW && bd_getButton(RIGHT_BTN_ID) == LOW) {
        changedState = false;
        state = STATE_CALIBRATE;
        shutdownMotor(X_MOTOR_ID);
        shutdownMotor(Y_MOTOR_ID);
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
        setStepsToGo(Y_MOTOR_ID, 10);
        Serial.println("Calibration...");
    }

    static bool yMotorCalibrated = false;

    if (!yMotorCalibrated) {
        if (bd_getButton(BOTTOM_BTN_ID) == HIGH) {
            setStepsToGo(Y_MOTOR_ID, 10);
            moveMotor(Y_MOTOR_ID, Y_MOTOR_CONF_SPEED);
        } else {
            yMotorCalibrated = true;
            // Stop the y motor
            setStepsToGo(Y_MOTOR_ID, 0);
            shutdownMotor(Y_MOTOR_ID);
        }
    } else if (bd_getButton(LEFT_BTN_ID) == HIGH) {
        // Move the motor
        setStepsToGo(X_MOTOR_ID, -10);
        moveMotor(X_MOTOR_ID, X_MOTOR_CONF_SPEED);
    } else {
        // Stop the motor and set it's position to zero
        setStepsToGo(X_MOTOR_ID, 0);
        setPosition(X_MOTOR_ID, 0);
        setPosition(Y_MOTOR_ID, 0);
        shutdownMotor(X_MOTOR_ID);
        yMotorCalibrated = false;

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
        Serial.println("Choose a cocktail");
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

                curTasks = drink_ginTonic;
                curTask = 0;
                taskLength = sizeof(drink_ginTonic) / sizeof(drink_ginTonic[0]);

                changedState = true;
                state = STATE_WORKING;
                break;
            case 2:
                Serial.println("|=========================|");
                Serial.println("|        Rum Cola         |");
                Serial.println("|=========================|");
                break;
            case 3:
                Serial.println("|=========================|");
                Serial.println("|    Sex on the beach     |");
                Serial.println("|=========================|");

                curTasks = drink_abstandTest;
                curTask = 0;
                taskLength =
                    sizeof(drink_abstandTest) / sizeof(drink_abstandTest[0]);

                changedState = true;
                state = STATE_WORKING;
                break;
            case 4:
                Serial.println("|=========================|");
                Serial.println("|         Wasser          |");
                Serial.println("|=========================|");

                curTasks = drink_water;
                curTask = 0;
                taskLength = sizeof(drink_water) / sizeof(drink_water[0]);

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

    int motorState =
        moveMotor(curTasks[curTask].motorId, curTasks[curTask].speed);

    if (motorState == 1 || motorState == -1 ||
        bd_getButton(curTasks[curTask].interruptBtnId) == LOW) {
        shutdownMotor(curTasks[curTask].motorId);
        delay(curTasks[curTask].delayAfter);
        curTask++;
        if (curTask >= taskLength) {
            changedState = true;
            state = STATE_CALIBRATE;
        } else {
            setStepsToGo(curTasks[curTask].motorId, curTasks[curTask].steps);
        }
    }
}

void loop() {
    loopLeds();

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