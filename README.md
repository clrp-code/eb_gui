# eb_gui
A Graphical User Interface (GUI) made for EGSnrc application egs_brachy, with additional features for converting to and from DICOM format and analyzing 3ddose files.

Installation:

Ensure Qt5 is installed, along with an appropriate version of qmake along with it.  Navigate to the source code directory, and invoke the following commands:

> qmake
> make
    
which will first create a Makefile using the appropriate Qt libraries on your computer qmake finds, then compile the actual program.  The program is fairly robust and takes a while to compile, especially the database.cpp file which contains tag data for every DICOM tag.  After compilation is complete, there should be an eb_gui file that is executeable in the parent directory.
