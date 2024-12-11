#include <ArduinoBLE.h>
#include <Arduino_LPS22HB.h>  // Library for the LPS22HB pressure sensor
#include <Arduino_LSM9DS1.h>  // Library for the LSM9DS1 IMU sensor

// Define standard environmental sensing service UUID and characteristics UUIDs
BLEService environmentalService("181A");  // Environmental Sensing Service UUID
BLECharacteristic temperatureCharacteristic("2A6E", BLERead | BLENotify, sizeof(int16_t)); // Temperature characteristic
BLECharacteristic pressureCharacteristic("2A6D", BLERead | BLENotify, sizeof(uint32_t));   // Pressure characteristic
BLECharacteristic waterActivityCharacteristic("2A6F", BLERead | BLENotify, sizeof(uint8_t)); // Water activity characteristic

// Variables for water activity detection
float z_prev = 0.0;   // Previous z-axis acceleration
float threshold = 0.1; // Threshold for detecting activity based on acceleration change
bool is_active = false; // Flag to indicate water surface activity

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Failed to start BLE!");
    while (1);
  }

  // Initialize pressure sensor
  if (!BARO.begin()) {
    Serial.println("Failed to initialize the barometric sensor!");
    while (1);
  }

  // Initialize IMU sensor
  if (!IMU.begin()) {
    Serial.println("Failed to initialize the IMU sensor!");
    while (1);
  }

  // Set device name and add the environmental sensing service
  BLE.setLocalName("EnvSensor");
  BLE.setAdvertisedService(environmentalService);

  // Add characteristics to the service
  environmentalService.addCharacteristic(temperatureCharacteristic);
  environmentalService.addCharacteristic(pressureCharacteristic);
  environmentalService.addCharacteristic(waterActivityCharacteristic);
  BLE.addService(environmentalService);

  // Start BLE advertising
  BLE.advertise();
  Serial.println("BLE peripheral started, waiting for connections...");
}

void loop() {
  // Wait for a central device to connect
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central device: ");
    Serial.println(central.address());

    // Maintain connection while the central device is connected
    while (central.connected()) {
      // Read temperature and pressure from the barometric sensor
      float temperature = BARO.readTemperature(); // Temperature in Celsius
      float pressure = BARO.readPressure();       // Pressure in kPa

      // Convert data to integers for BLE transmission
      int16_t temperatureValue = temperature * 100; // Convert to integer, preserving two decimals
      uint32_t pressureValue = pressure * 100;      // Convert to integer, preserving two decimals

      // Write the values to the BLE characteristics
      temperatureCharacteristic.writeValue(temperatureValue);
      pressureCharacteristic.writeValue(pressureValue);

      // Check for new acceleration data from the IMU
      float x, y, z;
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z); // Read acceleration data

        // Calculate the change in z-axis acceleration
        float delta_z = abs(z - z_prev);

        // Determine if the activity threshold is exceeded
        if (delta_z > threshold) {
          is_active = true;
        } else {
          is_active = false;
        }

        // Write the water activity status to the BLE characteristic
        uint8_t is_active_value = is_active ? 1 : 0;
        waterActivityCharacteristic.writeValue(is_active_value);

        // Output water activity status to the Serial Monitor
        if (is_active) {
          Serial.println("Water surface is active.");
        } else {
          Serial.println("Water surface is calm.");
        }

        // Update the previous z-axis acceleration value
        z_prev = z;
      }

      // Output temperature and pressure to the Serial Monitor
      Serial.print("Sent temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");

      Serial.print("Sent pressure: ");
      Serial.print(pressure);
      Serial.println(" kPa");

      // Keep the BLE connection alive
      BLE.poll();

      // Delay to update data once per second
      delay(1000);
    }

    // Central device disconnected
    Serial.println("Central device disconnected.");
  } else {
    // No connection, add delay to reduce power consumption
    delay(1000);
  }
}
