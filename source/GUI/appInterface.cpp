/*
################################################################################
#
#  egs_brachy_GUI appInterface.cpp
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
#include "appInterface.h"

// Constructors
appInterface::appInterface() {
	parent = (Interface*)parentWidget();
	
	createLayout();
	connectLayout();
}

appInterface::appInterface(Interface* p) {
	parent = p;
	createLayout();
	connectLayout();
}

// Destructor
appInterface::~appInterface() {
}

// Layout Settings
void appInterface::createLayout() {
	mainLayout = new QGridLayout();
	QString ttt = "";
	
	outputRT = new QPushButton("Output RT Dose");
	ttt = tr("Convert selected 3ddose file to RT dose file.");
	outputRT->setToolTip(ttt);
	
	mainLayout->addWidget(outputRT, 1, 0, 1, 1);	
	setLayout(mainLayout);
}

void appInterface::connectLayout() {
	connect(outputRT, SIGNAL(pressed()),
			this, SLOT(outputRTdose()));
}

void appInterface::outputRTdose() {
	int i = parent->doseListView->currentRow()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	Dose toBeRT;
	
	if (i >= parent->data->localDirDoses.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count.  Aborting"));		
		return;
	}
	
	// Get RT file save location
	QString rtFile = QFileDialog::getSaveFileName(this, tr("Save RT Dose File"), ".", tr("DICOM (*.dcm)"));
	
	if (rtFile.length() < 1) // No name selected
		return;
		
	if (!rtFile.endsWith(".dcm"))
		rtFile += ".dcm";
	
	QString doseFile = parent->data->localDirDoses[i]+parent->data->localNameDoses[i]; // Get file location
	
	// Connect the progress bar and load dose
	parent->resetProgress("Loading 3ddose file");
	connect(&toBeRT, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (doseFile.endsWith(".b3ddose"))
		toBeRT.readBIn(doseFile, 2);
	else if (doseFile.endsWith(".3ddose"))
		toBeRT.readIn(doseFile, 2);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected dose file is not of type 3ddose or b3ddose.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	// Now save RT Dose
	parent->data->outputRTDose(rtFile, &toBeRT);
	
	// Finish with progress bar
	parent->finishedProgress();
}

// Refresh
void appInterface::refresh() {
	mainLayout->addWidget(parent->doseFrame, 0, 0, 1, 1);
}