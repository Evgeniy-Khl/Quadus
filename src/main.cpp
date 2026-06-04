#include "main.h"
#include "my_settings.h"

SystemState state;

ESP8266WebServer server(80);

RTC_DS3231 rtc;                             // Create RTC object for DS3231

DHT dht(ONE_WIRE_BUS_PIN, DHT22);
OneWire oneWire(ONE_WIRE_BUS_PIN);          // Create OneWire instance for 1-Wire bus interaction
DallasTemperature sensors(&oneWire);        // Pass oneWire reference to DallasTemperature constructor
DeviceAddress sensorAddresses[MAX_DEVICE];  // Array for unique sensor addresses

LiquidCrystal_I2C lcd(0x20, 16, 2);         // Set the LCD address to 0x27 for a 16 chars and 2 line display

byte writePCF8574(byte data);

TM1638 module(13, 14, 12);                  // Create module object for TM1638
void ledSet(void);

void setup(){
  ESP.wdtEnable(5000); // Enable hardware watchdog with 5-second timeout
  #ifdef DEBUG
    Serial.begin(115200);                   // Initialize serial for debugging
  #endif
  //--------------------------------- initialize the LCD -----------------------------------
  lcd.begin();  // Wire.begin() is called inside. Initialize I2C (SDA, SCL default for ESP8266 - GPIO4, GPIO5)

  uint8_t temp = writePCF8574(0xFF);    // Set all pins LOW (if used as outputs)

  // Turn on the backlight and print a message.
  lcd.backlight();
  lcd.print(version);
  if(temp){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERROR PCF8574");
    delay(3000);
  }
  delay(1000);
  //----------------------------------- MOUNTING FS ----------------------------------------
  MYDEBUG_PRINTLN("mounting FS...");
  bool lFS = LittleFS.begin();
  if(lFS) {
    MYDEBUG_PRINTLN("mounted file system");
    //--------------------- checkSetpoint ----------------------------------
    temp = checkSetpoint();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      myPrint(error_,sizeof(error_));// ERROR 
      lcd.print(temp);
      lcd.setCursor(0,1);
      lcd.print("setpoint.json");
      delay(2000);
    }
  } else {
    MYDEBUG_PRINTLN("failed to mount FS");
    lcd.clear();
    lcd.setCursor(0,0);
    myPrint(error_,sizeof(error_)); lcd.print("- FS");// ERROR - FS
    lcd.setCursor(0,1);
    myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));// not connected
    delay(3000);
  }
  //---------------------------- WiFiManager initialization -----------------------------------
  if(settings.special & 0x03) initWiFiManag();
  else MYDEBUG_PRINTLN("WiFi connection disabled! Continuing in offline mode.");
  initEnvironment();
  //----------------------- detect connected sensor type --------------------------------
  sensorType();
  lcd.clear();
  lcd.setCursor(0,0);
  myPrint(sensorsWord,sizeof(sensorsWord));
  lcd.setCursor(0,1);
  switch (detectedSensor){
    case SENSOR_DS18B20:
        lcd.print("DS18B20: "); lcd.print(numberOfDevices); lcd.print("pcs.");
      break;
    case SENSOR_DHT22:
        lcd.print("DHT module.");
      break;
    case UNKNOWN:
        myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); 
      break;
  }
  //------------------------------------------------------------------------------------------
  digitalWrite(BEEP_PIN, HIGH); // Turn off beeper
  pinMode(BEEP_PIN, OUTPUT);    // Set beeper pin as output for LED only
  
  delay(3000);  
  lcd.clear();
  displSwitch();
  portOut.value = 0xFF;
  if(RTCENABLE){
    logicManager.processIrrigation();
    time_t utc_time = rtc.now().unixtime();
    timeinfo = localtime(&utc_time);
    logicManager.processLighting();
  }
}

