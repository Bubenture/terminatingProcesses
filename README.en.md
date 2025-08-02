#### The C++ program is designed for managing processes in the Windows operating system. It allows users to select active processes and set timers for their termination after a specified number of seconds. The program utilizes multithreading to simultaneously execute timers for multiple processes.

</br>
<div>
    <a href="README.md">
        <img src="https://img.shields.io/badge/README-RU-blue?color=006400&labelColor=006400&style=for-the-badge">
    </a>
    <a href="README.en.md">
        <img src="https://img.shields.io/badge/README-ENG-blue?color=44944a&labelColor=1C2325&style=for-the-badge">
    </a>
</div>

### Main Functions of the Program:
- **Getting the Process List**: The program creates a snapshot of all active processes using the `CreateToolhelp32Snapshot` function and stores the information in a vector.
- **String Conversion**: To work with process names that may be in `std::wstring` format, the program uses the `wstringToString` function to convert them to `std::string`.
- **Terminating Processes**: The `closeProcess` function starts a timer for the specified number of seconds, after which it terminates the process using the `taskkill` command.
- **User Interface**: The program displays a list of all active processes and prompts the user for the PID (Process Identifier) and the time until termination.
- **Multithreading**: A separate thread is created for each selected process, which starts the timer and terminates the process after the time elapses.

### How the Program Works
- **Initialization**: The program begins by initializing the necessary libraries and data structures. A mutex is created to ensure thread safety.
- **Getting Processes**: The `getProcesses` function is called, which fills the `allProcesses` vector with information about all active processes.
- **Displaying Processes**: The program outputs a list of all processes along with their PIDs and names.
- **User Input**: The user inputs the PIDs of the processes they wish to terminate and the time in seconds until termination. This data is stored in the `processes` vector.
- **Starting Timers**: For each process in the `processes` vector, a thread is created that runs the `closeProcess` function. This function waits for the specified time and then terminates the process.
- **Waiting for Thread Completion**: The main thread of the program waits for all created threads to finish.
- **Termination**: After all processes have been terminated, the program outputs a message indicating that it has completed its operation.

### Program Flow Diagram
```
+---------------------+
|   Start Program     |
+---------------------+
          |
          v
+---------------------+
|  Get Processes      |
|   (getProcesses)    |
+---------------------+
          |
          v
+---------------------+
|  Display List of    |
|  Active Processes   |
+---------------------+
          |
          v
+---------------------+
|  Input PID and Time |
|  Until Termination  |
+---------------------+
          |
          v
+---------------------+
| Create Threads for  |
| Process Termination |
+---------------------+
          |
          v
+---------------------+
|  Wait for All       |
|  Threads to Finish  |
+---------------------+
          |
          v
+---------------------+
|  End of Program     |
+---------------------+
```
