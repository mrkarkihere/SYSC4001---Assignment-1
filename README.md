# Assignment 1: Emulation of a Simplified Cyber-Physical System (CPS) Using Software Processes and Message Passing

## Running on Linux (Ubuntu specifically)

### Prerequisites
Make sure you have the necessary build tools installed on your Linux system, including `gcc`. You also need to have the `gnome-terminal` installed.

### NOTE
This will start 1 ECG server and 1 Image server and ONLY 1 Main CPS (2 clients, 1 ECG and 1 Image). To connect multiple servers, execute ```./MainCPS``` on a new terminal.

### Running the Application
1. Clone or download the project to your local machine.
2. Open a terminal and navigate to the project directory.
3. Execute the following command to compile the project:
    ```
    ./CPSSystem
    ```
4. This will open multiple terminal windows and run the necessary components of the application.

## Running on macOS

### Prerequisites
Ensure that you have the Xcode Command Line Tools installed, including the `gcc` compiler.

### Running the Application
1. Clone or download the project to your local machine.
2. Open a terminal and navigate to the project directory.
3. Execute the following command to compile the project:
    ```
    ./CPSSystemMacOS
    ```
4. This will compile all the necessary files. Execute each executable file on a seperate terminal.

## Additional Notes
Make sure to check for any specific system requirements mentioned in the project documentation. If you encounter any issues, refer to the troubleshooting section or contact the project maintainers for support.

## IMPORTANT
In the event 'CPSSystem' or 'CPSSystemMacOS' are not executable, use the following command to give permission to execute:
```chmod +x CPSSystem``` or ```chmod +x CPSSystemMacOS``` 
depending on your OS.