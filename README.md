# 🏎️ Autonomous Line-Following Robot (PD Control)

An advanced, high-speed line-following robot engineered to navigate complex tracks, sharp intersections, and grid patterns. Unlike basic "wobble" line followers that rely on simple if/else logic, this system utilizes a custom **Proportional-Derivative (PD) control algorithm**, dynamic braking, and state-machine overrides to smoothly handle complex curves and sharp 90-degree corners.

---

## ✨ Key Features

* **PD Control Loop:** Calculates real-time error corrections for smooth, high-speed curves without mechanical wobble.
* **State-Machine Overrides:** Automatically detects 90-degree corners and cross-junctions, interrupting the PD loop to execute precise, in-place tank turns.
* **Dynamic Active Braking:** Reverses motor polarity during sharp drifts to counter momentum and prevent track overshoot.
* **Line-Loss Memory:** Remembers the last known direction before losing the line and executes aggressive recovery spins to find it.
* **Sensor Debouncing:** Uses a 15ms double-check window to filter out false readings caused by track smudges or ambient light changes.

---

## 🛠️ Hardware Requirements & Specs

* **Microcontroller:** Arduino Uno / Nano / Mega (or compatible)
* **Motor Driver:** L298N (or similar H-Bridge driver)
* **Motors:** 2x High-Torque DC Gear Motors 
* **Sensors:** 3-Channel IR Sensor Array (Digital Out)
* **Chassis:** 2-Wheel Drive (2WD) chassis with a front caster wheel
* **Power:** 7.4V to 12V LiPo/Li-ion Battery pack

### 🔌 Pin Configuration

| Component | Pin Type | Microcontroller Pin |
| :--- | :--- | :--- |
| **Left IR Sensor** | Digital Input | `Pin 2` |
| **Center IR Sensor** | Digital Input | `Pin 4` |
| **Right IR Sensor** | Digital Input | `Pin 7` |
| **Left Motor (Forward)** | PWM Output | `Pin 3` |
| **Left Motor (Backward)**| PWM Output | `Pin 5` |
| **Right Motor (Forward)**| PWM Output | `Pin 6` |
| **Right Motor (Backward)**| PWM Output| `Pin 9` |

*Note: Ensure your motor driver ground and microcontroller ground are connected (common ground).*

---

## 🚀 Setup & Installation

1. Clone this repository to your local machine.
2. Open the included `.ino` file in the Arduino IDE.
3. Verify the pin definitions at the top of the sketch match your physical wiring.
4. Upload the code to your microcontroller.

---

## 🧠 How the Logic Works

1. **Read Sensors:** The main loop constantly reads the 3 IR sensors.
2. **Check for Intersections:** If the sensors detect a T-junction, crosshair, or 90-degree turn, the code bypasses the PD math, drives the wheels over the pivot axis, and spins until the center sensor locks onto the new line.
3. **Calculate Error:** If it's a standard line, the sensor states are converted into an "error" score (e.g., -2 to +2).
4. **Apply Math:** The PD algorithm multiplies the error by `Kp` and the rate of change by `Kd`.
5. **Drive Motors:** The resulting correction is added to the base speed of one wheel and subtracted from the other, steering the robot smoothly.

---

## ⚙️ Calibration & Tuning Guide

Because physical parameters like vehicle weight, motor torque, and surface friction vary wildly, you **must** calibrate the control coefficients at the top of the `.ino` file for your specific robot:

1. `Kp` **(Proportional Gain):** Set `Kd` to `0` and lower `BASE_SPEED` to `120`. Increase `Kp` gradually until the robot can follow sharp curves. If it oscillates or wobbles violently across straightaways, `Kp` is too high.
2. `Kd` **(Derivative Gain):** With `Kp` operational, slowly increase `Kd`. This term acts as a dampener, counteracting oscillations and smoothing out tracking.
3. `ALIGN_DELAY`: Adjust this value in increments of `10ms`. This determines how far the robot steps forward onto a 90-degree junction before turning. If it turns too early and cuts the corner, increase this delay. If it overshoots, reduce it.
4. `BASE_SPEED`: Start low (`100`-`150`) while tuning. Once the PD math is smooth, increase this for faster lap times.

---

## 🤝 Contributing
Feel free to fork this project, submit pull requests, or open issues if you find any bugs or have suggestions for improvements.

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).
