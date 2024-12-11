#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <TinyGPSPlus.h>  // Include TinyGPS++ library for GPS data parsing
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// UUIDs for BLE services and characteristics
#define SERVICE_UUID "181A"         // Environmental Sensing Service UUID
#define TEMPERATURE_UUID "2A6E"     // UUID for temperature characteristic
#define PRESSURE_UUID "2A6D"        // UUID for pressure characteristic
#define WATER_ACTIVITY_UUID "2A6F"  // UUID for water activity characteristic
#define DIRECTION_UUID "2B0E"       // UUID for direction characteristic
#define HEADING_UUID "2B0F"         // UUID for heading (angle) characteristic

// GPS module configuration
#define RXD2 16         // GPS module RX pin
#define TXD2 17         // GPS module TX pin
#define GPS_BAUD 9600   // Baud rate for GPS module communication

// Button GPIO pin definitions
#define BUTTON_K1 34
#define BUTTON_K2 35
#define BUTTON_K3 32
#define BUTTON_K4 33

// Flags to track button presses
volatile bool buttonK1Pressed = false;
volatile bool buttonK2Pressed = false;
volatile bool buttonK3Pressed = false;
volatile bool buttonK4Pressed = false;

// Variables to store selected target location
float targetLatitude = 0.0;         // Target latitude
float targetLongitude = 0.0;        // Target longitude
float highestLocations[3][2] = {    // Array to store top 3 recommended locations
    {0.0, 0.0}, 
    {0.0, 0.0}, 
    {0.0, 0.0}
};

// BLE connection flags and device pointers
bool doConnect = false;             // Flag to initiate connection
bool connected = false;             // Connection status flag
BLEAdvertisedDevice* myDevice;      // Pointer to the discovered device
BLERemoteService* pRemoteService = nullptr; // Pointer to the remote service

// Variables for sensor data
uint16_t temperatureValue = 0;      // Temperature value
uint32_t pressureValue = 0;         // Pressure value
bool waterActivity = false;         // Water surface activity flag

// Flags to track new data availability
bool newTemperature = false;
bool newPressure = false;
bool newWaterActivity = false;
bool needsRecalculation = true;     // Flag to indicate if recalculation is needed

// GPS instance and serial configuration
TinyGPSPlus gps;                    // GPS data parser object
HardwareSerial gpsSerial(2);        // Serial port for GPS communication

// Enumeration for menu states
enum MenuState { 
    MAIN_MENU,                      // Main menu state
    FISH_PREDICT,                   // Fishing prediction menu state
    LOCATION_RECOMMEND,             // Location recommendation menu state
    NAVIGATION                      // Navigation menu state
};
MenuState currentMenuState = MAIN_MENU; // Initialize to main menu state

// BLE client callback to handle connection events
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
    connected = true;
    Serial.println("Connected to peripheral device.");
  }

  void onDisconnect(BLEClient* pClient) {
    connected = false;
    Serial.println("Disconnected from peripheral device.");
  }
};

// BLE advertised device callback to handle discovered devices
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Check if the advertised device matches the target service UUID
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.println("Found target device, connecting...");
      myDevice = new BLEAdvertisedDevice(advertisedDevice); // Save the discovered device
      doConnect = true;                                     // Set connection flag
      BLEDevice::getScan()->stop();                        // Stop scanning upon finding the device
    }
  }
};

/**
 * @brief Connect to the BLE peripheral device.
 * This function attempts to establish a BLE connection with the target peripheral
 * and retrieves its environmental sensing service.
 */
void connectToPeripheral() {
  if (doConnect) {
    BLEClient* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());

    // Attempt to connect to the target device
    if (pClient->connect(myDevice)) {
      Serial.println("Connected to peripheral.");

      // Retrieve the environmental sensing service
      pRemoteService = pClient->getService(BLEUUID(SERVICE_UUID));
      if (pRemoteService == nullptr) {
        Serial.println("Failed to find our service UUID.");
        pClient->disconnect();
        return;
      }
      connected = true;  // Mark connection as successful
    } else {
      Serial.println("Failed to connect to peripheral.");
      connected = false;
    }

    doConnect = false; // Reset connection request flag
  }
}

/**
 * @brief Read characteristics from the remote BLE service.
 * This function reads temperature, pressure, water activity, direction,
 * and heading values from the remote device.
 */
