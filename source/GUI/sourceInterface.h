/*
################################################################################
#
#  egs_brachy_GUI sourceInterface.h
#  Copyright (C) 2021 Shannon Jarvis, Martin Martinov, and Rowan Thomson
#
#  This file is part of egs_brachy_GUI
#
#  egs_brachy_GUI is free software: you can redistribute it and/or modify it
#  under the terms of the GNU Affero General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  egs_brachy_GUI is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Affero General Public License for more details:
#  <http://www.gnu.org/licenses/>.
#
################################################################################
#
#  When egs_brachy is used for publications, please cite our paper:
#  M. J. P. Chamberland, R. E. P. Taylor, D. W. O. Rogers, and R. M. Thomson,
#  egs brachy: a versatile and fast Monte Carlo code for brachytherapy,
#  Phys. Med. Biol. 61, 8214-8231 (2016).
#
#  When egs_brachy_GUI is used for publications, please cite our paper:
#  To Be Announced
#
################################################################################
#
#  Author:        Shannon Jarvis
#                 Martin Martinov (martinov@physics.carleton.ca)
#
#  Contributors:  Rowan Thomson (rthomson@physics.carleton.ca)
#
################################################################################
*/
#ifndef SOURCEINTERFACE_H
#define SOURCEINTERFACE_H

#include <QtGui>
#include <iostream>
#include "../interface.h"

// Forward declaration of Interface to pass to the tab windows
class Interface;

// Declaration of the Main Window class, its variables and its methods
class sourceInterface : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
public:
    sourceInterface();
    sourceInterface(Interface* p);
    ~sourceInterface();

    Interface *parent; // Pointer to the parent to access data with
	
public:
// LAYOUT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QGridLayout *mainLayout;

    // Generic GUI methods
    void createLayout();
    void connectLayout();
	
	// Import DICOM data
	QFrame*      planFrame;
	QLabel*      planLabel;
	QGridLayout* planLayout;
	
	QLabel*      fileLabel;
	QLineEdit*   fileEdit;
	QPushButton* fileLoad;
	
	QPushButton* importButton;
	
	QLabel*      tagLabel;
	QLineEdit*   tagEdit;
	
	QLabel*      isotopeLabel;
	QLineEdit*   isotopeEdit;
	
	QLabel*      seedLabel;
	QLineEdit*   seedEdit;
	
	QLabel*      seedCountLabel;
	QLineEdit*   seedCountEdit;
	
	QLabel*      transformationLabel;
	QLineEdit*   transformationEdit;
	
	QLabel*      dwellLabel;
	QLineEdit*   dwellEdit;
	
	QLabel*      airKermaLabel;
	QLineEdit*   airKermaEdit;
	
public slots:
    void refresh (); // Reset all on screen values properly
	
	// DICOM functions
	void loadPlan (); // Read in DICOM plan file
	
	void parsePlan (); // Parse DICOM plan file
	
	int parseError (int err); // Output common DICOM errors

public:
	logWindow* log; // log window
};

#endif
