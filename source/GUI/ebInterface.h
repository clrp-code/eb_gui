/*
################################################################################
#
#  egs_brachy_GUI ebInterface.h
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
#ifndef EBINTERFACE_H
#define EBINTERFACE_H

#include <QtGui>
#include <iostream>
#include "../interface.h"

// Forward declaration of Interface to pass to the tab windows
class Interface;
class consoleWindow;

// Declaration of the Main Window class, its variables and its methods
class ebInterface : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
public:
    ebInterface();
    ebInterface(Interface* p);
    ~ebInterface();

    Interface* parent; // Pointer to the parent to access data with

public:
// LAYOUT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QGridLayout* mainLayout;
	
	QRegExpValidator allowedNums;
	
	QFrame*      simulationOptions;
	QGridLayout* simulationLayout;
	QLabel*      simulationLabel;
	
	QLabel*      fileNameLabel;
	QLineEdit*   fileNameEdit;
	
	QLabel*      ncaseLabel;
	QLineEdit*   ncaseEdit;
	
	QCheckBox*   volCorBox;
	QLabel*      volCorLabel;
	QLineEdit*   volCorDen;
	
	QLabel*      transportLabel;
	QLineEdit*   transportEdit;
	QPushButton* transportLoad;
	QLabel*      materialLabel;
	QLineEdit*   materialEdit;
	QPushButton* materialLoad;
	QLabel*      muenLabel;
	QLineEdit*   muenEdit;
	QPushButton* muenLoad;
	
	QCheckBox*   waterBox;
	QCheckBox*   edepBox;
	
	QLabel*      njobLabel;
	QComboBox*   njobBox;
	
	QLabel*      runModeLabel;
	QComboBox*   runModeBox;
	
	QPushButton* runButton;
	QPushButton* saveButton;
	QPushButton* egsViewButton;

    // Generic GUI methods
    void createLayout();
    void connectLayout();
	
public slots:
    void refresh(); // Reset all on screen values properly
	void loadTransport(); // Change transport file
	void loadMaterial(); // Change material file
	void loadMuen(); // Change muen file
	
// Running egs_brachy related functions
public:
	consoleWindow* console; // console window
	QProcess* ebProcess; // runs egs_brachy
	QVector <QProcess*> ebBatchProcess; // runs egs_brachy batches
	bool ebKillFlag; // track if user kills egs_brachy
	QString ebName; // holds name of current egs_brachy run

// Running egs_view
	QProcess* evProcess; // runs egs_view

public slots:
	void saveEB(); // Save egsinp file
    void runEB(); // Run egs_brachy
    void finishEB(int code); // Call egs_brachy clean-up
    void killEB(); // End interactive and parallel jobs;
	void writeOutputToConsole();
    void runEV(); // Run egs_view
	void saveLogEB(); // Save console output after simulation (egslog format)
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// consoleWindow~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Declaration of the console window class, used for egs_brachy output
class consoleWindow : public QMainWindow {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
             // that define what happens when you click on buttons
public:
    consoleWindow();
	~consoleWindow();
	
	// GUI Layout
	QWidget* frame;
	QGridLayout* layout;
	QPlainTextEdit* outputArea;
	QPushButton* save;
	QPushButton* kill;
	QPushButton* close;

    // Generic GUI methods
    void createLayout();
    void connectLayout();
};

#endif
