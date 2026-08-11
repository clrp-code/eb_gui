# eb_gui
A Graphical User Interface (GUI) made for EGSnrc application egs_brachy, with additional features for converting to and from DICOM format and analyzing 3ddose files.

Installation:

Ensure Qt 6 is installed (widgets and charts modules), along with an appropriate version of qmake. Navigate to the source code directory, and invoke the following commands:

> qmake;
> make
    
which will first create a Makefile using the appropriate Qt libraries on your computer qmake finds, then compile the actual program. The program is fairly robust and takes a while to compile, especially the database.cpp file which contains tag data for every DICOM tag. After compilation is complete, there should be an `eb_gui` executable in the parent directory.

On systems where both Qt 5 and Qt 6 are installed, invoke Qt 6's qmake explicitly (for example `qmake6` or the full path to Qt 6's `qmake`).
