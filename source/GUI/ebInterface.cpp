/*
################################################################################
#
#  egs_brachy_GUI ebInterface.cpp
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
#include "ebInterface.h"

// Constructors
ebInterface::ebInterface()
	: allowedNums(QRegExp(REGEX_REAL_POS)) {
	parent = (Interface*)parentWidget();
	
	console = new consoleWindow;
	ebProcess = new QProcess; // runs egs_brachy
	evProcess = new QProcess; // runs egs_view
	ebProcess->setProcessChannelMode(QProcess::MergedChannels); // Merge output and errors
	
	createLayout();                      
	connectLayout();
}

ebInterface::ebInterface(Interface* p)
	: allowedNums(QRegExp(REGEX_REAL_POS)) {
	parent = p;
	
	console = new consoleWindow();
	ebProcess = new QProcess; // runs egs_brachy
	evProcess = new QProcess; // runs egs_view
	ebProcess->setProcessChannelMode(QProcess::MergedChannels); // Merge output and errors
	
	createLayout();
	connectLayout();
}

// Destructor
ebInterface::~ebInterface() {
	delete console;
	delete ebProcess;
	delete evProcess;
}

// Layout Settings
void ebInterface::createLayout() {
	mainLayout = new QGridLayout();
	QString ttt = ""; // tool tip text
	
	// Define simulation options widget
	
	// Top level
	simulationOptions = new QFrame;
	simulationLayout  = new QGridLayout;
	simulationLabel   = new QLabel(tr("<b>Simulation Options</b>"));
	
	// The name for the simulation
	fileNameLabel     = new QLabel(tr("Simulation name"));
	fileNameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	fileNameEdit      = new QLineEdit(QString("ebGUI_")+QDateTime::currentDateTime().toString("yyMMMdd_hhmm"));
	
	ttt = tr("Input/output (egsinp/3ddose) file name");
	fileNameLabel->setToolTip(ttt);
	fileNameEdit->setToolTip(ttt);
	
	// Number of histories
	ncaseLabel        = new QLabel(tr("Histories"));
	ncaseLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ncaseEdit         = new QLineEdit(parent->data->def_ncase);
	
	ttt = tr("Number of histories (ncase) for simulation; "
			 "simulation time is linearly proportional to ncase and "
			 "dose uncertainty is inversely proportional to the square root of ncase.");
	ncaseLabel->setToolTip(ttt);
	ncaseEdit->setToolTip(ttt);
	
	// Volume correction
	volCorBox         = new QCheckBox(tr("Volume correction"));
	if (parent->data->def_volCor.compare("correct")==0)
		volCorBox->setChecked(true);
	volCorLabel       = new QLabel(tr("Point density"));
	volCorLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	volCorDen         = new QLineEdit(parent->data->def_volDen);
	
	ttt = tr("Corrects dose to voxels containing sources, by removing the fraction "
			 "of the volume occupied by the source.");
	volCorBox->setToolTip(ttt);
	
	ttt = tr("Number of points used to find the source volume fraction.");
	volCorLabel->setToolTip(ttt);
	volCorDen->setToolTip(ttt);
	
	// Data files used for the simulation	
	transportLabel    = new QLabel(tr("Transport options"));
	transportLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	transportEdit     = new QLineEdit(parent->data->transport_location);
	transportEdit->setEnabled(false);
	transportEdit->setToolTip(parent->data->transport_location);
	transportLoad     = new QPushButton(tr("Load"));
	
	ttt = tr("Generic EGSnrc transport templates, change with care.");
	transportLoad->setToolTip(ttt);
	transportLabel->setToolTip(ttt);
	
	materialLabel     = new QLabel(tr("Material data"));
	materialLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	materialEdit      = new QLineEdit(parent->data->material_location);
	materialEdit->setEnabled(false);
	materialEdit->setToolTip(parent->data->material_location);
	materialLoad      = new QPushButton(tr("Load"));
	
	ttt = tr("Generic EGSnrc transport templates, change with care.");
	materialLoad->setToolTip(ttt);
	materialLabel->setToolTip(ttt);
	
	muenLabel         = new QLabel(tr("Muen data"));
	muenLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	muenEdit          = new QLineEdit(parent->data->muen_location);
	muenEdit->setEnabled(false);
	muenEdit->setToolTip(parent->data->muen_location);
	muenLoad          = new QPushButton(tr("Load"));
	
	ttt = tr("Generic EGSnrc transport templates, change with care.");
	muenLoad->setToolTip(ttt);
	muenLabel->setToolTip(ttt);
	
	// Simulation parameters
	edepBox = new QCheckBox(tr("Score edep"));
	
	ttt = tr("Output additional 3ddose file where track length scoring "
	         "is not used for scoring, instead using the classical "
			 "interaction scoring scheme.");
	edepBox->setToolTip(ttt);
	
	waterBox = new QCheckBox(tr("TG-43"));
	
	ttt = tr("Replace phantom geometry with pure nominal density water "
	         "and use superposition mode for seeds to create a TG-43 "
			 "equivalent simulation.");
	waterBox->setToolTip(ttt);
	
	runModeLabel      = new QLabel(tr("Run mode"));
	runModeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	runModeBox        = new QComboBox;
	runModeBox->addItem("normal");
	runModeBox->addItem("superposition");
	
	ttt = tr("Run a normal simulation where sources are present at all locations "
			 "or a superposition simulation where only the source in which a particle "
			 "is generated is present for that particle's history, useful for TG-43.");
	runModeLabel->setToolTip(ttt);
	runModeBox->setToolTip(ttt);
	
	njobLabel         = new QLabel(tr("Number of cores"));
	njobLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	njobBox           = new QComboBox;
	njobBox->addItem("Interactive");
	njobBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	for (int i = 2; i <= QThread::idealThreadCount(); i++)
		njobBox->addItem(QString::number(i));
	
	ttt = tr("Run a simulation interactively (1 core) where all simulation information is "
			 "output to the console (useful for debugging), or run a simulation on n "
			 "separate cores.");
	njobLabel->setToolTip(ttt);
	njobBox->setToolTip(ttt);
	
	// Run egs_brachy
	runButton = new QPushButton(tr("Run egs_brachy"));
	ttt = tr("Start egs_brachy simulation and create 3ddose files.");
	runButton->setToolTip(ttt);
	
	// Other options
	saveButton = new QPushButton("Save egsinp");
	ttt = tr("Save egsinp file for use outside of eb_gui.");
	saveButton->setToolTip(ttt);
	
	egsViewButton = new QPushButton("Run egs_view");
	ttt = tr("Launch egs_view with current egsinp geometry.  Loading large egsphants may take a while.");
	egsViewButton->setToolTip(ttt);
	
	// Only allow ints
	ncaseEdit->setValidator(&allowedNums);
	volCorDen->setValidator(&allowedNums);
	
	// Setup the widget layout
	simulationLayout->addWidget(simulationLabel,  0, 0, 1, 3);
												  
	simulationLayout->addWidget(fileNameLabel  ,  1, 0, 1, 1);
	simulationLayout->addWidget(fileNameEdit   ,  1, 1, 1, 2);
												  
	simulationLayout->addWidget(ncaseLabel     ,  2, 0, 1, 1);
	simulationLayout->addWidget(ncaseEdit      ,  2, 1, 1, 2);
												  
	simulationLayout->addWidget(volCorBox      ,  3, 0, 1, 1);
	simulationLayout->addWidget(volCorLabel    ,  3, 1, 1, 1);
	simulationLayout->addWidget(volCorDen      ,  3, 2, 1, 1);
												  
	simulationLayout->addWidget(transportLabel ,  4, 0, 1, 1);
	simulationLayout->addWidget(transportLoad  ,  4, 1, 1, 1);
	simulationLayout->addWidget(transportEdit  ,  4, 2, 1, 1);
												  
	simulationLayout->addWidget(materialLabel  ,  5, 0, 1, 1);
	simulationLayout->addWidget(materialLoad   ,  5, 1, 1, 1);
	simulationLayout->addWidget(materialEdit   ,  5, 2, 1, 1);
												  
	simulationLayout->addWidget(muenLabel      ,  6, 0, 1, 1);
	simulationLayout->addWidget(muenLoad       ,  6, 1, 1, 1);
	simulationLayout->addWidget(muenEdit       ,  6, 2, 1, 1);
												  
	simulationLayout->addWidget(runModeLabel   , 10, 0, 1, 1);
	simulationLayout->addWidget(runModeBox     , 10, 1, 1, 1);
	simulationLayout->addWidget(waterBox       , 10, 2, 1, 1);
	
	simulationLayout->addWidget(njobLabel      , 11, 0, 1, 1);
	simulationLayout->addWidget(njobBox        , 11, 1, 1, 1);
	simulationLayout->addWidget(edepBox        , 11, 2, 1, 1);
	
	simulationLayout->addWidget(runButton      , 12, 0, 1, 1);
	simulationLayout->addWidget(saveButton     , 12, 1, 1, 1);
	simulationLayout->addWidget(egsViewButton  , 12, 2, 1, 1);
	
	simulationLayout->setRowStretch(7, 5);
	simulationOptions->setLayout(simulationLayout);
	
	mainLayout->addWidget(simulationOptions,1,2,1,1);
	
	mainLayout->setColumnStretch(0, 5);
	mainLayout->setColumnStretch(1, 5);
	mainLayout->setColumnStretch(2, 5);
	setLayout(mainLayout);
}

void ebInterface::connectLayout() {
	// Volume correction
	connect(volCorBox, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
	connect(waterBox, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
	
	// Change files
	connect(transportLoad, SIGNAL(released()),
			this, SLOT(loadTransport()));
	connect(materialLoad, SIGNAL(released()),
			this, SLOT(loadMaterial()));
	connect(muenLoad, SIGNAL(released()),
			this, SLOT(loadMuen()));
	
	// Connect console signals
	connect(runButton, SIGNAL(released()),
			this, SLOT(runEB()));
	connect(console->kill, SIGNAL(released()),
			this, SLOT(killEB()));
	connect(console->save, SIGNAL(released()),
			this, SLOT(saveLogEB()));
	connect(ebProcess, SIGNAL(finished(int)),
			this, SLOT(finishEB(int)));
	connect(ebProcess, SIGNAL(readyReadStandardOutput()),
			this, SLOT(writeOutputToConsole()));
			
	// Other signals
	connect(saveButton, SIGNAL(released()),
			this, SLOT(saveEB()));
	connect(egsViewButton, SIGNAL(released()),
			this, SLOT(runEV()));
}

// Refresh
void ebInterface::refresh() {
	// Parent 
	mainLayout->addWidget(parent->phantomFrame,       0, 0, 1, 1);
	mainLayout->addWidget(parent->sourceFrame,        0, 1, 1, 1);
	parent->sourceRefresh();
	mainLayout->addWidget(parent->transformationFrame,0, 2, 1, 1);
	parent->transformationRefresh();
	mainLayout->addWidget(parent->geometryFrame,      1, 0, 1, 2);
	
	if (volCorBox->isChecked()) {
		volCorDen->setEnabled(true);
		volCorLabel->setEnabled(true);
	}
	else {
		volCorDen->setEnabled(false);
		volCorLabel->setEnabled(false);
	}
	
	if (waterBox->isChecked()) {
		runModeLabel->setEnabled(false);
		runModeBox->setEnabled(false);
	}
	else {
		runModeLabel->setEnabled(true);
		runModeBox->setEnabled(true);
	}
}

// Load files
void ebInterface::loadTransport() {
	QString path = QFileDialog::getOpenFileName(this, tr("Load transportation file"), parent->data->eb_location+"/lib/transport", tr("Transportation file (*)"));
	
	// Check to see if path is empty
	if (path.length() < 1)
		return;
	
	// Check if it works
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file."));
        return;
	}
	
	// Check for start and stop blocks
	int blockFlag = 0;
	QString line;
	while (!file.atEnd()) {
		line = file.readLine();
        if (line.contains(":start MC transport parameter:"))
			blockFlag++;
		else if (line.contains(":stop MC transport parameter:"))
			blockFlag++;
    }
	
	if (blockFlag < 2) {
		QMessageBox::warning(0, "file error",
        tr("Did not find a start and stop flag for the MC transport parameter block."));
		return;
	}
	else if (blockFlag > 2) {
		QMessageBox::warning(0, "file error",
        tr("Found too many start and stop flags for the MC transport parameter block."));
		return;
	}
	
	transportEdit->setText(path);
	transportEdit->setToolTip(path);	
}

void ebInterface::loadMaterial() {
	QString path = QFileDialog::getOpenFileName(this, tr("Load material file"), parent->data->eb_location+"/lib/media", tr("Material file (*.dat)"));
	
	// Check to see if path is empty
	if (path.length() < 1)
		return;
	
	// Check if it works
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file."));
        return;
	}
	
	// Check for start and stop blocks
	int blockFlag = 0;
	while (!file.atEnd()) {
        if (file.readLine().contains("medium =")) {
			blockFlag++;
			break;
		}
    }
	
	if (!blockFlag) {
		QMessageBox::warning(0, "file error",
        tr("Did not find a medium definition in file."));
		return;
	}
	
	materialEdit->setText(path);
	materialEdit->setToolTip(path);	
}

void ebInterface::loadMuen() {
	QString path = QFileDialog::getOpenFileName(this, tr("Load material file"), parent->data->eb_location+"/lib/muen", tr("Material file (*.muendat)"));
	
	// Check to see if path is empty
	if (path.length() < 1)
		return;
	
	// Check if it works
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file."));
        return;
	}
	
	// Check for start and stop blocks
	int blockFlag = 0;
	while (!file.atEnd()) {
        if (file.readLine().contains("Muen values for medium MEDIUM =")) {
			blockFlag++;
			break;
		}
    }
	
	if (!blockFlag) {
		QMessageBox::warning(0, "file error",
        tr("Did not find a medium definition in file."));
		return;
	}
	
	muenEdit->setText(path);
	muenEdit->setToolTip(path);
}

// Console functions
void ebInterface::runEB() {
	// Is there currently a selected phantom, source, and transformation file
	if (parent->phantomListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "VPM error",
        tr("No virtual patient model selected."));
		return;
	}
	if (parent->sourceListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "source error",
        tr("No source selected."));
		return;
	}
	if (parent->transformationListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "source location error",
        tr("No source location file selected."));
		return;
	}
	
	// Quit if egs_brachy is running
	if (ebProcess->state() != 0) {// ebProcess is already running something
		QMessageBox::information(0, "egs_brachy error",
        tr("Simulations are already underway, wait for them to finish."));
		return;
	}
	
	// Show the console
	console->outputArea->clear();
	console->show();
	
	// Create the egsinp file
	ebName = fileNameEdit->text();
	QFile egsinp(parent->data->eb_location + "/" + ebName + ".egsinp");
	
	if (egsinp.exists()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Name already found",
			tr("An egsinp file named ") + ebName + tr(" already exists.  Would you like to overwrite it?"))) {
			// Delete file
			egsinp.remove();
		}
		else { // They did not proceed
			return;
		}
	}
	
    if (!egsinp.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::information(0, "egsinp file error",
        tr("Failed to create and open file:\n")
		+ parent->data->eb_location + "/" + ebName + ".egsinp\n" +
		tr("for simulations."));
        return;
	}
	
	if (parent->populateEgsinp()) // Build egsinp
		return; // Quit if error was encountered
	
	QTextStream output(&egsinp);
	output << parent->egsinp->buildInput();
	
	ebKillFlag = false; // Reset kill flag, tells user if job failed or was killed
	ebProcess->setWorkingDirectory(parent->data->eb_location); // Go to eb directory
	
	if (njobBox->currentIndex() == 0) // Interactive
		ebProcess->start(QString("egs_brachy -i ") + ebName);
	else                              // Parallel
		ebProcess->start(parent->data->ep_location + " -n" + njobBox->currentText() + " -d5 -f -v -c \"egs_brachy -i " + ebName + "\"");
}

void ebInterface::saveEB() {
	// Is there currently a selected phantom, source, and transformation file
	if (parent->phantomListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "phantom error",
        tr("No phantom selected."));
		return;
	}
	if (parent->sourceListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "source error",
        tr("No source selected."));
		return;
	}
	if (parent->transformationListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "transformation error",
        tr("No transformation file selected."));
		return;
	}
	
	// Create the egsinp file
	QString egsinpPath = QFileDialog::getSaveFileName(this, tr("Save egs_brachy Input File"), ".", tr("egs input (*.egsinp)"));
	
	if (egsinpPath.length() < 1) // No selection
		return;
		
	if (!egsinpPath.endsWith(".egsinp"))
		egsinpPath += ".egsinp";
	
	QFile egsinpFile(egsinpPath);
	if (egsinpFile.exists()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Name already found",
			tr("An egsinp file named ") + ebName + tr(" already exists.  Would you like to overwrite it?"))) {
			// Delete file
			egsinpFile.remove();
		}
		else { // They did not proceed
			return;
		}
	}
	
    if (!egsinpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::information(0, "egsinp file error",
        tr("Failed to create file:\n")
		+ egsinpPath + "\n" +
		tr("for simulations."));
        return;
	}
	
	if (parent->populateEgsinp()) // Build egsinp
		return; // Quit if error was encountered
	
	QTextStream output(&egsinpFile);
	output << parent->egsinp->buildInput();
}

void ebInterface::saveLogEB() {
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("egs_brachy log file (*.egslog)"));
	
	if (filePath.length() < 1) // No name selected
		return;
	
	if (!filePath.endsWith(".egslog")) // Doesn't have the right extension
		filePath += ".egslog";
		
	QFile egslogFile(filePath);
	
	if (!egslogFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "File error",
		tr("Failed to open the egslog file for saving.  Aborting"));	
		return;
	}
	
	QTextStream out(&egslogFile);
	out << console->outputArea->toPlainText();
	egslogFile.close();
}

void ebInterface::runEV() {
	// Is there currently a selected phantom, source, and transformation file
	if (parent->phantomListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "phantom error",
        tr("No phantom selected."));
		return;
	}
	if (parent->sourceListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "source error",
        tr("No source selected."));
		return;
	}
	if (parent->transformationListView->selectedItems().size() != 1) {
		QMessageBox::information(0, "transformation error",
        tr("No transformation file selected."));
		return;
	}
	
	// Quit if egs_view is running
	if (evProcess->state() != 0) {// evProcess is already running something
		QMessageBox::information(0, "egs_view error",
        tr("Program egs_view is already running, please close before starting new instance."));
		return;
	}
	
	// Create the egsinp file
	QString egsinpPath = parent->data->eb_location + "/" + ebName + ".preview.egsinp";
	
	QFile egsinpFile(egsinpPath);
	egsinpFile.remove();
	
    if (!egsinpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::information(0, "egsinp file error",
        tr("Failed to create file:\n")
		+ egsinpPath + "\n" +
		tr("for simulations."));
        return;
	}
	
	if (parent->populateEgsinp()) // Build egsinp
		return; // Quit if error was encountered
	
	QTextStream output(&egsinpFile);
	output << parent->egsinp->buildInput();
	
	evProcess->setWorkingDirectory(parent->data->eb_location); // Go to eb directory
	
	if (njobBox->currentIndex() == 0) // Interactive
		evProcess->start(QString("egs_view ") + ebName + ".preview.egsinp");
}

void ebInterface::finishEB(int code) {
	// Check for bad exit
	if (code == 1) {
		QMessageBox::warning(0, "egs_brachy error",
        tr("The egs_brachy simulation ") + ebName + tr(" failed to run successfully."));		
		return;
	}
	else if (ebKillFlag) {
		QMessageBox::information(0, "egs_brachy killed",
		tr("The egs_brachy simulation ") + ebName + tr(" has been killed."));
		return;
	}
	
	// If successful, copy 3ddose files to local database
	QDirIterator* files;
	QStringList doseNames;
	char doseFlag = false;
	files = new QDirIterator(parent->data->eb_location, {"*.3ddose","*.3ddose.gz"});
	while(files->hasNext()) {
		files->next();
		if (files->fileName().startsWith(ebName+".")) {
			doseNames << files->fileName();
			QFile::copy(files->filePath(),
			parent->data->gui_location + "/database/dose/" + files->fileName());
			QFile::remove(files->filePath());
			doseFlag = true;
			
			parent->data->localNameDoses << files->fileName();
			parent->data->localDirDoses << parent->data->gui_location + "/database/dose/";
		}
	}
	delete files;
	
	// Copy any egsinp and egslog files that were generated
	files = new QDirIterator(parent->data->eb_location, {"*.egsinp","*.egslog"});
	while(files->hasNext()) {
		files->next();
		if (files->fileName().startsWith(ebName+".egs")) {
			doseNames << files->fileName();
			QFile::copy(files->filePath(),
			parent->data->gui_location + "/database/dose/" + files->fileName());
			QFile::remove(files->filePath());
		}
	}
	delete files;
	
	if (doseFlag) {
		parent->doseRepopulate();
		QMessageBox::information(0, "egs_brachy simulation complete",
		tr("The egs_brachy simulation ") + ebName + tr(" finished successfully.\n") +
		tr(doseNames.size()>1?"Files:\n   - ":"File:\n   - ") +
		doseNames.join("\n   - ") +
		tr("\ncopied to local repository."));
	}
	else {
		QMessageBox::warning(0, "egs_brachy simulation complete",
		tr("The egs_brachy simulation ") + ebName + tr(" finished successfully, "
		   "but no output was found...\n"));
	}
}

void ebInterface::killEB() {
	ebKillFlag = true;
	
	// Kill interactive
	ebProcess->kill();
}

void ebInterface::writeOutputToConsole() {
	while(ebProcess->canReadLine()){
       console->outputArea->insertPlainText(ebProcess->readLine());
    }
	console->outputArea->moveCursor(QTextCursor::End);
}