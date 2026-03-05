أبشر يا طارق، عملتلك الـ **README** بتنسيق **Markdown** احترافي وجاهز للنسخ فوراً. التنسيق ده بيستخدم الـ Hierarchy والـ Tables عشان يخلي شكل مشروعك "Premium" على جيت هاب.

---

# 🚀 STM32 CAN Bus Handshake Project

## 📝 Project Overview

This project is a practical implementation of **bi-directional communication** (Handshake) between two microcontrollers using the **CAN Protocol** [cite: 2026-03-05]. Developed as a milestone after completing a comprehensive CAN Bus course on Udemy, it demonstrates how to handle data transmission, reception via interrupts, and hardware validation [cite: 2026-03-05].

---

## 🛠 Hardware Architecture

### **Core Components**

| Component | Description |
| --- | --- |
| **Microcontrollers** | 2x STM32F103C8T6 (Blue Pill) [cite: 2026-03-05] |
| **CAN Transceivers** | 2x WCMCU-230 (VP230) [cite: 2026-03-05] |
| **Display** | ST7735 TFT Screen (Master Node status) [cite: 2026-03-05] |
| **Termination** | 120Ω Resistors at each bus end (60Ω Total) [cite: 2026-03-05] |

### **Connection Pins**

* **CAN_TX:** PA12 [cite: 2026-03-05]
* **CAN_RX:** PA11 [cite: 2026-03-05]
* **TFT Interface:** SPI1 (PA5, PA7) + Control Pins [cite: 2026-03-05]

---

## 💻 Software & Logic

### **Handshake Flow**

1. **Master Node:** Transmits a data frame with `0xA1` every 2 seconds [cite: 2026-03-05].
2. **Responder Node:** Listens using **CAN RX FIFO0 Interrupts** [cite: 2026-03-05].
3. **Verification:** If the data matches `0xA1`, the responder sends back `0xB2` [cite: 2026-03-05].
4. **Master Display:** Updates the TFT screen to **"Data Received"** upon receiving the correct reply [cite: 2026-03-05].

### **Configuration Details**

* **Baud Rate:** 500 Kbps [cite: 2026-03-05].
* **Filter Mode:** Mask Mode (Open to specific Handshake IDs) [cite: 2026-03-05].
* **Architecture:** Event-driven using HAL Callbacks [cite: 2026-03-05].

---

## 🔍 Engineering Insights

> [!IMPORTANT]
> **Physical Layer Validation:**
> Before powering the system, the bus impedance was verified using a **Digital Multimeter**, measuring exactly **60Ω** across CAN_H and CAN_L to prevent signal reflections [cite: 2026-03-05].

* **Self-Diagnostics:** The Master node is programmed to detect and display errors like **No ACK** or **Bit Errors** directly on the TFT screen [cite: 2026-03-05].
* **Optimization:** Used `HAL_CAN_ActivateNotification` to ensure CPU availability while waiting for data [cite: 2026-03-05].

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

1. Clone the repo: `git clone https://github.com/TarekSobih/STM32-CAN-Handshake.git` [cite: 2026-03-05]
2. Open projects in **STM32CubeIDE** [cite: 2026-03-05].
3. Connect hardware as per the pinout [cite: 2026-03-05].
4. Flash the MCUs and monitor the TFT status [cite: 2026-03-05].

---
