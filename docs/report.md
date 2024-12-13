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

# 1. Introduction

## 1.1. Motivation & Objective

Fishing can be an enjoyable and rewarding activity, but it is not without its frustrations. One memorable experience involved using a fishing app that predicted excellent fish activity, prompting a fishing trip with friends. Unfortunately, not a single fish was caught that day, which highlighted the limitations of existing fishing prediction tools. This experience inspired the idea for a more reliable and data-driven solution. Both recreational and competitive anglers could benefit from such a system, as it would help them save time, reduce wasted effort, and improve their overall fishing success.

The primary objective of this project is to develop a portable device that predicts fish activity and recommends optimal fishing spots. This device will integrate environmental data such as temperature, pressure, and weather conditions to analyze fish behavior effectively. Additionally, the system will provide real-time insights using GPS and live weather updates, ensuring anglers have the most accurate information. By incorporating decision tree algorithms, this solution will improve prediction accuracy, reduce uncertainty, and create a more effective fishing experience.

## 1.2. State of the Art & Its Limitations

Currently available fishing apps rely heavily on general weather data and historical trends to provide predictions. While these tools offer a basic level of assistance, they have several limitations. For example, they often lack real-time data integration, which makes it challenging to adapt to changing environmental conditions. Additionally, these apps do not provide location-specific accuracy, which is crucial for anglers seeking reliable predictions for specific fishing spots. Personalized recommendations based on individual preferences or fishing goals are also absent in most existing solutions.

These limitations lead to inconsistent performance, which diminishes user trust and effectiveness. This project aims to address these shortcomings by providing a data-driven, accurate, and personalized solution tailored to the unique needs of anglers.

## 1.3. Novelty & Rationale

This project offers several unique features that set it apart from current fishing prediction tools. First, the predictions are based on scientifically collected data, ensuring high reliability and accuracy. The device also includes offline functionality, enabling it to operate independently of internet access by utilizing onboard sensors and pre-trained models. Furthermore, decision tree algorithms are employed to deliver precise predictions about fish activity, providing a significant improvement over generalized or heuristic approaches. These innovations make the system an effective tool for improving fishing outcomes in diverse conditions.

## 1.4. Potential Impact

This fishing prediction system provides anglers with accurate, location-specific insights, improving catch rates and reducing wasted effort. For competitive anglers, it offers a strategic advantage through real-time data and decision tree algorithms. Its offline functionality enables use in remote areas, while promoting sustainable fishing by minimizing environmental impact. With strong potential for commercial applications, the system addresses key gaps in current fishing technologies and enhances the overall fishing experience.

## 1.5. Challenges

Collecting a robust and comprehensive dataset for training the model within the constraints of limited time and resources is a significant challenge. To ensure the model's accuracy and generalizability, the dataset must include diverse fishing scenarios, incorporating varying skill levels of anglers, from beginners to experts. Additionally, it is essential to gather data from multiple locations within the same waterbody, covering a wide range of environmental conditions such as temperature, pressure, and water activity. These variations are critical for capturing the complex factors that influence fish behavior. Achieving this level of diversity in data collection requires extensive planning, coordination, and effort, making it one of the most demanding aspects of the project.

## 1.6. Required Skills and Resources

Successfully completing this project requires a combination of skills and hardware resources:

### Skills
- **Sensor Integration**: Proficiency in working with sensors such as thermometers, barometers, and IMUs to collect accurate environmental data.
- **Bluetooth Communication**: Knowledge of Bluetooth protocols to enable seamless data transfer between devices.
- **Microcontroller Programming**: Familiarity with embedded system programming, particularly for configuring and managing microcontroller operations.
- **PCB Design**: Experience in designing and fabricating printed circuit boards (PCBs) to integrate system components efficiently.
- **3D Modeling**: Ability to create 3D models for designing and manufacturing the device’s enclosure, ensuring durability and ergonomic design.

### Hardware Resources
- **ESP32**: A versatile microcontroller for managing system operations and communication.
- **Arduino Nano 33 BLE**: For low-power processing and Bluetooth communication.
- **0.97-inch OLED Display**: For displaying real-time predictions and system status.
- **Neo-6M GPS Module**: For obtaining accurate location data to improve fishing predictions.

## 1.7. Metrics of Success

The success of the fishing prediction system is evaluated using a combination of accuracy, performance, and efficiency metrics. Prediction accuracy is assessed through Mean Squared Error (MSE), which calculates the average squared difference between predicted and actual values, and Root Mean Squared Error (RMSE), which provides an interpretable error measure in the same units as the target variable. To analyze model performance, Relative Error is used to measure the difference between testing and training MSE as a percentage of the training MSE, highlighting potential overfitting or underfitting. Prediction speed is another key metric, focusing on the time required for the system to compute a prediction, which is critical for real-time applications. Additionally, power efficiency is evaluated by measuring average power consumption in milliwatts (mW) and estimating battery life in hours based on the available power source. These metrics collectively ensure the system's accuracy, responsiveness, and practicality in real-world scenarios.

