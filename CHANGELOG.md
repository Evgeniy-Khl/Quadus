# Changelog

All notable changes to the **Quadus Greenhouse Controller** project will be documented in this file.

## [2026-06-19]

### Added
- **Telegram Bot Integration**:
    - Created [TelegramBot.h](file:///E:/!PROJECTS/ESP/2025/Quadus/include/TelegramBot.h) and [TelegramBot.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/TelegramBot.cpp) implementing the Telegram API notification client using secure HTTP POST with auto-recovery and bypassed certificate verification (resilient against Telegram SSL rotations).
    - Added automatic message dispatch inside `Logger::log` in [Logger.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/Logger.cpp) for critical alarms (sensor loss, freeze, out-of-range thresholds) and their corresponding recovery events.
    - Added support for configuring `botToken` and `chatID` directly from the Web settings page [setup.html](file:///E:/!PROJECTS/ESP/2025/Quadus/data/setup.html).
    - Integrated loading/saving of Telegram credentials into `setpoint.json` in [procedure.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/procedure.cpp) and handled HTTP API endpoints in [server.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/server.cpp).
    - Implemented a mute logic using `tmrTelegramOff` inside [main.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/main.cpp) to silence Telegram notifications while the user is actively viewing the web control panel.
- **I2C Bus Recovery & Stability**:
    - Implemented `recoverI2C()` in [main.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/main.cpp) to release SDA lines hung by slaves by generating clock pulses and manual START/STOP sequences.
    - Improved `writePCF8574()` with automatic I2C recovery triggers, error counting, and retry. Added ESP8266 reboot if the bus fails to recover after 20 consecutive attempts.
    - Added definition for `readPCF8574()` in [main.cpp](file:///E:/!PROJECTS/ESP/2025/Quadus/src/main.cpp) using the same recovery logic.

## [2026-06-12]

### Added
- **Web Interface (UI/UX)**:
    - `data/setup.html`: Expanded relay configuration to support 3 channels (Relay 1, 2, 3).
    - `data/setup.html`: Added operating mode selection for each relay (Auxiliary/Thermostat, Timer Always, Timer No Light, Timer With Light).
    - `data/index.html`: Dynamic status labels (changed "Timer" to "Relay").
    - `data/index.html`: Dynamic icons (⏰ for timer modes, 🛠️ for auxiliary mode).
    - `data/index.html`: Improved status display logic (compact state for auxiliary, full countdown for timers).
- **Core Logic**:
    - `src/main.cpp`: Implemented per-second polling for Relay 1 and Relay 2 when in auxiliary mode for faster sensor response.
- **Server**:
    - `src/server.cpp`: Added relay mode indices (`mR1`, `mR2`, `mR3`) to the `/getvalues` JSON response.

### Fixed
- **System Time & RTC**:
    - `src/main.cpp`: Corrected initialization order. Timezone (TZ) and RTC synchronization now happen at the very start of `setup()`.
    - `src/main.cpp`: System clock is now synchronized with RTC immediately, ensuring correct local time/DST from the first log entry.
    - `src/procedure.cpp`: Refactored `initEnvironment()` to remove redundant RTC initialization.
- **Logging**:
    - `src/Logger.cpp`: Fixed `getTimestamp()` to correctly handle the year 2000 onwards (`tm_year >= 100`).
    - Fixed "zero time" `[00.00 00:00:00]` issue in initial startup logs.
- **Stability**:
    - `src/main.cpp`: Removed a corrupted duplicate function definition that caused a compilation error.
- **Display**:
    - `src/displLCD.cpp`: Fixed inverted logic for "Heating/Cooling" and "Humidifying/Dehumidifying" labels on the LCD screen.

## [Unreleased]

### Added
- `HeaterMode` and `HumidiMode` enumerations in `include/main.h` for better code readability.
- Real-time alarm color highlighting in `index.html` (orange pulse animation) calculated dynamically on the client side based on temperature and humidity thresholds.
- Instant loading popups ('Збереження параметрів...', 'Завантаження головної...') in `setup.html` to provide immediate visual feedback and prevent browser freeze during configuration saves.

### Changed
- **Relay Refactoring**: Repurposed `modeRelay1` and `modeRelay2` value `0` for Emergency modes (complementary Climate/Humidity control).
- **Relay Mode Shifting**: Timer-based relay modes (Always, Light OFF, Light ON) for all relays are now mapped to values `1, 2, 3` respectively.
- **Logic Refactoring**: Updated `LogicManager::checkDeviceState` to accept explicit hysteresis for improved reliability.
- **Settings Refactoring**: `modeHeater` and `modeHumidi` now explicitly control the operating mode (0 = Heating/Humidifying, 1 = Cooling/Dehumidifying) instead of light-based relay permissions.
- **Logic Refactoring**: Updated `LogicManager::checkDeviceState` and `processAlarm` to use explicit mode logic, removing previous light-dependent suppression for climate channels.
- **UI Update**: Simplified `setup.html` and keypad interface to toggle between the two new operating modes, removing legacy "Always/Only with light/Only without light" options for climate.
- Transitioned manual relay control from a global `isManualOverride` flag to individual operating modes for each of the 6 relays (Auto, Always ON, Always OFF) using `dataOut`.
- Redesigned `switch.html` with a 3-position radio button interface for each relay, featuring high-contrast active states.
- Optimized manual relay API (`/set_relay`) to apply physical states instantaneously, bypassing the 1-minute logic loop delay.
- Reduced blocking delays in `procedure.cpp` (`saveSetPoint`) from 3000ms to 300ms, significantly speeding up the `/seteeprom` API response.

### Fixed
- Resolved `LoadStoreAlignmentCause` (`Exception 9`) hardware crash on ESP8266 that occurred when saving settings or accessing endpoints, caused by unaligned memory access to the packed `Settings` struct.
- Fixed `net::ERR_CONNECTION_RESET` timeouts by sending the HTTP 200 OK response to the browser before executing heavy, blocking LittleFS write operations during EEPROM save.
- Fixed JavaScript type coercion bugs in `index.html` that prevented alarm indicators from activating for Sensor 2 (T2 / Humidity).
- Restored `processLighting` and `processIrrigation` functions to the 1-minute execution loop to prevent countdown timers from ticking 60 times too fast.

## [2026-06-04]

### Added
- Persistent event logging system using LittleFS.
- New `Logger` class with automatic size rotation.
- Web-based log viewer at `/view_logs` and API at `/logs`.
- Logging of critical events: reboots, sensor discovery, manual overrides, and climate errors.
- Manual relay control feature via `switch.html`.
- `hyst0` and `hyst1` settings to control the gap between turning ON and OFF, preventing relay chatter.
- Simultaneous support for DHT22 and multiple DS18B20 sensors on the same data pin.
- Logic to automatically prioritize DHT22 for primary climate control while using DS18B20 for supplemental monitoring.
- 0.1°C precision for sensor measurements and climate control.
- Fixed-point representation (value * 10) for temperatures and humidity in `Ds`, `Settings`, and `TableForOneHour` structures.
- Hardware Watchdog Timer (WDT) implementation in `main.cpp` with a 5-second timeout to improve system reliability.
- `LogicManager.cpp` and `LogicManager.h`: New class to handle greenhouse control logic (climate, irrigation, lighting).
- `SystemState` structure in `main.h`: Encapsulates global operational variables (sensor data, relay timers, flags).
- Compatibility macros in `main.h` to maintain support for legacy variable names while using the new `state` object.

### Changed
- Migrated settings API from GET (URL parameters) to POST with JSON payloads. This improves reliability and supports more complex data structures.
- Updated `data/setup.html` frontend to send settings as JSON via `fetch()`.
- Added support for configuring hysteresis thresholds from the web interface.
- Refactored time management to use the built-in ESP8266 `configTzTime()` mechanism.
- `programm.cpp`: Enhanced `eepromWrBuff` with page boundary safety logic for AT24C32 (32-byte pages) to prevent data corruption during multi-byte writes.
- `main.cpp`, `sensors.cpp`, `procedure.cpp`, `programm.cpp`, `server.cpp`: Translated all source code comments and technical documentation to English.
- `main.cpp`: Refactored `setup()` and `loop()` to use `LogicManager` methods. Cleaned up redundant logic.
- `procedure.cpp` & `procedure.h`: Removed control logic functions now managed by `LogicManager`.
- `my_settings.h`: Cleaned up global variable declarations to align with the new architecture.
- `IMPROVEMENTS.md`: Updated roadmap with completed architecture tasks.

### Fixed
- Improved project modularity by separating hardware interaction from business logic.

  ### 2. Реализация графиков в проекте  Quadus

  Поскольку архитектура проекта  Quadus  очень близка к  Climat_ESP8266 , я полностью перенес и адаптировал этот
  функционал в  Quadus . Проект успешно скомпилирован через PlatformIO без предупреждений и ошибок.

  #### Что было сделано:

  1. Настройка распределения памяти EEPROM в main.h:
  В отличие от инкубатора, в  Quadus  с адреса  0x000  до  0x600  хранятся суточные/часовые программы. Поэтому данные
  суточного графика мы разместили сразу после них:
    #define DAILY_DATA_START      0x700  // Память графиков (после 0x600)
    #define DAILY_DATA_REC_SIZE   6      // t1 (2б) + t2 (2б) + rh (2б)
    #define DAILY_DATA_MAX_REC    288    // 288 точек в сутки (каждые 5 минут)

  2. Добавление хелперов работы с EEPROM в AT24C32.cpp и AT24C32.h:
  Реализованы функции  eepromWriteInt16  и  eepromReadInt16  для работы с 16-битными сырыми показаниями датчиков.
  3. Логика записи и архивации ( saveDailyData ):
      • Создан заголовочный файл saveDailyData.h.
      • Создан файл реализации saveDailyData.cpp.
      • Внедрен алгоритм  checkAndManageSpace() . Если свободное место в LittleFS падает ниже 25 КБ, система находит
      самый старый архивный день по файлам  day_*_stats.json  и автоматически удаляет его, предотвращая переполнение
      памяти.
  4. Логирование в основном цикле в main.cpp:
  Внутри минутного обработчика:
      • Каждые 5 минут текущее состояние датчиков ( ds[0].pvT ,  ds[1].pvT  и  pvRH ) записывается по вычисленному
      суточному адресу в EEPROM.
      • При смене календарного дня (в 00:00 по часам реального времени DS3231) вчерашние данные сбрасываются в файл
      LittleFS вида  /day_X_graph.json  (где  X  — число месяца от 1 до 31), после чего EEPROM очищается для нового
      дня. Это создает 31-дневный цикличный архив, который автоматически перезаписывается каждый месяц.
  5. Добавление веб-обработчиков и роутинга:
      • В server.h добавлены объявления новых веб-методов.
      • В server.cpp добавлены реализации методов  handleArchiveList() ,  handleShowData() ,
      handleCurrentData() ,  handleGetGraph() ,  handleGetCurrentGraph() , а также хелпер  sendPageHeader()  с
      адаптивными стилями (тёмная тема, поддержка мобильных устройств) для генерации страниц архива.
      • В wifiManag.cpp зарегистрированы роуты на эти методы.
  6. Интеграция во фронтенд index.html:
      • В заголовок добавлен импорт Chart.js.
      • В разметку встроен контейнер с Canvas  mainChart .
      • В блоке кнопок добавлена кнопка «Архів», ведущая на  /archive .
      • Добавлен асинхронный JS-скрипт  fetchGraphData() , который забирает текущие точки по эндпоинту
      /get_current_graph  и раз в минуту перерисовывает график на главном экране.

Я реализовал предложенный вами функционал. Теперь архивы графиков хранятся с учетом месяца, а журнал событий
  (системный лог) разделен на посуточные файлы.

  ### Что конкретно было сделано:

  #### 1. Новый формат имен файлов графиков и статистики

  Имена файлов теперь содержат день и месяц в формате  DD_MM :

  •  /day_DD_MM_graph.json  — точки графиков (например,  /day_20_06_graph.json ).
  •  /day_DD_MM_stats.json  — минимальные/максимальные/средние значения за день.
  • Изменения внесены в saveDailyData.cpp и main.cpp.

  #### 2. Посуточные лог-файлы

  Вместо одного большого файла  /system.log  система теперь автоматически пишет лог за текущий день в отдельный файл:

  • Шаблон:  /day_DD_MM_log.txt  (например,  /day_20_06_log.txt ).
  • Метод  getLogFilename()  динамически определяет текущее имя файла на основе даты из часов DS3231 ( timeinfo ).
  Если время не синхронизировано, пишется в резервный файл  /day_00_00_log.txt .
  • Изменения внесены в Logger.h и Logger.cpp.

  #### 3. Обновление веб-интерфейса архивов

  В server.cpp переписан вывод страницы архивов:

  • Кнопки просмотра: на странице  /archive  для каждого найденного дня выводятся две кнопки: «Графік» (синяя) и
  «Логи» (зеленая). Вы можете просмотреть как график датчиков, так и лог событий за этот конкретный день.
  • Сортировка по дате: список дней теперь сортируется хронологически сверху вниз (более новые даты вверху). Месяц
  корректно учитывается.
  • Чтение архивов: эндпоинт  /logs  теперь принимает параметр  day  (например,  /logs?day=20_06 ). Если параметр
  передан, он отдает файл лога за этот день. Если не передан — отдает текущий лог.

  #### 4. Обновление JS-логики в logs.html

  В файле logs.html доработан JavaScript:

  • При переходе по ссылке «Логи» из архива (на  /view_logs?day=20_06 ) скрипт автоматически подставляет параметр в
  AJAX-запрос к контроллеру.
  • Заголовок страницы меняется на «Журнал подій за 20.06».
  • Кнопка «Очистити журнал» в режиме просмотра архивных логов автоматически скрывается, чтобы случайно не стереть
  прошлую историю.

  #### 5. Хронологическое умное удаление при переполнении памяти

  В saveDailyData.cpp полностью исправлен баг на стыке месяцев. Расстояние во времени ( distance ) от текущей даты
  вычисляется математически, учитывая переход месяцев и лет. При нехватке места LittleFS удалит самый старый день со
  всеми его файлами (график, статистика и лог-файл за этот день).

Опасное форматирование памяти  LittleFS.format()  при нажатии комбинации  KEY_6_5_4  успешно заменено на безопасный
  сброс к заводским настройкам ( safeFactoryReset ).

  ### Как теперь работает сброс ( KEY_6_5_4 ):

  Когда пользователь зажимает кнопки S7 + S3 + S6 (комбинация  KEY_6_5_4 ):

  1. Экран предупреждения: На дисплей выводится надпись  FACTORY RESET... .
  2. Очистка данных в EEPROM: Вызывается  clearEEPROM() , сбрасывающая суточный график в ноль.
  3. Безопасная очистка LittleFS:
  Программа сканирует файловую систему и удаляет только пользовательские файлы:
      • Историю графиков ( day_XX_XX_graph.json )
      • Файлы статистики ( day_XX_XX_stats.json )
      • Посуточные файлы логов ( day_XX_XX_log.txt )
      • Общий системный лог ( system.log )
      • При этом служебные HTML-файлы ( index.html ,  setup.html ,  logs.html ,  switch.html ) остаются нетронутыми.
  4. Сброс настроек:
  Вызывается функция  reset() , которая возвращает все уставки прибора к hardcoded-дефолтам, сохраняет их в
  setpoint.json , выводит на экран надпись  Config restored , дважды пищит и перезапускает ESP.
  ------
  ### Какие файлы были изменены:

  1. procedure.h — объявлена функция  safeFactoryReset() .
  2. procedure.cpp — написана реализация функции  safeFactoryReset() .
  3. keypad.cpp — в  case KEY_6_5_4  вызов  LittleFS.format()  заменен на  safeFactoryReset() .
