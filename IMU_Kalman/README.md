# PROJECT NAME

IMU_Kalman
  
# AUTHORS

* Paul Rancuret (pmrancuret@gmail.com)
* John McArthur (johnnyfisma@gmail.com)

# DESCRIPTION

This project, by Paul Rancuret and John McArthur, creates code for the ArduIMU (v3) board which includes Kalman filtering.

This software with the ArduIMU (v3) board creates a system which reads three-axis gyroscopes, accelerometers, and magnetometers, and estimates angular position and rate of change for the x, y, and z axis using a Kalman filtering technique.

# DEPENDENCIES

* GyroKalman
* MPU6000
* HMC5883
* Wire
* SPI
* avrfix
* Arduino_Duemilanove_w__ATmega328
  
# TARGET APPLICATION PROJECT SETUP

This project was created as an eclipse C++ project of type 'AVR Cross Target Application,' using the 'AVR-GCC' toolchain. Eclipse IDE for C/C++ Developers version 4.4.1, with the AVR Eclipse Plugin version 2.4.1.
  
To create the project in your eclipse workspace (assuming you have already checked out this folder from git), do the following:
* Ensure the AVR Eclipse Plugin is installed in your version of Eclipse
 * To do this, Click Help->Install New Software... and type http://avr-eclipse.sourceforge.net/updatesite in the 'Work With:' prompt.  Then, check the box for 'AVR Eclipse Plugin' if it is not already installed and follow on-screen installation instructions.
* Click File->New->C++ Project
* Enter IMU_Kalman next to 'Project Name'
* Uncheck 'Use default location' and browse to the location of the 'IMU_Kalman' folder checked out from git.
* In 'Project Type,' expand the 'AVR Cross Target Application' dropdown and select 'Empty Project'
* In 'Toolchains,' select AVR-GCC Toolchain and then click 'Next >'
* Optionally, un-check the 'Debug' configuration if you only plan to use the 'Release' configuration.  Click 'Next >'
* Enter the correct processor type and clock speed you are using, and click Finish.  For the ArduIMU board, the processor type is ATmega328p, and the clock speed is 16000000.
  
In order to use this target application project, all library files listed in the dependencies section must all be linked into the build.  To do this, right click the project and select Properties.  Then, expand the 'C/C++ build' submenu on the left and click on 'Settings.'  In the right side, under the 'Tool Settings' tab, expand the 'AVR C++ Linker' submenu and click on 'Libraries.'  In the 'Libraries (-l)' box on the right, add all the library names exactly as listed in the DEPENDENCIES section of this document.  Note that you should NOT enter the prefix 'lib' at the beginning, or the extension '.a' at the end of these library names.  Also, ensure that they are listed in the same order as described in this file.  In the 'Libraries Path (-L)' box on the lower right, add the paths to the libraries.  If your eclipse workspace is located at the top 'Arduino' folder of this git repository, and the full repository is checked out including libraries, then the following paths should work:
* "${workspace_loc}/libraries/GyroKalman/Release"
* "${workspace_loc}/libraries/MPU6000/Release"
* "${workspace_loc}/libraries/HMC5883/Release"
* "${workspace_loc}/libraries/Wire/Release"
* "${workspace_loc}/libraries/SPI/Release"
* "${workspace_loc}/libraries/avrfix"
* "${workspace_loc}/libraries/ArduPilotBoardSupport"
 
The target application project will also need to have include paths set up to find the library header files.  To do this, right click the project and select Properties.  Then, expand the C/C++ build submenu on the left and click on 'Settings.'  In the right side, under the 'Tool Settings' tab, expand both the 'AVR Compiler' and the 'AVR C++ Compiler' submenus.  Under each of these, select Directories, then click the 'Add...' button at the top right (with the green + sign).  In this dialog, enter the paths to the directories containing each of the required header files. If your eclipse workspace is located at the top 'Arduino' folder of this git repository, and the full repository is checked out including libraries, then the following path should work for the libraries:
* "${workspace_loc}/libraries/GyroKalman"
* "${workspace_loc}/libraries/MPU6000"
* "${workspace_loc}/libraries/HMC5883"
* "${workspace_loc}/libraries/Wire"
* "${workspace_loc}/libraries/SPI"
* "${workspace_loc}/libraries/avrfix"
* "${workspace_loc}/libraries/ArduPilotBoardSupport"