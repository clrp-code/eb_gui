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
	egsphantLabel  = new QLabel("<b>Patient Geometry</b>");
	transformLabel = new QLabel("<b>Source Positions</b>");
	doseLabel      = new QLabel("<b>Dose</b>");
	
	egsphant       = new QComboBox();
	transform      = new QComboBox();
	dose           = new QComboBox();
	
	// Structure metrics
	metricGrid        = new QGridLayout();
	
	contourFileName.resize(100);
	contourTitleLabel = new QLabel("Contour");
	loadMetricLabel   = new QLabel("Metrics");
	saveDVHLabel      = new QLabel("Output DVH");
	saveDiffLabel     = new QLabel("Output Differential");
	
	metricGrid->addWidget(contourTitleLabel, 0, 0, 1, 1);
	metricGrid->addWidget(loadMetricLabel  , 0, 1, 1, 1);
	metricGrid->addWidget(saveDVHLabel     , 0, 2, 1, 1);
	metricGrid->addWidget(saveDiffLabel    , 0, 3, 1, 1);
	
	for (int i = 0; i < STRUCT_COUNT; i++) {
		contourNameLabel.append(new QLabel(tr("no contour loaded")));
		loadMetricBox.append(new QComboBox());
		loadMetricBox.last()->addItems(parent->data->metricNames);
		loadMetricBox.last()->removeItem(parent->data->metricNames.size()-1);
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
	
		ttt = tr("Select metrics, DVHs, and differential dose \n"
		"histograms to output along with patient data.");
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
	outputRT = new QPushButton("Output RT Dose");
	ttt = tr("Convert selected 3ddose file to RT dose file.");
	outputRT->setToolTip(ttt);
	
	outputFullDataCSV   = new QPushButton("Output all data (csv metrics)");
	outputFullDataDICOM = new QPushButton("Output all data (DICOM metrics)");
	ttt = tr("Output egsphant, transformation, dose, input files, and associated logs to a patient folder.\n  "
			 "Additional metrics selected above can also be output.");
	outputFullDataCSV->setToolTip(ttt);
	outputFullDataDICOM->setToolTip(ttt);
	outputFullDataDICOM->setDisabled(true);  // to be implemented
	
	mainLayout->addWidget(parent->doseFrame  , 0, 0, 4, 2);
	mainLayout->addWidget(outputRT           , 4, 0, 1, 1);
	mainLayout->addWidget(egsphantLabel      , 0, 2, 1, 1);
	mainLayout->addWidget(transformLabel     , 1, 2, 1, 1);
	mainLayout->addWidget(doseLabel          , 2, 2, 1, 1);
	mainLayout->addWidget(egsphant           , 0, 3, 1, 1);
	mainLayout->addWidget(transform          , 1, 3, 1, 1);
	mainLayout->addWidget(dose               , 2, 3, 1, 1);
	mainLayout->addWidget(metricArea         , 3, 2, 1, 2);
	mainLayout->addWidget(outputFullDataCSV  , 4, 2, 1, 1);
	mainLayout->addWidget(outputFullDataDICOM, 4, 3, 1, 1);
	
	mainLayout->setColumnStretch(0,1);
	mainLayout->setColumnStretch(1,1);
	mainLayout->setColumnStretch(2,1);
	mainLayout->setColumnStretch(3,1);
	
	setLayout(mainLayout);
}

