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
	
	// Selections
	egsphantLabel  = new QLabel("Patient geometry");
	transformLabel = new QLabel("Source Positions");
	doseLabel      = new QLabel("Dose");
	
	egsphant       = new QComboBox();
	transform      = new QComboBox();
	dose           = new QComboBox();
	
	// Structure metrics
	metricGrid        = new QGridLayout();
	
	contourFileName.resize(100);
	contourTitleLabel = new QLabel("Contour");
	loadMetricLabel   = new QLabel("Metrics");
	saveDVHLabel      = new QLabel("Output DVH");
	saveDiffLabel     = new QLabel("Output differential");
	
	metricGrid->addWidget(contourTitleLabel, 0, 0, 1, 1);
	metricGrid->addWidget(loadMetricLabel  , 0, 1, 1, 1);
	metricGrid->addWidget(saveDVHLabel     , 0, 2, 1, 1);
	metricGrid->addWidget(saveDiffLabel    , 0, 3, 1, 1);
	
	for (int i = 0; i < STRUCT_COUNT; i++) {
		contourNameLabel.append(new QLabel(tr("no contour loaded")));
		loadMetricBox.append(new QComboBox());
		loadMetricBox.last()->addItems(parent->data->metricNames);
		saveDVHBox.append(new QCheckBox());
		saveDiffBox.append(new QCheckBox());
		
		contourNameLabel.last()->setDisabled(true);
		loadMetricBox.last()->setDisabled(true);
		saveDVHBox.last()->setDisabled(true);
		saveDiffBox.last()->setDisabled(true);
		
		metricGrid->addWidget(contourNameLabel.last() , i+1, 0, 1, 1);
		metricGrid->addWidget(loadMetricBox.last(), i+1, 1, 1, 1);
		metricGrid->addWidget(saveDVHBox.last()  , i+1, 2, 1, 1);
		metricGrid->addWidget(saveDiffBox.last()  , i+1, 3, 1, 1);
	
		ttt = tr("Select metrics, DVHs, and differential dose histograms to output along with patient data.");
		contourNameLabel.last()->setToolTip(ttt);
		loadMetricBox.last()->setToolTip(ttt);
		saveDVHBox.last()->setToolTip(ttt);
		saveDiffBox.last()->setToolTip(ttt);
	}
	
	metricFrame       = new QFrame();
	metricArea        = new QScrollArea();
	
	metricFrame->setLayout(metricGrid);
	metricArea->setWidget(metricFrame);
	metricArea->setWidgetResizable(true);
	
	metricFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	// Other
	outputRT = new QPushButton("Output RT dose");
	ttt = tr("Convert selected 3ddose file to RT dose file.");
	outputRT->setToolTip(ttt);
	
	outputFullData = new QPushButton("Output all data");
	ttt = tr("Output egsphant, transformation, dose, input files, and associated logs to a patient folder.\n"
			 "Additional metrics selected above can also be output.");
	outputRT->setToolTip(ttt);
	
	mainLayout->addWidget(parent->doseFrame, 0, 0, 4, 2);
	mainLayout->addWidget(outputRT         , 4, 0, 1, 2);
	mainLayout->addWidget(egsphantLabel    , 0, 2, 1, 1);
	mainLayout->addWidget(transformLabel   , 1, 2, 1, 1);
	mainLayout->addWidget(doseLabel        , 2, 2, 1, 1);
	mainLayout->addWidget(egsphant         , 0, 3, 1, 1);
	mainLayout->addWidget(transform        , 1, 3, 1, 1);
	mainLayout->addWidget(dose             , 2, 3, 1, 1);
	mainLayout->addWidget(metricArea       , 3, 2, 1, 2);
	mainLayout->addWidget(outputFullData   , 4, 2, 1, 2);
	
	mainLayout->setColumnStretch(0,1);
	mainLayout->setColumnStretch(1,1);
	mainLayout->setColumnStretch(2,1);
	mainLayout->setColumnStretch(3,1);
	
	setLayout(mainLayout);
}

void appInterface::connectLayout() {
	connect(outputRT, SIGNAL(pressed()),
			this, SLOT(outputRTdose()));
			
	connect(egsphant, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadStructs()));
}

// Refresh
void appInterface::refresh() {
	//mainLayout->addWidget(parent->doseFrame          , 0, 0, 4, 1); // used exclusively here, no need to refresh it
}

void appInterface::outputRTdose() {
	int i = parent->doseListView->currentRow();
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

void appInterface::loadStructs() {
	int i = egsphant->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local phantom count.  Aborting"));		
		return;
	}
	
	QString name = parent->data->localNamePhants[i]; // Get file location
			
	if (name.endsWith(".egsphant.gz")) {
		name = name.left(name.size()-12).split("/").last();
	}
	else if (name.endsWith(".begsphant")) {
		name = name.left(name.size()-10).split("/").last();
	}
	else if (name.endsWith(".egsphant")) {
		name = name.left(name.size()-9).split("/").last();
	}
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant.  Aborting"));
		return;
	}
	
	// fetch mask data
	QStringList maskNames;	
	QDirIterator files (parent->data->gui_location+"/database/mask/", {QString(name)+".*.egsphant.gz"},
						QDir::NoFilter, QDirIterator::Subdirectories);
	
	while(files.hasNext()) {
		files.next();
		maskNames << files.fileName();
	}
	
	QString tempName;
	for (int i = 0; i < maskNames.size() && i < STRUCT_COUNT; i++) {
		tempName = maskNames[i];
		contourFileName[i] = tempName;
		tempName = tempName.left(tempName.size()-17);
		tempName = tempName.right(tempName.size()-name.size()-1);
		contourNameLabel[i]->setText(tempName);
		
		contourNameLabel[i]->setDisabled(false);
		loadMetricBox[i]->setDisabled(false);
		saveDVHBox[i]->setDisabled(false);
		saveDiffBox[i]->setDisabled(false);
	}	
	for (int i = maskNames.size(); i < STRUCT_COUNT; i++) {
		contourNameLabel[i]->setText(tr("no contour loaded"));
		
		contourNameLabel[i]->setDisabled(true);
		loadMetricBox[i]->setDisabled(true);
		saveDVHBox[i]->setDisabled(true);
		saveDiffBox[i]->setDisabled(true);
	}
}