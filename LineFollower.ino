#define sensor1 A1
#define sensor2 A2
#define sensor3 A3
#define sensor4 A4
#define sensor5 A5

int sensorPins[5] = {sensor1, sensor2, sensor3, sensor4, sensor5};

#define PWMA 5
#define PWMB 6
#define motorA1 3
#define motorA2 4
#define motorB1 7
#define motorB2 8

int whiteThreshold = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(PWMA, INPUT);
  pinMode(PWMB, INPUT);
  pinMode(motorA1, INPUT);
  pinMode(motorA2, INPUT);
  pinMode(motorB1, INPUT);
  pinMode(motorB2, INPUT);

  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  pinMode(sensor5, INPUT);

  delay(1000);

  int tmp_array[5];

  readSensors(tmp_array);

  int tmp_sum = 0;

  for (int i = 0; i < 5; i ++){
    tmp_sum += tmp_array[i];
  }

  whiteThreshold = (tmp_sum/5)*0.70;
  //whiteThreshold = 400;
  Serial.begin(9600);
  Serial.println();
  Serial.println(whiteThreshold);
  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  int currSensorValues[5];

  readSensors(currSensorValues);

  // for (int i = 0; i < 5; i++) {
  //   printWithFourDigits(currSensorValues[i]);  // Print each value padded to 4 digits
  //   Serial.print(" ");  // Add space between values
  // }

  // Serial.println();

  int lowestIndex = 0;
  int highestIndex = 0;
  
  for (int i = 0; i < 5; i++){
    if (currSensorValues[i] < currSensorValues[lowestIndex])
      lowestIndex = i;
    if (currSensorValues[i] > currSensorValues[highestIndex]) 
      highestIndex = i;  // Update the index of the largest value
  }

  if (currSensorValues[lowestIndex] < whiteThreshold){
    digitalWrite(LED_BUILTIN, LOW);
  } else
    digitalWrite(LED_BUILTIN, HIGH);

  if (currSensorValues[lowestIndex] < whiteThreshold && currSensorValues[highestIndex] < whiteThreshold){
    setMotors(255,255);
  } else if (currSensorValues[lowestIndex] < whiteThreshold){
    switch(lowestIndex){
      case 0:
        setMotors(0,255);
        Serial.println("Hardleft");
        break;
      case 1:
        setMotors(150,255);
        Serial.println("left");
        break;
      case 2:
        setMotors(255,255);
        Serial.println("Straight");
        break;
      case 3:
        setMotors(255,150);
        Serial.println("right");
        break;
      case 4:
        setMotors(255,0);
        Serial.println("HardRight");
        break;
      default:
        break;
    }

  }
}

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

void readSensors(int (&sensorVals)[5]){
  for (int i = 0; i < 5; i++){
    sensorVals[i] = analogRead(sensorPins[i]);
  }
}

void setMotors(int Lspeed, int Rspeed){
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

  analogWrite(PWMA, abs(Rspeed));
  analogWrite(PWMB, abs(Lspeed));
}