void appInterface::connectLayout() {
	connect(outputRT, SIGNAL(released()),
			this, SLOT(outputRTdose()));
			
	connect(egsphant, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadStructs()));
	
	connect(outputFullDataCSV, SIGNAL(released()),
			this, SLOT(outputCSV()));
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
		tr("Somehow the selected dose index is larger than the local dose count. Aborting"));		
		return;
	}
	
	// Get RT file save location
	QString rtFile = QFileDialog::getSaveFileName(this, tr("Save RT files"), ".", tr("DICOM (*.dcm)"));
	
	if (rtFile.length() < 1) // No name selected
		return;
		
	if (rtFile.endsWith(".dcm"))
		rtFile = rtFile.left(rtFile.length()-4);
	
	QString rtFile2 = rtFile+".error.dcm";
	rtFile = rtFile+".dose.dcm";
	
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
		tr("Selected dose file is not of type 3ddose or b3ddose. Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	// Get egsinp file dose scaling if one exists
	QString doseScaling = "", line;
	
	if (doseFile.endsWith(".phantom.3ddose"))
		doseFile = doseFile.left(doseFile.length()-15);
	else if (doseFile.endsWith(".phantom.b3ddose"))
		doseFile = doseFile.left(doseFile.length()-16);
	
	QFile egsinp(doseFile+".egsinp");
	QTextStream egsinpinp(&egsinp);
	if (egsinp.open(QIODevice::ReadOnly | QIODevice::Text))
		do {
			line = egsinpinp.readLine();
			if (line.contains("dose scaling factor") && line.contains("=")) {
				doseScaling = line.split("=")[1].split("#")[0].trimmed();
				break;
			}
		} while (!egsinpinp.atEnd());
	
	// Now save RT Dose
	parent->data->outputRTDose(rtFile, rtFile2, &toBeRT, doseScaling);
	
	// Finish with progress bar
	parent->finishedProgress();
}

