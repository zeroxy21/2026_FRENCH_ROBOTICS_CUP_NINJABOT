
# STM32G4-Based Autonomous Robotics Firmware

This repository contains the production-grade embedded firmware developed for an autonomous mobile robot built on the STMicroelectronics STM32G4 MCU architecture. Utilizing FreeRTOS for deterministic multi-threaded task management, the system integrates closed-loop locomotion control, dynamic power monitoring, distance telemetry tracking, and competition match-strategy execution.

<img width="1537" height="2048" alt="69ca2a04-c7fc-42c1-9bf3-5930cfb1a68c" src="https://github.com/user-attachments/assets/f9fbb1dd-80be-46dd-b041-9cde56046e68" />

<img width="2558" height="1327" alt="pcb_2_ninja" src="https://github.com/user-attachments/assets/132ad6d9-ed92-49b5-bd7e-7b22cb163798" />



## System Architecture

The firmware leverages a dual-thread topology managed via CMSIS-RTOS V2 (FreeRTOS) to guarantee strict separation between high-speed behavioral logic and regular system telemetry sampling.

### Thread Breakdown

* **System_Task (Normal Priority - 100ms Interval)**
  * Executes continuous analog-to-digital conversions to poll hardware battery voltage.
  * Processes state machine transitions for real-time power level identification.
  * Manages DMA data streams to update NeoPixel visual status indicators.
  * Handles localized safety guard routines via proximity scanning (`HAND_STOP_DETECT`).

* **Action_Control (Above Normal Priority - High-Speed Loop)**
  * Latches external hardware interrupts to monitor the robot starting cord state (`tirette_is_on`).
  * Enforces a strict 90-second match window countdown utilizing high-resolution hardware ticks (`HAL_GetTick`).
  * Drives conditional trajectory execution loops based on team dynamic configurations (Yellow vs. Non-Yellow starting zones) to handle navigation, servo actuation, and vacuum-based item procurement.

---

## Hardware Configuration & Peripherals

The architecture relies heavily on specific hardware peripherals tailored for robotics applications:

| Peripheral | Configuration / Protocol | System Assignment |
| :--- | :--- | :--- |
| **USART3** | Half-Duplex Single-Wire (115200 baud) | Dynamixel AX-12 Smart Actuators (Left and Right Wheels) |
| **I2C1** | Standard Master Communication | Multi-sensor Time-of-Flight (ToF) Proximity Array |
| **TIM3** | PWM Generation (Channels 1, 2, 3) | KST Servo Angular Positioning & Vacuum Pump Relay |
| **TIM4** | PWM Generation with Direct Memory Access (CH4) | WS2812B NeoPixel Status Strip Data Carrier |
| **ADC1** | 12-bit Single-Ended Resolution (Channel 5) | Main Battery Supply Voltage Telemetry |
| **USART2** | Standard Asynchronous TX/RX | Low-Level Telemetry Debugging Interface |

---

## Technical Implementations

### Locomotion & Smart Actuators (`AXL_A12.c`)
Implements low-level instruction packet assembly and dynamic transceiver direction switching (`HAL_HalfDuplex_EnableTransmitter` / `HAL_HalfDuplex_EnableReceiver`) to drive Dynamixel AX-12 servos. The driver supports both Wheel Mode for continuous locomotion and Joint Mode for precise angular positioning. A discrete PID control loop structure is provided for wheel velocity stabilization.

### Intelligent Power Management (`Battery_manager.c` & `Neopixel.c`)
Applies a hardware-calibrated ADC tracking pipeline to interpret real-time cell voltage. Readings are filtered through an integrated state machine that shifts between discrete operational zones (`BAT_FULL`, `BAT_MEDIUM`, `BAT_LOW`, `BAT_CRITICAL`). Visual feedback is propagated through a non-blocking addressable RGB strip driver utilizing a Timer PWM signal backed by dedicated DMA channel memory transfers to enforce rigid WS2812 signal timing requirements without CPU overhead.

### Auxiliary Actuations (`kst.c` & `pump.c`)
Handles auxiliary mechanics through microsecond-accurate PWM pulse manipulation (ranging from 500µs to 2500µs) for KST high-torque positioning servos. Additionally, it controls the starting and stopping sequences of an internal vacuum relay subsystem used for element picking and manipulation during the match sequence.

---

## State Machine Execution Flow

```text
    [Power On] 
        │
        ▼
 [Hardware Init] ───► ADC Calibration, Timer PWM Setups & DMA Mapping
        │
        ▼
 [Kernel Start]  ───► Thread Spawning (System_Task & Action_Control)
        │
        ▼
┌────────────────────────────────────────────────────────┐
│               Action_Control Idle Loop                 │
│  Awaiting Starter Cord Pull Latch (EXTI Interruption)   │
└───────────────────────┬────────────────────────────────┘
                        │
                        ▼ [Cord Extracted]
┌────────────────────────────────────────────────────────┐
│             90-Second Match Timer Begins               │
│                                                        │
│  ├── Team Yellow Trajectory:                           │
│  │   Move ──► Rotate 45° ──► ToF Scan ──► Vacuum ON    │
│  │                                                     │
│  └── Alternative Trajectory:                           │
│      Move ──► Rotate -45° ──► ToF Scan ──► Vacuum ON   │
└───────────────────────┬────────────────────────────────┘
                        │
                        ▼ [90s Expiry / Strategy Break]
           [Safe Actuator Shutdown / System Halt]
