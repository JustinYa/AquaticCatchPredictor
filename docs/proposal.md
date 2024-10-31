# Proposal for Portable Fishing Prediction System

---

### 1. Motivation & Objective
The primary goal of this project is to increase fishing efficiency by providing anglers with a real-time fishing index based on various environmental factors, including temperature, barometric pressure, water activity, and GPS location. By analyzing these parameters on-site, the system aims to offer more accurate fishing predictions than generic weather-based solutions, ultimately enhancing the fishing experience and success rate.

---

### 2. State of the Art & Its Limitations
Currently, most fishing prediction systems are based on empirical knowledge and some known environmental factors rather than scientific analysis derived from actual data. These systems often fail to accurately capture the specific conditions of the fishing site, such as real-time water activity and localized weather patterns, which can significantly influence fish behavior. Additionally, traditional fishing apps depend on cloud services for data processing and prediction, which presents several limitations. First, they require a stable internet connection, making them unreliable in remote fishing locations with poor or no signal. Second, relying on cloud processing drains phone battery life quickly, which limits usability during extended fishing trips. In contrast, an on-device, low-power sensor system can continuously monitor environmental factors specific to the fishing location. This approach is not only more energy-efficient but also works offline, enhancing both longevity and privacy.

---

### 3. Novelty & Rationale
Our approach introduces several novel elements to fishing prediction. First, by using localized, real-time data, our system relies on actual data from the fishing site, such as water movement detected by an accelerometer, along with temperature, pressure, and location, to provide a comprehensive fishing index. Second, the system supports offline capability. By utilizing pre-trained machine learning models, it processes data locally, eliminating the need for cloud-based computations and enabling functionality in remote fishing locations without cellular connectivity. Additionally, for pattern recognition, the device uses decision trees or SVM (Support Vector Machines) to analyze patterns from the collected data, allowing it to dynamically identify optimal fishing conditions without requiring constant internet access. Furthermore, this system could be beneficial in competitive fishing events like Bassmaster, where it provides insights into fish activity patterns in specific water areas. By offering data-driven analyses, it has the potential to increase the event's appeal to audiences. This combination of real-time, location-specific analysis with energy-efficient, offline operation sets our device apart from conventional fishing prediction systems.

---

### 4. Potential Impact
The successful implementation of this project could have a significant impact on recreational and competitive fishing. By offering more accurate, real-time predictions, anglers can make informed decisions about where and when to fish, improving their success rate and overall satisfaction. Additionally, with better data on fish activity, anglers can target specific times for fishing, potentially reducing overfishing and promoting sustainable practices. Furthermore, this project demonstrates the potential of TinyML and low-power sensor fusion in outdoor, real-time applications, encouraging further development in portable IoT solutions.

---

### 5. Challenges
This project faces several challenges. First, unlike weather forecasting, fishing prediction requires specific environmental and behavioral data for different fishing locations and times. There is no existing comprehensive database, necessitating the collection of real-world data across various conditions. Second, to ensure the data accurately represents different fishing environments, we need input from anglers of varied experience levels in multiple geographic and environmental settings. This diversity is crucial to train a model that generalizes well across conditions.

---

### 6. Requirements for Success
The success of this project hinges on several technical skills and hardware components. Key technical skills include proficiency in C++ and Python programming, particularly for embedded systems development. Familiarity with machine learning, especially in model optimization for embedded devices (TinyML), is crucial for achieving efficient, on-device processing. Additionally, 3D modeling skills are needed to design a portable and ergonomic casing for the device. A basic knowledge of fishing is also beneficial for understanding and identifying relevant environmental factors that influence fish activity.

The hardware requirements for this project include a microcontroller, such as the ESP32, to act as the core processing unit. Environmental sensors, specifically for temperature and barometric pressure, will provide real-time data about the fishing conditions. A water activity sensor, like an accelerometer, is essential for detecting movements in the water that may signal fish activity. The system also requires a GPS module for precise location tracking and a portable power source, such as a rechargeable battery, to ensure the device can operate for extended periods in remote locations.

---

### 7. Metrics of Success
The effectiveness of the fishing prediction system will be evaluated through several performance metrics. Prediction accuracy is critical; the system should reliably estimate fish activity based on environmental data. High accuracy will indicate the device’s effectiveness in analyzing real-time conditions. Another important metric is user satisfaction, which will be assessed through a survey with anglers. The survey will evaluate the prediction accuracy, ease of use, and overall utility of the device in real fishing scenarios, providing valuable feedback for further improvements.

---

### 8. Execution Plan
The execution plan for this project is divided into several phases. In the **Data Collection Phase**, we will gather data on temperature, pressure, water movement, and GPS location across various fishing sites. Collaborating with anglers to collect a diverse dataset is crucial to ensure the model’s generalizability and accuracy in different fishing conditions. 

Following data collection, the **Model Training & Optimization** phase will involve training a machine learning model, such as decision trees or SVM, on the collected data to identify patterns in fish activity. Optimization techniques like quantization and pruning will be applied to reduce model size and power requirements, enabling efficient operation on embedded hardware. 

In the **Prototype Development** phase, we will build a prototype that integrates the selected sensors, microcontroller, and power supply. Testing in real-world fishing conditions will allow us to refine the device’s sensitivity and accuracy. 

The **Evaluation and Refinement** phase involves multiple rounds of testing and iteration based on performance metrics, user feedback, and battery life optimization. Finally, in the **Deployment & Field Testing** phase, the finalized prototype will be field-tested with a larger user base for additional feedback and adjustments.

---

### 9. Related Work
Though resources on freshwater fishing prediction are limited, we can draw insights from related research on ocean fishing. For example, studies such as "Fishing Spot Prediction by Sea Temperature Pattern Learning" and "An Approach to Recommend Fishing Location and Forecast Fish Production by Using Big Data Analysis and Distributed Deep Learning" provide valuable methodologies. Although these works primarily utilize data from existing databases, they offer useful experimental frameworks that can guide our approach to data collection and analysis in a freshwater context.

---

### 10. References
- M. Iiyama, K. Zhao, A. Hashimoto, H. Kasahara and M. Minoh, "Fishing Spot Prediction by Sea Temperature Pattern Learning," 2018 OCEANS - MTS/IEEE Kobe Techno-Oceans (OTO), Kobe, Japan, 2018, pp. 1-4, doi: 10.1109/OCEANSKOBE.2018.8559299. keywords: {Ocean temperature;Support vector machines;Training data;Machine learning;Boats;Temperature distribution}.

- M. -T. Thai, T. -N. Chu-Ha, T. -A. Vo and T. -H. Do, "An Approach to Recommend Fishing Location and Forecast Fish Production by Using Big Data Analysis and Distributed Deep Learning," 2022 RIVF International Conference on Computing and Communication Technologies (RIVF), Ho Chi Minh City, Vietnam, 2022, pp. 322-327, doi: 10.1109/RIVF55975.2022.10013876. keywords: {Deep learning;Costs;Time series analysis;Predictive models;Big Data;Fish;Data models;Fishing forecast;Big Data;Time Series;Recommendation System;Distributed Deep Learning}.

--- 
