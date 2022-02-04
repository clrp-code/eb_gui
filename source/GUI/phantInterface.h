/*
################################################################################
#
#  egs_brachy_GUI phantInterface.h
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
#ifndef PHANTINTERFACE_H
#define PHANTINTERFACE_H

#include <QtGui>
#include <iostream>
#include "../interface.h"

// Forward declaration of Interface to pass to the tab windows
class Interface;
class logWindow;

// Declaration of the Main Window class, its variables and its methods
class phantInterface : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
public:
    phantInterface();
    phantInterface(Interface* p);
    ~phantInterface();

    Interface *parent; // Pointer to the parent to access data with

public:
// LAYOUT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QGridLayout *mainLayout;
	
	QRegExpValidator allowedNums;
	
	// Select DICOM files
	QLabel*      dcmImport;
	
	QLabel*      phantNameLabel;
	QLineEdit*   phantNameEdit;
	
	QPushButton* ctImportFiles;
	QPushButton* ctImportDir;
	QListWidget* ctListView;
	QPushButton* ctDelete;
	QPushButton* ctDeleteAll;
	
	QLabel*      structLabel;
	QLineEdit*   structEdit;
	QPushButton* structLoad;
	
	QLabel*      calibLabel;
	QLineEdit*   calibEdit;
	QPushButton* calibLoad;
	
	QPushButton* create;
	
	QGridLayout* dcmGrid;
	QFrame*      dcmFrame;
	
	// Tissue assignment scheme
	QLabel*              contourAssign;
	
	QLabel*              defaultTASLabel;
	QComboBox*           defaultTASBox;
	
	QCheckBox*           truncBox;
	QLabel*              truncLabel;
	QLineEdit*           truncEdit;
	
	QLabel*              contourTASMaskLabel;
	QLabel*              contourTASLabelLabel;
	QLabel*              contourTASBoxLabel;
	QVector <QCheckBox*> contourTASMask;
	QVector <QLabel*>    contourTASLabel;
	QVector <QComboBox*> contourTASBox;
	QGridLayout*         contourScrollGrid;
	QFrame*              contourScrollFrame;
	QScrollArea*         contourScrollArea;
	
	QGridLayout*         contourGrid;
	QFrame*              contourFrame;
	
	// Contour priority
	QLabel*      prioContour;
	QListWidget* prioView;
	
	QGridLayout* prioGrid;
	QFrame*      prioFrame;
	
	// Metallic artifact reduction
	QLabel*      marLabel;
	
	QCheckBox*   marEnable;
	
	QLabel*      marTransLabel;
	QComboBox*   marTransformation;
	QLabel*      marDefaultLabel;
	QComboBox*   marDefault;
	QLabel*      marLTLabel;
	QLineEdit*   marLTEdit;
	QLabel*      marUTLabel;
	QLineEdit*   marUTEdit;
	QLabel*      marDenLabel;
	QLineEdit*   marDenEdit;
	QLabel*      marRadLabel;
	QLineEdit*   marRadEdit;
	
	QCheckBox*   marContour;
	QComboBox*   marContourBox;
	
	QGridLayout* marGrid;
	QFrame*      marFrame;
	
	// MAR settings arrays
	QStringList defMARs, LTs, UTs, Dens, Rads;

    // Generic GUI methods
    void createLayout();
    void connectLayout();
	
public slots:
    void refresh(); // Reset all on screen values properly
	
	// MAR functions
	void loadMARdefaults(); // Loads all the MAR files for default values
	void fillMARvalues(); // Changes the fields to match the MAR defaults
	
	// DICOM functions
	void loadStruct(); // Read in DICOM struct file
	void loadHU2rho(); // Change material file
	
	void loadCTFiles(); // Load CT files into memory
	void loadCTDir(); // Load CT file directory into memory
	void repopulateCT(); // Refill the CT item table
	void deleteCT(); // Remove CT files from memory
	void deleteAllCT(); // Remove all CT file from memory
	
	void mergeSort(QVector <DICOM *> &data, int n); // Sort DICOM CT by z
	void submerge(QVector <DICOM *> &data, int i, int c, int f); // Help sort DICOM CT by z
	
	int parseError (int err); // Output DICOM parsing error
	
	void createEGSphant(); // Invoke data's egsphant making function

public:
	logWindow* log; // log window
};

#endif