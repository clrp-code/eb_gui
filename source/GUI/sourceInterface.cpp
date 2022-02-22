/*
################################################################################
#
#  egs_brachy_GUI sourceInterface.cpp
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
#include "sourceInterface.h"

// Constructors
sourceInterface::sourceInterface() {
	parent = (Interface*)parentWidget();
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

sourceInterface::sourceInterface(Interface* p) {
	parent = p;
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

// Destructor
sourceInterface::~sourceInterface() {
	delete log;
}

// Layout Settings
void sourceInterface::createLayout() {
	mainLayout = new QGridLayout();
	QString ttt = ""; // tool tip text
	
	planFrame           = new QFrame();     
	planLabel           = new QLabel(tr("<b>Import DICOM RT PLAN Data</b>"));
	planLayout          = new QGridLayout();
	
	fileLabel           = new QLabel(tr("Select DICOM file"));     
	fileEdit            = new QLineEdit(tr("none selected"));  
	fileLoad            = new QPushButton(tr("Load"));
	ttt = tr("The DICOM plan file to be parsed.");
	fileLabel->setToolTip(ttt);
	fileEdit->setToolTip(ttt);
	fileEdit->setDisabled(true);
	fileLoad->setToolTip(ttt);
	
	importButton        = new QPushButton(tr("Import"));
	ttt = tr("Import the fields below from plan file, and generate a source location file. \n "
			 "Dwell times, variable activity, eye plaque geometries, and applicators will also be imported.");
	importButton->setToolTip(ttt);
	
	tagLabel            = new QLabel(tr("Plan name"));     
	tagEdit             = new QLineEdit("DICOM_plan");
	ttt = tr("The name tag used to name the source locations and dwell/activity files.");
	tagLabel->setToolTip(ttt);
	tagEdit->setToolTip(ttt);
	
	isotopeLabel        = new QLabel(tr("Isotope"));
	isotopeEdit         = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Isotope read in from DICOM file.  This field is only for display purposes, seed selection is still manual.");
	isotopeLabel->setToolTip(ttt);
	isotopeEdit->setToolTip(ttt);
	isotopeEdit->setDisabled(true);
	
	seedLabel           = new QLabel(tr("Seed name"));
	seedEdit            = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Seed name read in from DICOM file.  This field is only for display purposes, seed selection is still manual.");
	seedLabel->setToolTip(ttt);
	seedEdit->setToolTip(ttt);
	seedEdit->setDisabled(true);
	
	seedCountLabel      = new QLabel(tr("Seed count"));
	seedCountEdit       = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Seed count read in from DICOM file.  This field is only for display purposes, seed transformation selection is still manual.");
	seedCountLabel->setToolTip(ttt);
	seedCountEdit->setToolTip(ttt);
	seedCountEdit->setDisabled(true);
	
	transformationLabel = new QLabel(tr("Source locations"));
	transformationEdit  = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Seed transformation generated when reading in DICOM file.  This field is only for display purposes, seed transformation selection is still manual.");
	transformationLabel->setToolTip(ttt);
	transformationEdit->setToolTip(ttt);
	transformationEdit->setDisabled(true);
	
	airKermaLabel       = new QLabel(tr("Air kerma strength"));
	airKermaEdit        = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Seed air kerma strength.  This field is only for display purposes, air kerma strength input is still manual.");
	airKermaLabel->setToolTip(ttt);
	airKermaEdit->setToolTip(ttt);
	airKermaEdit->setDisabled(true);
	
	dwellLabel          = new QLabel(tr("Activity/dwell file"));
	dwellEdit           = new QLineEdit(tr("no DICOM file selected"));
	ttt = tr("Seed transformation generated when reading in DICOM file.  This field is only for display purposes, dwell/activity file selection is still manual.");
	dwellLabel->setToolTip(ttt);
	dwellEdit->setToolTip(ttt);
	dwellEdit->setDisabled(true);
	
	// Setup the widget layout
	planLayout->addWidget(planLabel          ,  0, 0, 1, 3);
	
	planLayout->addWidget(fileLabel          ,  1, 0, 1, 1);
	planLayout->addWidget(fileLoad           ,  2, 0, 1, 1);
	planLayout->addWidget(fileEdit           ,  2, 1, 1, 2);
	
	planLayout->addWidget(importButton       ,  3, 0, 1, 3);
	
	planLayout->addWidget(tagLabel           ,  4, 0, 1, 1);
	planLayout->addWidget(tagEdit            ,  4, 1, 1, 2);
	
	planLayout->addWidget(isotopeLabel       ,  6, 0, 1, 1);
	planLayout->addWidget(isotopeEdit        ,  6, 1, 1, 2);
	
	planLayout->addWidget(seedLabel          ,  7, 0, 1, 1);
	planLayout->addWidget(seedEdit           ,  7, 1, 1, 2);
	
	planLayout->addWidget(seedCountLabel     ,  8, 0, 1, 1);
	planLayout->addWidget(seedCountEdit      ,  8, 1, 1, 2);
	
	planLayout->addWidget(transformationLabel,  9, 0, 1, 1);
	planLayout->addWidget(transformationEdit ,  9, 1, 1, 2);
	
	planLayout->addWidget(airKermaLabel      , 10, 0, 1, 1);
	planLayout->addWidget(airKermaEdit       , 10, 1, 1, 2);
	
	planLayout->addWidget(dwellLabel         , 11, 0, 1, 1);
	planLayout->addWidget(dwellEdit          , 11, 1, 1, 2);
	
	planLayout->setRowStretch(5,5);
	planFrame->setLayout(planLayout);
	
	mainLayout->addWidget(planFrame,0,0,1,2);
	
	setLayout(mainLayout);
}

void sourceInterface::connectLayout() {
	connect(fileLoad, SIGNAL(released()),
			this, SLOT(loadPlan()));
	
	connect(importButton, SIGNAL(released()),
			this, SLOT(parsePlan()));
}


void sourceInterface::loadPlan() {
	// Get a file and try to loaded
	QString path = QFileDialog::getOpenFileName(this, tr("Load DICOM plan file"));
	
	DICOM* planFile = parent->data->plan_data; // temporary pointer
	Attribute* tempAtt;
	
	if (parent->data->plan_loaded) {
		parent->data->plan_loaded = false;
		delete planFile;
	}
	
	planFile = new DICOM(&parent->data->tag_data);
	
	// Quit if you can't parse the file
	if (!parseError(planFile->parse(path))) {
		QMessageBox::warning(0, "DICOM error",
        tr("Failed to parse DICOM file."));
		delete planFile;
		return;
	}
	
	tempAtt = planFile->getEntry(0x0008, 0x0060); // Get att closest to (0008,0060)
	if (tempAtt->tag[0] == 0x0008 && tempAtt->tag[1] == 0x0060) {// See if it is (0008,0060)
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		
		if (temp.trimmed().compare("RTPLAN")) {
			QMessageBox::warning(0, "DICOM error",
			tr("DICOM modality (0008,0060) is not of type \"RTPLAN\"."));
			delete planFile;
			return;
		}
	}
	else {
		QMessageBox::warning(0, "DICOM error",
		tr("DICOM modality (0008,0060) field not found."));
		delete planFile;
		return;
	}
	
	// Get patient name for the transformation label
	tempAtt = planFile->getEntry(0x0010, 0x0010); // Get att closest to (0010,0010)
	if (tempAtt->tag[0] == 0x0010 && tempAtt->tag[1] == 0x0010) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		
		tagEdit->setText(temp);
	}
	else {
		tagEdit->setText("DICOM_plan");
	}
	
	// Update the struct file field
	fileEdit->setText(path);
	fileEdit->setToolTip(path);
	
	// Save file in data
	parent->data->plan_data = planFile;
	parent->data->plan_loaded = true;
}

int sourceInterface::parseError(int err) {
	if (err == 501) {
		QMessageBox::warning(0, "file error",
        tr("Could not open selected file.Aborting."));
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

void sourceInterface::parsePlan() {
	// Check if plan data is loaded
	if (!parent->data->plan_loaded) {
		QMessageBox::warning(0, "Import RT Plan error",
		tr("No DICOM RT Plan file is loaded. Aborting."));
		return;		
	}
	
	QString fileName = tagEdit->text().trimmed().replace(" ", "_");
	QList <QListWidgetItem*> matchingNames = parent->transformationListView->findItems(fileName,Qt::MatchExactly);
	
	// Check if corresponding transformation file exists
	if (matchingNames.size()) {
		if (QMessageBox::Yes == QMessageBox::question(this, "Name already found",
		tr("A source location file named ") + fileName + tr(" already exists.  Would you like to overwrite it?"))) {
			// Get the file index
			int i = parent->data->localNameTransforms.indexOf(fileName);
			
			// Delete all associated files
			QFile(parent->data->localDirTransforms[i]+fileName).remove();
			QFile(parent->data->localDirTransforms[i]+fileName+".log").remove();
			QFile(parent->data->localDirTransforms[i]+fileName+".dwell").remove();
			QFile(parent->data->localDirTransforms[i]+fileName+".activity").remove();
			
			// Delete the file references
			parent->data->localNameTransforms.removeAt(i);
			parent->data->localDirTransforms.removeAt(i);
			
			// Remove it from the list widget
			delete matchingNames[0];
		}
		else { // They did not proceed
			return;
		}
	}
	
	// Invoke build parsePlan from data
	QString textLog;
	int err;
	err = parent->data->parsePlan(&textLog);
	
	if (err == 0) {
		// Output transformation file
		QFile transFile(parent->data->gui_location+"/database/transformation/"+fileName);
		if (transFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&transFile);
			out << QString("# Seed Location Transformations\n");
			out << QString("# Air Kerma Strength = ") + QString::number(parent->data->airKerma) + "\n";
			for (int i = 0; i < parent->data->seedPos.size(); i++) {
				out << QString("\n# Seed ") + QString::number(i) + "\n";
				out << QString(":start transformation:\n");
				out << QString("    translation = %1 %2 %3\n").arg(parent->data->seedPos[i].x())
				.arg(parent->data->seedPos[i].y()).arg(parent->data->seedPos[i].z());
				out << QString(":stop transformation:\n");
			}
			transFile.close();
		}
		
		parent->data->localNameTransforms << fileName;
		parent->data->localDirTransforms << parent->data->gui_location+"/database/transformation/";
		parent->transformationRepopulate();
				
		// Output log file
		QFile logFile(parent->data->gui_location+"/database/transformation/"+fileName+".log");
		if (logFile.open(QIODevice::WriteOnly)) {
			QTextStream out(&logFile);
			out << textLog;
			logFile.close();
		}
		
		// Output dwell file (if there is variable activity)
		if (parent->data->treatmentTime > 0) {
			QFile dwellFile(parent->data->gui_location+"/database/transformation/"+fileName+".dwell");
			if (dwellFile.open(QIODevice::WriteOnly)) {
				QTextStream out(&dwellFile);
				out << parent->data->seedTime[0];
				for (int i = 1; i < parent->data->seedTime.size(); i++) {
					out << QString(" ") << parent->data->seedTime[i];
				}
				dwellFile.close();
			}
		}
		
		// Fill the proper fields
		parent->transformationListView->setCurrentRow(parent->transformationListView->count()-1);
		isotopeEdit->setText(parent->data->isotopeName);
		seedEdit->setText(parent->data->seedInfo);
		seedCountEdit->setText(QString::number(parent->data->seedPos.size()));
		transformationEdit->setText(fileName);
		if (parent->data->treatmentTime > 0) {
			dwellEdit->setText(fileName+".dwell");
			dwellEdit->setToolTip(fileName+".dwell");
			parent->transformationDwell->setChecked(true);
			parent->sourcePermTime->setChecked(false);
			parent->sourceTempTimeEdit->setText(QString::number(parent->data->treatmentTime));
		}
		else {
			dwellEdit->setText(tr("none"));
			parent->transformationDwell->setChecked(false);
		}
		airKermaEdit->setText(QString::number(parent->data->airKerma));
		
		parent->sourceScaleBox->setCurrentText("Air kerma strength");
		parent->sourceScaleEdit->setText(QString::number(parent->data->airKerma));
		
		// Show the log
		log->outputArea->clear();
		log->outputArea->setPlainText(textLog);
		log->show();
	}
	else if (err == 101)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Treatment Type (300A, 0202)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 102)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Treatment Technique (300A, 0200)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 103)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Source Sequence (300A, 0210)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 201)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Reference Air Kerma Rate (300A, 022A)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 202)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Source Isotope Half Life (300A, 0228)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 203)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Source Isotope Name (300A, 0226)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 301)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Application Setup Sequence (300A, 0230)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 302)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Channel Sequence (300A, 0280)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 303)
		QMessageBox::warning(0, "DICOM error",
        tr("Could not find field ") + "Brachy Control Point Sequence (300A, 02D0)" + tr (" in CT DICOM file.  DICOM parsing aborted."));
	else if (err == 401)
		QMessageBox::warning(0, "Seed count error",
        tr("Found some seed dwell times but number doesn't match the overall seed count.  DICOM parsing aborted."));
	else
		QMessageBox::warning(0, "DICOM error",
        tr("Unknown error ") + QString::number(err) + tr(" occurred.  DICOM parsing aborted."));
}

// Refresh
void sourceInterface::refresh() {
	mainLayout->addWidget(parent->sourceFrame,1,0,1,3);
	parent->sourceRefresh();
	mainLayout->addWidget(parent->transformationFrame,1,3,1,3);
	parent->transformationRefresh();
	mainLayout->addWidget(parent->geometryFrame,0,2,1,4);
}