# STM32F407 FreeRTOS Multitasking Control Deck

A preemptive, multi-tasking Real-Time Operating System (RTOS) application designed for the **ARM Cortex-M4 (STM32F407G-DISC1)** discovery board. This project serves as a foundational demonstration of bare-metal peripheral integration, thread-safe Inter-Task Communication (IPC), and real-time hardware interrupt handling.

## 🏗️ Architecture Overview

The system runs on the **FreeRTOS kernel** using the **CMSIS_V2** API wrapper. The system architecture decouples data sampling from the user interface and asynchronous inputs across three concurrent tasks:

1. **Task 1: Temperature Sampler (`StartTempTask` | High Priority)**
   * Sequentially controls power to the internal **ADC1** peripheral.
   * Pulls 12-bit raw digital voltage snapshots from the internal chip temperature channel.
   * Performs floating-point scaling into Celsius and pushes data to a thread-safe message queue every 1000ms.

2. **Task 2: LED Visualizer (`StartLedTask` | Normal Priority)**
   * Rest-blocks at 0% CPU consumption until a data packet lands in the `TempQueue`.
   * Manages the system **Heartbeat (Milestone 1)** by toggling the Green LED (PD12).
   * Runs validation logic against a thermal threshold (32°C/35°C), triggering the Red LED (PD14) if exceeded.

3. **Task 3: Interrupt Monitor (`StartButtonTask` | Below Normal Priority)**
   * Sits frozen in an absolute block waiting for a **Binary Semaphore**.
   * When the physical Blue User Button (PA0) is pressed, a hardware External Interrupt (**EXTI0**) triggers an Interrupt Service Routine (ISR) that releases the semaphore.
   * Instantly wakes up, toggles the Orange LED (PD13), and enforces a software debounce before returning to a blocked state.

## 🛠️ Key Technical Skills Demonstrated
* **System Timebase Mapping:** Remapped the standard HAL timebase to **TIM1** to prevent conflicts with the core FreeRTOS SysTick engine.
* **Thread-Safe IPC:** Utilized FreeRTOS Message Queues to prevent race conditions or memory collisions across varying priority tasks.
* **Low-Latency ISR:** Deferred execution out of the hardware interrupt callback by utilizing binary semaphores, ensuring the ISR remains incredibly short.
