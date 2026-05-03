VS Final Code






import serial
import csv
import time
import os
import serial.tools.list_ports
import pandas as pd
from openpyxl import load_workbook

# Function to list available COM ports
def list_serial_ports():
    ports = serial.tools.list_ports.comports()
    return [port.device for port in ports]

# ✅ Update this if your ESP is on another port
port = "COM5"
baudrate = 115200

# Save single Excel file (all subjects in different sheets)
script_dir = os.path.dirname(os.path.abspath(__file__))
excel_filename = os.path.join(script_dir, "esp_log.xlsx")

try:
    ser = serial.Serial(port, baudrate, timeout=1)
    time.sleep(2)  # wait for connection
except Exception as e:
    print(f"Error opening {port}: {e}")
    exit()

print("Logging started... (will stop after 60 values)")

header = ["EMG_Raw", "EMG_Smoothed", "Accel_X", "Accel_Y", "Accel_Z",
          "Accel_Mag", "Gyro_X", "Gyro_Y", "Gyro_Z", "Gyro_Mag"]

rows = []

count = 0
while count < 60:
    line = ser.readline().decode("utf-8").strip()
    if line:
        data = line.split("\t")
        try:
            numeric_data = [float(x) for x in data]  # convert to floats
            rows.append(numeric_data)  # store in memory
            print(f"Logged: {numeric_data}")
            count += 1
        except ValueError:
            continue

ser.close()

# ✅ Convert rows to DataFrame
df = pd.DataFrame(rows, columns=header)

# ✅ Add averages as last row
avg_row = df.mean(numeric_only=True).to_frame().T
avg_row.index = ["Average"]
df = pd.concat([df, avg_row])

# ✅ Ask for subject name (so each run gets its own sheet)
subject_name = input("Enter subject name: ").strip()
if not subject_name:
    subject_name = f"Subject_{int(time.time())}"  # fallback unique name

# ✅ Save into the same Excel, different sheets
if os.path.exists(excel_filename):
    with pd.ExcelWriter(excel_filename, mode="a", engine="openpyxl", if_sheet_exists="replace") as writer:
        df.to_excel(writer, sheet_name=subject_name, index=False)
else:
    with pd.ExcelWriter(excel_filename, engine="openpyxl") as writer:
        df.to_excel(writer, sheet_name=subject_name, index=False)

print(f"✅ Data saved to {excel_filename} in sheet '{subject_name}'")