void readCharacteristics(BLERemoteService* pRemoteService) {
  // Read temperature characteristic
  BLERemoteCharacteristic* pTempCharacteristic = pRemoteService->getCharacteristic(BLEUUID(TEMPERATURE_UUID));
  if (pTempCharacteristic != nullptr && pTempCharacteristic->canRead()) {
    temperatureValue = pTempCharacteristic->readUInt16();
    newTemperature = true;
    //Serial.print("Temperature: ");
    //Serial.println(temperatureValue / 100.0);
  }

  // Read pressure characteristic
  BLERemoteCharacteristic* pPressureCharacteristic = pRemoteService->getCharacteristic(BLEUUID(PRESSURE_UUID));
  if (pPressureCharacteristic != nullptr && pPressureCharacteristic->canRead()) {
    pressureValue = pPressureCharacteristic->readUInt32();
    newPressure = true;
    //Serial.print("Pressure: ");
    //Serial.println(pressureValue / 100.0);
  }

  // Read water activity characteristic
  BLERemoteCharacteristic* pWaterActivityCharacteristic = pRemoteService->getCharacteristic(BLEUUID(WATER_ACTIVITY_UUID));
  if (pWaterActivityCharacteristic != nullptr && pWaterActivityCharacteristic->canRead()) {
    waterActivity = pWaterActivityCharacteristic->readUInt8();
    newWaterActivity = true;
    //Serial.print("Water Activity: ");
    //Serial.println(waterActivity ? "Active" : "Calm");
  }

  // Read direction characteristic
  BLERemoteCharacteristic* pDirectionCharacteristic = pRemoteService->getCharacteristic(BLEUUID(DIRECTION_UUID));
  if (pDirectionCharacteristic != nullptr && pDirectionCharacteristic->canRead()) {
    String direction = pDirectionCharacteristic->readValue().c_str();
    //Serial.print("Direction: ");
    //Serial.println(direction);
  } else {
    Serial.println("Direction characteristic not found or cannot read.");
  }

  // Read heading characteristic
  BLERemoteCharacteristic* pHeadingCharacteristic = pRemoteService->getCharacteristic(BLEUUID(HEADING_UUID));
  if (pHeadingCharacteristic != nullptr && pHeadingCharacteristic->canRead()) {
    uint16_t headingValue = pHeadingCharacteristic->readUInt16();  // Read as 16-bit integer
    float heading = headingValue / 100.0;                          // Convert to float
    //Serial.print("Heading (degrees): ");
    //Serial.println(heading);
  } else {
    Serial.println("Heading characteristic not found or cannot read.");
  }
}

/**
 * @brief Display the current menu on the OLED screen.
 * This function updates the OLED display with the current menu state and options.
 */
void displayMenu() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Display menu options based on the current menu state
  switch (currentMenuState) {
    case MAIN_MENU:
      display.println("Main Menu:");
      display.println("  1. Fish Predict");
      display.println("  2. Suggest Spots");
      break;
    case FISH_PREDICT:
      display.println("Fish Prediction:");
      display.println("  Press * to return");
      break;
    case LOCATION_RECOMMEND:
      display.println("Suggest Spots:");
      display.println("  Press * to return");
      break;
  }

  display.display(); // Refresh the OLED screen
}

/**
 * @brief Calculate the predicted bite rate of fish.
 * This function uses environmental variables to estimate the fish bite rate
 * in a given location using decision tree logic.
 * 
 * @param temperature Current temperature in °C
 * @param pressure Atmospheric pressure in hPa
 * @param waterActivity Water activity indicator (0 or 1)
 * @param latitude Latitude of the location
 * @param longitude Longitude of the location
 * @return Predicted fish bite rate
 */
float calculateBiteRate(float temperature, float pressure, float waterActivity, float latitude, float longitude) {
  float fishPrediction = 0.0;

  // Decision tree logic to predict fish bite rate
  if (temperature <= 20.11) {
    if (latitude <= 34.01) {
      if (waterActivity <= 0.50) {
        fishPrediction = 2.42;
      } else {
        if (latitude <= 34.01) {
          fishPrediction = 0.00;
        } else {
          fishPrediction = 0.79;
        }
      }
    } else {
      if (pressure <= 1007.80) {
        fishPrediction = 2.00;
      } else {
        fishPrediction = 2.83;
      }
    }
  } else {
    if (pressure <= 1011.90) {
      if (temperature <= 24.20) {
        fishPrediction = 3.67;
      } else {
        if (latitude <= 34.01) {
          fishPrediction = 0.40;
        } else {
          fishPrediction = 2.33;
        }
      }
    } else {
      if (latitude <= 34.01) {
        if (waterActivity <= 0.50) {
          fishPrediction = 4.60;
        } else {
          fishPrediction = 2.84;
        }
      } else {
        fishPrediction = 4.61;
      }
    }
  }
  
  return fishPrediction;
}

