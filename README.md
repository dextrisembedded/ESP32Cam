
# ESP32 Runtime Wi-Fi Configuration & OTA Update POC

## Table of Contents
1. [Introduction](#introduction)
2. [Objectives](#objectives)
3. [Development Environment](#development-environment)
4. [Phase 1 – Basic Wi-Fi Connectivity](#phase-1--basic-wi-fi-connectivity)
5. [Phase 2 – OTA Firmware Update](#phase-2--ota-firmware-update)
6. [Phase 3 – Runtime Wi-Fi Configuration Evaluation](#phase-3--runtime-wi-fi-configuration-evaluation)
7. [Wi-Fi Credential Reconfiguration](#wi-fi-credential-reconfiguration)
8. [FreeRTOS Considerations](#freertos-considerations)
9. [Setup Instructions](#setup-instructions)
10. [Libraries Used](#libraries-used)
11. [Challenges Encountered](#challenges-encountered)
12. [Selection Criteria](#selection-criteria)
13. [Real-World Deployment Flow](#real-world-deployment-flow)
14. [Validation Evidence](#validation-evidence)
15. [Proposed Repository Structure](#proposed-repository-structure)
16. [Comparative Analysis](#comparative-analysis)
17. [Lessons Learned](#lessons-learned)
18. [Final Recommendation](#final-recommendation)
19. [Future Enhancements](#future-enhancements)
20. [Conclusion](#conclusion)


## 1. Introduction
This Proof of Concept (POC) evaluates different methods for configuring Wi-Fi credentials on an ESP32 device at runtime without modifying or recompiling the firmware. The study also explores OTA (Over-The-Air) firmware updates, BLE provisioning, and FreeRTOS task architecture for scalable, production-ready IoT deployments.



## 2. Objectives
* Enable runtime Wi-Fi configuration.
* Eliminate the need for hardcoded Wi-Fi credentials.
* Support Wi-Fi credential modification and recovery post-deployment.
* Evaluate multiple provisioning approaches (WiFiManager, Custom HTTP, BLE).
* Understand and implement GitHub-based OTA update workflows.
* Study FreeRTOS applicability for networking operations.
* Recommend the most reliable solution for production.


## 3. Development Environment

### Hardware
* **Device:** ESP32 Dev Module / DOIT ESP32 DEVKIT V1
* **Peripherals:** USB Cable, Android Mobile Device
* **Host Machine:** Laptop

### Software
* **IDE:** Arduino IDE (with ESP32 Board Package)
* **Libraries:** WiFiManager, Native ESP32 Libraries
* **Tools:** nRF Connect Mobile App, Serial Monitor
* **Hosting:** GitHub (for OTA firmware binary hosting)


## 4. Phase 1 – Basic Wi-Fi Connectivity

### Objective
Establish a baseline connection to a local Wi-Fi network using standard libraries.

### Experiment & Observations
* **Concepts Studied:** `WiFi.begin()`, `WiFi.status()`, `WL_CONNECTED`, `WiFi.localIP()`
* **Observation:** Connecting via hardcoded credentials correctly acquired an IP address.
* **Limitation:** Hardcoded credentials are strictly unsuitable for deployed commercial hardware.


## 5. Phase 2 – OTA Firmware Update

### Objective
Update target device firmware remotely without relying on a physical USB connection.

### OTA Execution Flow
1. **ESP32 Boot** ➔ 2. **Check `update.json` on GitHub** ➔ 3. **Compare Versions** ➔ 4. **Download Binary** ➔ 5. **Install Update** ➔ 6. **Reboot**

### Validation Status
* **Update Trigger:** Successfully pulled binaries from raw GitHub URLs.
* **Behavior Change:** Hardware changes (LED status toggles) were successfully observed post-update, confirming successful write operations.



## 6. Phase 3 – Runtime Wi-Fi Configuration Evaluation

### Method 1: WiFiManager (Recommended)
Utilizes an on-demand captive portal for robust runtime configuration.
* **Flow:** Device Boots ➔ No Wi-Fi Saved ➔ Spins up Setup Hotspot ➔ User Connects ➔ Web Portal Prompts for Credentials ➔ Device Saves to Flash & Connects.
* **Pros:** Exceptionally stable, native captive portal support, auto-saves to flash memory, excellent end-user experience.
* **Cons:** Requires the end-user to temporarily disconnect from cellular/home data networks during setup.

### Method 2: Custom HTTP Configuration Portal
Hosts a custom HTML/CSS page via a lightweight web server on the ESP32.
* **Pros:** Highly customizable UI matching brand requirements; allows configuration of non-network parameters simultaneously.
* **Cons:** Substantial engineering overhead, manual error/retry handling, higher maintenance requirement.

### Method 3: BLE Provisioning
Configures runtime Wi-Fi settings over Bluetooth Low Energy (BLE) via a mobile app.
* **Test Results:** Advertising (`ESP32_BLE_SETUP`), secure connection, and string data transfers (`hello somu`, `hii`) were successfully verified using the nRF Connect app.
* **Critical Bottleneck:** Combining the full classic BLE stack with the standard Wi-Fi network stack caused severe memory allocation faults on default ESP32 partition schemes. Deferred to future optimization using lightweight NimBLE.


## 7. Wi-Fi Credential Reconfiguration
To support real-world field stability, users must be able to change credentials on demand.

**Proposed Solution: Hardware Reset Button**
1. User presses an onboard tactile button for 5–10 seconds.
2. Firmware clears NV storage blocks via `Preferences` / `WiFi.disconnect()`.
3. ESP32 reboots automatically.
4. WiFiManager captive portal initializes for secure re-entry of new credentials.


## 8. FreeRTOS Considerations
Transitioning from a monolithic single-thread `loop()` structure to an asynchronous multi-task architecture is strongly advised for scaling.

**Proposed Architecture:**
* **Task 1:** Wi-Fi Management (Status loops, portal listening)
* **Task 2:** Sensor Interfacing (Hardware-level data ingestion)
* **Task 3:** Cloud Communications (MQTT, OTA checks)
* **Task 4:** Core Application Logic (State machines)


## 9. Setup Instructions

1. **Board Manager:** Add the Espressif ESP32 package to your Arduino IDE.
2. **Library Requirements:** Install `WiFiManager` via the Arduino Library Manager.
3. **Flashing:** Upload the baseline firmware to the ESP32.
4. **Provisioning:** Connect your phone/laptop to the ESP32's setup Access Point, enter local Wi-Fi credentials in the captive portal, and save.
5. **OTA Configuration:** Upload `firmware.bin` and `update.json` to a GitHub repository, ensuring the raw URL endpoint is configured in your firmware.


## 10. Libraries Used
* `WiFi.h` (Network Core)
* `WiFiManager` (Captive Portal)
* `WebServer.h` & `HTTPClient.h` (HTTP Operations)
* `Update.h` (Flash Memory Overwriting)
* `Preferences.h` (NV Storage)
* `BLEDevice.h`, `BLEServer.h`, `BLEUtils.h`, `BLECharacteristic.h` (Bluetooth Subsystem)


## 11. Challenges Encountered
* **Hardcoded Credentials:** Resolved by switching strictly to runtime provisioning.
* **OTA Deployment:** Mastered by setting up a reliable GitHub JSON-parsing pipeline.
* **BLE Memory Limitations:** Combined BLE and Wi-Fi stacks exceeded available flash memory `(124% of program storage space)`. Resolved by pivoting to WiFiManager for MVP, reserving NimBLE configurations for V2.
* **Field Recovery:** Resolved by designing a long-press hardware reset workflow.


## 12. Selection Criteria
Candidates were evaluated on:
* Implementation Complexity
* End-User Experience Simplicity
* Field Operations Overhead
* Architecture Scalability & Firmware Footprint


## 13. Real-World Deployment Flow

Customer Powers Device
       ↓
ESP32 Attempts Wi-Fi Connection
       ↓
Credentials Available?
   ├── YES ──> Connect to Wi-Fi ──> Normal Operation
   └── NO
       ↓
Launch WiFiManager Captive Portal
       ↓
User Connects to Setup Hotspot
       ↓
Enter Local Wi-Fi Credentials via Webpage
       ↓
Store Credentials to NV Storage
       ↓
Reconnect to Wi-Fi ──> Normal Operation



## 14. Validation Evidence

* **Baseline Wi-Fi:** DHCP IP assignment verified.
* **OTA Updates:** Version validation loops and flash writes verified.
* **WiFiManager:** Captive portal intercepts, data retention across power cycles verified.
* **BLE Transfers:** Confirmed reliable string transmission.

## 15. Proposed Repository Structure

ESP32Cam/
├── README.md                  # Main project landing page
├── docs/                      # Schematics and APIs
├── firmware/                  # Production codebase
├── examples/                  # Test peripheral sketches
├── images/                    # Asset compilation directory
└── resources/                 # Config JSONs and templates


## 16. Comparative Analysis

| Feature | WiFiManager | HTTP Portal | BLE |
| --- | --- | --- | --- |
| **Implementation Curve** | Simple | Moderate | Highly Complex |
| **Mobile App Needed** | No | No | Yes |
| **Auto Flash Storage** | Yes | Custom Coding | Custom Coding |
| **Resource Cost** | Low | Medium | High |
| **User Friendliness** | High | Medium | High |
| **Production Fit** | **Recommended Base** | Partial Fit | Future Upgrade |


## 17. Lessons Learned

* Effective implementation of local Wi-Fi verification on Espressif targets.
* File-safe remote OTA deployment infrastructures utilizing GitHub.
* Secure NV storage recovery using hardware `Preferences`.
* The necessity of FreeRTOS for advanced multi-threaded networking stability.


## 18. Final Recommendation

**Recommended Architecture: WiFiManager + Physical Hardware Reset Button**
This combination offers the lowest development complexity, requires no custom mobile applications, features automatic parameter saving, and provides a completely resilient recovery path for consumer field deployments.


## 19. Future Enhancements

* Design integration of a dedicated physical Wi-Fi reset switch.
* Flash memory optimization for NimBLE integration.
* Transitioning to full FreeRTOS multi-tasking.
* MQTT integration for fleet management.



## 20. Conclusion

This Proof of Concept successfully validated runtime Wi-Fi provisioning, automated GitHub OTA deployments, and low-level peripheral communications on the ESP32 platform. The **WiFiManager portal architecture supported by an asynchronous hardware reset routine** is the selected primary path forward for production implementation.
