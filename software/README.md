# Software Folder

This folder contains all the necessary software files for the **Fishing Prediction System**. It includes Arduino IDE sketches for the central and peripheral devices, as well as a Python script for training the decision tree model.

## Contents

### 1. **Arduino Files**
These `.ino` files are used to program the central and peripheral devices using the **Arduino IDE**:
- **`ble_central.ino`**:  
  The code for the **central device**, handling data collection, Bluetooth communication, and predictions using pre-trained models.
- **`ble_peripheral.ino`**:  
  The code for the **peripheral device**, responsible for gathering environmental data (temperature, pressure, etc.) and transmitting it to the central device via Bluetooth.

### 2. **Python Script**
- **`decision_tree_training.ipynb`**:  
  This Python notebook is used to train the **decision tree model**. It processes collected data, trains the model, and exports the decision tree rules for integration into the Arduino sketches.

## Usage Instructions
1. **Arduino Sketches**:  
   - Open `ble_central.ino` and `ble_peripheral.ino` in the Arduino IDE.
   - Upload the respective code to the central and peripheral devices to enable system functionality.
2. **Python Script**:  
   - Run `decision_tree_training.ipynb` in a Jupyter Notebook environment to train the decision tree model.
   - Export the trained model logic and incorporate it into the central device's Arduino code for predictions.

## Notes
- Ensure all required libraries for Arduino IDE are installed before uploading the sketches.
- The Python script requires standard machine learning libraries like `scikit-learn` for training the decision tree model.

For further assistance, refer to the main repository README or contact the project team.
