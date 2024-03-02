# USB Keyboard Sniffer Device
![enter image description here](https://github.com/bbas121/UsbKeyboardSniffer/blob/main/logo.jpg?raw=true)
## Use cases
This device can be inserted between a keyboard and a computer to remotely monitor keystrokes, capture passwords, and remotely operate the computer's keyboard. 

## Personal Reflection

This project was to apply the knowledge I gained from an STM32 USB drivers course, combined with my prior experience with STM32  in general, as well as my experience with the ESP32 from past projects. Initially, I thought it would be a fast project to complete; however, it demanded an entire week of effort. Despite this, the result is very good. I am convinced that by designing a custom PCB and utilizing a single STM32 equipped with dual USB modules, it's possible to create a much more compact device.
Please consider this project as a testament to my skills and potential for future employment opportunities.


## Summary
This device serves as an intermediary between a USB keyboard and a computer. By connecting the keyboard to the device's USB input and the device's USB output to the computer, it allows for standard keyboard usage. Additionally, the device hosts a web server that creates a hidden SSID hotspot. Users can connect to this web server to view keystrokes in real-time and remotely inject keystrokes.
## Features
This device offers several key features:

-   Real-time monitoring of keyboard inputs as text.
-   Storage of raw input data on an SD card, with the information saved in a text file that can be easily downloaded.
-   The ability to view the entire keyboard HID (Human Interface Device) structure in real-time, including up to six characters that can be pressed simultaneously, along with the status of control keys (Ctrl, Shift, Start, Alt) and a timestamp featuring the date and time.
-   An option to download the complete HID structure data in CSV format. That is stored in a memory card
-   Capability to set the internal real-time clock and date of the controller.
-   Remote key injection feature, allowing the user to send keystrokes to the host computer from afar.
-   A "rick roll" button designed for practical jokes, which opens the music video for Rick Astley's "Never Gonna Give You Up" on the remote computer.
## Video

[![Alt text](https://i.ibb.co/ZJzNykh/YOutube.jpg)](https://www.youtube.com/watch?v=iMoOqLtzJ7c&t=1s)

# Web server
The web server is configured with a static IP address: 192.618.1.184. On its homepage, there's a description of its capabilities along with several interactive buttons:

![enter image description here](https://i.ibb.co/YL9R3TW/home-Page-1-80-1.webp)
## Show TXT File - Capture keystrokes in realtime 
On this page, you can see in real-time raw keyboard input as text with an option to download the Txt file and clear the file.
![enter image description here](https://i.ibb.co/bXswWFx/ShowTxt.jpg)

## Show CSV file  - Display the full HID key structure
The HID (Human Interface Device) structure includes six keys in addition to control keys. Each keystroke is captured and logged in a CSV format, complete with timestamps that mark the date and time. The control keys consist of Shift, Ctrl, Alt, and the Start button. Additionally, special keys, such as Tab, Caps Lock, and Space, are recognized as ASCII keys that are not control keys.
![enter image description here](https://i.ibb.co/Y3cpkrt/ShowCSV.jpg)

## Set Time For CSV TimeStamp
On this page, you have the ability to configure the internal RTC (Real Time Clock) of the ESP32 with the desired time and date to be used in the CSV time and date stamp.
![enter image description here](https://i.ibb.co/KGFVT1k/SetTime.jpg)
## Key injection
On this page, you can input keystrokes that will be transmitted to the host PC. It supports sending only one key at a time, including control keys. To send a control key in combination with another key, you must select the checkbox corresponding to the control key.
![enter image description here](https://i.ibb.co/QH9DdzG/Key-Injection.jpg)
## Activate Rick Roll
That button will press Start + R button to open the run window and type "chrome youtube.com/watch?v=dQw4w9WgXcQ" to open  "Rick Astley - Never Gonna Give You Up" in Youtube on the host PC

![enter image description here](https://i.ibb.co/VMyvpmW/RickRoll.jpg)

## Hardware

-   The STM32F411 serves as a USB host, capturing keystrokes from a connected keyboard.
-   The STM32F103CBT functions as a USB HID (Human Interface Device) keyboard.
-   The ESP32-WROOM-32 operates as a hotspot with a hidden SSID, hosting a web server.
-   A memory card adapter equipped with 47k ohm resistors for pull-up.

## Schematics
![enter image description here](https://i.ibb.co/wJvzRDD/Schematic.jpg)

![enter image description here](https://i.ibb.co/4P2dS0v/IMG-20240217-203609.jpg)
- I've used green tape to secure the memory card since it doesn't stay in place on its own. 
- The small soldering breadboard is there for the pull-up resistors.
## Overview of connections between the boards

| STM32F411  | Usb Input | STM32F103CBT | ESP32 | Micro SD Card |
|------------|-----------|--------------|-------|---------------|
| 5V         | VBUS      | 5V           | 5V    |               |
| GND        | GND       | GND          | GND   |               |
| D11        | D-        |              |       |               |
| A12        | D+        |              |       |               |
| A2  UART2 TX       |           | B11  UART3 RX         |       |               |
|            |           | A3  UART2 RX         | G17 TX2   |               |
|            |           | B10 UART3 TX         | G16 RX2  |               |
|            |           |          	| G12   |   DATA2(Pull up to 3.3V)      |
|            |           |        		| G13   |   DATA3 (Pull up to 3.3V)      |
|            |           |        		|  G15  |   CMD         |
|            |           |        		|  3.3  |   VDD         |
|            |           |        		|  G14  |   CLK         |
|            |           |        		|  GND  |   VSS         |
|            |           |        		|  G2  |   DATA0(Pull up to 3.3V)         
|            |           |        		|  G4  |   DATA1(Pull up to 3.3V)         |

# Working principle

## Memory Card Management

The ESP32 utilizes the SD_MMC library for managing the memory card and the FS library for file system.

## Relay Keystrokes from USB Input to USB Output

 -   The STM32F411 operates as a USB Host, capturing keyboard inputs and storing them in a structure known as HID_KEYBD_Info_TypeDef, which is part of the STM32 USB host driver.
 -   This captured data is then transmitted to the STM32F103CBT via UART by the STM32F411.
 -   The STM32F103CBT Acting as a USB keyboard using STM32 USB HID device driver. it receives keystroke data for the STM32F411 using UART. It converts the incoming structure into a USB device keystroke structure named keyboardHID from the STM32 USB Device driver.
 - The STM32F103CBT forwards these keystrokes to the USB output.
 -   All necessary drivers are provided by STMicroelectronics.

## Logging Keystrokes to a Text File on the ESP32

-   The STM32F103CBT transfers the received keystroke data to the ESP32.
-   Upon receiving the data, the ESP32 extracts the HID code of the first key from the structure and converts it into ASCII format using a function provided by the STM32 USB driver.
-   The converted key is then saved to a memory card in a file named "key_info.txt"
-  A boolean variable tracks the state of the Caps Lock, toggling with each press. This Caps Lock state is maintained internally and may not synchronize with the host PC, potentially leading to discrepancies.
-   When Caps Lock is active or the Shift key is pressed, ASCII letters are converted to uppercase.
-   Conversely, if both Caps Lock is active and the Shift key is pressed, ASCII letters are converted to lowercase.


## Recording the Complete HID Structure in a CSV File

-   The ESP32 leverages the ESP32Time library to utilize the real-time clock.
-   When the keystroke structure is received, the ESP32 converts each HID key into its ASCII equivalent and records each element of the structure in a CSV format, storing it on the memory card within the "key_info.csv" file.
-   The recorded structure comprises six keys, control keys, and a separate field for special keys, which are not part of the HID structure.
-   For special keys represented in ASCII, a specific function is available to transform non-character ASCII keys into descriptive strings for the key's function, For example the Caps Lock, Tab, Space etc..



##   Web Server Management

The ESP32 employs the WebServer library to establish and manage a web server for presenting the webpage. Additionally, it utilizes the ArduinoJson library for handling JSON data within the server's operations.

## Displaying Text and CSV Files on a Web Page
The web server fetches the text and CSV files from the memory card and displays them on a web page. It uses JavaScript to dynamically refresh the content, ensuring up-to-date information is shown.


## Key Injection Feature
The key injection interface captures keystrokes through JavaScript event codes, then employs a function to convert these codes into the keyboardHID format for transmission to the STM32F103CBT, which operates as a USB keyboard. Given that the event codes capture only one key at a time, to include additional control keys in the keystrokes, users must select the corresponding control key checkboxes. The ESP32 receives the checkbox states and adjusts the control key states in the keyboardHID structure accordingly before sending it.


## Rick Roll Feature
The Rick Roll feature operates similarly to the key injection function, sending keystrokes to the STM32F103CBT, which functions as a USB keyboard. It initiates the Win + R command to open the Run dialog, inputs "chrome [https://www.youtube.com/watch?v=dQw4w9WgXcQ](https://www.youtube.com/watch?v=dQw4w9WgXcQ)" into the field, and then hits Enter. Following each keystroke sent, the function dispatches an empty structure to the PC to signal the release of the key press.
 

# Additional Note
- The STM32F411 has a USB Type-C port, so you can just plug in a USB OTG adapter directly—no need to solder on a female USB-A connector like I ended up doing. I didn't have the right cable on hand, so I just worked with what I had.
-   Connecting the 3.3V to the memory card module during boot causes a boot loop, the reason for which is unclear. To avoid this, disconnect the 3.3V from the memory card during boot. Once you observe "Connecting to WiFi.." in the serial output, reconnect the 3.3V to the memory card.
-   The ESP32-CAM module, which includes an integrated memory card slot, functions properly without this issue.
- Alternatively, you can utilize SPI memory card modules.
-   When using the ESP32-CAM, it's necessary to alter the pin assignment for Serial2 TX to G0, as the default pin conflicts with other uses. Implement this adjustment in the `void setup` with the command `Serial2.begin(115200, SERIAL_8N1, 16, 0);`.
Additionally, connect the B10 pin on the STM32F103CBT to the reassigned pin, G0, on the ESP32.