# 2. Related Work

Fishing apps are widely available on the market and are popular among anglers for their ability to predict fish activity. These apps typically gather environmental data such as temperature, barometric pressure, and wind speed through internet-based weather services. By combining this information with basic fishing heuristics—such as how weather conditions influence fish behavior—they aim to provide users with insights into the best times and locations to fish. However, the reliance on simplistic rules rather than advanced data-driven algorithms often results in limited prediction accuracy and adaptability, particularly in dynamic or less-studied fishing environments.

Most of these applications are designed for use on smartphones, leveraging their convenience and portability. However, they are heavily dependent on an active internet connection to fetch real-time data. This reliance introduces a significant limitation: in remote locations where internet access is unavailable or unreliable, these apps become unusable. Such scenarios are common in rural or wilderness fishing spots, leaving anglers without reliable tools to guide their efforts.

This project seeks to address these limitations by designing a fully offline solution that does not rely on internet connectivity. Instead, it integrates onboard sensors to collect local environmental data and employs advanced machine learning algorithms to provide accurate predictions. By eliminating the need for external data sources, this system ensures functionality in remote areas while offering a higher degree of reliability and precision compared to existing fishing apps. This offline capability not only enhances usability but also makes the system a more practical and versatile tool for anglers in diverse environments.

# 3. Technical Approach

## 3.1. System Architecture

The system is designed with a modular architecture comprising two main components: a peripheral device and a central device. Each component has distinct responsibilities that work in tandem to collect, process, and deliver fishing predictions. The two devices communicate wirelessly via Bluetooth, ensuring efficient data transfer and seamless functionality in real-time.

The **peripheral device** is built around the Arduino Nano 33 BLE, chosen for its low power consumption and integrated Bluetooth capabilities. Powered by a 500mAh LiPo battery, it is equipped with several onboard sensors to capture environmental data. These include:
- **Water Temperature Sensor**: Measures the temperature of the water.
- **Barometric Pressure Sensor**: Records atmospheric pressure for weather analysis.
- **Inertial Measurement Unit (IMU)**: Detects water surface activity based on motion data.
- **Magnetometer**: Monitors orientation to account for device positioning.

The peripheral device collects these data points and transmits them wirelessly to the central device, ensuring that the system can operate effectively even in remote locations.

The **central device**, powered by an ESP32 microcontroller, is the computational hub of the system. It processes the data received from the peripheral device and generates fishing predictions using pre-trained machine learning models. The central device is equipped with:
- A **Neo-6M GPS Module** for obtaining precise location data, critical for tailoring predictions to specific fishing spots.
- An **OLED Display** for presenting fishing recommendations and system status to the user.
- **Push Buttons** to allow for user interaction, such as inputting preferences or navigating the interface.
- A **500mAh LiPo Battery**, providing sufficient power for extended use in outdoor environments.

The communication between the peripheral and central devices relies on Bluetooth, ensuring fast and reliable data transfer without the need for internet connectivity. This modular design allows for flexibility and scalability, with the peripheral device specializing in data collection and the central device handling computation and user interaction. By separating these roles, the system achieves higher efficiency and adaptability, making it suitable for various fishing scenarios and environments.

## 3.2. Data Collection

The data collection process was designed to ensure a comprehensive dataset by incorporating variations in fishing skill levels, locations, environmental conditions, and weather patterns.

### Participants
Three participants with varying fishing expertise—beginner, intermediate, and expert—were selected to contribute to the dataset. This diversity ensures that the collected data reflects a wide range of fishing behaviors and skill levels, which is crucial for building a robust prediction model.

### Locations
Data was collected simultaneously at three distinct spots within the same waterbody. Peripheral devices were connected to the nRF App on smartphones for real-time logging. These spots were chosen to capture diverse environmental conditions, enhancing the dataset’s representativeness.

### Process
Each data collection session lasted 30 minutes, with participants rotating between the three locations every 10 minutes. This rotation ensured that each location included data from all three participants, thereby improving the objectivity and reliability of the dataset. Efforts were made to cover the entire lake during data collection, but certain areas, such as the North-West section, were inaccessible due to obstructions like reeds.

### Duration
Data collection was conducted over six days, with three hours of collection each day. The weather conditions during these six days were carefully distributed: two days were categorized as hot, two as cold, and two as moderate. This distribution allowed the dataset to capture the influence of varying weather conditions on fish behavior. Each day yielded 18 individual data entries, resulting in a total of 108 data points for the entire experiment.