/**
 * @brief Recommend fishing spots based on the bite rate.
 * This function calculates and ranks the top 3 fishing spots based on environmental
 * variables and user-selected criteria.
 * 
 * @param temperature Current temperature in °C
 * @param pressure Atmospheric pressure in hPa
 * @param waterActivity Indicator of water activity (true for active, false for calm)
 */
void recommendFishingSpots(float temperature, float pressure, bool waterActivity) {
  Serial.println("Location Rec. Starts");
  // Skip recalculation if already computed
  if (!needsRecalculation) {
    displayTop3Spots();
    return;
  }

  // Array to store the highest bite rates
  float highestBiteRates[3] = { 0.0, 0.0, 0.0 };

  // Predefined list of GPS coordinates for evaluation
  float gpsList[][2] = {
      {34.009983, -118.370949}, {34.010011, -118.370909},
      {34.010037, -118.370852}, {34.010049, -118.370787},
      {34.010047, -118.370708}, {34.010039, -118.370624},
      {34.010010, -118.370571}, {34.009972, -118.370529},
      {34.009927, -118.370498}, {34.009884, -118.370466},
      {34.009847, -118.370460}, {34.009798, -118.370453},
      {34.009757, -118.370408}, {34.009746, -118.370347},
      {34.009767, -118.370252}, {34.009626, -118.370251},
      {34.009571, -118.370226}, {34.009522, -118.370214},
      {34.009468, -118.370214}, {34.009409, -118.370226},
      {34.009338, -118.370229}, {34.009273, -118.370216},
      {34.009194, -118.370169}, {34.009132, -118.370129},
      {34.009047, -118.370118}, {34.008995, -118.370185},
      {34.008946, -118.370241}, {34.008909, -118.370585},
      {34.008935, -118.370671}, {34.008983, -118.370737},
      {34.009030, -118.370795}, {34.009069, -118.370842},
      {34.009126, -118.370894}, {34.009194, -118.370913},
      {34.009246, -118.370913}, {34.009303, -118.370910},
      {34.009351, -118.370900}
  };

  int gpsListSize = sizeof(gpsList) / sizeof(gpsList[0]);

  // Evaluate each GPS location for bite rate
  for (int i = 0; i < gpsListSize; i++) {
    float testLatitude = gpsList[i][0];
    float testLongitude = gpsList[i][1];

    // Calculate the bite rate at the given coordinates
    float biteRate = calculateBiteRate(temperature, pressure, waterActivity, testLatitude, testLongitude);

    // Update the top 3 locations if the current location is better
    for (int j = 0; j < 3; j++) {
      if (biteRate > highestBiteRates[j]) {
        // Shift lower-ranked locations down
        for (int k = 2; k > j; k--) {
          highestBiteRates[k] = highestBiteRates[k - 1];
          highestLocations[k][0] = highestLocations[k - 1][0];
          highestLocations[k][1] = highestLocations[k - 1][1];
        }
        // Update the current top location
        highestBiteRates[j] = biteRate;
        highestLocations[j][0] = testLatitude;
        highestLocations[j][1] = testLongitude;
        break;
      }
    }
  }

  // Display the menu to select one of the top spots
  display.println("Select Spot:");
  display.println("1. Spot 1");
  display.println("2. Spot 2");
  display.println("3. Spot 3");
  display.display();

  // Handle user input for selecting a spot
  if (buttonK1Pressed) {
    buttonK1Pressed = false;  // Reset flag
    targetLatitude = highestLocations[0][0];
    targetLongitude = highestLocations[0][1];
    currentMenuState = NAVIGATION; // Switch to navigation mode
  } else if (buttonK2Pressed) {
    buttonK2Pressed = false;
    targetLatitude = highestLocations[1][0];
    targetLongitude = highestLocations[1][1];
    currentMenuState = NAVIGATION;
  } else if (buttonK3Pressed) {
    buttonK3Pressed = false;
    targetLatitude = highestLocations[2][0];
    targetLongitude = highestLocations[2][1];
    currentMenuState = NAVIGATION;
  }

  // Mark recalculation as complete and display the results
  needsRecalculation = false;
  displayTop3Spots();
  Serial.println("Location Rec. Ends");
}

