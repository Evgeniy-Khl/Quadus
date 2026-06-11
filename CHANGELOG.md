# Changelog

All notable changes to the **Quadus Greenhouse Controller** project will be documented in this file.

## [Unreleased]

### Added
- `HeaterMode` and `HumidiMode` enumerations in `include/main.h` for better code readability.
- Real-time alarm color highlighting in `index.html` (orange pulse animation) calculated dynamically on the client side based on temperature and humidity thresholds.
- Instant loading popups ('Збереження параметрів...', 'Завантаження головної...') in `setup.html` to provide immediate visual feedback and prevent browser freeze during configuration saves.

### Changed
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