The structured process of data collection, combined with diverse participant expertise, strategically chosen locations, and a variety of weather conditions, ensures that the dataset is comprehensive, reliable, and suitable for training the fishing prediction model.

## 3.3. Data Processing

The data collected during the experiment was processed to create a structured and reliable dataset for analysis and model training. Data from the nRF App was exported and consolidated into an Excel spreadsheet, including variables such as date, latitude, longitude, temperature, pressure, water activity, bite rate, and fishing spot location. The bite rate, calculated by summing the catches from all three participants at each location during a 30-minute session, served as the key target variable for the model. This approach ensured a comprehensive measure of fish activity, accounting for variations in skill levels and environmental conditions. The dataset was thoroughly reviewed for quality, with no missing values identified. However, minor GPS shifts were observed and manually adjusted to maintain accuracy. These steps ensured the dataset's readiness for machine learning applications, forming a solid foundation for reliable fishing predictions.

## 3.4. Model Training

![Decision Tree](https://github.com/JustinYa/AquaticCatchPredictor/blob/main/docs/media/decision_tree_regressor.png)

The prediction model was built using a decision tree regressor implemented in Python. The input features included environmental variables such as temperature, pressure, water surface activity, and geographical location. These variables were selected for their direct impact on fish behavior and bite rates. The dataset was split into training and testing sets using an 80/20 ratio to ensure a balanced evaluation of the model’s performance. The decision tree algorithm was chosen for its interpretability and ability to handle non-linear relationships within the data. 

## 3.5. Model Tuning

During initial training, the decision tree model exhibited signs of overfitting, with the training Mean Squared Error (MSE) being significantly lower than the testing MSE. To address this issue, parameters such as the maximum depth of the tree and the minimum number of samples required at a leaf node were introduced to constrain the model's complexity. The best performance was achieved with a maximum depth of 4 and a minimum of 4 samples per leaf, which balanced the trade-off between accuracy and generalization. This tuning step ensured that the model could make reliable predictions without overfitting to the training data.

# 4. Evaluation and Results

## 4.1. Results

The performance of the fishing prediction system was evaluated across multiple dimensions, including prediction accuracy, processing speed, and power efficiency. These evaluations reflect the system’s suitability for real-world fishing applications and its ability to operate under various constraints.

### Prediction Accuracy
The decision tree model demonstrated reliable predictive performance, achieving the following metrics:
- **Testing MSE**: 0.2498  
- **Training MSE**: 0.2751  
- **Testing RMSE**: 0.4998  
- **Training RMSE**: 0.5245  
- **Relative Error**: 10.12% (difference between Training MSE and Testing MSE)

The small difference between training and testing MSE indicates that the model has effectively avoided overfitting, maintaining good generalization across unseen data. The RMSE values further provide an interpretable measure of error in the same units as the target variable, confirming the model's accuracy for predicting fish bite rates.

### Processing Speed
The system achieved an average prediction speed of **15 ms** per query, enabling near-instantaneous feedback for users in real-time fishing scenarios. This rapid response ensures that anglers can make timely decisions based on current environmental conditions.

### Power Efficiency
The power efficiency of the system was analyzed for both the central and peripheral devices:

#### Central Device
- **Average Power Consumption**: 408.6 mW  
- **Battery Life**: Approximately 4.77 hours on a 500mAh battery  
- **Memory Usage**: 90%

The central device’s higher power consumption is attributed to its computational workload, which includes processing data, running machine learning models, and managing the user interface. Despite this, the device maintains sufficient battery life for typical fishing trips.

#### Peripheral Device
- **Average Power Consumption**: 70.2 mW  
- **Battery Life**: Approximately 27.76 hours on a 500mAh battery  
- **Memory Usage**: 23%

The peripheral device’s low power consumption and memory usage highlight its role as a dedicated data collection unit, with an extended battery life that ensures uninterrupted operation during long fishing sessions. Combined with the central device’s efficient power management and high computational capability, the results demonstrate a well-balanced system that excels in prediction accuracy, responsiveness, and energy efficiency. The low relative error confirms the reliability and generalization of the model, while the fast prediction speed makes it practical for real-time use. These features collectively ensure prolonged usability in outdoor environments, even under limited power availability, making the system a robust and effective tool for enhancing the fishing experience for both recreational and professional anglers.

## 4.2. Findings

A user study involving five participants provided valuable insights into how the fishing prediction system was used by different groups and its effectiveness under real-world conditions. 

### Beginners 
Beginners exhibited a high level of dependence on the system, using it more frequently to guide their fishing efforts. They primarily relied on the system's location recommendations to identify optimal fishing spots, particularly when navigating unfamiliar environments. Despite this reliance, their actual catch rates were generally lower than the system's predictions. This discrepancy can be attributed to two main factors: the participants' lack of fishing experience and the limitations of the training dataset. During data collection, the dataset did not include data from individuals with no fishing experience, leading to a slight mismatch between the model's predictions and the performance of completely inexperienced users. For this group, the system acted as a critical tool to improve their understanding of fishing conditions and strategies, even if their results initially fell short of predictions. 

To address this limitation in the future, a potential improvement could involve adding a **rating functionality** to the system. Users could self-assess and input their fishing skill level, allowing the system to appropriately scale its predictions based on their experience. This feature would enable more personalized and accurate recommendations for users of all skill levels.

### Intermediate and Advanced Users
Intermediate and advanced users demonstrated a different usage pattern, interacting with the system less frequently compared to beginners. These participants showed a high alignment between their actual catch rates and the system's predictions, reflecting their ability to effectively combine personal experience with system insights. For this group, the fish activity predictions were particularly valuable, as they used the system to refine their fishing strategies rather than as a primary guide. The system served as a supplementary tool to enhance their fishing efficiency and decision-making.

The study highlighted the system's ability to cater to a wide range of users. Beginners benefited most from the location recommendations, using the system to gain confidence and improve their results. In contrast, more experienced anglers used the system to enhance their existing skills, demonstrating its adaptability and effectiveness across skill levels. These findings underscore the system’s potential as both a learning aid and a strategic tool for diverse fishing scenarios, with opportunities for future enhancements such as skill-based scaling to improve its accuracy and usability.

# 5. Discussion and Conclusions

## Discussion

### Hardware Improvements
The system’s hardware components are functional but present opportunities for improvement. Replacing the current GPS module with an Adafruit GPS module could lower power consumption and support low-power modes, extending battery life for prolonged use. Additionally, optimizing the device enclosure to make it more compact and portable would enhance user convenience, especially for outdoor activities.

### Data Collection
Expanding the scope of data collection to include more sessions and diverse water bodies would significantly improve the model's accuracy and generalizability. Incorporating data from a wider range of environmental conditions and user experiences, including completely inexperienced anglers, could help address current limitations and refine the system’s predictions.

### Future Features
Future iterations of the system could include additional measurements such as water depth and dissolved oxygen to improve prediction accuracy. Furthermore, integrating a user rating feature would allow individuals to specify their skill level. This feature would enable the system to scale predictions accordingly, providing personalized recommendations tailored to each user’s expertise.

## Conclusions
The fishing prediction system successfully helps both beginners and experienced anglers by providing actionable fishing predictions. Beginners benefit most from location recommendations, frequently using the system as a guide to navigate unfamiliar conditions. Experienced users rely more on fish activity predictions to refine their strategies, using the system as a supplementary tool. 

The decision tree model performs well, achieving low testing errors and accurate predictions under diverse conditions. The system’s real-time prediction capability and low power consumption make it a reliable choice for extended fishing trips. User feedback underscores the system’s value, with 80% of participants expressing a willingness to recommend it to others.

Overall, the system demonstrates its potential to enhance the fishing experience for a wide range of users, while the proposed improvements and features highlight opportunities to further increase its effectiveness and user satisfaction.

# 6. Demo Links

* [Prediction Mode Demo Video](https://drive.google.com/file/d/1JTeIy1hggbkSafa4Nath8DWK5p9Q1THv/view?usp=sharing)
* [Location Suggestion & Navigation Modes Demo Video](https://drive.google.com/file/d/1JZ9xc8-V4VDqHVV1dyS5ZUOEnqnyqMGk/view?usp=sharing)

# 7. References

1. M. Iiyama, K. Zhao, A. Hashimoto, H. Kasahara, and M. Minoh, “Fishing spot prediction by sea temperature pattern learning,” *2018 OCEANS - MTS/IEEE Kobe Techno-Oceans (OTO)*, pp. 1–4, May 2018. doi: [10.1109/oceanskobe.2018.8559299](https://doi.org/10.1109/oceanskobe.2018.8559299)

2. M.-T. Thai, T.-N. Chu-Ha, T.-A. Vo, and T.-H. Do, “An approach to recommend fishing location and forecast fish production by using Big Data Analysis and distributed deep learning,” *2022 RIVF International Conference on Computing and Communication Technologies (RIVF)*, pp. 322–327, Dec. 2022. doi: [10.1109/rivf55975.2022.10013876](https://doi.org/10.1109/rivf55975.2022.10013876)

3. Gauravduttakiit, “Hyperparameter tuning in decision trees,” *Kaggle*, [https://www.kaggle.com/code/gauravduttakiit/hyperparameter-tuning-in-decision-trees](https://www.kaggle.com/code/gauravduttakiit/hyperparameter-tuning-in-decision-trees) (accessed Dec. 12, 2024).

