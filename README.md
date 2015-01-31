# REPOSITORY NAME

Arduino

# AUTHOR

Paul Rancuret

# DESCRIPTION

This repository is used as an eclipse workspace, containing Arduino projects for the Multirotor Geeks.  Once this directory has been checked out from git, projects from within the subdirectories can be created in the eclipse workspace.  See each subdirectory within this repository for instructions on how to set up the respective project.

# INFORMATION ABOUT ECLIPSE IDE

The [Eclipse IDE for C/C++ Developers](https://eclipse.org/downloads/packages/eclipse-ide-cc-developers/keplersr2) was used to create the projects within this repository.  The version used is: Luna Service Release 1a (4.4.1).

Eclipse can be customized in many ways, by clicking Window->Preferences.  This repository also contains a preferences file called [PaulsEclipsePrefs.epf](PaulsEclipsePrefs.epf) which provides the preferences I (Paul Rancuret) prefer to use on my computer.  You can set up your eclipse environment however you wish.  If you want to try importing my preferences as a starting point, you can do so using the following steps:
* Clone this repository onto your computer, so you have a local copy of the files (if you haven't already done so)
* Click File->Import...
* Under the 'General' submenu, select 'Preferences,' and click 'Next >'
* Browse to the PaulsEclipsePrefs.epf file on your local machine
* Select which preferences you want to import (or click import all), then click Finish

The following eclipse plug-ins were used when creating projects in this repository:
* AVR Eclipse Plugin (version 2.4.1)
* Uncrustify Plugin (version 1.0.0)

Details about each plugin are provided below:

## AVR Eclipse Plugin

[This plugin](http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_Eclipse_Plugin) is necessary to create projects for AVR microcontrollers (by Atmel), as it provides the toolchain for building and loading these projects.  These microcontrollers are used on Arduino boards.

To install the AVR Eclipse Plugin, follow these steps:
* In Eclipse, click Help->Install New Software...
* Next to 'Work With:' enter the following url: http://avr-eclipse.sourceforge.net/updatesite
* Select the 'AVR Eclipse Plugin' checkbox.  (Note: if it doesn't appear, you may already have it installed.  Click on the 'What is already installed?' link to check)
* Click 'Next >' and then follow prompts to install the plugin.
* Configure the Plugin for your computer, and set appropriate paths.
 * To use this plugin for [Arduino](http://www.arduino.cc/) projects, the Arduino IDE will first need to be [downloaded and installed](http://arduino.cc/en/Main/Software).
 * Next, configure the plugin in eclipse to point to the correct directories.  Help can be found on how to do so at [this website](http://playground.arduino.cc/Code/Eclipse) and [this website](http://interactive-matter.eu/how-to/developing-software-for-the-atmel-avr-with-avr-eclipse-avr-gcc-avrdude/).
 
 Once configured, using the toolchain provided by the plugin is simple.  In eclipse, the normal 'build' button can now be used to build AVR projects.  The plugin also provides a new button that says 'AVR' with a green arrow.  This button is used to download built projects into a serial-connected microcontroller.

## Uncrustify Plugin

This plugin allows the user to quickly and easily invoke the 'Uncristify' program from within eclipse.  This program is used to automatically adjust code formatting (white space, variable alignment, etc.) so that it has a clean, consistent look and feel.

To install the Uncrustify Plugin, follow these steps:
* [Download and Install](http://sourceforge.net/projects/uncrustify/files/uncrustify/) the uncrustify program on your computer.
* Clone the [Plugins](https://github.com/MultirotorGeeks/Plugins) repository from git, placing it in a known location on your computer.
* In Eclipse, click Help->Install New Software...
* Next to 'Work With:' enter file:/<path_to_local_Plugins_Repo> where <path_to_local_Plugins_Repo> is the path to the repository you just cloned.
* Select the 'Uncrustify Plugin' checkbox underneath Eclipse Plugins.  (Note: if it doesn't appear, you may already have it installed.  Click on the 'What is already installed?' link to check)
* Click 'Next >' and then follow prompts to install the plugin.
* Configure the uncrustify plugin using the following steps:
 * In Eclipse, click Window->Preferences and then select the 'Uncrustify' menu option on the left.
 * Next to 'Uncrustify path,' browse to the uncrustify.exe program which you just installed.
 * Keeping the 'Modify files in-place' option checked will prevent the program from creating backup copies of each file each time it is modified to clean up code formatting.  Since git version control is used for our files, this should be relatively safe, and will prevent adding a bunch of unnecessary files to version control.
 * In the extension list, provide a comma separated list of file extensions you wish to use the plugin for.  At this point, I am only using it for c,cpp,h files.  However, it can be used for other files as well, depending on the configuration file settings.
 * Next to 'Configuration File,' browse to a configuration file of your choice.  It will have a .cfg extension.  Here are several options for where to obtain a configuration file:
  * The [Plugins](https://github.com/MultirotorGeeks/Plugins) repository contains a configuration file called custom.cfg.  This is the configuration used when cleaning up the source files in this [Arduino](https://github.com/MultirotorGeeks/Arduino) repository.  This one is recommended when modifying files in this repository.
  * The uncrustify program installation includes some sample configuration files, in the same directory as the executable.
  * You can create your own, or modify an existing one.  The files are standard ascii, so they can be modified by any text editor.  There are online tools such as [UniversalIndentGUI](http://universalindent.sourceforge.net/) which provide a graphical way of creating this file as well.
  
Once the plugin is installed, it can be used to clean code formatting of entire files, or of selections within files.  To clean up one or more files in eclipse, select the file(s) from within the 'Project Explorer,' then right click and select 'Uncrustify Selection.'  To clean up text within a file, simply highlight the text, right click it, and select 'Uncrustify Selection.'