#ifdef __cplusplus
extern "C" {
#endif

// Init the library and define how many motors you want to use
bool initStepper(int count);

// Define the id ans ports of a motor
// for id, id >= 0 && id < motor-count 
bool initMotor(int id, int *ports);

// Call this in the loop-function to move the moter with the id at a speed
// Returns true if motor is ready and false if the motor id is wrong or the motor is still moving.
bool moveMotor(int id, long stepSpeed);

// Define how far the motor with the id should move
void setStepsToGo(int id, long steps);

// Get e string with some stats about the motor with the id
char* getStats(int id);
#ifdef __cplusplus
}
#endif