void loop(){
  ESP.wdtFeed(); // Feed the hardware watchdog
	long now = millis();
  server.handleClient(); // Handle incoming requests
  //-------------------------------------------- 10 mSec. --------------------------------------
  if(now - counter10 > 10){
    counter10 = now;

    if(beepOn) beepOn--; else digitalWrite(BEEP_PIN, HIGH);   // Turn off beeper

    keys = module.getButtons();
    if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}  // reset wait time if no button is held.
  }
  //-------------------------------------------- KEYPAD --------------------------------------
    if(now - counterWait > waitCheckKeyPad){
      counterWait = now;
      keys = module.getButtons();
      
      if(lastKey == keys && keys > 0){
        keyCount++;
        checkkey(keys);
        if(setupNum == 0) displSwitch(); else setupSwitch();
      } 
      else if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}
      else lastKey = keys;
    }
  //============================= NEW HALF-SECOND =================================
  if(now - counter1s > 500){
    counter1s = now;
    halfSecond++; 
    if(resetDispl){
      if(--resetDispl == 0) {
        if(setupNum) saveSetPoint();
        setupNum = 0; // return to main display
        lcd.clear(); 
        displSwitch();
      }
    }
    if(halfSecond & 2){//-------- NEW SECOND -----------------------
      countSeconds++; errorsFlag.value = 0;
      sensorCheck();
      
      logicManager.processClimate();
      logicManager.processAlarms();
      logicManager.updateStatusLeds();

      if(setupNum == 0) displSwitch(); else setupSwitch();
    } //---------------------------------------------------------------
    if(halfSecond > 119){//------ NEW MINUTE ------------------------
      halfSecond = 0; countSeconds = 0; minutes++;
      if(RTCENABLE){
        time_t utc_time = rtc.now().unixtime();
        timeinfo = localtime(&utc_time);
        
        logicManager.processLighting();
        logicManager.processIrrigation();

        #ifdef DEBUG
        MYDEBUG_PRINTLN("processLighting():");
        printBinary(portOut.value);
        #endif

        // Converted local time for our timezone
        MYDEBUG_PRINT("Converted Local Time  (EET/EEST): ");
        DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                      timeinfo->tm_mday, timeinfo->tm_hour,
                      timeinfo->tm_min, timeinfo->tm_sec);
        uint16_t heapSize = ESP.getFreeHeap();    // Memory check
        DEBUG_PRINTF("Free heap size: %d\n", heapSize);
      }
      //---------------------------- NEW HOUR ----------------------------------
      if(++minutes > 59){
        minutes = 0;
        if(RTCENABLE){
          if(WIFIENABLE){
            // ------------- Daily synchronization logic --------------
            // Sync with RTC at midnight (00:00)
            if (timeinfo->tm_mday != lastSyncDay && timeinfo->tm_hour == 0) { 
              MYDEBUG_PRINTLN("\nMidnight sync: Updating RTC from NTP...");
              configTzTime(tzInfo, ntpServer); // Ensure background sync is active
              
              // Wait a bit for NTP to update system time if needed, 
              // but don't block heavily as it's a background process in ESP8266 core
              time_t now_t = time(nullptr);
              if (now_t > 1000000000) { // If system time is valid
                rtc.adjust(DateTime(now_t));
                lastSyncDay = timeinfo->tm_mday;
                MYDEBUG_PRINTLN("RTC updated successfully.");
              }
            }
            MYDEBUG_PRINT("Update Local Time  (EET/EEST): ");
            DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                      timeinfo->tm_mday, timeinfo->tm_hour,
                      timeinfo->tm_min, timeinfo->tm_sec);
          }
        }
      } // ------------------------- hour ----------------------------
    } //--------------------------- minute --------------------------
  } //-------------------------- half-second ------------------------
}//============================================== END LOOP =============================================

// Function to write byte to PCF8574
byte writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error != 0) {
    MYDEBUG_PRINT("\nError writing to PCF8574. Error code: ");
    MYDEBUG_PRINTLN(error);
  }
  return error;
}