/**
 * @brief Display the top 3 recommended fishing spots on the OLED screen.
 */
void displayTop3Spots() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  //display.println("Location Suggested:");
  display.println("Top 3 Recommendation:");
  display.println();

  // Display each top spot with its latitude and longitude
  for (int i = 0; i < 3; i++) {
    display.print(i + 1);
    display.print(": Lat: ");
    display.print(highestLocations[i][0], 6);
    display.println(",");
    display.print("  Lon: ");
    display.println(highestLocations[i][1], 6);
  }

  //display.println("Press * to return");
  display.display();
}

/**
 * @brief Navigate to the target location.
 * Calculates the distance, direction, and turn angle to reach the target location.
 * 
 * @param currentLat Current latitude of the user
 * @param currentLon Current longitude of the user
 * @param heading Current heading direction of the user
 * @param targetLat Target latitude
 * @param targetLon Target longitude
 */
void navigateToTarget(float currentLat, float currentLon, float heading, float targetLat, float targetLon) {
  // Calculate the distance to the target
  float distance = TinyGPSPlus::distanceBetween(currentLat, currentLon, targetLat, targetLon);

  // Calculate the angle to the target
  float angleToTarget = TinyGPSPlus::courseTo(currentLat, currentLon, targetLat, targetLon);

  // Print current and target coordinates for debugging
  Serial.print("Current Location: Lat = ");
  Serial.print(currentLat, 6);
  Serial.print(", Lon = ");
  Serial.println(currentLon, 6);

  Serial.print("Target Location: Lat = ");
  Serial.print(targetLatitude, 6);
  Serial.print(", Lon = ");
  Serial.println(targetLongitude, 6);

  // Calculate the relative direction to turn
  float relativeHeading = angleToTarget - heading;
  if (relativeHeading < 0) relativeHeading += 360.0;
  if (relativeHeading > 360.0) relativeHeading -= 360.0;

  // Clear the display and show navigation information
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Navigation Info:");
  display.println();
  display.print("Target Dis: ");
  display.print(distance, 2);
  display.println(" m");

  display.print("Target Dir: ");
  display.print(angleToTarget, 2);
  display.println(" de");
  display.println();

  display.print("Cur Dir: ");
  display.print(heading, 2);
  display.println(" deg");

  display.print("Turn ");
  if (relativeHeading < 180) {
    display.print("Right ");
    display.print(relativeHeading, 2);
    display.println(" deg");
  } else {
    display.print("Left ");
    display.print(360 - relativeHeading, 2);
    display.println(" deg");
  }

  //display.println("Press * to return");
  display.display();
}

/**
 * @brief Interrupt handler for Button K1.
 */
void IRAM_ATTR handleButtonK1() {
  buttonK1Pressed = true;
}

/**
 * @brief Interrupt handler for Button K2.
 */
void IRAM_ATTR handleButtonK2() {
  buttonK2Pressed = true;
}

/**
 * @brief Interrupt handler for Button K3.
 */
void IRAM_ATTR handleButtonK3() {
  buttonK3Pressed = true;
}

/**
 * @brief Interrupt handler for Button K4.
 */
void IRAM_ATTR handleButtonK4() {
  buttonK4Pressed = true;
}


 /**
 * @brief Setup function to initialize the system.
 * This function configures the display, buttons, GPS, BLE, and connects to the peripheral.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Central ESP32 to connect to Nano 33 BLE...");

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Halt execution if display initialization fails
  }
  display.clearDisplay();
  display.display();

  // Configure button pins with pull-up resistors
  pinMode(BUTTON_K1, INPUT_PULLUP);
  pinMode(BUTTON_K2, INPUT_PULLUP);
  pinMode(BUTTON_K3, INPUT_PULLUP);
  pinMode(BUTTON_K4, INPUT_PULLUP);

  // Attach interrupt handlers to buttons
  attachInterrupt(digitalPinToInterrupt(BUTTON_K1), handleButtonK1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_K2), handleButtonK2, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_K3), handleButtonK3, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_K4), handleButtonK4, FALLING);

  // Initialize BLE and start scanning
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30, false);

  // Initialize GPS serial communication
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("GPS Serial started at 9600 baud rate");

  // Attempt to connect to BLE peripheral
  connectToPeripheral();

  // Display the main menu
  displayMenu();
}

/**
 * @brief Main loop function to handle menu navigation and system logic.
 */
