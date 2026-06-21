#include "main.h"
#include "my_settings.h"

SystemState sysState;

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

  // --- Initialize Timezone and Sync System Time from RTC immediately ---
  setenv("TZ", tzInfo, 1);
  tzset();

  if (rtc.begin()) {
    RTCENABLE = true;
    time_t utc_time = rtc.now().unixtime();
    timeinfo = localtime(&utc_time);
    rtcTimeSet = true;
    
    // Set system time from RTC for core time functions
    struct timeval tv = { .tv_sec = utc_time };
    settimeofday(&tv, nullptr);
  } else {
    RTCENABLE = false;
  }

  //--------------------------------- initialize the LCD -----------------------------------
  lcd.begin();  // Wire.begin() is called inside. Initialize I2C (SDA, SCL default for ESP8266 - GPIO4, GPIO5)
  uint8_t temp = writePCF8574(0xFF);    // Set all pins LOW (if used as outputs)

  // Turn on the backlight and print a message.
  lcd.backlight();
  myPrint(quadus_, sizeof(quadus_));
  lcd.print(version);
  if(temp){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERROR PCF8574");
    delay(3000);
  }
  delay(1000);
  sysLogger.log(String(getMsg(MSG_STARTUP)) + version);
  //----------------------------------- MOUNTING FS ----------------------------------------
  MYDEBUG_PRINTLN("mounting FS...");
  bool lFS = LittleFS.begin();
  if(lFS) {
    MYDEBUG_PRINTLN("mounted file system");
    listFilesAndSizes();
    //--------------------- checkSetpoint ?????????? ----------------------------------
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
  
  if (hasDHT22) {
      lcd.print("DHT22 ");
      sysLogger.log(getMsg(MSG_DHT22_FOUND));
  }
  if (numberOfDS18 > 0) {
      lcd.print("DS18B20: "); lcd.print(numberOfDS18);
      sysLogger.log(String(getMsg(MSG_DS18B20_FOUND)) + ": " + String(numberOfDS18));
      sensors.requestTemperatures();
  }
  if (!hasDHT22 && numberOfDS18 == 0) {
      myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); 
      sysLogger.log(getMsg(MSG_SENSORS_NONE));
  }

  //------------------------------------------------------------------------------------------
  digitalWrite(BEEP_PIN, HIGH); // Turn off beeper
  pinMode(BEEP_PIN, OUTPUT);    // Set beeper pin as output for LED only
  
  delay(3000);
  sensorCheck();
  displNum = 1;  
  lcd.clear();
  displSwitch();
  portOut.value = 0xFF;
  if(RTCENABLE){
    logicManager.processIrrigation();
    logicManager.processLighting();
  }
  #ifdef SIMULATION
    ds[0].pvT = 200;
    ds[0].pvErr = 0;
    ds[1].pvT = 160;
    ds[1].pvErr = 0;
  #endif
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
    if(halfSecond % 2 == 0){//-------- NEW SECOND -----------------------
      countSeconds++; 
      if (tmrTelegramOff > 0) {
        tmrTelegramOff--;
      }
      if(RTCENABLE){
        time_t utc_time = rtc.now().unixtime();
        timeinfo = localtime(&utc_time);
      }
      #ifndef SIMULATION  
        sensorCheck();                                                  // Опрос датчиков должен быть всегда
      #else
        #define MAXPOINT 20
        // В режиме отладки можно оставить симуляцию, если датчики не подключены
        if(HEATER == PCF_ON){
          if(++ds[0].pvErr > MAXPOINT) ds[0].pvErr = MAXPOINT;
        }  else {
          if(--ds[0].pvErr < -MAXPOINT) ds[0].pvErr = -MAXPOINT;
        }
        if(ds[0].pvErr > 0) ds[0].pvT++; else ds[0].pvT--;
        if(HUMIDI == PCF_ON) {
          if(++ds[1].pvErr > MAXPOINT) ds[1].pvErr = MAXPOINT;
        }  else {
          if(--ds[1].pvErr < -MAXPOINT) ds[1].pvErr = -MAXPOINT;
        }
        if(ds[1].pvErr > 0) ds[1].pvT++; else ds[1].pvT--;
      #endif
      logicManager.processClimate();
      
      // Fast response for auxiliary modes (thermostat/hygrostat)
      if ((settings.modeRelay1 & 0x03) == 0) logicManager.relaySwitch(1);
      if ((settings.modeRelay2 & 0x03) == 0) logicManager.relaySwitch(2);

      writePCF8574(portOut.value);

      logicManager.processAlarms();
      logicManager.updateStatusLeds();

      if(setupNum == 0) displSwitch(); else setupSwitch();
    } //---------------------------------------------------------------
    if(halfSecond > 119){//------ NEW MINUTE ------------------------
      halfSecond = 0; countSeconds = 0; minutes++;
      if(RTCENABLE){
        logicManager.processLighting();
        logicManager.processIrrigation();

        // Запись точек графиков каждые 5 минут
        if (timeinfo && timeinfo->tm_min % 5 == 0) {
            static int lastLoggedMinute = -1;
            if (timeinfo->tm_min != lastLoggedMinute) {
                lastLoggedMinute = timeinfo->tm_min;
                int period_of_day = (timeinfo->tm_hour * 60 + timeinfo->tm_min) / 5;
                int address = DAILY_DATA_START + period_of_day * DAILY_DATA_REC_SIZE;
                eepromWriteInt16(address, ds[0].pvT);
                eepromWriteInt16(address + 2, ds[1].pvT);
                eepromWriteInt16(address + 4, (int16_t)pvRH);
                
                #ifdef DEBUG
                DEBUG_PRINTF("Graph logged: period=%d, t1=%d, t2=%d, rh=%d\n", period_of_day, ds[0].pvT, ds[1].pvT, pvRH);
                #endif
            }
        }

        // Проверка смены суток для сохранения логов вчерашнего дня
        static int lastSavedDay = -1;
        static int lastSavedMonth = -1;
        if (timeinfo) {
            if (lastSavedDay == -1) {
                lastSavedDay = timeinfo->tm_mday;
                lastSavedMonth = timeinfo->tm_mon + 1;
            }
            if (timeinfo->tm_mday != lastSavedDay) {
                saveDailyDataToFile(lastSavedDay, lastSavedMonth);
                clearEEPROM();
                lastSavedDay = timeinfo->tm_mday;
                lastSavedMonth = timeinfo->tm_mon + 1;
            }
        }
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
                sysLogger.log(getMsg(MSG_RTC_SYNC));
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

// Function to recover the I2C bus if SDA or SCL hang
void recoverI2C() {
  static unsigned long lastRecoveryTime = 0;
  if (millis() - lastRecoveryTime < 200) return; // Limit recovery rate
  lastRecoveryTime = millis();

  MYDEBUG_PRINTLN("Attempting I2C bus recovery...");

  const uint8_t sda = 4; 
  const uint8_t scl = 5;

  // 1. Pre-configure pins to INPUT_PULLUP
  pinMode(sda, INPUT_PULLUP);
  pinMode(scl, INPUT_PULLUP);
  delay(1);

  // 2. If SDA or SCL are low, the bus is physically busy
  MYDEBUG_PRINT("Bus state before pulses: SDA=");
  MYDEBUG_PRINT(digitalRead(sda));
  MYDEBUG_PRINT(", SCL=");
  MYDEBUG_PRINTLN(digitalRead(scl));

  // 3. Generate up to 20 clock pulses on SCL to release SDA
  pinMode(scl, OUTPUT);
  for (int i = 0; i < 20; i++) {
    digitalWrite(scl, LOW);
    delayMicroseconds(20);
    digitalWrite(scl, HIGH);
    delayMicroseconds(20);
    if (digitalRead(sda) == HIGH && i > 8) {
       MYDEBUG_PRINT("SDA released after ");
       MYDEBUG_PRINT(i);
       MYDEBUG_PRINTLN(" pulses.");
       break;
    }
  }

  // 4. Generate START + STOP conditions manually
  pinMode(sda, OUTPUT);
  digitalWrite(sda, LOW);    // START
  delayMicroseconds(20);
  digitalWrite(scl, LOW);
  delayMicroseconds(20);
  digitalWrite(scl, HIGH);   // STOP setup
  delayMicroseconds(20);
  digitalWrite(sda, HIGH);   // STOP
  delayMicroseconds(20);

  // 5. Return pins to Wire control
  pinMode(sda, INPUT_PULLUP);
  pinMode(scl, INPUT_PULLUP);

  Wire.begin(sda, scl);
  Wire.setClock(100000); // 100 kHz for stability
  
  delay(10); // Wait for stabilization
  
  if (digitalRead(sda) == LOW || digitalRead(scl) == LOW) {
    MYDEBUG_PRINT("I2C recovery: FAILED. SDA=");
    MYDEBUG_PRINT(digitalRead(sda));
    MYDEBUG_PRINT(", SCL=");
    MYDEBUG_PRINTLN(digitalRead(scl));
  } else {
    MYDEBUG_PRINTLN("I2C recovery: SUCCESS");
  }
}

static uint8_t i2c_error_count = 0; // Consecutive error counter

// Function to write byte to PCF8574 with auto-recovery
byte writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  byte error = Wire.endTransmission();
  
  if(error) {
    i2c_error_count++;
    MYDEBUG_PRINT("\nError writing to PCF8574. Code: ");
    MYDEBUG_PRINT(error);
    MYDEBUG_PRINT(" | Fail count: ");
    MYDEBUG_PRINTLN(i2c_error_count);

    if (i2c_error_count >= 20) { // If 20 consecutive failures occur
      MYDEBUG_PRINTLN("CRITICAL I2C ERROR: Restarting ESP...");
      delay(1000);
      ESP.restart();
    }

    recoverI2C(); // Attempt soft bus recovery
    
    // Retry transmission
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(data);
    error = Wire.endTransmission();
    
    if (error == 0) {
      i2c_error_count = 0; // Reset count on success
    }
  } else {
    i2c_error_count = 0; // Reset count
  }
  return error;
}

// Function to read byte from PCF8574 with auto-recovery
byte readPCF8574() {
  uint8_t count = Wire.requestFrom((uint8_t)PCF8574_ADDRESS, (uint8_t)1);
  if (count > 0) {
    i2c_error_count = 0; // Reset count on success
    return Wire.read();
  } else {
    i2c_error_count++;
    MYDEBUG_PRINT("\nError reading from PCF8574. Fail count: ");
    MYDEBUG_PRINTLN(i2c_error_count);

    if (i2c_error_count >= 20) {
      MYDEBUG_PRINTLN("CRITICAL I2C ERROR: Restarting ESP...");
      delay(1000);
      ESP.restart();
    }

    recoverI2C(); // Attempt soft bus recovery
    return 0xFF; // Return default high
  }
}
