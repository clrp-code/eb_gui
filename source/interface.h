/*
################################################################################
#
#  egs_brachy_GUI interface.h
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
#ifndef INTERFACE_H
#define INTERFACE_H

#include <QtGui>
#include "data.h"
#include "libraries/gzstream.h"

// Global QFrame widgets' frame line width
#define LINE_WIDTH 2

// Global maximum structure input/output count
#define STRUCT_COUNT 100

// List of media indices used in egsphants
#define EGSPHANT_CHARS "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

// Regex strings used to validate different input types
// 0) [-]? - Allow negative sign
// 1) \\d - Look for at least one decimal character
// 2) \\d* - Look for any number of decimal characters
// 3) ([.]\\d*)? - Allow a decimal point followed by any number of decimal characters
// 4a) ([eE][+-]? - Allow an e or E character, followed by an allowed plus or minus character
// 4b) \\d{1,3})? - and look for 1-3 digits to follow the 4a input
#define REGEX_REAL "[-]?\\d\\d*([.]\\d*)?[eE][+-]?\\d{1,3}"
#define REGEX_REAL_POS "\\d\\d*([.]\\d*)?[eE][+-]?\\d{1,3}"

// Same as above, but add a space or comma afterwards and allow repetition
#define REGEX_REAL_POS_ARR "(\\d\\d*([.]\\d*)?([eE][+-]?\\d{1,3})?[ ,])*"

// 0) [-]? - Allow negative sign
// 1) \\d* - Look for any number of decimal characters
#define REGEX_NATURAL "[-]?\\d*"
#define REGEX_NATURAL_POS "\\d*"

// 1) \\d - Look for one decimal character
// 2) (\\d)? - Allow a second decimal character
// 3) ([.]\\d*)? - Allow a period followed by up to five decimal characters afterwards
// 3) | - Or
// 4) 100 - Look for 100
#define REGEX_PERCENT "((\\d(\\d)?([.]\\d{1,5})?)|100)"

// Same as above, but add a space or comma afterwards and allow repetition
#define REGEX_PERCENT_ARR "(((\\d(\\d)?([.]\\d{1,5})?)|100)[ ,])*"

// Forward declare the EGS_geom class defined at the bottom so that they can
// be included in the layout
class EGS_geom;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Interface~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaration of the main window class, its variables and its methods
class Interface : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
	
public:
    Interface();
    ~Interface();
	
// DATA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
public:
	Data* data;
	EGSInput* egsinp;
	
public slots:
	int populateEgsinp();
	
// GLOBAL WIDGETS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
	void createGlobalWidgets();

	QRegExpValidator allowedNums; // Positive, real numbers
	bool firstDelete = true; // Ask if they're sure they want to delete something the firstDelete
						     // time they try
	//Maximum dwell time
	double maxDwellTime=0;
	// Phantom list	
	QLabel*      phantomLabel;
	QCheckBox*   phantomOnlyLocal;
	QListWidget* phantomListView;
	QPushButton* phantomCreate;
	QPushButton* phantomView;
	QPushButton* phantomDelete;
	
	QGridLayout* phantomGrid;
	QFrame*      phantomFrame;

	// Source list	
	QLabel*      sourceLabel;
	QComboBox*   sourceChooser;
	QCheckBox*   sourceShowWrapped;
	QListWidget* sourceListView;
	
	QComboBox*   sourceScaleBox;
	QLineEdit*   sourceScaleEdit;
	
	QCheckBox*   sourcePermTime;
	QLabel*      sourceTempTimeLabel;
	QLineEdit*   sourceTempTimeEdit;
	
	QGridLayout* sourceGrid;
	QFrame*      sourceFrame;

	// Transformations list	
	QLabel*      transformationLabel;
	QCheckBox*   transformationOnlyLocal;
	QListWidget* transformationListView;
	QPushButton* transformationCreate;
	QPushButton* transformationView;
	QPushButton* transformationDelete;
	
	QCheckBox*   transformationDwell;
	QLabel*      transformationDwellLabel;
	QPushButton* transformationDwellButton;
	QLineEdit*   transformationDwellEdit;
						  
	QGridLayout* transformationGrid;
	QFrame*      transformationFrame;

	// Geometry list	
	QLabel*      geometryLabel;
	QComboBox*   geometryChooser;
	QListWidget* geometryListView;
	QPushButton* geometryCreate;
	QPushButton* geometryDelete;
	QPushButton* geometryAdd;
	QTabWidget*  geometryList;
						  
	QGridLayout* geometryGrid;
	QFrame*      geometryFrame;

	// Dose list	
	QLabel*      doseLabel;
	QListWidget* doseListView;
	QPushButton* doseImport;
	QPushButton* doseDelete;
	
	QGridLayout* doseGrid;
	QFrame*      doseFrame;
	
public slots:
	void phantomRepopulate();
	void sourceRepopulate();
	void transformationRepopulate();
	void geometryRepopulate();
	void doseRepopulate();
	
	void geometryAddNew();
	
	//void phantomCreateFile();
	//void transformCreateFile();
	
	void phantomViewLog();
	void transformViewLog();
	
	void phantomDeleteFile();
	void transformDeleteFile();
	
// TABS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
	QWidget* phantInt; // phantInterface holder for layout purposes
	QWidget* sourceInt; // sourceInterface holder for layout purposes
	QWidget* ebInt; // ebInterface holder for layout purposes
	QWidget* doseInt; // doseInterface holder for layout purposes
	QWidget* appInt; // appInterface holder for layout purposes
	
// LAYOUT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
    QGridLayout *mainLayout;
    QPushButton *closeButton;
	
    QTabWidget *tabWidget;
	
    // Generic GUI methods
    void createLayout();
    void connectLayout();
	
public slots:
    void refresh(); // Reset all on screen values properly
    void sourceRefresh(); // Reset all on screen values properly
    void transformationRefresh(); // Reset all on screen values properly
    void transformationLoadDwells(); // Manually change activity/dwell file
    void transformationLoadDwellsAuto(int i); // Automatically change activity/dwell file

// PROGRESS BAR~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
    double *progLevel;
    QWidget *progWin;
	QLabel *progLabel;
    QGridLayout *progLayout;
    QProgressBar *progress;
	
	void createProgress();
	void connectProgress();
	void deleteProgress();
	
public slots:
	void resetProgress(QString title);
	void nameProgress(QString text);
	void updateProgress(double percent);
	void finishedProgress();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// EGS_geom~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaration of the geometry class, a widget which holds egs_brachy geometries
// outside of sources and phantoms
class EGS_geom : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
	
	// main class objects
public:
	QString name;
	int index;
	QRegExpValidator allowedNums;
	QIntValidator allowedPrios;
	
	// Transformations list	
	QLabel*      labelTab;
	QLabel*      labelGeom;
	QPushButton* remove;
	QLabel*      labelPos;
	QLineEdit*   xPos;
	QLineEdit*   yPos;
	QLineEdit*   zPos;
	QLabel*      labelRot;
	QLineEdit*   xRot;
	QLineEdit*   yRot;
	QLineEdit*   zRot;
	QLabel*      labelPrio;
	QLineEdit*   Prio;

	QGridLayout* geomGrid;
	
	EGS_geom();
	EGS_geom(QString n, int ind);

    // Generic GUI methods
    void createLayout();
	
public slots:
	void deleteMyself();
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// logWindow~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaration of the console window class, used for egs_brachy output
class logWindow : public QMainWindow {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
             // that define what happens when you click on buttons
public:
    logWindow();
	~logWindow();
	
	// GUI Layout
	QWidget* frame;
	QGridLayout* layout;
	QPlainTextEdit* outputArea;
	QPushButton* close;

    // Generic GUI methods
    void createLayout();
    void connectLayout();
};
#endif
