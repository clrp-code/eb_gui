/*
################################################################################
#
#  egs_brachy_GUI phantInterface.cpp
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
#include "phantInterface.h"

// Constructors
phantInterface::phantInterface()
// [\d*] one digit followed by:
// 1) some number of digits
// 2) (e|E)[+]?\d{1,2} character e or E, an optional +, then 1-2 digits 
// 3) .\d*(e|E)[+]?\d{1,2} dot ., some number of digits, character e or E, an optional +, then 1-2 digits 
	: allowedNums(QRegExp(REGEX_REAL_POS)) {
	parent = (Interface*)parentWidget();
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

phantInterface::phantInterface(Interface* p)
// [\d*] one digit followed by:
// 1) some number of digits
// 2) (e|E)[+]?\d{1,2} character e or E, an optional +, then 1-2 digits 
// 3) .\d*(e|E)[+]?\d{1,2} dot ., some number of digits, character e or E, an optional +, then 1-2 digits 
	: allowedNums(QRegExp(REGEX_REAL_POS)) {
	parent = p;
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

// Destructor
phantInterface::~phantInterface() {
	delete log;
}

// Layout Settings
void phantInterface::createLayout() {
	mainLayout = new QGridLayout();
	QString ttt = ""; // tool tip text
	
	// Select DICOM files
	dcmImport      = new QLabel     (tr("<b>Import DICOM virtual patient model </b>"));
	
	phantNameLabel = new QLabel     (tr("<b>VPM name</b>"));
	phantNameEdit  = new QLineEdit  ("DICOM_VPM");
	ttt = tr("The name used for the output.");
	phantNameLabel->setToolTip(ttt);
	phantNameEdit->setToolTip(ttt);
	
	ctImportFiles  = new QPushButton(tr("Import CT files"));
	ctImportDir    = new QPushButton(tr("Import CT directory"));
	ctListView     = new QListWidget();
	ctListView->setSelectionMode(QAbstractItemView::MultiSelection);
	ctDelete       = new QPushButton(tr("Delete"));
	ctDeleteAll    = new QPushButton(tr("Delete all"));
	ttt = tr("Select CT data files to use to make the egsphant. Displayed in ascending z order.");
	ctImportFiles->setToolTip(ttt);
	ctImportDir->setToolTip(ttt);
	ctListView->setToolTip(ttt);
	ctDelete->setToolTip(ttt);
	ctDeleteAll->setToolTip(ttt);
	
	structLabel    = new QLabel     (tr("<b>Import struct data</b>"));
	structLoad     = new QPushButton(tr("Load"));
	structEdit     = new QLineEdit  ("none selected");
	structEdit->setDisabled(true);
	
	ttt = tr("Select the struct files to use to make the egsphant.");
	structLabel->setToolTip(ttt);
	structLoad->setToolTip(ttt);
	
	ttt = tr("none selected");
	structEdit->setToolTip(ttt);
	
	calibLabel     = new QLabel     (tr("<b>HU to density table<b>"));
	calibLoad      = new QPushButton(tr("Load"));
	calibEdit      = new QLineEdit  (parent->data->hu_location);
	calibEdit->setToolTip(parent->data->hu_location);
	calibEdit->setDisabled(true);
	
	ttt = tr("Select the CT HU to density conversion table.");
	calibLabel->setToolTip(ttt);
	calibLoad->setToolTip(ttt);
	
	ttt = parent->data->hu_location;
	structEdit->setToolTip(ttt);
	
	create         = new QPushButton(tr("Create virtual patient model"));
	
	ttt = tr("Generate egsphant in local directory.");
	structEdit->setToolTip(ttt);
	
	dcmGrid        = new QGridLayout();
	dcmFrame       = new QFrame     ();
	
	dcmGrid->addWidget(dcmImport     , 0, 0, 1, 6);
	
	dcmGrid->addWidget(phantNameLabel, 1, 0, 1, 2);
	dcmGrid->addWidget(phantNameEdit , 1, 2, 1, 4);
	
	dcmGrid->addWidget(ctImportFiles , 2, 0, 1, 3);
	dcmGrid->addWidget(ctImportDir   , 2, 3, 1, 3);
	dcmGrid->addWidget(ctListView    , 3, 0, 1, 6);
	dcmGrid->addWidget(ctDelete      , 4, 0, 1, 3);
	dcmGrid->addWidget(ctDeleteAll   , 4, 3, 1, 3);
	
	dcmGrid->addWidget(structLabel   , 5, 0, 1, 2);
	dcmGrid->addWidget(structLoad    , 5, 2, 1, 2);
	dcmGrid->addWidget(structEdit    , 5, 4, 1, 2);
	
	dcmGrid->addWidget(calibLabel    , 6, 0, 1, 2);
	dcmGrid->addWidget(calibLoad     , 6, 2, 1, 2);
	dcmGrid->addWidget(calibEdit     , 6, 4, 1, 2);
	
	//dcmGrid->addWidget(create        , 7, 0, 1, 6); // Moved to elsewhere
	
	dcmFrame->setLayout(dcmGrid);
	dcmFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	// Tissue assignment scheme
	contourAssign        = new QLabel(tr("<b>Tissue Assignment Schemes</b>"));
	
	ttt = tr("This section uses Tissue Assignment Schemes (TASes) to assign media using CT density.\n  "
	         "Additionally, using the mask output creates additional egsphant data \n"
			 "to be used with 3ddose_tools analysis.");
	contourAssign->setToolTip(ttt);
	
	defaultTASLabel      = new QLabel(tr("Default TAS"));
	defaultTASBox        = new QComboBox();
	defaultTASBox->addItems(parent->data->TAS_names);
	
	truncBox   = new QCheckBox(tr("Truncate to structures")); 
	truncLabel = new QLabel(tr("Truncation buffer (cm)"));  
	truncEdit  = new QLineEdit("5"); 
	ttt = tr("The phantom can be truncated to a volume that fully contains all contours with"
			 " plus a chosen buffer in every direction.");
	truncBox->setToolTip(ttt);
	truncLabel->setToolTip(ttt);
	truncEdit->setToolTip(ttt);
	truncLabel->setDisabled(true);
	truncEdit->setDisabled(true);
	truncEdit->setValidator(&allowedNums);
	
	ttt = tr("The default TAS will be used to assign media everywhere in the virtual patient,\n"
             "unless otherwise specified in the contour specific TAS selection below.");
	defaultTASLabel->setToolTip(ttt);
	defaultTASBox->setToolTip(ttt);
	
	contourScrollGrid    = new QGridLayout();
	contourScrollFrame   = new QFrame();
	contourScrollFrame->setLayout(contourScrollGrid);
	contourScrollArea    = new QScrollArea();
	ttt = tr("The TAS selected here will only be used in the specified contour.");
	contourScrollFrame->setToolTip(ttt);
	
	contourTASMaskLabel  = new QLabel("Metrics");
	contourTASLabelLabel = new QLabel("Contour");
	contourTASBoxLabel   = new QLabel("TAS");
	
	contourScrollGrid->addWidget(contourTASMaskLabel , 0, 0, 1, 1);
	contourScrollGrid->addWidget(contourTASLabelLabel, 0, 1, 1, 1);
	contourScrollGrid->addWidget(contourTASBoxLabel  , 0, 2, 1, 1);
	contourScrollGrid->setColumnStretch(0,0);
	contourScrollGrid->setColumnStretch(1,1);
	contourScrollGrid->setColumnStretch(2,2);
	
	contourGrid          = new QGridLayout();
	contourFrame         = new QFrame();
	
	contourGrid->addWidget(contourAssign     , 0, 0, 1, 3);
	contourGrid->addWidget(defaultTASLabel   , 1, 0, 1, 1);
	contourGrid->addWidget(defaultTASBox     , 1, 1, 1, 2);
	contourGrid->addWidget(truncBox          , 2, 0, 1, 1);
	contourGrid->addWidget(truncLabel        , 2, 1, 1, 1);
	contourGrid->addWidget(truncEdit         , 2, 2, 1, 1);
	contourGrid->addWidget(contourScrollArea , 3, 0, 1, 3);
	
	for (int i = 0; i < STRUCT_COUNT; i++) {
		contourTASMask.append(new QCheckBox());
		contourTASLabel.append(new QLabel(tr("no contour selected")));
		contourTASBox.append(new QComboBox());
		contourTASBox.last()->addItem(tr("default"));
		contourTASBox.last()->addItems(parent->data->TAS_names);
		
		contourTASMask.last()->setDisabled(true);
		contourTASLabel.last()->setDisabled(true);
		contourTASBox.last()->setDisabled(true);
		
		contourScrollGrid->addWidget(contourTASMask.last() , i+1, 0, 1, 1);
		contourScrollGrid->addWidget(contourTASLabel.last(), i+1, 1, 1, 1);
		contourScrollGrid->addWidget(contourTASBox.last()  , i+1, 2, 1, 1);
	
		ttt = tr("The TAS selected here will only be used in the specified contour.");
		contourTASMask.last()->setToolTip(ttt);
		contourTASLabel.last()->setToolTip(ttt);
		contourTASBox.last()->setToolTip(ttt);
	}
	contourScrollArea->setWidget(contourScrollFrame);
	contourScrollArea->setWidgetResizable(true);
	
	contourFrame->setLayout(contourGrid);
	contourFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	// Contour priority
	prioContour = new QLabel("<b>Contour priority </b>(higher position overrules)");
	prioView    = new QListWidget();
	prioView->setDragDropMode(QAbstractItemView::InternalMove);
	
	ttt = tr("This priority list is used when determining which TAS to use when two contours overlap.\n "
			 "Drag and drop to move items up and down the list.");
	prioContour->setToolTip(ttt);
	prioView->setToolTip(ttt);
	
	prioGrid    = new QGridLayout();
	prioFrame   = new QFrame();
	
	prioGrid->addWidget(prioContour, 0, 0, 1, 1);
	prioGrid->addWidget(prioView   , 1, 0, 1, 1);
	
	prioFrame->setLayout(prioGrid);
	prioFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	// Metallic artifact reduction
	marLabel          = new QLabel(tr("<b>Metallic artifact reduction<b>"));
	
	marEnable         = new QCheckBox(tr("Enable"));
	marEnable->setChecked(true);
	
	ttt = tr("Metallic Artifact Reduction (MAR) is a feature used to remove the high density \n "
			 "volume in the CT scan caused by brachytherapy seeds and artifact streaking.");
	marLabel->setToolTip(ttt);
	marEnable->setToolTip(ttt);
	
	marTransLabel     = new QLabel("Source locations");
	marTransformation = new QComboBox();
	
	ttt = tr("Choose the source locations at which to perform MAR.");
	marTransLabel->setToolTip(ttt);
	marTransformation->setToolTip(ttt);
	
	marDefaultLabel = new QLabel("Tissue settings");
	marDefault      = new QComboBox();
	
	ttt = tr("Choose the default (or custom) MAR values.");
	marDefaultLabel->setToolTip(ttt);
	marDefault->setToolTip(ttt);
	
	marLTLabel        = new QLabel(tr("Lower threshold"));
	marLTEdit         = new QLineEdit("");
	marUTLabel        = new QLabel(tr("Upper threshold"));
	marUTEdit         = new QLineEdit("");
	
	ttt = tr("Correction will be applied to any voxel with density not within the thresholds.");
	marLTLabel->setToolTip(ttt);
	marLTEdit->setToolTip(ttt);
	marUTLabel->setToolTip(ttt);
	marUTEdit->setToolTip(ttt);
	
	marDenLabel       = new QLabel(tr("Replacement density (g/cm<sup>3</sup>)"));
	marDenEdit        = new QLineEdit("");
	marRadLabel       = new QLabel(tr("Replacement radius (cm)"));
	marRadEdit        = new QLineEdit("");
	
	ttt = tr("The density used to substitute densities outside of thresholds.");
	marDenLabel->setToolTip(ttt);
	marDenEdit->setToolTip(ttt);
	
	ttt = tr("The box side-length used to determine voxel replacement regions.");
	marRadLabel->setToolTip(ttt);
	marRadEdit->setToolTip(ttt);
	
	marLTEdit->setValidator(&allowedNums);
	marUTEdit->setValidator(&allowedNums);
	marDenEdit->setValidator(&allowedNums);
	marRadEdit->setValidator(&allowedNums);
	
	marContour       = new QCheckBox("Limit to contour");
	marContourBox    = new QComboBox();
	marContourBox->addItem("no DICOM file selected");
	marContourBox->setDisabled(true);
	
	ttt = tr("Only apply MAR within the selected contour.");
	marContour->setToolTip(ttt);
	marContourBox->setToolTip(ttt);
	
	marGrid          = new QGridLayout();
	marFrame         = new QFrame();
	
	loadMARdefaults();
	fillMARvalues();
	
	marGrid->addWidget(marLabel         ,  0, 0, 1, 3);
	marGrid->addWidget(marEnable        ,  1, 0, 1, 2);
	
	marGrid->addWidget(marTransLabel    ,  2, 0, 1, 1);
	marGrid->addWidget(marTransformation,  2, 1, 1, 2);
	marGrid->addWidget(marDefaultLabel  ,  3, 0, 1, 1);
	marGrid->addWidget(marDefault       ,  3, 1, 1, 2);
	
	marGrid->addWidget(marLTLabel       ,  5, 0, 1, 1);
	marGrid->addWidget(marLTEdit        ,  5, 1, 1, 2);
	marGrid->addWidget(marUTLabel       ,  6, 0, 1, 1);
	marGrid->addWidget(marUTEdit        ,  6, 1, 1, 2);
	marGrid->addWidget(marDenLabel      ,  7, 0, 1, 1);
	marGrid->addWidget(marDenEdit       ,  7, 1, 1, 2);
	marGrid->addWidget(marRadLabel      ,  8, 0, 1, 1);
	marGrid->addWidget(marRadEdit       ,  8, 1, 1, 2);
	marGrid->addWidget(marContour       , 10, 0, 1, 2);
	marGrid->addWidget(marContourBox    , 11, 0, 1, 3);
	
	marGrid->setRowStretch(4, 5);
	marGrid->setRowStretch(9, 5);
	
	marFrame->setLayout(marGrid);
	marFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);
	
	mainLayout->addWidget(dcmFrame    , 0, 0, 2, 1);
	mainLayout->addWidget(prioFrame   , 2, 0, 2, 1);
	mainLayout->addWidget(contourFrame, 0, 1, 4, 1);
	mainLayout->addWidget(marFrame    , 0, 2, 1, 1);
	mainLayout->addWidget(create      , 3, 2, 1, 1);
	
	mainLayout->setColumnStretch(0, 5);
	mainLayout->setColumnStretch(1, 5);
	mainLayout->setColumnStretch(2, 5);
	
	setLayout(mainLayout);
}

void phantInterface::connectLayout() {
	connect(marEnable, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
	connect(marDefault, SIGNAL(currentTextChanged(QString)),
			this, SLOT(fillMARvalues()));
    connect(marContour, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
			
	connect(structLoad, SIGNAL(released()),
			this, SLOT(loadStruct()));
	connect(calibLoad, SIGNAL(released()),
			this, SLOT(loadHU2rho()));
			
	connect(truncBox, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
			
	connect(ctImportFiles, SIGNAL(released()),
			this, SLOT(loadCTFiles()));
	connect(ctImportDir, SIGNAL(released()),
			this, SLOT(loadCTDir()));
	connect(ctDelete, SIGNAL(released()),
			this, SLOT(deleteCT()));
	connect(ctDeleteAll, SIGNAL(released()),
			this, SLOT(deleteAllCT()));
			
	connect(create, SIGNAL(released()),
			this, SLOT(createEGSphant()));
}

// Load MAR data
void phantInterface::loadMARdefaults() {
	QFile file (parent->data->mar_location);
	
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(0, "Loading MAR error",
		tr("Could not find ")+parent->data->mar_location+tr(", no default settings added."));
		marDefault->addItem("Custom");
		return;
	}
	
	QTextStream in(&file);
	QStringList fields;	
	while(!in.atEnd()) {
		fields = in.readLine().split(",");
		if (fields.size() == 5) {
			defMARs.append(fields[0]);
			LTs.append(fields[1]);
			UTs.append(fields[2]);
			Dens.append(fields[3]);
			Rads.append(fields[4]);
			marDefault->addItem(defMARs.last());
		}
	}
	marDefault->addItem("Custom");
}

void phantInterface::fillMARvalues() {
	int i = marDefault->currentIndex();
	if (i < defMARs.size()) {
		marLTEdit->setText(LTs[i]);
		marUTEdit->setText(UTs[i]);
		marDenEdit->setText(Dens[i]);
		marRadEdit->setText(Rads[i]);		
	}
	
	refresh();
}

// Pull DICOM data
void phantInterface::createEGSphant() {
	// Check if CT data is loaded
	if (parent->data->CT_data.isEmpty()) {
		QMessageBox::warning(0, "Creating egsphant error",
		tr("No CT data is loaded. Aborting."));
		return;		
	}
	
	QString fileName = phantNameEdit->text().trimmed().replace(" ", "_");
	QList <QListWidgetItem*> matchingNames = parent->phantomListView->findItems(fileName+".egsphant.gz",Qt::MatchExactly);
	
	if (matchingNames.size()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Name already found",
		tr("An egsphant named ") + fileName + tr(" already exists.  Would you like to overwrite it?"))) {
			// Get the file index
			int i = parent->data->localNamePhants.indexOf(fileName+".egsphant.gz");
			
			// Delete all associated files
			QFile(parent->data->localDirPhants[i]+fileName+".egsphant.gz").remove();
			QFile(parent->data->localDirPhants[i]+fileName+".log").remove();
			QFile(parent->data->localDirPhants[i]+fileName+".tg43.geom").remove();
			
			// Delete the file references
			parent->data->localNamePhants.removeAt(i);
			parent->data->localDirPhants.removeAt(i);
			
			// Delete all the masks
			QDirIterator files (parent->data->gui_location+"/database/mask/", {QString(fileName)+".*.egsphant.gz"},
								QDir::NoFilter, QDirIterator::Subdirectories);
			
			while(files.hasNext()) {
				files.next();
				QFile (parent->data->gui_location+"/database/mask/"+files.fileName()).remove();
			}
			
			// Remove it from the list widget
			delete matchingNames[0];
		}
		else { // They did not proceed
			return;
		}
	}
	
	// Pass the MAR variables to data
	parent->data->do_MAR = marEnable->isChecked();
	if (marEnable->isChecked()) {
		parent->data->transformFile = marTransformation->currentText();
		parent->data->lowerThresh   = marLTEdit->text().toDouble();
		parent->data->upperThresh   = marUTEdit->text().toDouble();
		parent->data->marDen        = marDenEdit->text().toDouble();
		parent->data->marRad        = marRadEdit->text().toDouble();
		if (marContour->isChecked()) {
			parent->data->marContour    = marContourBox->currentText();
			parent->data->marContourInd = marContourBox->currentIndex();
		}
		else {
			parent->data->marContour = "";
		}
	}
	
	// Set the default tas
	int defaultTAS = -1;
	for (int i = 0; i < parent->data->TAS_names.size(); i++) {
		if (parent->data->TAS_names[i].compare(defaultTASBox->currentText()) == 0)
			defaultTAS = i;
	}
	if (defaultTAS < 0) {
		QMessageBox::warning(0, "Creating VPM error",
		tr("Problem collecting default TAS, aborting."));
		return;
	}
	
	// Set the contour specific arrays
	QVector <int> structIndex(prioView->count()), tasIndex(prioView->count());
	QVector <EGSPhant*> makeMasks;
	parent->data->marContourInd = -1;
	
	for (int j = 0; j < prioView->count(); j++) {
		for (int i = 0; i < contourTASLabel.count(); i++)
			if (contourTASLabel[i]->text().compare(prioView->item(j)->text()) == 0) {
				structIndex[j] = i;
				tasIndex[j] = contourTASBox[i]->currentIndex()-1; // -1 is default
				if (contourTASLabel[i]->text().compare(parent->data->marContour) == 0) // Get MAR contour
					parent->data->marContourInd = i;
				break;
			}
	}
	
	for (int i = 0; i < structIndex.size(); i++)
		if (structIndex[i] < 0) {
			QMessageBox::warning(0, "Creating VPM error",
			tr("Problem assigning contour TASes, aborting."));
			return;
		}
	
	// Invoke build egsphant from data
	EGSPhant phantom;
	QString textLog;
	int err;
	
	if (truncBox->isChecked()) {
		err = parent->data->buildEgsphant(&phantom, &textLog, structIndex.size(), defaultTAS,
										  &structIndex, &tasIndex, &makeMasks, truncEdit->text().toDouble());
	}
	else {
		err = parent->data->buildEgsphant(&phantom, &textLog, structIndex.size(), defaultTAS,
										  &structIndex, &tasIndex, &makeMasks);
	}
	
	if (err == 0) {
		// Connect the progress bar
		parent->nameProgress("Saving in local egsphant database");
		connect(&phantom, SIGNAL(madeProgress(double)),
				parent, SLOT(updateProgress(double)));
		
		// Output egsphant file
		phantom.savegzEGSPhantFilePlus(parent->data->gui_location+"/database/egsphant/"+fileName+".egsphant.gz");
		parent->data->localNamePhants << fileName+".egsphant.gz";
		parent->data->localDirPhants << parent->data->gui_location+"/database/egsphant/";
		parent->phantomRepopulate();
		
		// Output and delete masks
		for (int i = 0; i < structIndex.size(); i++) {
			if (contourTASMask[i]->isChecked())
				makeMasks[i]->savegzEGSPhantFile(parent->data->gui_location+"/database/mask/"+fileName+"."+contourTASLabel[i]->text()+".mask.egsphant.gz");
			delete makeMasks[i];
		}
		
		// Output log file
		QFile logFile(parent->data->gui_location+"/database/egsphant/"+fileName+".log");
		if (logFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&logFile);
			out << textLog;
			logFile.close();
		}
		
		parent->phantomListView->setCurrentRow(parent->phantomListView->count()-1);
		
		// Show the log
		log->outputArea->clear();
		log->outputArea->setPlainText(textLog);
		log->show();
	}
	else if (err == 101)
		QMessageBox::warning(0, "HU to density error",
        tr("Could not open ") + parent->data->hu_location + tr(" file.  Aborting"));
	else if (err == 102)
		QMessageBox::warning(0, "HU to density error",
        tr("Could not parse the hu2rho file. Aborting"));
	else if (err == 201)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Pixel Spacing (0028,0030)" + tr (" in CT DICOM file.  Aborting"));
	else if (err == 202)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Slice Thickness (0018,0050)" + tr (" in CT DICOM file.  Aborting"));
	else if (err == 203)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Image Position (0028,0032)" + tr (" in CT DICOM file.  Aborting"));
	else if (err == 204)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Rows (0028,0010)" + tr (" in CT DICOM file.  Aborting"));
	else if (err == 205)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Columns (0028,0011)" + tr (" in CT DICOM file.  Aborting"));
	else if (err == 208)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "HU values (7fe0,0010)" + tr (" in CT DICOM file.  Aborting"));
		
	parent->finishedProgress();
}

void phantInterface::loadCTFiles() {
	QStringList paths = QFileDialog::getOpenFileNames(this, tr("Load DICOM CT files"));
	QStringList failedFiles;
	QString patientName;
	
	if (paths.isEmpty()) // If you didn't get any files, quit
		return;
	
	double increment = 100.0/paths.size();
    parent->resetProgress("Loading DICOM files");
	
	for (int i = 0; i < paths.size(); i++) {
		parent->data->CT_data.append(new DICOM(&parent->data->tag_data));
		
		// Check if it is a proper CT DICOM file
		if (parent->data->CT_data.last()->parse(paths[i])) {
			failedFiles.append(paths[i].split("/").last() + tr(" is not DICOM format"));
			delete parent->data->CT_data.last();
			parent->data->CT_data.removeLast();
		}
		else {
			Attribute* tempAtt;
			
			tempAtt = parent->data->CT_data.last()->getEntry(0x0008, 0x0060); // Get att closest to (0008,0060)
			if (tempAtt->tag[0] != 0x0008 && tempAtt->tag[1] != 0x0060) { // See if it is (0008,0060)
				failedFiles.append(paths[i].split("/").last() + tr(" did not have DICOM modality field (0008,0060)"));
				delete parent->data->CT_data.last();
				parent->data->CT_data.removeLast();
			}
			else {
				QString temp = "";
				for (unsigned int s = 0; s < tempAtt->vl; s++) {
					temp.append(tempAtt->vf[s]);
				}
				
				if (temp.trimmed().compare("CT")) { // See if the field contains CT
					failedFiles.append(paths[i].split("/").last() + tr(" is not CT modality"));
					delete parent->data->CT_data.last();
					parent->data->CT_data.removeLast();
				}
			}
		}
		parent->updateProgress(increment);
	}
	
	// Get patient name for the egsphant label
	Attribute* tempAtt;
	tempAtt = parent->data->CT_data.first()->getEntry(0x0010, 0x0010); // Get att closest to (0010,0010)
	if (tempAtt->tag[0] == 0x0010 && tempAtt->tag[1] == 0x0010) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		
		phantNameEdit->setText(temp);
	}
	else {
		phantNameEdit->setText("DICOM_VPM");
	}
	
	if (failedFiles.size()) {
		QMessageBox::information(0, "DICOM CT import complete",
		tr("All selected DICOM data successfully imported, except as noted below:\n - ") +
		failedFiles.join("\n - "));
	}
	else {
		QMessageBox::information(0, "DICOM CT import complete",
		tr("All selected DICOM data successfully imported."));		
	}
	
	// Sort all CT slices by z height
	mergeSort(parent->data->CT_data,parent->data->CT_data.size());
	parent->finishedProgress();
	
	// Repopulate CT list
	repopulateCT();
}

void phantInterface::loadCTDir() { // Very similar to CT files with an extra step
	QString path = QFileDialog::getExistingDirectory(this, tr("Select DICOM CT directory"));
	QStringList paths;
	QStringList failedFiles;
	
	double increment = 60.0/paths.size();
    parent->resetProgress("Loading DICOM files");
	
	// Get all files in subdirectories
	QDirIterator dirIt (path, QDirIterator::Subdirectories);
	while (dirIt.hasNext()) {
		paths.append(dirIt.next());
		if (paths.last().split("/").last().compare(".") == 0 ||
			paths.last().split("/").last().compare("..") == 0)
			paths.removeLast();
	}
	
	if (paths.isEmpty()) // If you didn't get any files, quit
		return;

	for (int i = 0; i < paths.size(); i++) {
		parent->data->CT_data.append(new DICOM(&parent->data->tag_data));
		
		// Check if it is a proper CT DICOM file
		if (parent->data->CT_data.last()->parse(paths[i])) {
			failedFiles.append(paths[i].split("/").last() + tr(" is not DICOM format"));
			delete parent->data->CT_data.last();
			parent->data->CT_data.removeLast();
		}
		else {
			Attribute* tempAtt;
			
			tempAtt = parent->data->CT_data.last()->getEntry(0x0008, 0x0060); // Get att closest to (0008,0060)
			if (tempAtt->tag[0] != 0x0008 && tempAtt->tag[1] != 0x0060) { // See if it is (0008,0060)
				failedFiles.append(paths[i].split("/").last() + tr(" did not have DICOM modality field (0008,0060)"));
				delete parent->data->CT_data.last();
				parent->data->CT_data.removeLast();
			}
			else {
				QString temp = "";
				for (unsigned int s = 0; s < tempAtt->vl; s++) {
					temp.append(tempAtt->vf[s]);
				}
				
				if (temp.trimmed().compare("CT")) { // See if the field contains CT
					failedFiles.append(paths[i].split("/").last() + tr(" is not CT modality"));
					delete parent->data->CT_data.last();
					parent->data->CT_data.removeLast();
				}
			}
		}
		parent->updateProgress(increment);
	}
	
	// Get patient name for the egsphant label
	Attribute* tempAtt;
	tempAtt = parent->data->CT_data.first()->getEntry(0x0010, 0x0010); // Get att closest to (0010,0010)
	if (tempAtt->tag[0] == 0x0010 && tempAtt->tag[1] == 0x0010 && tempAtt->vl) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		
		phantNameEdit->setText(temp);
	}
	else {
		phantNameEdit->setText("DICOM_VPM");
	}
	
	// Sort all CT slices by z height
	mergeSort(parent->data->CT_data,parent->data->CT_data.size());
	parent->updateProgress(40);
	
	parent->finishedProgress();
	if (failedFiles.size()) {
		QMessageBox::information(0, "DICOM CT import complete",
		tr("All selected DICOM data successfully imported, except as noted below:\n - ") +
		failedFiles.join("\n - "));
	}
	else {
		QMessageBox::information(0, "DICOM CT import complete",
		tr("All selected DICOM data successfully imported."));		
	}
	
	// Repopulate CT list
	repopulateCT();
}

// Basically resets what the console shows to match what data has stored in memory
// As long as mergeSort is called whenever new data is added, and the user isn't
// allowed to move files around, it should always be sorted in ascending z order
void phantInterface::repopulateCT() {
	ctListView->clear();
	for (int i = 0; i < parent->data->CT_data.size(); i++)
		ctListView->addItem(parent->data->CT_data[i]->path.split("/").last());
}

void phantInterface::deleteCT() {
	// I'm operating under the assumption that parent->data->CT_data indexing
	// matches ctListView, which could be a dangerous assumption, may be worth
	// revisiting
	QList <QListWidgetItem*> toBeDeleted = ctListView->selectedItems();
	
	QVector <int> ind;
	for (int i = toBeDeleted.size()-1; i >= 0; i--)
		ind.append(ctListView->row(toBeDeleted[i]));
		
	for (int i = 0; i < ind.size(); i++) {
		delete parent->data->CT_data[ind[i]];
		parent->data->CT_data.remove(ind[i]);
	}
	
	// Repopulate CT list
	repopulateCT();
}

void phantInterface::deleteAllCT() {		
	for (int i = parent->data->CT_data.size()-1; i >= 0; i--) {
		delete parent->data->CT_data[i];
		parent->data->CT_data.remove(i);
	}
	
	// Repopulate CT list
	repopulateCT();
}

void phantInterface::loadStruct() {
	// Get a file and try to loaded
	QString path = QFileDialog::getOpenFileName(this, tr("Load DICOM struct file"));
	
	DICOM* structFile = parent->data->struct_data; // temporary pointer
	Attribute* tempAtt;
	
	if (parent->data->struct_loaded) {
		parent->data->struct_loaded = false;
		delete structFile;
	}
	
	structFile = new DICOM(&parent->data->tag_data);
	
	// Quit if you can't parse the file
	if (!parseError(structFile->parse(path)))
		return;
	
	tempAtt = structFile->getEntry(0x3006, 0x0020); // Get att closest to (3006,0020)
	if (tempAtt->tag[0] != 0x3006 && tempAtt->tag[1] != 0x0020) {// See if it is (3006,0020)
		QMessageBox::warning(0, "DICOM error",
        tr("Did not find field \"Structure Set ROI Sequence\" in DICOM file."));
		return;
	}
	
	tempAtt = structFile->getEntry(0x3006, 0x0039); // Get att closest to (3006,0039)
	if (tempAtt->tag[0] != 0x3006 && tempAtt->tag[1] != 0x0039) {// See if it is (3006,0039)
		QMessageBox::warning(0, "DICOM error",
        tr("Did not find field \"ROI Contour Sequence\" in DICOM file."));
		return;
	}
	
	// File successfully loaded and parsed, and it seems to contain the proper tags, now to
	// fetch contour data, we need to fill the following arrays:
	
	// Arrays in Structure Set ROI Sequence and a lookup to fetch names using ROI Contour indices
	QVector <int> globalStructReference; // Global structure reference number
	QVector <QString> globalStructName; // Global structure name
	QMap <int, int> globalStructLookup; // Maps local reference number to global reference number
	
	// ROI Contour Sequence actual data with a reference pointing to the above arrays
	QVector <QVector <QPolygonF> > structPos; // Holds actual contour points per slice
	QVector <QVector <double> > structZ; // Holds contour z positions
	QVector <int> structReference; // Local structure reference number
	
	// Temp holders to make sure we only add full structures
	QVector <QPolygonF> structPosTemp;
	QVector <double> structZTemp;
	int structReferenceTemp;
	
	// Temp arrays needed for parsing
	QVector <Attribute*> *att, *att2;
	QByteArray tempData, tempData2;
	QStringList pointData;
	
	// Get structure names
	tempAtt = structFile->getEntry(0x3006, 0x0020);
	for (int k = 0; k < tempAtt->seq.items.size(); k++) {
		tempData = QByteArray((char*)tempAtt->seq.items[k]->vf,tempAtt->seq.items[k]->vl);
		QDataStream dataStream(tempData);
		
		att = new QVector <Attribute*>;
		if (!structFile->parseSequence(&dataStream, att)) {
			QMessageBox::warning(0, "DICOM error",
			tr("Failed to parse field in \"Structure Set ROI Sequence\" in DICOM file."));
			delete structFile;
			return;
		}
		
		QString tempS = ""; // Get the name
		QString tempI = ""; // Get the number
		for (int l = 0; l < att->size(); l++) {
			if (att->at(l)->tag[0] == 0x3006 && att->at(l)->tag[1] == 0x0026)
				for (unsigned int s = 0; s < att->at(l)->vl; s++)
					tempS.append(att->at(l)->vf[s]);
			else if (att->at(l)->tag[0] == 0x3006 && att->at(l)->tag[1] == 0x0022)
				for (unsigned int s = 0; s < att->at(l)->vl; s++)
					tempI.append(att->at(l)->vf[s]);
		}
		tempS = tempS.trimmed().replace(" ", "_");
		
		if (globalStructName.contains(tempS.trimmed())) {
			char letter = 'a';
			tempS = tempS + " (" + letter + ")";
			while (globalStructName.contains(tempS.trimmed())) {
				letter++;
				tempS = tempS.left(tempS.size()-4) + " (" + letter + ")";
			}
		}
		globalStructName.append(tempS.trimmed());
		globalStructReference.append(tempI.toInt());
		globalStructLookup[tempI.toInt()] = globalStructName.size()-1;
		
		for (int l = 0; l < att->size(); l++)
			delete att->at(l);
		delete att;
	}
	
	// Get structure data
	tempAtt = structFile->getEntry(0x3006, 0x0039);
		
	for (int k = 0; k < tempAtt->seq.items.size(); k++) {
		tempData = QByteArray((char*)tempAtt->seq.items[k]->vf,tempAtt->seq.items[k]->vl);
		QDataStream dataStream(tempData);
		
		att = new QVector <Attribute*>;
		if (!structFile->parseSequence(&dataStream, att)) {
			QMessageBox::warning(0, "DICOM error",
			tr("Failed to parse field in \"ROI Contour Sequence\" in DICOM file."));
			delete structFile;
			return;
		}
		
		QString tempS = ""; // Get the contour, it's another nested sequence, so we must go deeper with parseSequence
		structPosTemp.clear();
		structZTemp.clear();
		structReferenceTemp = -1;
		
		for (int l = 0; l < att->size(); l++) {
			if (att->at(l)->tag[0] == 0x3006 && att->at(l)->tag[1] == 0x0040)
			{
				for (int k = 0; k < att->at(l)->seq.items.size(); k++) {
					structPosTemp.resize(structPosTemp.size()+1);
					tempData2 = QByteArray((char*)att->at(l)->seq.items[k]->vf,att->at(l)->seq.items[k]->vl);
					QDataStream dataStream2(tempData2);
					
					att2 = new QVector <Attribute*>;
					if (!structFile->parseSequence(&dataStream2, att2)) {
						QMessageBox::warning(0, "DICOM error",
						tr("Failed to parse field in \"Contour Sequence\" in DICOM file."));
						delete structFile;
						return;
					}
					
					QString tempS = ""; // Get the points
					for (int m = 0; m < att2->size(); m++)
						if (att2->at(m)->tag[0] == 0x3006 && att2->at(m)->tag[1] == 0x0050)
							for (unsigned int s = 0; s < att2->at(m)->vl; s++)
								tempS.append(att2->at(m)->vf[s]);
					
					pointData = tempS.split('\\');
					structZTemp.append(pointData[2].toDouble()/10.0);
					for (int m = 0; m < pointData.size(); m+=3)
						structPosTemp.last() << QPointF(pointData[m].toDouble()/10.0, pointData[m+1].toDouble()/10.0);
					
					for (int m = 0; m < att2->size(); m++)
						delete att2->at(m);
					delete att2;
				}
			}
			else if (att->at(l)->tag[0] == 0x3006 && att->at(l)->tag[1] == 0x0084) {
				QString tempI = ""; // Get the number
				for (unsigned int s = 0; s < att->at(l)->vl; s++)
					tempI.append(att->at(l)->vf[s]);
				structReferenceTemp = tempI.toInt();
			}
		}
		
		if (structReferenceTemp > -1 && structZTemp.size() && structPosTemp.size()) {
			structReference.append(structReferenceTemp);
			structZ.append(structZTemp);
			structPos.append(structPosTemp);
		}
							
		for (int l = 0; l < att->size(); l++)
			delete att->at(l);
		delete att;
	}
	
	// Build a local struct name array using local indices (instead of global indices
	QVector <QString> structName;
	for (int i = 0; i < structReference.size(); i++) {
		for (int j = 0; j < globalStructReference.size(); j++) {
			if (structReference[i] == globalStructReference[j])
				structName.append(globalStructName[globalStructLookup[globalStructReference[j]]]);
		}
	}
	
	// Exit if we didn't find enough contour names
	if (structReference.size() != structName.size()) {
		QMessageBox::warning(0, "DICOM error",
		tr("Mismatch in the references between \"ROI Contour Sequence\" and \"Structure Set ROI Sequence\" defined in in DICOM file."));
		delete structFile;
		return;
	}
	
	// We should only be here once the structure data is properly read in
	
	// Assign positions and z values
	parent->data->structPos  = structPos;
	parent->data->structZ    = structZ;
	parent->data->structName = structName;
	
	// Filter out all structs which are essentially empty from the UI
	QStringList validStructName;
	for (int i = 0; i < structName.size(); i++) {
		if (structZ[i].size() > 0) { // We have at least one layer
			validStructName << structName[i];
		}
	}
	
	// Now update the UI	
	int i;
	for (i = 0; i < validStructName.size(); i++) {
		contourTASMask[i]->setChecked(false);
		contourTASLabel[i]->setText(validStructName[i]);
		
		contourTASMask[i]->setEnabled(true);
		contourTASLabel[i]->setEnabled(true);
		contourTASBox[i]->setEnabled(true);
	}
	for (; i < STRUCT_COUNT; i++) {
		contourTASLabel[i]->setText("no further structs");
		
		contourTASMask[i]->setEnabled(false);
		contourTASLabel[i]->setEnabled(false);
		contourTASBox[i]->setEnabled(false);
	}
	
	prioView->clear();
	prioView->addItems(validStructName);
	
	marContourBox->clear();
	marContourBox->addItems(validStructName);
	
	// Update the struct file field
	structEdit->setText(path);
	structEdit->setToolTip(path);
	
	// Save file in data
	parent->data->struct_data = structFile;
	parent->data->struct_loaded = true;
}

int phantInterface::parseError(int err) {
	if (err == 501) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file. Aborting."));
        return 0;		
	}
	else if (200 > err && err > 100) {
		QMessageBox::warning(0, "file error",
        tr("Did not read proper DICOM header. Aborting."));
        return 0;		
	}
	else if (300 > err && err > 200) {
		QMessageBox::warning(0, "file error",
        tr("Failed to properly read element tag/size. Aborting."));
        return 0;			
	}
	else if (400 > err && err > 300) {
		QMessageBox::warning(0, "file error",
        tr("Failed to properly read element data. Aborting."));
        return 0;			
	}
	return 1;
}

// Refresh
void phantInterface::refresh() {
	mainLayout->addWidget(parent->phantomFrame, 1, 2, 2, 1);
	
	if (truncBox->isChecked()) {
		truncLabel->setDisabled(false);
		truncEdit->setDisabled(false);
	}
	else {
		truncLabel->setDisabled(true);
		truncEdit->setDisabled(true);
	}
	
	if (marEnable->isChecked()) {
		marLTLabel->setDisabled(false);
		marTransLabel->setDisabled(false);
		marTransformation->setDisabled(false);
		marDefaultLabel->setDisabled(false);
		marDefault->setDisabled(false);
		if (marDefault->currentIndex() == defMARs.size()) {
			marLTLabel->setDisabled(false);
			marLTEdit->setDisabled(false);
			marUTLabel->setDisabled(false);
			marUTEdit->setDisabled(false);
			marDenLabel->setDisabled(false);
			marDenEdit->setDisabled(false);
			marRadLabel->setDisabled(false);
			marRadEdit->setDisabled(false);
		}
		else {
			marLTLabel->setDisabled(true);
			marLTEdit->setDisabled(true);
			marUTLabel->setDisabled(true);
			marUTEdit->setDisabled(true);
			marDenLabel->setDisabled(true);
			marDenEdit->setDisabled(true);
			marRadLabel->setDisabled(true);
			marRadEdit->setDisabled(true);
		}
		marContour->setDisabled(false);
		if (marContour->isChecked())
			marContourBox->setDisabled(false);
		else
			marContourBox->setDisabled(true);
	}
	else {
		marLTLabel->setDisabled(true);
		marTransLabel->setDisabled(true);
		marTransformation->setDisabled(true);
		marDefaultLabel->setDisabled(true);
		marDefault->setDisabled(true);
		marLTLabel->setDisabled(true);
		marLTEdit->setDisabled(true);
		marUTLabel->setDisabled(true);
		marUTEdit->setDisabled(true);
		marDenLabel->setDisabled(true);
		marDenEdit->setDisabled(true);
		marRadLabel->setDisabled(true);
		marRadEdit->setDisabled(true);
		marContour->setDisabled(true);
		marContourBox->setDisabled(true);
	}
}

void phantInterface::loadHU2rho() {
	QString path = QFileDialog::getOpenFileName(this, tr("Load HU to density conversion table"), parent->data->gui_location+"/database/HU_conversion", tr("HU2RHO file (*.HU2RHO)"));
	
	if (path < 1)
		return;
	
	// Check if it opens as a text file (at the very least)
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file."));
        return;
	}
	
	// No other checks, leave it for actual egsphant creation time
	
	calibEdit->setText(path);
	parent->data->hu_location = path;
	calibEdit->setToolTip(path);
}

// DICOM CT array sorting functions
void phantInterface::submerge(QVector <DICOM *> &data, int i, int c, int f) {
	// We have three indices, l for one subsection, r the other, and j for the new sorted array
	int l = i, r = c+1, j = 0;
	QVector <DICOM *> temp(f-i+1); // Set aside memory for sorted array
	
	// While we have yet to iterate through either subsection
	while (l <= c && r <= f) {
		// If value at r index is smaller then add it to temp and move to next r
		if (data[l]->z > data[r]->z)
			temp[j++] = data[r++];
		// If value at l index is smaller then add it to temp and move to next l
		else
			temp[j++] = data[l++];
	}
	
	// Add all the remaining elements
	while (r <= f)
		temp[j++] = data[r++];
	while (l <= c)
		temp[j++] = data[l++];

	// Reassign all the data values to the temp values
	for (int k = 0; k < j; k++) {
		data[i+k] = temp[k]; 
	}
}

void phantInterface::mergeSort(QVector <DICOM *> &data, int n) {
	// If our array is size 1 or less quit
	if (n <= 1)
		return;
	
	int subn = 1; // subn the size of subsections that are being submerged
	int i = 0; // i is the current index of the array at which we are at
	
	// While we are still submerging sections that are smaller than the size of the array
	while (subn < n) {
		// Reset the index to 0
		i = 0;
		
		// Iterate through n/(2*subn) sections, truncated
		while (i < n - subn) {
			
			// submerge two subn sized portions of data of the array
			if (i + (2 * subn) < n)
				submerge (data, i, i + subn - 1, i + 2 * subn - 1);
			
			// Or submerge a subn sized section and whatever is left of the array
			else 
				submerge (data, i, i + subn - 1, n - 1);
			
			// Move the index to submerge the next 2 subsections
			i += 2 * subn; 
		}
		
		// Double the size of subsection to be merged
		subn *= 2; 
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// logWindow~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
logWindow::logWindow() {
	createLayout();
	connectLayout();
}

logWindow::~logWindow() {
	
}
	
// GUI Layout
void logWindow::createLayout() {
	frame      = new QWidget();
	layout     = new QGridLayout;
	close      = new QPushButton("Close");
	outputArea = new QPlainTextEdit;
	outputArea->setReadOnly(true);
    outputArea->setFont(QFont("Monospace"));
	
	layout->addWidget(outputArea, 0, 0, 1, 4);
	layout->addWidget(close     , 1, 3, 1, 1);
	layout->setColumnStretch(0, 5);
	
	frame->setLayout(layout);
	setCentralWidget(frame);
    setWindowTitle("egsphant creation log");
	resize(1280,720);
	hide();
}

void logWindow::connectLayout(){
	connect(close, SIGNAL(released()),
			this, SLOT(hide()));	
}