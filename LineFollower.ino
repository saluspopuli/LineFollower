// SENSOR PINS SETUP ========================================================
// This setup is made to be used with the PCB Lab TPR-105F IR Sensor
// boards but it can theoretically be used with any analog IR boards
// that have 5-sensor setups.

// When defining your sensors, make sure they are put from left to right.
// sensor1 - leftmost
// sensor5 - rightmost
#define sensor1     A1
#define sensor2     A2
#define sensor3     A3
#define sensor4     A4
#define sensor5     A5
int sensorPins[5] = {sensor1, sensor2, sensor3, sensor4, sensor5};

// This threshold value will be set when the setup code runs for the first time.
int whiteThreshold = 0;

// MOTOR PINS SETUP =========================================================
// These pins are made to be used with the TB6612FNG motor driver board.
// The names should correspond to where your wires are connected on the board
// itself.
#define PWMA        5
#define PWMB        6
#define motorA1     3
#define motorA2     4
#define motorB1     7
#define motorB2     8

// MAIN CODE ================================================================

void setup() {

  // Sets up the pins of the motor driver
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);

  // Sets up the pins of the IR sensors
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);

  // Turns off builtin LED to indicate that it will calibrate.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Brief delay of 1 second to ensure system is fully operational before reading
  // the value of white.
  delay(1000);

  // !!! IMPORTANT !!!
  // This part reads the current value of the sensors and then uses that as the 
  // threshold between black/white. THIS MEANS THAT THE ROBOT MUST BE TURNED ON
  // WITH THE SENSORS FACING A WHITE SURFACE.
  int tmp_array[5];
  readSensors(tmp_array);
  int tmp_sum = 0;

  for (int i = 0; i < 5; i ++){
    tmp_sum += tmp_array[i];
  }

  // The average is multiplied by a ratio to give a bit more allowance before the
  // robot reads a black value.
  whiteThreshold = (tmp_sum/5)*0.75;


  // Outputs the current white threshold value for debugging purposes.
  Serial.begin(9600);
  Serial.println();
  Serial.println(whiteThreshold);


  delay(1000);

  // The built in LED will go high to indicate that the robot is now ready to be placed on black.
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // The robot should only read the sensors ONCE PER LOOP because ADC conversion takes time and the 
  // robot needs to be quick and responsive.
  int currSensorValues[5];
  readSensors(currSensorValues);

  // This part of the code looks for which sensor has the highest and lowest value.
  // It iterates through the array and takes note of which is highest and lowest.
  int lowestIndex = 0;
  int highestIndex = 0;

  for (int i = 0; i < 5; i++){
    if (currSensorValues[i] < currSensorValues[lowestIndex])
      lowestIndex = i;
    if (currSensorValues[i] > currSensorValues[highestIndex]) 
      highestIndex = i;
  }

  // This IF statement turns off the LED when it atleast one sensor is on black. It goes
  // high when all sensors are on white. This is just for debugging.
  if (currSensorValues[lowestIndex] < whiteThreshold){
    digitalWrite(LED_BUILTIN, LOW);
  } else
    digitalWrite(LED_BUILTIN, HIGH);


  if (currSensorValues[lowestIndex] < whiteThreshold && currSensorValues[highestIndex] < whiteThreshold){
    // This condition checks if all sensors are on black. If true, the robot will go straight because
    // the assumption is it is going through a giant black box. In that case, we want to minimize it 
    // turning as much as possible.
    setMotors(255,255);

  } else if (currSensorValues[lowestIndex] < whiteThreshold){
    // Otherwise, this condition checks to see where the black line is, since the assumption is that
    // the lower the value, the more the sensor is on the dark line. Since there are only 5 sensors,
    // it really only needs 5 adjustment functions.

    switch(lowestIndex){

      case 0: // Line is on the far left
        setMotors(0,255); // Turn hard left
        break;
        
      case 1: // Line is on the near left
        setMotors(150,255); // Turn slightly left
        break;

      case 2: // Line is in the middle
        setMotors(255,255); // Go straight
        break;

      case 3: // Line is on the near right
        setMotors(255,150); // Turn slightly right
        break;

      case 4: // Line is on the far right
        setMotors(255,0); // Turn hard right
        break;

      default:
        break;
    }
    // Notice how there is no default else statement, so what does the robot do if it all sensors are on white?
    // Remember how setting the motors once does not mean it will stop moving after. So if all sensors are on white, 
    // the robot will not do anything, instead, the motors will keep turning in the same direction they were just
    // turning. So if the robot overshoots, it will basically get back on the line if the sensors detected the line
    // properly.

  }
}

// UTILITY/EXTRA FUNCTIONS =================================================

// Debugging function used to print the sensor values. Since printing them
// directly looks very bad as it constantly fluctuates
void printWithFourDigits(int value) {
  if (value < 10) {
    Serial.print("---");  // Add three leading zeros
  } else if (value < 100) {
    Serial.print("--");   // Add two leading zeros
  } else if (value < 1000) {
    Serial.print("-");    // Add one leading zero
  }
  Serial.print(value);  // Print the value after leading zeros
}

// This function takes in an array and populates it with ADC readings from the sensor.
void readSensors(int (&sensorVals)[5]){
  for (int i = 0; i < 5; i++){
    sensorVals[i] = analogRead(sensorPins[i]);
  }
}

// This function can take in 2 signed integers from -255 to 255. If negative, the
// Motor will just go backwards. 
void setMotors(int Lspeed, int Rspeed){

  // Just change the digitalWrite functions from LOW -> HIGH or HIGH -> LOW if
  // the motors are not performing as expected. I just mapped them to these boolean
  // values because I messed up the wiring and I didn't want to change it hehe.
  if (Lspeed > 0){
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
  } else {
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
  }

  if (Rspeed > 0){
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
  } else {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
  }

  // The absolute value of the speed is used because the negative values were only used
  // to determine motor direction.
  analogWrite(PWMA, abs(Rspeed));
  analogWrite(PWMB, abs(Lspeed));
}
