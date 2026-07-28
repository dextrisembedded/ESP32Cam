# ESP32 TFT Touchscreen UI with WiFi and Camera Control

## 📌 Project Overview

This project demonstrates a touchscreen-based user interface developed using an **ESP32 DevKit**, **ILI9341 TFT display**, **LVGL**, and **EEZ Studio**.

The user interface contains three screens:

- Home Screen
- WiFi Screen
- Camera Screen

Users can navigate between the screens using touch-based buttons. The project provides a basic graphical user interface for WiFi status and camera control.

---

## ✨ Features

- ESP32-based touchscreen user interface
- 2.8-inch ILI9341 TFT display
- 240 × 320 display resolution
- Touch-based screen navigation
- Home screen with WiFi and Camera options
- WiFi connection status screen
- Camera control screen
- Capture Picture button
- Back-to-Home navigation
- UI designed using EEZ Studio
- Graphics rendered using LVGL

---

## 🖥️ User Interface Screens

### 1. Home Screen

The Home screen acts as the main dashboard.

**Buttons:**

- Go to WiFi
- Go to Camera

### 2. WiFi Screen

The WiFi screen displays the WiFi connection status.

**Features:**

- WiFi connection status
- Back to Home button

### 3. Camera Screen

The Camera screen provides basic camera controls.

**Buttons:**

- Capture Picture
- Back to Home

---

## 🔄 Screen Navigation Flow

```text
                 HOME
                /    \
               /      \
              ▼        ▼
           WiFi      Camera
             │          │
             └────┬─────┘
                  ▼
                HOME