void appInterface::loadStructs() {
	int i = egsphant->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local phantom count. Aborting"));		
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
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant. Aborting"));
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

void appInterface::outputCSV() {
	// Exit statements
	int iD = dose->currentIndex()-1;
	if (iD < 0) {return;} // Exit if none is selected or box is empty in setup
		
	if (iD >= parent->data->localDirDoses.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count. Aborting"));		
		return;
	}
	
	int iP = egsphant->currentIndex()-1;
	if (iP < 0) {return;} // Exit if none is selected or box is empty in setup
		
	if (iP >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local dose count. Aborting"));		
		return;
	}
	
	int iT = transform->currentIndex()-1;
	if (iT < 0) {return;} // Exit if none is selected or box is empty in setup
		
	if (iT >= parent->data->localDirTransforms.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count. Aborting"));		
		return;
	}
	
	// Get output directory
	QString path = QFileDialog::getExistingDirectory(this, tr("Select DICOM CT directory")), line;
	
	if (path.length() < 1)
		return;
	
	Dose doseData; // Hold dose in memory for computations
	
	// Make subdirectories
	if (!QDir(path+"/phantom").exists())
		QDir().mkdir(path+"/phantom");
	if (!QDir(path+"/plan").exists())
		QDir().mkdir(path+"/plan");
	if (!QDir(path+"/simulation").exists())
		QDir().mkdir(path+"/simulation");
	
	// Get RT file save location
	QString rtFile = path+"/"+parent->data->localDirDoses[iD];
	if (rtFile.endsWith(".phantom.b3ddose"))
		rtFile = rtFile.left(rtFile.size()-16);
	else if (rtFile.endsWith(".phantom.3ddose"))
		rtFile = rtFile.left(rtFile.size()-15);
	
	QString rtFile2 = rtFile+".error.dcm";
	rtFile = rtFile+".dose.dcm";
	
	QString doseFile = parent->data->localDirDoses[iD]+parent->data->localNameDoses[iD]; // Get file location
		
	// Connect the progress bar and load dose
	parent->resetProgress("Loading 3ddose file");
	parent->progLabel->setText("Loading 3ddose file");
	connect(&doseData, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (doseFile.endsWith(".b3ddose"))
		doseData.readBIn(doseFile, 4);
	else if (doseFile.endsWith(".3ddose"))
		doseData.readIn(doseFile, 4);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected dose file is not of type 3ddose or b3ddose. Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	if (doseFile.endsWith(".phantom.3ddose"))
		doseFile = doseFile.left(doseFile.length()-15);
	else if (doseFile.endsWith(".phantom.b3ddose"))
		doseFile = doseFile.left(doseFile.length()-16);
	
	// Get egsinp file dose scaling if one exists
	QString doseScaling = "";
	
	QFile egsinp(doseFile+".egsinp");
	QTextStream egsinpinp(&egsinp);
	if (egsinp.open(QIODevice::ReadOnly | QIODevice::Text))
		do {
			line = egsinpinp.readLine();
			if (line.contains("dose scaling factor") && line.contains("=")) {
				doseScaling = line.split("=")[1].split("#")[0].trimmed();
				break;
			}
		} while (!egsinpinp.atEnd());
		
	// Copy all additional files associated with dose
	QString tempPath, tempName;
	
	// Copy egsphant
	tempPath = parent->data->localDirPhants[iP];
	tempName = parent->data->localNamePhants[iP];
	
	QFile(tempPath+tempName).copy(path+"/phantom/"+tempName);
	
	if (tempName.endsWith(".gz"))
		tempName = tempName.left(tempName.size()-3);
	
	if (tempName.endsWith(".egsphant"))
		tempName = tempName.left(tempName.size()-9);
	else if (tempName.endsWith(".begsphant"))
		tempName = tempName.left(tempName.size()-10);
	else if (tempName.endsWith(".geom"))
		tempName = tempName.left(tempName.size()-5);
	
	QFile(tempPath+tempName+".log").copy(path+"/phantom/"+tempName+".log"); // Get egsphant log
	
	// Copy transformation
	tempPath = parent->data->localDirTransforms[iT];
	tempName = parent->data->localNameTransforms[iT];
	
	QFile(tempPath+tempName).copy(path+"/plan/"+tempName);
		
	QFile(tempPath+tempName+".log").copy(path+"/plan/"+tempName+".log"); // Get plan log
	QFile(tempPath+tempName+".dwell").copy(path+"/plan/"+tempName+".dwell"); // Get dwell times if they exist
	QFile(tempPath+tempName+".activity").copy(path+"/plan/"+tempName+".activity"); // Get activity times if they exist
	
	// Copy dose
	tempPath = parent->data->localDirDoses[iD];
	tempName = parent->data->localNameDoses[iD];
	
	QFile(tempPath+tempName).copy(path+"/simulation/"+tempName);
	
	if (tempName.endsWith("phantom.b3ddose"))
		tempName = tempName.left(tempName.size()-16);
	else if (tempName.endsWith("phantom.3ddose"))
		tempName = tempName.left(tempName.size()-15);
	
	QFile(tempPath+tempName+".egsinp").copy(path+"/simulation/"+tempName+".egsinp"); // Get input file
	QFile(tempPath+tempName+".egslog").copy(path+"/simulation/"+tempName+".egslog"); // Get output log file
	
	// Setup metric extraction data
	QVector <QVector <DV> > data;
	QVector <double> volume;
	QVector <QString> Dx, Dcc, Vx, pD;
	for (int i = 0; i < masks.size(); i++)
		delete masks[i];
	masks.clear();
	
	int structCount; // Get structure count
	for (structCount = 0; structCount < STRUCT_COUNT; structCount++)
		if (!contourNameLabel[structCount]->text().compare("no contour loaded"))
			break;
	
	data.resize(structCount);
	volume.resize(structCount);
	masks.resize(structCount);
	Dx.resize(structCount);
	Dcc.resize(structCount);
	Vx.resize(structCount);
	pD.resize(structCount);
	
	// Load masks
	tempPath = parent->data->localDirPhants[iP].left(parent->data->localDirPhants[iP].size()-9)+"mask/";
	tempName = parent->data->localNamePhants[iP];
	
	if (tempName.endsWith(".gz"))
		tempName = tempName.left(tempName.size()-3);
	
	if (tempName.endsWith(".egsphant"))
		tempName = tempName.left(tempName.size()-9);
	else if (tempName.endsWith(".begsphant"))
		tempName = tempName.left(tempName.size()-10);
	else if (tempName.endsWith(".geom"))
		tempName = tempName.left(tempName.size()-5);
	
	int j;
	parent->progLabel->setText("Loading masks file");
	for (int i = 0; i < structCount; i++) {
		masks[i] = new EGSPhant();
		masks[i]->loadgzEGSPhantFile(tempPath+tempName+"."+contourNameLabel[i]->text()+".mask.egsphant.gz");
		j      = loadMetricBox[i]->currentIndex();
		Dx[i]  = parent->data->metricDx[j];
		Dcc[i] = parent->data->metricDcc[j];
		Vx[i]  = parent->data->metricVx[j];
		pD[i]  = parent->data->metricDp[j];
		parent->updateProgress(5.0/structCount);
	}
	
	parent->progLabel->setText("Filtering data");
	doseData.getDVs(&data,&masks,&volume);
	
	int histoCount = 2; // Count metrics as 2, I guess
	for (int i = 0; i < structCount; i++) {
		if (saveDVHBox[i]->isChecked())
			histoCount++;
		if (saveDiffBox[i]->isChecked())
			histoCount++;
	}
	double increment = 5.0/histoCount;
	
	parent->progLabel->setText("Outputting metrics");
	QString csvText;
	for (int i = 0; i < structCount; i++){
		csvText = doseData.getMetricCSV(&(data[i]), volume[i], contourNameLabel[i]->text(),
										Dx[i], Dcc[i], Vx[i], pD[i]);
		QFile csvFile(path+"/"+contourNameLabel[i]->text()+"_metrics.csv");
		if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream csvOut(&csvFile);
			csvOut << csvText;
		}
		csvFile.close();
	}
	parent->updateProgress(increment*2);
	
	parent->progLabel->setText("Generating DVHs");
	for (int i = 0; i < structCount; i++) {
		if (saveDVHBox[i]->isChecked()) {
			csvText  = contourNameLabel[i]->text()+",\n";
			csvText += "dose / Gy, volume / %\n";
			
			int sInc = 1;
			// Drop every (n-1)th point, where n is the multiple of full 200s in the data set
			if (data[i].size() > 200) {
				sInc = data[i].size()/200.0;
			}
			
			for (int j = 0; j < data[i].size(); j++) {
				if (!(j%sInc)) // Only add up to 399 points (start skipping at 400+)
					csvText += QString::number(data[i][j].dose)+","+
							   QString::number(100.0*double(data[i].size()-j)/double(data[i].size()))+"\n";
			}
			
			if ((data[i].size()%sInc)) {// Add end point if it would be skipped
				csvText += QString::number(data[i].last().dose)+","+
						   QString::number(100.0/double(data[i].size()))+"\n";
			}
			
			QFile csvFile(path+"/"+contourNameLabel[i]->text()+"_DVH.csv");
			if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream csvOut(&csvFile);
				csvOut << csvText;
			}
			
			csvFile.close();
			parent->updateProgress(increment);
		}
	}
	
	parent->progLabel->setText("Generating differential histograms");
	for (int i = 0; i < structCount; i++) {
		if (saveDVHBox[i]->isChecked()) {
			csvText  = contourNameLabel[i]->text()+",\n";
			csvText += "dose / Gy, Volume / voxels\n";
			int binCount = parent->data->histogramBinCount;
			double sInc = (data[i].last().dose-data[i][0].dose)/double(binCount);
			double s0 = data[i][0].dose;
			double prev = s0, cur = s0;
			int dataIndex = 0, dataCount = 0;
			
			for (int j = 1; j <= binCount; j++) {
				cur = s0+sInc*j;
				
				dataCount = 0;
				while (dataIndex < data[i].size()) {
					if (data[i][dataIndex].dose > cur)
						break;
					dataCount++;
					dataIndex++;
				}
				
				csvText += QString::number((cur+prev)/2.0)+","+QString::number(dataCount)+"\n";
				
				prev = cur;
			}
			
			QFile csvFile(path+"/"+contourNameLabel[i]->text()+"_diff.csv");
			if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
				QTextStream csvOut(&csvFile);
				csvOut << csvText;
			}
			
			csvFile.close();
			parent->updateProgress(increment);
		}
	}
	
	// Finally saved RT Dose
	parent->data->outputRTDose(rtFile, rtFile2, &doseData, doseScaling, 3.0);
	
	// Finish with progress bar
	parent->finishedProgress();
}