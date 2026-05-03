//aurdino uno code 1

#include <Wire.h>
#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mySensor;

const int emgPin = A0; // Analog pin for EMG signal input
int readingCount = 0;  // Counter to track readings

void setup() {
  Serial.begin(115200);

  // Setup I2C for Arduino Uno (A4 = SDA, A5 = SCL)
  Wire.begin();
  mySensor.setWire(&Wire);

  // Check MPU9250 ID
  uint8_t id;
  mySensor.readId(&id);
  if (id == 0xFF || id == 0x00) {
    Serial.println("ERROR: MPU9250 not detected. Please check wiring.");
    while (1); // Halt system
  }

  // Initialize accelerometer and gyroscope
  mySensor.beginAccel();
  mySensor.beginGyro();

  delay(2000); // Let sensors stabilize

  // Initial header
  printHeader();
}

void loop() {
  // Read EMG signal (0–1023 analog)
  int emgValue = analogRead(emgPin);

  // Update MPU9250 sensor readings
  mySensor.accelUpdate();
  mySensor.gyroUpdate();

  // Get Accelerometer Data (in g)
  float ax = mySensor.accelX();
  float ay = mySensor.accelY();
  float az = mySensor.accelZ();
  float accelMag = sqrt(ax * ax + ay * ay + az * az);

  // Get Gyroscope Data (in deg/s)
  float gx = mySensor.gyroX();
  float gy = mySensor.gyroY();
  float gz = mySensor.gyroZ();
  float gyroMag = sqrt(gx * gx + gy * gy + gz * gz);

  // Print header every 15 readings
  if (readingCount % 15 == 0) {
    printHeader();
  }

  // Print data in tab-separated columns
  Serial.print(emgValue); Serial.print("\t");
  Serial.print(ax, 2);    Serial.print("\t");
  Serial.print(ay, 2);    Serial.print("\t");
  Serial.print(az, 2);    Serial.print("\t");
  Serial.print(accelMag, 2); Serial.print("\t");
  Serial.print(gx, 2);    Serial.print("\t");
  Serial.print(gy, 2);    Serial.print("\t");
  Serial.print(gz, 2);    Serial.print("\t");
  Serial.println(gyroMag, 2);

  readingCount++;
  delay(1000); // ~1Hz sampling rate
}

void printHeader() {
  Serial.println("EMG_Value\tAccel_X\tAccel_Y\tAccel_Z\tAccel_Mag\tGyro_X\tGyro_Y\tGyro_Z\tGyro_Mag");
}
