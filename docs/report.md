# Table of Contents
* Abstract
* [Introduction](#1-introduction)
* [Related Work](#2-related-work)
* [Technical Approach](#3-technical-approach)
* [Evaluation and Results](#4-evaluation-and-results)
* [Discussion and Conclusions](#5-discussion-and-conclusions)
* [Demo Links](#6-demo-links)
* [References](#7-references)

# Abstract

The AquaticCatchPredictor project utilizes Machine Learning techniques to analyze fishing data, including GPS location, temperature, air pressure, and water activity. By collecting real-time environmental data, the system predicts catch rates under current fishing conditions and provides on-site recommendations to enhance fishing success. This approach aims to assist anglers in making informed decisions based on environmental factors, thereby improving their catch rates. The integration of machine learning ensures that predictions adapt to varying conditions, making the AquaticCatchPredictor a valuable tool for recreational and professional fishing alike.

# 1. Introduction

## 1.1. Motivation & Objective
1. **Personal Inspiration**  
   As an avid fishing enthusiast, I encountered a frustrating experience where a fishing app predicted excellent fish activity, leading me to invite friends for a fishing trip. Unfortunately, not a single fish was caught that day. This sparked the idea for a more reliable and data-driven solution.

2. **Significance**  
   - **Who cares?**  
     Recreational and competitive anglers aiming to improve their skills and catch rates.  
   - **Why does it matter?**  
     Saves time and resources, creating a more enjoyable and effective fishing experience.

### Objective

- **Portable Device**: Create a compact system capable of predicting fish activity and recommending optimal fishing spots.
- **Environmental Analysis**: Integrate data from temperature, pressure, and weather conditions to assess fish behavior.
- **Real-time Insights**: Use GPS and live weather updates to identify fishing spots with the highest success potential.
- **Enhanced Experience**: Reduce uncertainty and improve catch rates through data-driven decision-making using decision trees for prediction.


## 1.2. State of the Art & Its Limitations





## 1.3. Novelty & Rationale



## 1.4. Potential Impact



## 1.5. Challenges


## 1.6. Requirements for Success

The hardware required for this project is as follows:
* 1x [XIAO ESP32S3 Sense](https://www.seeedstudio.com/XIAO-ESP32S3-Sense-p-5639.html) by SEEED Studio (equipped with OV2640 camera module) 
* 1x [XIAO ESP32S3](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html) by SEEED Studio
* Panasonic EKMB1303111K PIR sensor

The skills required are the following:
* Familiarity with embedded systems development, especially with sensing and power management
* Proficiency low-level programming in C/C++
* Understanding of machine learning, in particularly model optimization for deployment on a microcontroller (TinyML)
* Understanding of internet protocols and web server hosting

## 1.7. Metrics of Success



# 2. Related Work



# 3. Technical Approach

![Circuit_Diagram](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/general%20circuit%20diagram.png)

The general circuit diagram above outlines the main components of our system. The hardware of our system is partitioned into two: PIR_XIAO circuit and PD_XIAO circuit. The PIR_XIAO circuit remains in an always-on state, whereas the PD_XIAO circuit remains in an always-off state. PIR_XIAO circuit controls the power delivery to PD_XIAO circuit using a P-MOSFET as a high-side switch based on motion events detected. PD_XIAO uses a GPIO interconnection between the two XIAOs to signal for power cutoff upon task completion.

![PIR_XIAO](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/PIR_XIAO_loop.png)

PIR_XIAO subsystem functions as a state machine that controls power delivery to PD_XIAO based on the output of the PIR sensor attached via GPIO. Normally, the system remains in deep sleep with an external GPIO interrupt configured to the output of the PIR sensor. Upon detecting motion, the subsystem enables power delivery by pulling down the GPIO pin connected to the gate pin of the P-MOSFET. Next, the system immediately returns to deep sleep with an external interrupt configured to the GPIO interconnection between the two subsystems (designated PD_done signal). After the PD_XIAO subsystem completes its task, the PD_done signal will be generated through said GPIO interconnection, waking up PIR_XIAO from deep sleep. Afterwards, power delivery to PD_XIAO will be cut off, and the system will either return to deep sleep with PIR wakeup if no more motion is detected or initiate a new cycle if another motion event immediately follows.

![PD_XIAO](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/PD_XIAO_loop.png)

On the other hand, the workflow of the PD_XIAO subsystem is mostly sequential. Upon receiving power, the system will first perform the necessary initialization before executing the user program. To minimize system uptime, we parallelize the program by running ML model initialization and camera initialization followed by image capturing on different cores at the same time. After which, the person detection model is executed on the frame captured. Based on the inference result, the subsystem will optionally begin footage capturing, processing, and storing/transmitting when a person is detected. Lastly, the PD_done signal will be generated on the GPIO interconnection to signal for power cutoff.

![prototype](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/prototype.png)

Here we showcase the prototype and the corresponding circuit diagram. Initially, we made use of a few RC circuits in order to address a few issues (outlined below in [Technical Challenge](#32-technical-challenges)), however, in our final prototype, no external RC circuit is needed. The green LED used in this prototype is for indicating PD_XIAO subsystem ON/OFF status.

## 3.1. Software Framework

The development environment used for this project was [ESP-IDF](https://github.com/espressif/esp-idf), a development framework provided by Espressif, the company that produced ESP32S3. The programming language used is C/C++.

The person detection model used in our system is a pre-trained model provided by ESP TFLite Micro] (https://github.com/espressif/esp-tflite-micro). ESP TFLite Micro is an optimized version of Tensorflow Lite Micro specifically for ESP32 devices. Since this model is highly optimized and well suited for our specific use case, we implemented it into our code as is.

The person detection model takes in a single 96x96 grayscale image. The OV2640 camera module on PD_XIAO has the ability to directly output images in this format. The camera module is initially configured to store its frame buffers on the internal RAM for faster access. Before capturing footage when a person is detected, the camera module is reconfigured to a higher resolution in RGB at runtime with frame buffers allocated onto the 8MB PSRAM due to increased memory usage.

The footage capturing and streaming capabilities are demonstrated in [demo video 3](https://www.youtube.com/watch?v=NmRjGxvr8ks).

## 3.2. Technical Challenges

Here we outline a selection of challenges we faced during the development of this project and the solutions we employed to address these challenges.

### 3.2.1. Monochrome Camera Module Sourcing Issues

In our original design, we planned on using a standalone monochrome camera module to perform person detection with the intention of reducing energy consumption by keeping the main colored camera in sleep. However, the first module we purchased was incompatible with our system, and we also faced difficulties sourcing for another module. Ultimately, we decided to capture the image for person detection on our main camera instead. The energy consumption penalty is in fact insignificant compared to the rest of the system while greatly simplifying the development process.

### 3.2.2. Deep Sleep Power Consumption Over Budget

Our original design only uses a single XIAO ESP32S3 Sense to both interface with a PIR sensor and perform person detection. However, we discovered that the deep sleep power consumption was 3.8V at 3.00mA, much higher than the 3.8V at 14μA of our expectation. This was due to the Sense board module that the camera is attached to consuming excess current at all times. We attempted to isolate all GPIO pins used by the Sense board during deep sleep, however, since its power circuitry is directly connected to the 3.3V power rail, we were unable to reduce the current consumption.

Our next approach was to employ an external hardware circuitry that controls power delivery to the XIAO. We experimented with designing a custom digital circuit with microamp current consumption, which has been proven very difficult given the resources available and the timeframe. As a result, we opted to use a microcontroller for this purpose, specifically another XIAO ESP32S3. The control of power delivery is done using a P-MOSFET as a high-side switch connected to a GPIO pin. A high-side switching setup completely cuts off the access to 3.3V on the PD_XIAO subsystem, greatly reducing leakage current. The signaling mechanism for power cutoff is done using GPIO interconnection.

### 3.2.3. PIR Erratic Output

![PIR_output](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/PIR%20output.png)

The PIR sensor used in this project has an erratic output behavior. When a single swift motion is produced within the detection range, no matter the distance from the sensor, the output will almost always contain 2-3 pulses, each lasting around 0.5-3 seconds. This behavior causes the PIR_XIAO subsystem to falsely enable power delivery multiple times in a row, which leads to unnecessary energy consumption.

Our initial solution was to shorten the pulses using a small capacitor in series. When the PIR output is switched to HIGH, the capacitor will quickly charge up and disconnect the circuit, effectively shortening the pulse. However, this approach was ultimately unsuccessful because the output LOW time in between of each pulse was too long, allowing the capacitor to fully discharge in between of each pulse. In the end, there are still multiple pulses on the output, just shorter in duration.

The final solution we chose was to simply include a software delay. A delay in energy-constrained microcontroller development is extremely costly. To address this, instead of having the system idle during the delay, we put the PIR_XIAO subsystem to deep sleep with a timer wakeup. This allows the subsystem to only suffer a slight penalty of a few tens of milliseconds of an extra wakeup while effectively debouncing the PIR sensor. In addition, this delay also serves as a way to limit the frequency that person detection can be performed, preventing unnecessary energy consumption if the system were to malfunction.

### 3.2.4. GPIO Undefined State During Power On

![rc_delay](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/RC%20delay.png)

In our final setup, the PIR_XIAO subsystem controls power delivery to the PD_XIAO subsystem. A GPIO HIGH signal is initiated by PD_XIAO to signal PIR_XIAO to cut off power. However, when PD_XIAO first receives power, the GPIO pins are all in an undefined state. Through experimentation, we found that the voltage on the GPIO interconnection for PD_done signal is roughly 2.8V during initialization, above the threshold voltage for a HIGH state. This causes power to be shut off immediately after it is enabled, due to PIR_XIAO receiving a false PD_done signal. This is demonstrated on the image above: the blue signal is the output of the PD_done signal with the initial peak generated during initialization and the second peak being the true PD_done signal.

We have attempted to initialize this GPIO pin on PD_XIAO as early as possible in the bootup sequence to no avail. As a first solution, we introduced an RC delay circuit that ensures that the voltage on the line will not rise above the HIGH threshold during initialization. However, this has induced a heavy latency penalty to power shut off due to the nature of an RC delay circuit. Our final solution was to implement a software delay in the PIR_XIAO code that blocks the PD_done signal for a sufficient amount of time after first enabling power delivery. Since the PD_XIAO subsystem consumes current in the magnitude of hundreds of milliamps, keeping PIR_XIAO in idle briefly during this has minimal energy consumption penalty. 

# 4. Evaluation and Results

The primary goal of the project was to achieve a multimodal intrusion detection system in the ultra low power regime (sub 1 mW regime).

As mentioned previously, the state of the art vision-only custom-ASIC approach has a total system power consumption of 170µW, which is equivalent to 14688mJ/day [1]. Initially, we decided that the metric of successful this project would be to achieve a total energy consumption within this budget while achieving 50 wakeups/day.

We partition our system to two behavioral states: a passive deep sleep state and an active ON-state. The current consumption is measured using a Keithley 2601A source meter.

In the deep sleep state, only the PIR_XIAO subsystem remains active, consuming a current of 18µA in total at 3.3V, out of which, 12µA is consumed by the PIR_XIAO in deep sleep, 6µA is consumed by the PIR sensor in idle state. In total, this sums up to 5132mJ of energy per day that has to be allocated to the passive deep sleep state.

![energy_disttribution](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/energy%20distribution.png)

The active ON-state current consumption is graphed below. By taking an average of the current consumption across the duration of a wakeup event, we were able to estimate the energy consumption of each wakeup event to be 395mJ.

For an aforementioned 14688mJ of power budget, we are able to achieve 24 wakeups per day. Note that the energy consumption calculation for active ON-state does not include footage capturing or any related processing and transmission, as the energy optimization of these areas is out of the scope of this project.

The following graph showcases the current consumption of 3 consecutive wakeup events. Each wakeup event contains a main peak (shown in magenta) and a subsequent peak (shown in red). The main peak contains the main person detection tasks, whereas the subsequent peak corresponds to a brief wakeup event of the PIR_XIAO after debouncing the PIR sensor using a timer wakeup.

![energy_disttribution](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/current%20consumption%20-%203%20peaks.png)

The main peak is partitioned into individual phases. The regions of operation are indicated in the graph below. Note that the person detection neural network consumes almost the most amount of current, but only takes 58ms to execute. The circuit delay portion also consumes significant current, but it takes 381ms to execute. As previously mentioned in [Section 3.2.4](#324-gpio-undefined-state-during-power-on), we have removed the RC circuit in our final prototype, hence the current consumption induced by the circuit delay is no longer relevant. This has also been demonstrated in the [second demo video](https://youtube.com/shorts/ZmwHzCMsa_o?feature=share).

With the RC circuit removed, the active ON-state current consumption per wakeup cycle is reduced to 225mJ/wakeup, which allows for a 42 wakeups/day.

![main_peak](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/main%20peak.png)
![energy_energy_per_region](https://raw.githubusercontent.com/AnanayG/multimodal_intrusion_detection/main/docs/media/duration%2Benergy%20of%20each%20region.png)

# 5. Discussion and Conclusions

In the end, we achieved 42 wakeups/day at 170µW power consumption. Even though we were not able to achieve 50 wakeups/day as we previously planned for, we are close enough to the target to demonstrate the effectiveness of our design. In addition, our project is done only using off-the-shelf components; the fact that we are able to compete with a custom ASIC design clearly demonstrates the superiority of our multimodal approach to person detection compared to the traditional vision-only approach.

In the TinyML community, 1mW is considered a significant milestone in terms of power consumption. After achieving this threshold, the system can easily last “forever” with the help of minimal energy harvesting. For a power budget of 1mW, our system can achieve around 247 wakeups/day, which is more than sufficient for the typical security application. With the help of a small solar panel, our security camera prototype will be able to provide a maintenance-free user experience.

In conclusion, we consider our project a success despite not strictly meeting the goal we first set out to reach. Since many significant challenges we faced during the project were fundamentally due to the limitations of off-the-shelf components, if we were to use custom hardware similar to that in state-of-the-art implementations today, such as implementing PIR_XIAO subsystem with an FPGA, we will undoubtedly achieve an even more impressive power consumption.

# 6. Demo Links

* [Master Demo Video](https://www.youtube.com/watch?si=UEJS8G_MfGbzbT3T&v=8JuSE7JUu0g&feature=youtu.be)
* [No RC Circuit Demo Video](https://youtube.com/shorts/ZmwHzCMsa_o?feature=share)
* [Wifi Streaming Demo Video](https://www.youtube.com/watch?v=NmRjGxvr8ks)
* [System power data](https://docs.google.com/spreadsheets/d/1SeqJhA1LRMm0-gP1tuYFk-gqm2a6v0A1QVVGwjIOnUg/edit?usp=sharing)
* [Final Presentation Link](https://docs.google.com/presentation/d/1U2bdHlef-A1cob_oj03bhjhw77Q7Prq9A0cpEgVMt5c/edit?usp=sharing)
  
# 7. References

* An, H. et al. (2021) ‘An ultra-low-power image signal processor for hierarchical image recognition with deep neural networks’, IEEE Journal of Solid-State Circuits, 56(4), pp. 1071–1081. doi:10.1109/jssc.2020.3041858.
* Chowdhery, A. et al. (2019) Visual wake words dataset, arXiv.org. Available at: https://arxiv.org/abs/1906.05721 (Accessed: 11 November 2023).
* [ESP TFLite Micro GitHub Repository](https://github.com/espressif/esp-tflite-micro)
* [ESP32S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
* [ESP32S3 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
* [ESP-IDF GitHub Repository](https://github.com/espressif/esp-idf)
* [SEEED Studio XIAO ESP32S3 Wiki](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
* [Panasonic PIR Sensor Datasheet](https://api.pim.na.industrial.panasonic.com/file_stream/main/fileversion/251032)
* [ESP32-CAM Person Detection Experiment With TensorFlow Lite](https://www.instructables.com/ESP32-CAM-Person-Detection-Expreiment-With-TensorF/)
* [TinyML Talks - Yung-Hsiang Lu: Low-Power Computer Vision](https://www.youtube.com/watch?v=GdAs4P_0wEk&t=137s&ab_channel=tinyML)
