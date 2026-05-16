# Haptic Visual Baby Monitoring System

## Project Overview

This project is an IoT-based baby monitoring system designed to improve infant safety through haptic and visual alert mechanisms.

The system detects critical situations such as crying sounds or abnormal movements and instantly notifies caregivers using wireless communication.

The project consists of three main units:

- **Baby Unit** → Detects sound and motion conditions.
- **Light Unit** → Provides visual LED alerts.
- **Wearable Unit** → Sends vibration-based notifications to caregivers.

## Project Prototype

![Project Prototype](images/project_prototype.jpg)

## System Architecture

The system works through sensor-based monitoring and ESP-NOW wireless communication between units.

### Baby Unit
- Detects crying sounds using a sound sensor
- Monitors motion using MPU6050
- Sends alert signals to other units

![Baby Unit](images/baby_unit.png)

### Wearable Unit
- Provides haptic feedback through vibration
- Helps caregivers receive alerts instantly

![Wearable Unit](images/wearable_unit_prototype.jpg)

### Light Unit
- Provides visual LED notifications

![Light Unit](images/light_unit.png)

## Hardware Components

- ESP8266 NodeMCU
- MPU6050 Motion Sensor
- MAX4466 Sound Sensor
- LED Module
- Vibration Motor
- Breadboard & Jumper Wires

## Technologies Used

- Arduino IDE
- Embedded C/C++
- ESP-NOW Communication
- IoT Systems

## Source Code Structure

- `01_MAC_Address_Finder/` → MAC address detection
- `02_Light_Unit/` → Light unit code
- `03_Wearable_Unit/` → Wearable bracelet code
- `04_Baby_Unit/` → Baby monitoring unit code

## Authors

Ayşe Tuğba Peköz  


Computer Engineering Project
