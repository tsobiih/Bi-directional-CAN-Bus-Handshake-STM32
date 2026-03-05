# 🚀 STM32 CAN Bus Handshake Project

## 📝 Project Overview

This project is a practical implementation of **bi-directional communication** (Handshake) between two microcontrollers using the **CAN Protocol**. Developed as a milestone after completing a comprehensive CAN Bus course on Udemy, it demonstrates how to handle data transmission, reception via interrupts, and hardware validation.

---

## 🛠 Hardware Architecture

### **Core Components**

| Component | Description |
| --- | --- |
| **Microcontrollers** | 2x STM32F103C8T6 (Blue Pill) |
| **CAN Transceivers** | 2x WCMCU-230 (VP230) |
| **Display** | ST7735 TFT Screen (Master Node status) |
| **Termination** | 120Ω Resistors at each bus end (60Ω Total) |

### **Connection Pins**

* **CAN_TX:** PA12
* **CAN_RX:** PA11
* **TFT Interface:** SPI1 (PA5, PA7) + Control Pins

---

## 💻 Software & Logic

### **Handshake Flow**

1. **Master Node:** Transmits a data frame with `0xA1` every 2 seconds.
2. **Responder Node:** Listens using **CAN RX FIFO0 Interrupts**.
3. **Verification:** If the data matches `0xA1`, the responder sends back `0xB2`.
4. **Master Display:** Updates the TFT screen to **"Data Received"** upon receiving the correct reply.

### **Configuration Details**

* **Baud Rate:** 500 Kbps
* **Filter Mode:** Mask Mode (Open to specific Handshake IDs)
* **Architecture:** Event-driven using HAL Callbacks

---

## 🔍 Engineering Insights

> [!IMPORTANT]
> **Physical Layer Validation:**
> Before powering the system, the bus impedance was verified using a **Digital Multimeter**, measuring exactly **60Ω** across CAN_H and CAN_L to prevent signal reflections.

* **Self-Diagnostics:** The Master node is programmed to detect and display errors like **No ACK** or **Bit Errors** directly on the TFT screen.
* **Optimization:** Used `HAL_CAN_ActivateNotification` to ensure CPU availability while waiting for data.

---

## 📂 Project Structure

```text
├── Master_Node/          # Master source code & CubeMX file
├── Responder_Node/       # Responder source code & CubeMX file
├── Schematics/           # Wiring diagrams
└── README.md             # Project documentation

```

---

## 🚀 How to Use

1. Clone the repo: `git clone https://github.com/TarekSobih/STM32-CAN-Handshake.git`
2. Open projects in **STM32CubeIDE**.
3. Connect hardware as per the pinout.
4. Flash the MCUs and monitor the TFT status.
