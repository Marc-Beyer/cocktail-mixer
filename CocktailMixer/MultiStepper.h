#ifdef __cplusplus
extern "C" {
#endif

// Init the library and define how many motors you want to use
bool initStepper(int count);

// Define the id ans ports of a motor
// for id, id >= 0 && id < motor-count 
bool initMotor(int id, int *ports);

// Call this in the loop-function to move the motor with the id at a speed
// Returns 1 if motor is ready, -1 if the motor id is wrong and 0 the motor is still moving.
int moveMotor(int id, long stepSpeed);

/*
 *  GETTER AND SETTER
 */

// Get the current position of the motor
long getPosition(int id);

// Reset the position of a motor if it's calibrated
void setPosition(int id, long pos);

// Get how "far" the motor will move
long getStepsToGo(int id);

// Define how far the motor with the id should move
void setStepsToGo(int id, long steps);

#ifdef __cplusplus
}
#endif
