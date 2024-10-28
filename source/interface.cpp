/*
################################################################################
#
#  egs_brachy_GUI interface.cpp
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
#include "GUI/phantInterface.h"
#include "GUI/sourceInterface.h"
#include "GUI/ebInterface.h"
#include "GUI/doseInterface.h"
#include "GUI/appInterface.h"

// Constructor
Interface::Interface()
// [\d*] one digit followed by:
// 1) some number of digits
// 2) (e|E)[+-]?\d{1,2} character e or E, an optional +, then 1-2 digits 
// 3) .\d*(e|E)[+]?\d{1,2} dot ., some number of digits, character e or E, an optional +, then 1-2 digits 
	: allowedNums(QRegExp(REGEX_REAL_POS)) {
	// Create the data backend
	data = new Data();
	int err = data->loadDefaults(); // load default parameters
	
	if (err == 101) {
		QMessageBox::critical(0, "egs_brachy_GUI fatal error",
        tr("No environmental variable for EGS_HOME detected, "
		"please ensure EGSnrc is installed properly.\n"
		"Exiting egs_brachy_GUI."));
	}
	
	egsinp = new EGSInput();
	
	createGlobalWidgets(); // To populate the tabs with
	
	// Create the tabs
	ebInt     = (QWidget*)(new ebInterface(this));
	sourceInt = (QWidget*)(new sourceInterface(this));
	phantInt  = (QWidget*)(new phantInterface(this));
	doseInt   = (QWidget*)(new doseInterface(this));
	appInt    = (QWidget*)(new appInterface(this));
	
    createLayout();
    connectLayout();

    refresh();
}

// Destructor
Interface::~Interface() {
	// Qt handles the destruction of any QWidgets that are in the main
	// window inheritance tree (Interface->tabWidget->tabs etc..)
	
	// We need to delete all QWidgets not in the inheritance tree
	// or non QWidget objects
	
	deleteProgress(); // Delete progress bar vars
}
	
// Global widgets
void Interface::createGlobalWidgets() {
	QString ttt = "";
	
	// Phantom list
	phantomLabel     = new QLabel(tr("<b>Virtual Patient Models</b>")); 
	phantomOnlyLocal = new QCheckBox(tr("only local"));
	phantomOnlyLocal->setChecked(true);
	ttt = "Only show the models saved in the local eb_gui database or "
	      "include the default phantoms distributed with egs_brachy.";
	phantomOnlyLocal->setToolTip(ttt);
	
	phantomListView  = new QListWidget();
	phantomListView->setSelectionMode(QAbstractItemView::SingleSelection);
	phantomCreate    = new QPushButton(tr("Create")); 
	phantomView      = new QPushButton(tr("View log")); 
	phantomDelete    = new QPushButton(tr("Delete"));
	ttt = "Select the model for the simulation.";
	phantomListView->setToolTip(ttt);
	ttt = "Create a new simple phantom.";
	phantomCreate->setToolTip(ttt);
	ttt = "View the log file generated when the DICOM data was imported.";
	phantomView->setToolTip(ttt);
	ttt = "Delete the file along with associated log and mask (contour) files.";
	phantomDelete->setToolTip(ttt);
	
	phantomCreate->setDisabled(true); // To be implemented
	
	phantomGrid  = new QGridLayout();
	phantomFrame = new QFrame();
		
	phantomGrid->addWidget(phantomLabel    , 0, 0, 1, 4);
	phantomGrid->addWidget(phantomOnlyLocal, 0, 4, 1, 2);
	phantomGrid->addWidget(phantomListView , 1, 0, 1, 6);
	phantomGrid->addWidget(phantomCreate   , 2, 0, 1, 2);
	phantomGrid->addWidget(phantomView     , 2, 2, 1, 2);
	phantomGrid->addWidget(phantomDelete   , 2, 4, 1, 2);
	
	phantomFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
	phantomFrame->setLineWidth(LINE_WIDTH);
	phantomFrame->setLayout(phantomGrid);
	
	// Source list
	sourceLabel       = new QLabel(tr("<b>Sources</b>")); 
	sourceChooser     = new QComboBox();
	sourceShowWrapped = new QCheckBox(tr("add unwrapped sources"));
	sourceShowWrapped->setChecked(false);
	sourceListView    = new QListWidget();
	sourceListView->setSelectionMode(QAbstractItemView::SingleSelection);
	
	ttt = "Select the isotope to filter different seeds (or sources) for the simulation.";
	sourceChooser->setToolTip(ttt);
	ttt = "By default, only wrapped sources are used which can avoid common geometry errors.  "
		  "When checked, this will include all possible sources included with egs_brachy, including "
		  "the unwrapped version of wrapped seeds.";
	sourceShowWrapped->setToolTip(ttt);
	ttt = "Select the seed (or source) for the simulation.";
	sourceListView->setToolTip(ttt);
	
	sourceScaleBox      = new QComboBox();
	sourceScaleEdit     = new QLineEdit("1");
	sourceScaleEdit->setValidator(&allowedNums);
	sourceScaleBox->addItem("Air kerma strength");
	sourceScaleBox->addItem("Dose scaling factor");
	
	ttt = "Select whether to scale simulation dose (Gy per history) by air kerma strength, which is "
		  "read in from DICOM plan files and can be found in source locations log files, or by a single "
		  "factor.";
	sourceScaleEdit->setToolTip(ttt);
	sourceScaleBox->setToolTip(ttt);
										
	sourcePermTime      = new QCheckBox("Permanent implant treatment");
	sourcePermTime->setChecked(true);
	sourceTempTimeLabel = new QLabel("Treatment duration (hours)");
	sourceTempTimeEdit  = new QLineEdit("1");
	sourceTempTimeLabel->setDisabled(true);
	sourceTempTimeEdit->setDisabled(true);
	sourceTempTimeEdit->setValidator(&allowedNums);
	
	ttt = "When scaling with air kerma strength, selecting permanent is used to determine the final "
		  "dose (all activity absorbed by the patient).";
	sourcePermTime->setToolTip(ttt);
	ttt = "When scaling with air kerma strength and permanent is not selected, the total treatment "
		  "is required to determine the integral over the activity curve during treatment time.";
	sourceTempTimeLabel->setToolTip(ttt);
	sourceTempTimeEdit->setToolTip(ttt);
	
	sourceGrid  = new QGridLayout();
	sourceFrame = new QFrame();
	
	sourceChooser->addItem("I125");
	sourceChooser->addItem("Pd103");
	sourceChooser->addItem("Ir192");
	sourceChooser->addItem("Cs131");
		
	sourceGrid->addWidget(sourceLabel        , 0, 0, 1, 2);
	sourceGrid->addWidget(sourceChooser      , 1, 0, 1, 2);
	sourceGrid->addWidget(sourceShowWrapped  , 1, 2, 1, 1);
	sourceGrid->addWidget(sourceListView     , 2, 0, 1, 3);
	
	sourceGrid->addWidget(sourceScaleBox     , 3, 0, 1, 1);
	sourceGrid->addWidget(sourceScaleEdit    , 3, 1, 1, 2);
	
	sourceGrid->addWidget(sourcePermTime     , 4, 0, 1, 3);
	sourceGrid->addWidget(sourceTempTimeLabel, 5, 0, 1, 1);
	sourceGrid->addWidget(sourceTempTimeEdit , 5, 1, 1, 2);
	
	sourceFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
	sourceFrame->setLineWidth(LINE_WIDTH);
	sourceFrame->setLayout(sourceGrid);
	
	// Transform list
	transformationLabel      = new QLabel(tr("<b>Source Locations</b>")); 
	transformationOnlyLocal  = new QCheckBox(tr("only local"));
	transformationOnlyLocal->setChecked(true);
	
	ttt = "Only show the source locations saved in the local eb_gui database or "
	      "include the default phantoms distributed with egs_brachy.";
	transformationOnlyLocal->setToolTip(ttt);
	
	transformationListView   = new QListWidget();
	transformationListView->setSelectionMode(QAbstractItemView::SingleSelection);
	transformationCreate     = new QPushButton(tr("Create"));
	transformationView       = new QPushButton(tr("View log"));
	transformationDelete     = new QPushButton(tr("Delete"));
	
	ttt = "When scaling with air kerma strength, selecting permanent is used to determine the final "
		  "dose (all activity absorbed by the patient).";
	transformationListView->setToolTip(ttt);
	ttt = "Create a source location file using a basic cell editor.";
	transformationCreate->setToolTip(ttt);
	ttt = "View the log file generated when the DICOM data was imported.";
	transformationView->setToolTip(ttt);
	ttt = "Delete the file along with associated log files.";
	transformationDelete->setToolTip(ttt);
	
	transformationDwell       = new QCheckBox("Dwell times");
	transformationDwellLabel  = new QLabel("Dwell file");
	transformationDwellButton = new QPushButton(tr("Load"));
	transformationDwellEdit   = new QLineEdit("none selected");
	transformationDwellEdit->setDisabled(true);
	
	ttt = "Select dwell time (or variable activity) file used to generate relative activity in seeds.";
	transformationDwell->setToolTip(ttt);
	transformationDwellLabel->setToolTip(ttt);
	transformationDwellButton->setToolTip(ttt);
	transformationDwellEdit->setToolTip(ttt);
	
	transformationCreate->setDisabled(true); // To be implemented
	
	transformationGrid  = new QGridLayout();
	transformationFrame = new QFrame();
		
	transformationGrid->addWidget(transformationLabel      , 0, 0, 1, 4);
	transformationGrid->addWidget(transformationOnlyLocal  , 0, 4, 1, 2);
	transformationGrid->addWidget(transformationListView   , 1, 0, 1, 6);
	transformationGrid->addWidget(transformationCreate     , 2, 0, 1, 2);
	transformationGrid->addWidget(transformationView       , 2, 2, 1, 2);
	transformationGrid->addWidget(transformationDelete     , 2, 4, 1, 2);
	
	transformationGrid->addWidget(transformationDwell      , 3, 0, 1, 6);
	transformationGrid->addWidget(transformationDwellLabel , 4, 0, 1, 2);
	transformationGrid->addWidget(transformationDwellButton, 4, 2, 1, 1);
	transformationGrid->addWidget(transformationDwellEdit  , 4, 3, 1, 3);
	
	transformationFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
	transformationFrame->setLineWidth(LINE_WIDTH);
	transformationFrame->setLayout(transformationGrid);
	
	// Geometry list
	geometryLabel     = new QLabel(tr("<b>Additional Geometries</b>"));
	geometryChooser   = new QComboBox();
	geometryListView  = new QListWidget();
	geometryCreate    = new QPushButton(tr("Create")); 
	geometryDelete    = new QPushButton(tr("Delete")); 
	geometryAdd       = new QPushButton(tr("Add")); 
	geometryList      = new QTabWidget();
	
	ttt = "Select a geometry model filter to view egs_brachy default geometries with.";
	geometryChooser->setToolTip(ttt);
	ttt = "Select a type of geometry to add to the simulation.";
	geometryListView->setToolTip(ttt);
	ttt = "Create a custom geometry using an editor.";
	geometryCreate->setToolTip(ttt);
	ttt = "Add the selected geometry to the simulation.";
	geometryAdd->setToolTip(ttt);
	ttt = "Delete a custom geometry.";
	geometryDelete->setToolTip(ttt);
	
	geometryGrid  = new QGridLayout();
	geometryFrame = new QFrame();
	
	geometryChooser->addItem("eye_plaques");
	geometryChooser->addItem("applicators");
	
	geometryCreate->setDisabled(true); // To be implemented
	geometryDelete->setDisabled(true); // To be implemented
	
	geometryList->setTabPosition(QTabWidget::West);
	
	geometryGrid->addWidget(geometryLabel   , 0, 0, 1, 6);
	geometryGrid->addWidget(geometryChooser , 1, 0, 1, 6);
	geometryGrid->addWidget(geometryListView, 2, 0, 1, 6);
	geometryGrid->addWidget(geometryCreate  , 3, 0, 1, 3);
	geometryGrid->addWidget(geometryDelete  , 3, 3, 1, 3);
	geometryGrid->addWidget(geometryAdd     , 4, 0, 1, 6);
	geometryGrid->addWidget(geometryList    , 0, 6, 5, 1);
	
	geometryFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
	geometryFrame->setLineWidth(LINE_WIDTH);
	geometryFrame->setLayout(geometryGrid);
	
	// Dose list
	doseLabel     = new QLabel(tr("<b>Dose Distributions</b>"));
	doseListView  = new QListWidget();
	doseListView->setSelectionMode(QAbstractItemView::SingleSelection);
	doseImport    = new QPushButton(tr("Import"));
	doseImport->setDisabled(true); // To be implemented
	doseDelete    = new QPushButton(tr("Delete"));
	
	ttt = "Select a geometry model filter to view egs_brachy default geometries with.";
	geometryChooser->setToolTip(ttt);
	ttt = "Select a calculated dose file.";
	doseListView->setToolTip(ttt);
	ttt = "Import an external 3ddose file to the eb_gui database.";
	doseImport->setToolTip(ttt);
	ttt = "Delete selected dose file.";
	doseDelete->setToolTip(ttt);
	
	doseGrid  = new QGridLayout();
	doseFrame = new QFrame();
		
	doseGrid->addWidget(doseLabel   , 0, 0, 1, 2);
	doseGrid->addWidget(doseListView, 1, 0, 1, 2);
	doseGrid->addWidget(doseImport  , 2, 0, 1, 1);
	doseGrid->addWidget(doseDelete  , 2, 1, 1, 1);

	doseFrame->setFrameStyle(QFrame::Box | QFrame::Raised);
	doseFrame->setLineWidth(LINE_WIDTH);
	doseFrame->setLayout(doseGrid);
}

// Layout Settings
void Interface::createLayout() {
	createProgress(); // Set up progress bar
	
	mainLayout  = new QGridLayout();
    closeButton = new QPushButton(tr("Close"));
    tabWidget   = new QTabWidget();
	
	tabWidget->addTab(ebInt    ,tr("Run egs_brachy"));
	tabWidget->addTab(sourceInt,tr("Import DICOM Plan"));
	tabWidget->addTab(phantInt ,tr("Import DICOM Virtual Patient Model"));
	tabWidget->addTab(doseInt  ,tr("Analyze Results"));
	tabWidget->addTab(appInt   ,tr("Export Results"));
	
	mainLayout->addWidget(tabWidget  ,0,0,1,2);
	mainLayout->addWidget(closeButton,1,1,1,1);
	mainLayout->setColumnStretch(0,5);
	
	// Populate the global widgets
	phantomRepopulate();
	sourceRepopulate();
	transformationRepopulate();
	geometryRepopulate();
	doseRepopulate();
	
	// Set up window
	setLayout(mainLayout);
    setWindowTitle("eb_gui");
	resize(1280,720);
    show();
    activateWindow();
    raise();
}

void Interface::connectLayout() {
	connectProgress(); // Connect progress bar
	
    connect(tabWidget, SIGNAL(currentChanged(int)),
			this, SLOT(refresh()));
    connect(closeButton, SIGNAL(clicked()),
			qApp, SLOT(quit()));
	
	// Global widgets
    connect(phantomOnlyLocal, SIGNAL(stateChanged(int)),
			this, SLOT(phantomRepopulate()));
			
	connect(sourceChooser, SIGNAL(currentTextChanged(QString)),
			this, SLOT(sourceRepopulate()));
	connect(sourceShowWrapped, SIGNAL(stateChanged(int)),
			this, SLOT(sourceRepopulate()));
	connect(sourcePermTime, SIGNAL(stateChanged(int)),
			this, SLOT(sourceRefresh()));
			
	connect(transformationDwell, SIGNAL(stateChanged(int)),
			this, SLOT(transformationRefresh()));
	connect(transformationDwellButton, SIGNAL(clicked()),
			this, SLOT(transformationLoadDwells()));			
			
	connect(geometryChooser, SIGNAL(currentTextChanged(QString)),
			this, SLOT(geometryRepopulate()));
	connect(geometryAdd, SIGNAL(clicked()),
			this, SLOT(geometryAddNew()));
			
	// Global widget create/view/delete buttons
	connect(phantomView, SIGNAL(clicked()),
			this, SLOT(phantomViewLog()));
	connect(transformationView, SIGNAL(clicked()),
			this, SLOT(transformViewLog()));
	connect(phantomDelete, SIGNAL(clicked()),
			this, SLOT(phantomDeleteFile()));
	connect(transformationDelete, SIGNAL(clicked()),
			this, SLOT(transformDeleteFile()));
}

// Global widget functions
void Interface::phantomRepopulate() {
	phantomListView->clear();
	
	((doseInterface*)doseInt)->phantSelect->clear();
	((doseInterface*)doseInt)->phantSelect->addItem("none");
	
	((doseInterface*)doseInt)->histPhantSelect->clear();
	((doseInterface*)doseInt)->histPhantSelect->addItem("none");
	
	((doseInterface*)doseInt)->profPhantSelect->clear();
	((doseInterface*)doseInt)->profPhantSelect->addItem("none");
	
	((appInterface*)appInt)->egsphant->clear();
	((appInterface*)appInt)->egsphant->addItem("none");
	
	for (int i = 0; i < data->localNamePhants.size(); i++) {
		phantomListView->addItem(data->localNamePhants[i]);
		((doseInterface*)doseInt)->phantSelect->addItem(data->localNamePhants[i]);
		((doseInterface*)doseInt)->histPhantSelect->addItem(data->localNamePhants[i]);
		((doseInterface*)doseInt)->profPhantSelect->addItem(data->localNamePhants[i]);
		((appInterface*)appInt)->egsphant->addItem(data->localNamePhants[i]);
	}
	if (!phantomOnlyLocal->isChecked())
		for (int i = 0; i < data->libNamePhants.size(); i++) {
			phantomListView->addItem(data->libNamePhants[i]);		
		}
}

void Interface::sourceRepopulate() {
	sourceListView->clear();
	for (int i = 0; i < data->libNameSources.size(); i++) {
		if (data->libDirSources[i].contains(QString("/")+sourceChooser->currentText()+"_")) {
			if (data->libNameSources[i].endsWith("_wrapped") && !sourceShowWrapped->isChecked())
				sourceListView->addItem(data->libNameSources[i]);
			else if (!data->libNameSources[i].endsWith("_wrapped") && sourceShowWrapped->isChecked())
				sourceListView->addItem(data->libNameSources[i]);
		}
	}
}

void Interface::transformationRepopulate() {
	transformationListView->clear();
	
	((appInterface*)appInt)->transform->clear();
	((appInterface*)appInt)->transform->addItem("none");
	
	((phantInterface*)phantInt)->marTransformation->clear();
	for (int i = 0; i < data->localNameTransforms.size(); i++) {
		transformationListView->addItem(data->localNameTransforms[i]);
		((phantInterface*)phantInt)->marTransformation->addItem(data->localNameTransforms[i]);
		((appInterface*)appInt)->transform->addItem(data->localNameTransforms[i]);
	}
	if (!transformationOnlyLocal->isChecked())
		for (int i = 0; i < data->libNameTransforms.size(); i++) {
			transformationListView->addItem(data->libNameTransforms[i]);	
		}
}

void Interface::geometryRepopulate() {
	geometryListView->clear();
	for (int i = 0; i < data->libNameGeometries.size(); i++)
		if (data->libDirGeometries[i].contains(QString("/")+geometryChooser->currentText()+"/"))
			geometryListView->addItem(data->libNameGeometries[i]);
}

void Interface::geometryAddNew() {
	if (geometryListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "geometry error",
        tr("No geometry selected."));
		return;
	}
	EGS_geom* temp = new EGS_geom(geometryListView->currentItem()->text(),geometryListView->currentRow());
	
	geometryList->insertTab(0, temp, "");
	geometryList->tabBar()->setTabButton(0, QTabBar::LeftSide, temp->labelTab);
	geometryList->setCurrentIndex(0);
	temp->Prio->setText(QString::number(geometryList->count()*10+100));
	
	connect(temp->remove, SIGNAL(clicked()),
			temp, SLOT(deleteMyself()));
}

void Interface::doseRepopulate() {
	doseListView->clear();
	
	((doseInterface*)doseInt)->mapDoseBox->clear();
	((doseInterface*)doseInt)->isoDoseBox[0]->clear();
	((doseInterface*)doseInt)->isoDoseBox[1]->clear();
	((doseInterface*)doseInt)->isoDoseBox[2]->clear();
	((doseInterface*)doseInt)->histDoseSelect->clear();
	((doseInterface*)doseInt)->profDoseSelect->clear();
	((doseInterface*)doseInt)->resetDoses();
	
	((doseInterface*)doseInt)->mapDoseBox->addItem("none");
	((doseInterface*)doseInt)->isoDoseBox[0]->addItem("none");
	((doseInterface*)doseInt)->isoDoseBox[1]->addItem("none");
	((doseInterface*)doseInt)->isoDoseBox[2]->addItem("none");
	
	((appInterface*)appInt)->dose->clear();
	((appInterface*)appInt)->dose->addItem("none");
	
	for (int i = 0; i < data->localNameDoses.size(); i++) {
		doseListView->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->mapDoseBox->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->isoDoseBox[0]->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->isoDoseBox[1]->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->isoDoseBox[2]->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->histDoseSelect->addItem(data->localNameDoses[i]);
		((doseInterface*)doseInt)->profDoseSelect->addItem(data->localNameDoses[i]);
		((appInterface*)appInt)->dose->addItem(data->localNameDoses[i]);
	}
}

void Interface::phantomViewLog() {
	QList <QListWidgetItem*> matchingNames = phantomListView->selectedItems();
	
	if (!matchingNames.size()) {
		QMessageBox::warning(0, "Load VPM log error",
		tr("No virtual patient model file selected."));
		return;		
	}
	
	QString fileName = matchingNames[0]->text();
	
	// Get the file index
	int i = data->localNamePhants.indexOf(fileName);
	
	// Get the raw file name
	if (fileName.endsWith(".gz"))
		fileName = fileName.left(fileName.size()-3);
	if (fileName.endsWith(".egsphant"))
		fileName = fileName.left(fileName.size()-9);
	if (fileName.endsWith(".geom"))
		fileName = fileName.left(fileName.size()-5);
	
	QFile file(data->localDirPhants[i]+"/"+fileName+".log");
	if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
		QMessageBox::warning(0, "Log file could not load",
		tr("Failed to open ")+fileName+".log"+tr(" text file for reading."));
		return;		
	}
	
	QString content;
	while(!file.atEnd())
		content.append(file.readLine());
	file.close();
	
	((phantInterface*)phantInt)->log->outputArea->clear();
	((phantInterface*)phantInt)->log->outputArea->setPlainText(content);
	((phantInterface*)phantInt)->log->show();
}

void Interface::transformViewLog() {
	QList <QListWidgetItem*> matchingNames = transformationListView->selectedItems();
	
	if (!matchingNames.size()) {
		QMessageBox::warning(0, "Load source locations log error",
		tr("No source locations file selected."));
		return;		
	}
	
	QString fileName = matchingNames[0]->text();
	
	// Get the file index
	int i = data->localNameTransforms.indexOf(fileName);
	
	// Load the text
	QFile file(data->localDirTransforms[i]+"/"+fileName+".log");
	if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
		QMessageBox::warning(0, "Log file could not load",
		tr("Failed to open ")+fileName+".log"+tr(" text file for reading."));
		return;		
	}
	
	QString content;
	while(!file.atEnd())
		content.append(file.readLine());
	file.close();
	
	// Launch the console
	((sourceInterface*)sourceInt)->log->outputArea->clear();
	((sourceInterface*)sourceInt)->log->outputArea->setPlainText(content);
	((sourceInterface*)sourceInt)->log->show();
}

void Interface::phantomDeleteFile() {
	QList <QListWidgetItem*> matchingNames = phantomListView->selectedItems();
	
	if (!matchingNames.size()) {
		QMessageBox::warning(0, "Delete phantom error",
		tr("No phantom file selected."));
		return;		
	}
		
	if (firstDelete)
		if (QMessageBox::No == QMessageBox::question(this, "Deleting phantom",
		tr("Are you sure you want to delete ") + matchingNames[0]->text() + tr("?")))
			return;
	firstDelete = false;
		
	QString fileName = matchingNames[0]->text();
	if (fileName.endsWith(".gz"))
		fileName = fileName.left(fileName.size()-3);
	if (fileName.endsWith(".egsphant"))
		fileName = fileName.left(fileName.size()-9);
	if (fileName.endsWith(".geom"))
		fileName = fileName.left(fileName.size()-5);
	
	// Get the file index
	int i = data->localNamePhants.indexOf(matchingNames[0]->text());
	
	// Delete all associated files
	QFile(data->localDirPhants[i]+matchingNames[0]->text()).remove();
	QFile(data->localDirPhants[i]+fileName+".log").remove();
	
	QDirIterator files (data->gui_location+"/database/mask/", {QString(fileName)+".*.egsphant.gz"},
						QDir::NoFilter, QDirIterator::Subdirectories);
	
	while(files.hasNext()) {
		files.next();
		QFile (data->gui_location+"/database/mask/"+files.fileName()).remove();
	}
	
	// Delete the file references
	data->localNamePhants.removeAt(i);
	data->localDirPhants.removeAt(i);
	
	// Remove it from the list widget
	delete matchingNames[0];
}

void Interface::transformDeleteFile() {
	QList <QListWidgetItem*> matchingNames = transformationListView->selectedItems();
	
	if (!matchingNames.size()) {
		QMessageBox::warning(0, "Delete source locations error",
		tr("No source locations file selected."));
		return;		
	}
	
	if (firstDelete)
		if (QMessageBox::No == QMessageBox::question(this, "Deleting source locations",
		tr("Are you sure you want to delete ") + matchingNames[0]->text() + tr("?")))
			return;
	firstDelete = false;
	
	QString fileName = matchingNames[0]->text();
	
	// Get the file index
	int i = data->localNameTransforms.indexOf(fileName);
	
	// Delete all associated files
	QFile(data->localDirTransforms[i]+fileName).remove();
	QFile(data->localDirTransforms[i]+fileName+".log").remove();
	QFile(data->localDirTransforms[i]+fileName+".dwell").remove();
	QFile(data->localDirTransforms[i]+fileName+".activity").remove();
	
	// Delete the file references
	data->localNameTransforms.removeAt(i);
	data->localDirTransforms.removeAt(i);
	
	// Remove it from the list widget
	delete matchingNames[0];
}

// Refresh
void Interface::refresh() {
	// call the refresh function of the appropriate tab
	switch (tabWidget->currentIndex()) {
		case 0 : 
			((ebInterface*)ebInt)->refresh();
		break;
		case 1 :
			((sourceInterface*)sourceInt)->refresh();		
		break;
		case 2 : 
			((phantInterface*)phantInt)->refresh();		
		break;
		case 3 : 
			((doseInterface*)doseInt)->refresh();		
		break;
		case 4 : 
			((appInterface*)appInt)->refresh();		
		break;
		default :
			;// do nothing if none are selected somehow
	};
}

void Interface::sourceRefresh() {
	if (sourcePermTime->isChecked()) {
		sourceTempTimeLabel->setDisabled(true);
		sourceTempTimeEdit->setDisabled(true);
	}
	else {
		sourceTempTimeLabel->setDisabled(false);
		sourceTempTimeEdit->setDisabled(false);
	}
}

void Interface::transformationRefresh() {
	if (transformationDwell->isChecked()) {
		transformationDwellLabel->setDisabled(false);
		transformationDwellButton->setDisabled(false);
		
		connect(transformationListView, SIGNAL(currentRowChanged(int)),
				this, SLOT(transformationLoadDwellsAuto(int)));
	}
	else {
		transformationDwellLabel->setDisabled(true);
		transformationDwellButton->setDisabled(true);
		
		disconnect(transformationListView, SIGNAL(currentRowChanged(int)),
				   this, SLOT(transformationLoadDwellsAuto(int)));
	}
}

// Transformation functions
void Interface::transformationLoadDwells() {
	QString path = QFileDialog::getOpenFileName(this, tr("Load activity/dwell file"), data->gui_location+"/database/transformation/", tr("Activity File (*.dwell *.activity)"));
	
	// Check if it works
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file."));
        return;
	}
	
	// Count the number of doubles
	QTextStream in(&file);
	in.skipWhiteSpace();
	
	int dwells = 0;
	double temp;
	maxDwellTime=0;
	while (!in.atEnd()) {
        in >> temp;
		if (temp>maxDwellTime){
			maxDwellTime=temp;
		}
		dwells++;
    }
	file.close();
	
	// Compare to selected transformation file count
	int i = transformationListView->currentRow();
	QString path2 = i < data->localDirTransforms.size()?
					data->localDirTransforms[i]:data->libDirTransforms[i-data->localDirTransforms.size()]; // GUI parameter
	path2 = path2 + (i < data->localDirTransforms.size()?
			data->localNameTransforms[i]:data->libNameTransforms[i-data->localNameTransforms.size()]); // GUI parameter
	
	QFile file2(path2);
	if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open transformation file."));
        return;
	}
	
	int seeds = 0;
	
	while (!file2.atEnd())
        if (file2.readLine().contains("start transformation"))
			seeds++;
	file2.close();
	
	if (seeds != dwells) {
		QMessageBox::warning(0, "seed count error",
        tr("Chosen activity/dwell file count %1 does not match transformation file %2, aborting.").arg(dwells).arg(seeds));
        return;
	}
	
	transformationDwellEdit->setText(path);
	transformationDwellEdit->setToolTip(path);
}

void Interface::transformationLoadDwellsAuto(int i) {
	// Compare to selected transformation file count
	QString path2 = i < data->localDirTransforms.size()?
					data->localDirTransforms[i]:data->libDirTransforms[i-data->localDirTransforms.size()]; // GUI parameter
	path2 = path2 + (i < data->localDirTransforms.size()?
			data->localNameTransforms[i]:data->libNameTransforms[i-data->localNameTransforms.size()]); // GUI parameter
	
	QFile file2(path2);
	if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open transformation file."));
        return;
	}
	
	int seeds = 0;
	
	while (!file2.atEnd())
        if (file2.readLine().contains("start transformation"))
			seeds++;
	file2.close();
		
	// Check if it works
	QFile file(path2+".dwell"), file3(path2+".activity"), *final_file;
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {	
		if (!file3.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QMessageBox::warning(0, "no dwell or activity file found",
			tr("Please load one manually if one is desired."));
			
			transformationDwellEdit->setText("none selected");
			transformationDwellEdit->setToolTip("none selected");
			
			return;
		}
		else {
			final_file = &file3;
		}
	}
	else {
		final_file = &file;
	}
	
	// Count the number of doubles
	QTextStream in(final_file);
	in.skipWhiteSpace();
	
	int dwells = 0;
	double temp;
	
	while (!file.atEnd()) {
        in >> temp;
		in.skipWhiteSpace();
		dwells++;
    }
	final_file->close();
	
	if (seeds != dwells) {
		QMessageBox::warning(0, "seed count error",
        tr("Chosen activity/dwell file seed count does not match transformation file, aborting."));
        return;
	}
}

// Other functions
int Interface::populateEgsinp() {
	int i = -1;
	QString s = "";
	
	// run control
	egsinp->RC_ncase        = ((ebInterface*)ebInt)->ncaseEdit->text(); // GUI parameter
	egsinp->RC_nbatch       = data->def_nbatch; // Configuration parameter
	egsinp->RC_nchunk       = data->def_nchunk; // Configuration parameter
	egsinp->RC_geomLimit    = data->def_geomLimit; // Configuration parameter
	egsinp->RC_calculation  = "first"; // Pre-set parameter
	egsinp->RC_egsdatFormat = "text";  // Pre-set parameter
	
	// run mode
	egsinp->RM_normal = ((ebInterface*)ebInt)->runModeBox->currentText(); // GUI parameter
	if (((ebInterface*)ebInt)->waterBox->isChecked())
		egsinp->RM_normal = "superposition"; // GUI parameter
	
	// media definition
	egsinp->Med_AE   = data->def_AE; // Configuration parameter
	egsinp->Med_UE   = data->def_UE; // Configuration parameter
	egsinp->Med_AP   = data->def_AP; // Configuration parameter
	egsinp->Med_UP   = data->def_UP; // Configuration parameter
	egsinp->Med_file = ((ebInterface*)ebInt)->materialEdit->text(); // Configuration/GUI parameter
	
	// geometry definition
	i = phantomListView->currentRow();
	
	egsinp->phantomFile = i < data->localDirPhants.size()?
						  data->localDirPhants[i]:data->libDirPhants[i-data->localDirPhants.size()]; // GUI parameter
	egsinp->phantomFile = egsinp->phantomFile + (i < data->localDirPhants.size()?
						  data->localNamePhants[i]:data->libNamePhants[i-data->localDirPhants.size()]); // GUI parameter
	
	if (((ebInterface*)ebInt)->waterBox->isChecked()) {
		// make TG-43 phantom
		// Parse egsphant file
		if (egsinp->phantomFile.endsWith(".egsphant.gz") || egsinp->phantomFile.endsWith(".egsphant")) {
			std::istream* input;
			igzstream ginp;
			std::ifstream tinp;
			
			if (egsinp->phantomFile.endsWith(".egsphant.gz")) {
				ginp.open(egsinp->phantomFile.toStdString().c_str());
				input = &ginp;
			}
			else {
				tinp.open(egsinp->phantomFile.toStdString().c_str());
				input = &tinp;
			}
			
			// get and ignore media
			int nmed;
			(*input) >> nmed;
			std::string med;
			for (int i=0; i < nmed; i++) {
				(*input) >> med;
			}
			
			// estepe is ignored
			double estepe;
			for (int i=0; i < nmed; i++) {
				(*input) >> estepe;
			}
			
			// read in all bounds
			std::string bound2;
			
			int nx, ny, nz;
			(*input) >> nx >> ny >> nz;
			QStringList x, y, z;
			
			for (int i=0; i < nx+1; i++) {
				(*input) >> bound2;
				x.append(QString(bound2.c_str()));
			}
			for (int i=0; i < ny+1; i++) {
				(*input) >> bound2;
				y.append(QString(bound2.c_str()));
			}
			for (int i=0; i < nz+1; i++) {
				(*input) >> bound2;
				z.append(QString(bound2.c_str()));
			}
			
			ginp.close();
			tinp.close();
			
			// Generate XYZ geom file
			QString text = "";
			
			text += ":start geometry definition:\n";
			text += "    :start geometry:\n";
			text += "        name    = TG43_egsphant\n";
			text += "        library = egs_ndgeometry\n";
			text += "        type    = EGS_XYZGeometry\n";
			text += QString("        x-planes = ")+x.join(" ")+"\n";
			text += QString("        y-planes = ")+y.join(" ")+"\n";
			text += QString("        z-planes = ")+z.join(" ")+"\n";
			text += "        :start media input:\n";
			text += "            media = WATER_0.998\n";
			text += "        :stop media input:\n";
			text += "    :stop geometry:\n";
			text += "    simulation geometry = TG43_egsphant\n";
			text += ":stop geometry definition:\n";
			
			// Make geom file
			QString fileName = phantomListView->currentItem()->text();
			if (fileName.endsWith(".gz")) fileName = fileName.left(fileName.length()-3);
			if (fileName.endsWith(".egsphant")) fileName = fileName.left(fileName.length()-9);
			fileName += ".tg43.geom";
			
			QFile geomFile (data->gui_location+"/database/egsphant/"+fileName);
			//if (!geomFile.remove()) { // If it exists, delete it and if it doesn't, add the location
			//	data->localDirPhants << data->gui_location+"/database/egsphant/";
			//	data->localNamePhants << fileName;
			//}
			
			if (geomFile.exists())
				geomFile.remove();
			
			if (geomFile.open(QFile::WriteOnly | QFile::Truncate)) {
				QTextStream out(&geomFile);
				out << text;
			}
			geomFile.close();
			
			egsinp->phantomFile = data->gui_location+"/database/egsphant/"+fileName;
			//phantomRepopulate();
		}
		else {
			QMessageBox::warning(0, "TG-43 error",
			tr("TG-43 option only works with an egsphant, continuing with non-egsphant assuming it is pure water."));			
		}
	}
	
	s = sourceListView->currentItem()->text();
	i = data->libNameSources.indexOf(s);
	egsinp->sourceGeomFile = data->libDirSources[i] + data->libNameSources[i] + ".geom"; // GUI parameter
	egsinp->discoverPoints = data->def_seedDisc; // Pre-set parameter
	
	// Add additional geometries
	egsinp->additional_geomNames.clear(); // GUI parameter
	egsinp->additional_geomFiles.clear(); // GUI parameter
	egsinp->prio.clear();                 // GUI parameter
	egsinp->pos.clear();                  // GUI parameter
	egsinp->rot.clear();                  // GUI parameter
	
	EGS_geom* curGeom;
	for (int j = 0; j < geometryList->count(); j++) {
		curGeom = (EGS_geom*)geometryList->widget(j);
		
		s = curGeom->labelGeom->text();
		egsinp->additional_geomNames.append(s+QString::number(j));
		
		for (i = 0; i < data->libNameGeometries.size(); i++)
			if (!data->libNameGeometries[i].compare(s))
				break;
			
		egsinp->additional_geomFiles.append(data->libDirGeometries[i] + data->libNameGeometries[i] + ".geom");
		
		egsinp->prio.append(curGeom->Prio->text());
		
		egsinp->pos.append(QVector3D(curGeom->xPos->text().toDouble(),
									 curGeom->yPos->text().toDouble(),
									 curGeom->zPos->text().toDouble()));
									 
		egsinp->rot.append(QVector3D(curGeom->xRot->text().toDouble(),
									 curGeom->yRot->text().toDouble(),
									 curGeom->zRot->text().toDouble()));
	}
	
	// volume correction
	egsinp->VC_type    = ((ebInterface*)ebInt)->volCorBox->isChecked()?"correct":"none"; // GUI parameter
	egsinp->VC_density = ((ebInterface*)ebInt)->volCorDen->text(); // GUI parameter
	i = phantomListView->currentRow();
	s = sourceListView->currentItem()->text();
	i = data->libNameSources.indexOf(s);
	egsinp->VC_bound   = data->libDirSources[i]+"boundary.shape"; // GUI parameter
	egsinp->VC_thresh  = "99.9%"; // Pre-set parameter
	
	// source definition
	i = transformationListView->currentRow();
	egsinp->sourceTransFile = i < data->localDirTransforms.size()?
							  data->localDirTransforms[i]:data->libDirTransforms[i-data->localDirTransforms.size()]; // GUI parameter
	egsinp->sourceTransFile = egsinp->sourceTransFile + (i < data->localDirTransforms.size()?
						      data->localNameTransforms[i]:data->libNameTransforms[i-data->localNameTransforms.size()]); // GUI parameter
							  
	s = sourceListView->currentItem()->text();
	i = data->libNameSources.indexOf(s);
	s = data->libDirSources[i];
	
	if (s.contains("Cs131"))
		egsinp->sourceSpecFile = data->eb_location+"/lib/spectra/Cs131_NNDC_2.6_line.spectrum"; // Pre-set parameter
	else if (s.contains("Ir192"))
		egsinp->sourceSpecFile = data->eb_location+"/lib/spectra/Ir192_NNDC_2.6_line.spectrum"; // Pre-set parameter
	else if (s.contains("Pd103"))
		egsinp->sourceSpecFile = data->eb_location+"/lib/spectra/Pd103_NNDC_2.6_line.spectrum"; // Pre-set parameter
	else
		egsinp->sourceSpecFile = data->eb_location+"/lib/spectra/I125_NNDC_line.spectrum"; // Pre-set parameter

        s = sourceListView->currentItem()->text();
        if (s.endsWith("_wrapped"))
                s.chop(8);
        i = data->libNameSources.indexOf(s);
        s = data->libDirSources[i]+s;
        egsinp->sourceSeedFile = s+".shape"; // GUI parameter
	
	s = sourceListView->currentItem()->text();
	i = data->libNameSources.indexOf(s);
	s = data->libDirSources[i];
	egsinp->sourceShapeFile = s+"boundary.shape";  // GUI parameter
	
	if (transformationDwell->isChecked()) {
		QString fileName = transformationDwellEdit->text();	// GUI parameter
		QFile actFile (fileName);
		if (actFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in (&actFile);
			egsinp->sourceDwells = in.readLine(); // GUI parameter
		}
		else {
			QMessageBox::warning(0, "dwell/activity file error",
			tr("Failed to open dwell/activity file for source, distributing activity evenly."));
			egsinp->sourceDwells = "";
		}
		actFile.close();
	}
	else {
		egsinp->sourceDwells = "";
	}
	
	// scoring options
	egsinp->SO_edep     = "no"; // GUI parameter
	egsinp->SO_muenFile = ((ebInterface*)ebInt)->muenEdit->text(); // Configuration/GUI parameter
	
	double tempScale = sourceScaleEdit->text().toDouble(); // Flat scaling or Gy*cm^2/h
	if (!sourceScaleBox->currentText().compare("Air kerma strength")) {
		double airKermaSeed = 0; // Gy*cm^2/h
		
		QString fileName = egsinp->sourceGeomFile;		
		QFile skFile (fileName);
		if (skFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in (&skFile);
			QString line;
			while (!in.atEnd()) {
				line = in.readLine();
				if (line.contains("#") && line.contains("air kerma:")) {
					line = line.split(":").last().trimmed();
					break;
				}
			}
			airKermaSeed = line.toDouble();
			
			if (airKermaSeed) { // if it worked
				tempScale /= airKermaSeed; // tempScale will now have the ratio of experiment/MC per hour with units of history/hour
			}
			else // if none was found
				QMessageBox::warning(0, "air kerma error",
				tr("Failed to find air kerma in source geom file, scaling dose by 1 instead."));
			
			// Assign half-life based on seed folder
			double half_life = 0;
			if (s.contains("Cs131"))
				half_life = 9.7; // Pre-set parameter
			else if (s.contains("Ir192"))
				half_life = 73.83; // Pre-set parameter
			else if (s.contains("Pd103"))
				half_life = 16.99; // Pre-set parameter
			else { // Assume I-125 otherwise
				half_life = 59.49; // Pre-set parameter
			}

			// Note, the tempScale in not permanent needs a scaling factor of the highest seed weight

			double tau = (half_life/0.6931471805)*24.0; // go from half-life in days to mean lifetime in hours
			if (!sourcePermTime->isChecked())
				tempScale *= maxDwellTime/3600; //HDR scaling factor
			else if (sourcePermTime->isChecked())
				tempScale *= tau; //LDR scaling factor
			
			skFile.close();
		}
		else {
			QMessageBox::warning(0, "air kerma error",
			tr("Failed to open geom file for source, scaling dose by 1 instead."));
			tempScale = 1;
		}
		skFile.close();
	}
	else if (!sourceScaleBox->currentText().compare("Dose scaling factor")) {
		// do nothing
	}
	
	egsinp->SO_scale = QString::number(tempScale); // Configuration/GUI parameter

	// fetch
	QStringList media;
	int failFlag = 0, nmed = 0;
	QString line = "";
	
	// Parse one of 3 different types of phantom files for media in the simulation
	if (egsinp->phantomFile.endsWith(".egsphant.gz") || egsinp->phantomFile.endsWith(".egsphant")) {
		std::istream* input;
		igzstream ginp;
		std::ifstream tinp;
		
		if (egsinp->phantomFile.endsWith(".egsphant.gz")) {
			ginp.open(egsinp->phantomFile.toStdString().c_str());
			input = &ginp;
		}
		else {
			tinp.open(egsinp->phantomFile.toStdString().c_str());
			input = &tinp;
		}
		
		(*input) >> nmed;
        for (int i=0; i < nmed; i++) {
			std::string med;
			(*input) >> med;
			media << QString(med.c_str());
		}
		
        ginp.close();
		tinp.close();		
		
		if (media.length() == 0)
			failFlag++;
	}
	else if (egsinp->phantomFile.endsWith(".geom")) {
		QFile geomFile(egsinp->phantomFile);
		
		if (geomFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
			while (!geomFile.atEnd()) {
				line = geomFile.readLine().trimmed();
				
				if (line.startsWith("media =")) {
					if (line.contains(","))
						media =	line.split('=')[1].trimmed().split(",",QString::SkipEmptyParts);
					else
						media = line.split('=')[1].trimmed().split(" ",QString::SkipEmptyParts);
					break;
				}
			}
		}
		
		if (media.length() == 0)
			failFlag++;
	}
	else
		failFlag++;
	
	
	// Check muen data against muen file
	QStringList mediaFinal;
	QFile muen(egsinp->SO_muenFile);
	QTextStream qtinp(&muen);
	if (muen.open(QIODevice::ReadOnly | QIODevice::Text))
		do {
			line = qtinp.readLine();
			if (line.startsWith("Muen values for medium MEDIUM ="))
				for (int i = 0; i < media.size(); i++)
					if (line.contains(media[i])) {
						mediaFinal << media[i];
						media.removeAt(i);
					}
		} while (!qtinp.atEnd());
	else { // Couldn't open muendat file
		QMessageBox::warning(0, "muen file error",
        tr("Failed to open muendat file, aborting simulation."));
		return 101;
	}
	
	if (failFlag) { // Couldn't parse media data
		QMessageBox::warning(0, "geometry media error",
        tr("Failed to read in any media data from selected phantom, aborting simulation."));
		return 102;
	}
	
	if (mediaFinal.isEmpty()) { // Couldn't find any corresponding muen data
		QMessageBox::warning(0, "muen media error",
        tr("Failed to read in any muen data for phantom media, aborting simulation."));
		return 103;
	}
	
	if (!media.isEmpty()) { // Couldn't find all corresponding muen data
		QMessageBox::information(0, "muen media missing",
        tr("Did not find muen data for the following media:\n") +
		media.join(" ") +
		tr("\nSimulation will proceed with no dose scored in the above media."));
	}
	
	egsinp->SO_muenMed = mediaFinal.join(" ");
	
	// transport parameters
	egsinp->TP_file = ((ebInterface*)ebInt)->transportEdit->text(); // Configuration/GUI parameter
	
	return 0;
}

// Progress bar
void Interface::createProgress(){
	progLevel = new double (0);
	progWin = new QWidget();
	progLayout = new QGridLayout();
	progLabel = new QLabel();
	progress = new QProgressBar();
	
    progLayout->addWidget(progLabel, 0, 0);
    progLayout->addWidget(progress, 1, 0);
    progWin->setLayout(progLayout);
    progWin->resize(300, 0);
    progress->setRange(0, 100);
}

void Interface::connectProgress(){
    connect(data, SIGNAL(newProgress(QString)),
		 this, SLOT(resetProgress(QString)));
    connect(data, SIGNAL(madeProgress(double)),
		 this, SLOT(updateProgress(double)));
    connect(data, SIGNAL(completedProgress()),
		 this, SLOT(finishedProgress()));
    connect(data, SIGNAL(newProgressName(QString)),
		 this, SLOT(nameProgress(QString)));
}

void Interface::deleteProgress(){
	delete progWin;
	delete progLevel;
}

void Interface::resetProgress(QString title){
    progress->reset();
    *progLevel = 0;
	progLabel->setText("Loading...");
    progWin->setWindowTitle(title);
    progWin->show();
    progWin->raise();
}

void Interface::updateProgress(double percent){
	if (*progLevel+percent > 100)
		finishedProgress();
	
	*progLevel += percent;
    progress->setValue(int(*progLevel));
	
	QApplication::processEvents();
}

void Interface::nameProgress(QString text){
	progLabel->setText(text);
	progLabel->repaint();
}

void Interface::finishedProgress(){
    progWin->hide();	
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// EGS_geom~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EGS_geom::EGS_geom(QString n, int ind) : name (n), index(ind),
allowedNums(QRegExp(REGEX_REAL)),
// -?[\d*] potential -, then one digit followed by:
// 1) some number of digits
// 2) (e|E)[+-]?\d{1,2} character e or E, an optional + or -, then 1-2 digits 
// 3) .\d*(e|E)[+-]?\d{1,2} dot ., some number of digits, character e or E, an optional + or -, then 1-2 digits 
allowedPrios(100,1000) {
	createLayout();
}

EGS_geom::EGS_geom() : name ("Error"), index(-1), allowedNums(), allowedPrios() {createLayout();}

// Generic GUI methods
void EGS_geom::createLayout() {
	labelTab  = new QLabel(name.left(20));
	labelGeom = new QLabel(name);
	remove    = new QPushButton("Remove");
	labelPos  = new QLabel("Position");
	xPos      = new QLineEdit("0");
	yPos      = new QLineEdit("0");
	zPos      = new QLineEdit("0");
	labelRot  = new QLabel("Rotation");
	xRot      = new QLineEdit("0");
	yRot      = new QLineEdit("0");
	zRot      = new QLineEdit("0");
	labelPrio = new QLabel("Priority");
	Prio      = new QLineEdit("100");

	geomGrid  = new QGridLayout();
	
	// Only allow doubles
	xPos->setValidator(&allowedNums);
	yPos->setValidator(&allowedNums);
	zPos->setValidator(&allowedNums);
	xRot->setValidator(&allowedNums);
	yRot->setValidator(&allowedNums);
	zRot->setValidator(&allowedNums);
	zRot->setValidator(&allowedPrios);
	
	// Set up frame
	geomGrid->addWidget(labelGeom, 0, 0, 1, 3);
	geomGrid->addWidget(labelPos , 1, 0, 1, 3);
	geomGrid->addWidget(xPos     , 2, 0, 1, 1);
	geomGrid->addWidget(yPos     , 2, 1, 1, 1);
	geomGrid->addWidget(zPos     , 2, 2, 1, 1);
	geomGrid->addWidget(labelRot , 3, 0, 1, 3);
	geomGrid->addWidget(xRot     , 4, 0, 1, 1);
	geomGrid->addWidget(yRot     , 4, 1, 1, 1);
	geomGrid->addWidget(zRot     , 4, 2, 1, 1);
	geomGrid->addWidget(labelPrio, 5, 0, 1, 3);
	geomGrid->addWidget(Prio     , 6, 0, 1, 1);
	geomGrid->addWidget(remove   , 8, 0, 1, 3);
	
	geomGrid->setRowStretch(7,2);	
	setLayout(geomGrid);
}

void EGS_geom::deleteMyself() { // This is a risky approach, but it if works it
	delete this;                // will be very clean
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// consoleWindow~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
consoleWindow::consoleWindow() {
	createLayout();
	connectLayout();
}

consoleWindow::~consoleWindow() {
	
}
	
// GUI Layout
void consoleWindow::createLayout() {
	frame      = new QWidget();
	layout     = new QGridLayout;
	save       = new QPushButton("Save");
	kill       = new QPushButton("Kill");
	close      = new QPushButton("Close");
	outputArea = new QPlainTextEdit;
	outputArea->setReadOnly(true);
    outputArea->setFont(QFont("Monospace"));
	
	
	layout->addWidget(outputArea, 0, 0, 1, 4);
	layout->addWidget(save      , 1, 1, 1, 1);
	layout->addWidget(kill      , 1, 2, 1, 1);
	layout->addWidget(close     , 1, 3, 1, 1);
	layout->setColumnStretch(0, 5);
	
	frame->setLayout(layout);
	setCentralWidget(frame);
    setWindowTitle("egs_brachy console");
	resize(1280,720);
	hide();
}

void consoleWindow::connectLayout(){
	connect(close, SIGNAL(released()),
			this, SLOT(hide()));	
}