void loop() {
  // Check for GPS data and decode it
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // Handle the main menu
  if (currentMenuState == MAIN_MENU) {
    if (buttonK1Pressed) {
      buttonK1Pressed = false; // Reset flag
      currentMenuState = FISH_PREDICT; // Switch to fish prediction menu
      displayMenu();
    } else if (buttonK2Pressed) {
      buttonK2Pressed = false; // Reset flag
      currentMenuState = LOCATION_RECOMMEND; // Switch to location recommendation menu
      displayMenu();
    }
    return; // Exit to avoid executing further logic
  }

  // Handle fish prediction menu
  if (currentMenuState == FISH_PREDICT) {
    Serial.println("Perdiction Starts");
    if (connected && pRemoteService != nullptr) {
      // Read sensor characteristics
      readCharacteristics(pRemoteService);

      float temperature = temperatureValue / 100.0;
      float pressure = pressureValue / 100.0;
      float latitude = gps.location.isValid() ? gps.location.lat() : 0.0;
      float longitude = gps.location.isValid() ? gps.location.lng() : 0.0;

      // Calculate fish bite rate
      float fishPrediction = calculateBiteRate(temperature, pressure, waterActivity, latitude, longitude);

      // Display prediction on the OLED
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println("Fish Prediction:");
      display.print("Prediction: ");
      display.println(fishPrediction);
      display.println("Press * to return");
      display.display();
      Serial.println("Perdiction Ends");
    } else {
      Serial.println("Disconnected. Reconnecting...");
      connectToPeripheral();
    }

    // Return to main menu if the back button is pressed
    if (buttonK4Pressed) {
      buttonK4Pressed = false;
      currentMenuState = MAIN_MENU;
      displayMenu();
    }
    return;
  }

  // Handle location recommendation menu
  if (currentMenuState == LOCATION_RECOMMEND) {
    if (needsRecalculation) {
      // Recalculate top fishing spots
      float temperature = temperatureValue / 100.0;
      float pressure = pressureValue / 100.0;
      recommendFishingSpots(temperature, pressure, waterActivity);
      needsRecalculation = false;
    } else {
      // Display pre-calculated spots
      displayTop3Spots();
    }

    // Handle navigation button logic
    if (buttonK1Pressed) {
      buttonK1Pressed = false; // Reset flag
      targetLatitude = highestLocations[0][0];
      targetLongitude = highestLocations[0][1];
      currentMenuState = NAVIGATION; // Switch to navigation menu
      displayMenu();
    } else if (buttonK2Pressed) {
      buttonK2Pressed = false; // Reset flag
      targetLatitude = highestLocations[1][0];
      targetLongitude = highestLocations[1][1];
      currentMenuState = NAVIGATION;
      displayMenu();
    } else if (buttonK3Pressed) {
      buttonK3Pressed = false; // Reset flag
      targetLatitude = highestLocations[2][0];
      targetLongitude = highestLocations[2][1];
      currentMenuState = NAVIGATION;
      displayMenu();
    }

    // Return to main menu if the back button is pressed
    if (buttonK4Pressed) {
      buttonK4Pressed = false;
      currentMenuState = MAIN_MENU;
      needsRecalculation = true; // Mark for recalculation
      displayMenu();
    }
    return;
  }

  // Handle navigation menu
  if (currentMenuState == NAVIGATION) {
    // Get the current location from GPS
    float currentLat = gps.location.isValid() ? gps.location.lat() : 0.0;
    float currentLon = gps.location.isValid() ? gps.location.lng() : 0.0;
    uint16_t headingValue = 0;

    // Read heading direction from BLE
    if (connected && pRemoteService != nullptr) {
      BLERemoteCharacteristic* pHeadingCharacteristic = pRemoteService->getCharacteristic(BLEUUID(HEADING_UUID));
      if (pHeadingCharacteristic != nullptr && pHeadingCharacteristic->canRead()) {
        headingValue = pHeadingCharacteristic->readUInt16();
      }
    }
    float heading = headingValue / 100.0; // Convert to float

    // Display navigation information
    navigateToTarget(currentLat, currentLon, heading, targetLatitude, targetLongitude);

    // Return to main menu if the back button is pressed
    if (buttonK4Pressed) {
      buttonK4Pressed = false;
      currentMenuState = MAIN_MENU;
      needsRecalculation = true; // Mark for recalculation
      displayMenu();
    }
    return;
  }
}
