/*
################################################################################
#
#  egs_brachy_GUI doseInterface.cpp
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
#include "doseInterface.h"

// Constructors
doseInterface::doseInterface() :
allowedReals(QRegExp("[-]?\\d(\\d*|(e|E)[+]?\\d{1,2}|[.]\\d*(e|E)[+-]?\\d{1,2})")),
allowedPosReals(QRegExp("\\d(\\d*|(e|E)[+]?\\d{1,2}|[.]\\d*(e|E)[+-]?\\d{1,2})")),
allowedNats(QRegExp("d{1,2})")) {
	parent = (Interface*)parentWidget();
	
	createLayout();
	connectLayout();
}

doseInterface::doseInterface(Interface* p) {
	parent = p;
	createLayout();
	connectLayout();
}

// Destructor
doseInterface::~doseInterface() {
}

// Layout Settings
void doseInterface::createLayout() {
	mainLayout = new QGridLayout();
	
	// Shared objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	optionsBox      = new QComboBox();
	optionsBox->addItem("egsphant & colour maps");
	optionsBox->addItem("histograms");
	optionsBox->addItem("profiles");
				    
	canvasArea      = new QScrollArea();
	canvas          = new QLabel();
	saveDataButton  = new QPushButton("Save data");
	saveImageButton = new QPushButton("Save image");
	canvasInfo      = new QLabel("");
	
	canvasArea->setWidget(canvas);
				    
	rendering       = new QFrame();
	renderingLayout = new QGridLayout();
	renderButton    = new QPushButton("Render");
	renderCheckBox  = new QCheckBox("Live render");
	resolutionLabel = new QLabel("Resolution scaling");
	resolutionScale = new QLineEdit("1");
	
	resolutionScale->setValidator(&allowedNats);
	renderingLayout->addWidget(renderButton   , 0, 0, 1, 1);
	renderingLayout->addWidget(renderCheckBox , 0, 1, 1, 1);
	renderingLayout->addWidget(resolutionLabel, 1, 0, 1, 1);
	renderingLayout->addWidget(resolutionScale, 1, 1, 1, 1);
	rendering->setLayout(renderingLayout);
	rendering->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	
	previewLayout = new QVBoxLayout();
	histoLayout = new QVBoxLayout();
	profileLayout = new QVBoxLayout();
	
	// Preview ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Preview bounds
	dimFrame          = new QFrame();
	dimLayout         = new QGridLayout();
	
	xAxisButton       = new QRadioButton("x axis");
	yAxisButton       = new QRadioButton("y axis");
	zAxisButton       = new QRadioButton("z axis");
	zAxisButton->setChecked(true);
	
	vertBoundaryLabel = new QLabel("x range");
	vertBoundaryMin   = new QLineEdit("-10");
	vertBoundaryMax   = new QLineEdit("10");
	vertBoundaryMin->setValidator(&allowedReals);
	vertBoundaryMax->setValidator(&allowedReals);
	
	horBoundaryLabel  = new QLabel("y range");
	horBoundaryMin    = new QLineEdit("-10");
	horBoundaryMax    = new QLineEdit("10");
	horBoundaryMin->setValidator(&allowedReals);
	horBoundaryMax->setValidator(&allowedReals);
	
	depthLabel        = new QLabel("z depth");
	depthMin          = new QLineEdit("0");
	depthMin->setValidator(&allowedReals);
	
	expandToBounds    = new QPushButton("reset");
	legendLabel       = new QLabel("legend");
	legendBox         = new QComboBox();
	legendBox->addItem("none");
	legendBox->addItem("phantom");
	legendBox->addItem("colour map");
	legendBox->addItem("isodose");
	
	unitsLabel        = new QLabel("Units");
	unitsEdit         = new QLineEdit("Gy");
	
	dimLayout->addWidget(xAxisButton      , 0, 0, 1, 2);
	dimLayout->addWidget(yAxisButton      , 0, 2, 1, 2);
	dimLayout->addWidget(zAxisButton      , 0, 4, 1, 2);
	dimLayout->addWidget(vertBoundaryLabel, 1, 0, 1, 4);
	dimLayout->addWidget(vertBoundaryMin  , 2, 0, 1, 2);
	dimLayout->addWidget(vertBoundaryMax  , 3, 0, 1, 2);
	dimLayout->addWidget(horBoundaryLabel , 1, 2, 1, 2);
	dimLayout->addWidget(horBoundaryMin   , 2, 2, 1, 2);
	dimLayout->addWidget(horBoundaryMax   , 3, 2, 1, 2);
	dimLayout->addWidget(depthLabel       , 1, 4, 1, 2);
	dimLayout->addWidget(depthMin         , 2, 4, 1, 2);
	dimLayout->addWidget(expandToBounds   , 3, 4, 2, 2);
	dimLayout->addWidget(legendLabel      , 5, 0, 1, 3);
	dimLayout->addWidget(legendBox        , 6, 0, 1, 3);
	dimLayout->addWidget(unitsLabel       , 5, 3, 1, 3);
	dimLayout->addWidget(unitsEdit        , 6, 3, 1, 3);
	
	dimFrame->setLayout(dimLayout);
	dimFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(dimFrame);
	
	// Phantom selection
	phantFrame    = new QFrame();
	phantLayout   = new QGridLayout();
	
	mediaButton   = new QRadioButton("media");
	densityButton = new QRadioButton("density");
	phantSelect   = new QComboBox();
	mediaButton->setChecked(true);
	phantSelect->addItem("none");
	
	phantLoaded   = false;
	
	densityLabel  = new QLabel("Density range");
	densityMin    = new QLineEdit("0");
	densityMax    = new QLineEdit("3");
	densityMin->setValidator(&allowedPosReals);
	densityMax->setValidator(&allowedPosReals);
	
	phantLayout->addWidget(phantSelect  , 0, 0, 1, 2);
	phantLayout->addWidget(mediaButton  , 1, 0, 1, 1);
	phantLayout->addWidget(densityButton, 1, 1, 1, 1);
	phantLayout->addWidget(densityLabel , 2, 0, 1, 2);
	phantLayout->addWidget(densityMin   , 3, 0, 1, 1);
	phantLayout->addWidget(densityMax   , 3, 1, 1, 1);
	
	phantFrame->setLayout(phantLayout);
	phantFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(phantFrame);
	
	// Map selection
	mapFrame      = new QFrame();
	mapLayout     = new QGridLayout();
				  
	mapDoseBox    = new QComboBox();
	mapDoseBox->addItem("none");
				  
	mapDoseLoaded = false;
	
	mapMinLabel   = new QLabel("min");
	mapMaxLabel   = new QLabel("max");
	
	mapMinDose    = new QLineEdit("0");
	mapMaxDose    = new QLineEdit("100");
	mapMinDose->setValidator(&allowedPosReals);
	mapMaxDose->setValidator(&allowedPosReals);
	
	mapMinButton  = new QPushButton();
	mapMaxButton  = new QPushButton();
	mapMinButton->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	mapMaxButton->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	
	mapLayout->addWidget(mapDoseBox  , 0, 0, 1, 2);
	mapLayout->addWidget(mapMinLabel , 1, 0, 1, 1);
	mapLayout->addWidget(mapMaxLabel , 1, 1, 1, 1);
	mapLayout->addWidget(mapMinDose  , 2, 0, 1, 1);
	mapLayout->addWidget(mapMaxDose  , 2, 1, 1, 1);
	mapLayout->addWidget(mapMinButton, 3, 0, 1, 1);
	mapLayout->addWidget(mapMaxButton, 3, 1, 1, 1);
	
	mapFrame->setLayout(mapLayout);
	mapFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(mapFrame);
	
	// Isodose selection
	isoFrame       = new QFrame();
	isoLayout      = new QGridLayout();
	
	isoColourLabel = new QLabel("Colours");
	
	// Lines 1 - 3
	isoDoseLabel.append(new QLabel("solid")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose()); isoDosesLoaded.append(false);
	isoDoseLabel.append(new QLabel("dashed")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose()); isoDosesLoaded.append(false);
	isoDoseLabel.append(new QLabel("dotted")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose()); isoDosesLoaded.append(false);
	
	// Colors 1 - 5
	isoColourDose.append(new QLineEdit("20")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("40")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,255,255)}");
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("60")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,255,0)}");
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("80")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(255,255,0)}");
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("100")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	isoColourDose.last()->setValidator(&allowedPosReals);
	
	for (int i = 0; i < 3; i++) {
		isoLayout->addWidget(isoDoseLabel[i], i, 0, 1, 2);
		isoLayout->addWidget(isoDoseBox[i]  , i, 2, 1, 3);
	}
	
	isoLayout->addWidget(isoColourLabel, 3, 0, 1, 5);
	
	for (int i = 0; i < 5; i++) {
		isoLayout->addWidget(isoColourDose[i]  , 4, i, 1, 1);
		isoLayout->addWidget(isoColourButton[i], 5, i, 1, 1);
	}
	
	isoFrame->setLayout(isoLayout);
	isoFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(isoFrame);
	
	// Histogram ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	// Profile ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	// Main layout
	mainLayout->addWidget(optionsBox     , 0, 0, 1, 1);
	mainLayout->addWidget(canvasArea     , 0, 1, 4, 4);
	mainLayout->addWidget(rendering      , 3, 0, 2, 1);
	mainLayout->addWidget(canvasInfo     , 4, 1, 1, 1);
	mainLayout->addWidget(saveDataButton , 4, 3, 1, 1);
	mainLayout->addWidget(saveImageButton, 4, 4, 1, 1);
	
	mainLayout->addLayout(previewLayout  , 1, 0, 1, 1);	
	
	mainLayout->setColumnStretch(0, 1);
	mainLayout->setColumnStretch(1, 5);
	mainLayout->setRowStretch(2, 5);
	
	setLayout(mainLayout);
}

void doseInterface::connectLayout() {
	
}

// Swap panels
void doseInterface::resetLayout() {
	switch(optionsBox->currentIndex()) {
		case 0 :
			
			break;
		case 1 :
			
			break;
		case 2 :
			
			break;
		default :
			// change nothing
			break;
	}
}

// Refresh
void doseInterface::refresh() {
	if (renderCheckBox->isChecked()) {
		renderButton->setDisabled(true);
		render();
	}
	else
		renderButton->setDisabled(false);
	
	switch(optionsBox->currentIndex()) {
		case 0 :
			previewRefresh();
			break;
		case 1 :
			histoRefresh();
			break;
		case 2 :
			profileRefresh();
			break;
		default :
			// change nothing
			break;
	}
}

void doseInterface::previewRefresh() {
}

void doseInterface::histoRefresh() {
}

void doseInterface::profileRefresh() {
}

// Render functions
void doseInterface::render() {
	switch(optionsBox->currentIndex()) {
		case 0 :
			previewRender();
			break;
		case 1 :
			histoRender();
			break;
		case 2 :
			profileRender();
			break;
		default :
			// change nothing
			break;
	}
}
	
void doseInterface::previewCanvasRender() {
	
}

void doseInterface::previewPhantRender() {
	
}

void doseInterface::previewMapRender() {
	
}

void doseInterface::previewIsoRender() {
	
}

void doseInterface::previewRender() {
	
}

void doseInterface::histoRender() {
	
}

void doseInterface::profileRender() {
	
}