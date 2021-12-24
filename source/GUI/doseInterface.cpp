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
allowedReals(QRegExp(REGEX_REAL)),
allowedPosReals(QRegExp(REGEX_REAL_POS)),
allowedPosRealArrs(QRegExp(REGEX_REAL_POS_ARR)),
allowedNats(QRegExp(REGEX_NATURAL_POS)),
allowedPercents(QRegExp(REGEX_PERCENT)),
allowedPercentArrs(QRegExp(REGEX_PERCENT_ARR)) {
	parent = (Interface*)parentWidget();
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

doseInterface::doseInterface(Interface* p) :
allowedReals(QRegExp(REGEX_REAL)),
allowedPosReals(QRegExp(REGEX_REAL_POS)),
allowedPosRealArrs(QRegExp(REGEX_REAL_POS_ARR)),
allowedNats(QRegExp(REGEX_NATURAL_POS)),
allowedPercents(QRegExp(REGEX_PERCENT)),
allowedPercentArrs(QRegExp(REGEX_PERCENT_ARR)) {
	parent = p;
	
	log = new logWindow();
	
	createLayout();
	connectLayout();
}

// Destructor
doseInterface::~doseInterface() {
	delete canvasPic;
	delete blackPic;
	delete phantPic;
	delete phant;
	delete mapPic;
	delete mapDose;
	delete isoDoses[2];
	delete isoDoses[1];
	delete isoDoses[0];
	delete bufferLayout;
	delete log;
}

// Layout Settings
void doseInterface::createLayout() {
	mainLayout = new QGridLayout();
	bufferLayout = new QGridLayout();
	
	// Shared objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //				    
	canvasArea      = new QScrollArea();
	canvas          = new QLabel();
	canvasChart     = new QChartView();
	canvasPic       = new QImage();
	saveDataButton  = new QPushButton("Save data");
	saveImageButton = new QPushButton("Save image");
	canvasInfo      = new QLabel("");
	
	canvasArea->setWidget(canvas);
				    
	rendering       = new QFrame();
	renderingLayout = new QGridLayout();
	renderButton    = new QPushButton("Render");
	renderCheckBox  = new QCheckBox("Live render");
	resolutionLabel = new QLabel("Pixels per cm");
	resolutionScale = new QLineEdit("20");
	
	resolutionScale->setValidator(&allowedNats);
	renderingLayout->addWidget(renderButton   , 0, 0, 1, 1);
	renderingLayout->addWidget(renderCheckBox , 0, 1, 1, 1);
	renderingLayout->addWidget(resolutionLabel, 1, 0, 1, 1);
	renderingLayout->addWidget(resolutionScale, 1, 1, 1, 1);
	rendering->setLayout(renderingLayout);
	rendering->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	
	previewLayout = new QVBoxLayout();
	histoLayout   = new QVBoxLayout();
	profileLayout = new QVBoxLayout();
	
	previewFrame  = new QFrame();
	histoFrame    = new QFrame();
	profileFrame  = new QFrame();
	
	optionsTab    = new QTabWidget();
	
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
	
	legendLabel->setDisabled(true); // To be implemented
	legendBox->setDisabled(true); // To be implemented
	unitsLabel->setDisabled(true); // To be implemented
	unitsEdit->setDisabled(true); // To be implemented
	
	int width  = abs(horBoundaryMax->text().toDouble()-horBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	int height = abs(vertBoundaryMax->text().toDouble()-vertBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	blackPic = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
	blackPic->fill(qRgb(0,0,0));
	
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
	phantPic      = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
	phant         = new EGSPhant();
	
	phantFrame    = new QFrame();
	phantLayout   = new QGridLayout();
	phantLabel    = new QLabel("Phantom");
	
	mediaButton   = new QRadioButton("media");
	densityButton = new QRadioButton("density");
	phantSelect   = new QComboBox();
	phantSelect->addItem("none");
	phantSelect->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	
	densityLabel  = new QLabel("Density range");
	densityMin    = new QLineEdit("0");
	densityMax    = new QLineEdit("3");
	densityMin->setValidator(&allowedPosReals);
	densityMax->setValidator(&allowedPosReals);
	
	mediaButton->setChecked(true);
	densityLabel->setDisabled(true);
	densityMin->setDisabled(true);
	densityMax->setDisabled(true);
	
	phantLayout->addWidget(phantLabel   , 0, 0, 1, 2);
	phantLayout->addWidget(phantSelect  , 0, 2, 1, 4);
	phantLayout->addWidget(mediaButton  , 1, 0, 1, 3);
	phantLayout->addWidget(densityButton, 1, 3, 1, 3);
	phantLayout->addWidget(densityLabel , 2, 0, 1, 6);
	phantLayout->addWidget(densityMin   , 3, 0, 1, 3);
	phantLayout->addWidget(densityMax   , 3, 3, 1, 3);
	
	phantFrame->setLayout(phantLayout);
	phantFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(phantFrame);
	
	// Map selection
	mapPic        = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);	
	mapDose       = new Dose();
	
	mapFrame      = new QFrame();
	mapLayout     = new QGridLayout();
	mapLabel      = new QLabel("Colour map");
				  
	mapDoseBox    = new QComboBox();
	mapDoseBox->addItem("none");
	mapDoseBox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	
	mapMinLabel   = new QLabel("min");
	mapMaxLabel   = new QLabel("max");
	
	mapMinDose    = new QLineEdit("20");
	mapMaxDose    = new QLineEdit("100");
	mapMinDose->setValidator(&allowedPosReals);
	mapMaxDose->setValidator(&allowedPosReals);
	
	mapMinButton  = new QPushButton();
	mapMidButton  = new QPushButton();
	mapMaxButton  = new QPushButton();
	mapMinButton->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	mapMidButton->setStyleSheet("QPushButton {background-color: rgb(0,255,0)}");
	mapMaxButton->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	
	mapOpacLabel = new QLabel("Opacity");
	mapOpacSlider = new QSlider(Qt::Horizontal);
	mapOpacSlider->setRange(0,100);
	mapOpacSlider->setValue(50);
	
	mapLayout->addWidget(mapLabel     , 0, 0, 1, 2);
	mapLayout->addWidget(mapDoseBox   , 0, 2, 1, 4);
	mapLayout->addWidget(mapMinLabel  , 1, 0, 1, 3);
	mapLayout->addWidget(mapMaxLabel  , 1, 3, 1, 3);
	mapLayout->addWidget(mapMinDose   , 2, 0, 1, 3);
	mapLayout->addWidget(mapMaxDose   , 2, 3, 1, 3);
	mapLayout->addWidget(mapMinButton , 3, 0, 1, 2);
	mapLayout->addWidget(mapMidButton , 3, 2, 1, 2);
	mapLayout->addWidget(mapMaxButton , 3, 4, 1, 2);
	mapLayout->addWidget(mapOpacLabel , 4, 0, 1, 2);
	mapLayout->addWidget(mapOpacSlider, 4, 2, 1, 4);
	
	mapFrame->setLayout(mapLayout);
	mapFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(mapFrame);
	
	// Isodose selection
	isoFrame       = new QFrame();
	isoLayout      = new QGridLayout();
	
	isoColourLabel = new QLabel("Colours");
	
	// Lines 1 - 3
	isoDoseLabel.append(new QLabel("solid line")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	isoDoseLabel.append(new QLabel("dashed line")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	isoDoseLabel.append(new QLabel("dotted line")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	
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
		isoLayout->addWidget(isoDoseLabel[i], 0, i*5, 1, 5);
		isoLayout->addWidget(isoDoseBox[i]  , 1, i*5, 1, 5);
	}
	
	isoLayout->addWidget(isoColourLabel, 2, 0, 1, 15);
	
	for (int i = 0; i < 5; i++) {
		isoLayout->addWidget(isoColourDose[i]  , 3, i*3, 1, 3);
		isoLayout->addWidget(isoColourButton[i], 4, i*3, 1, 3);
	}
	
	isoFrame->setLayout(isoLayout);
	isoFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	isoFrame->setDisabled(true); // Fix/reimplement getContour in dose class
	previewLayout->addWidget(isoFrame);
	
	// Histogram ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Phantom selection
	histPhantLabel  = new QLabel("Phantom");
	histPhant       = new EGSPhant();
	histPhantSelect = new QComboBox();
	histPhantSelect->addItem("none");
	
	histPhantFrame  = new QFrame();
	histPhantLayout = new QGridLayout();
	
	histPhantLayout->addWidget(histPhantLabel , 0, 0, 1, 1);
	histPhantLayout->addWidget(histPhantSelect, 1, 0, 1, 1);
	
	histPhantFrame->setLayout(histPhantLayout);
	histPhantFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histPhantFrame);
	
	// Filters
	histFilterLabel  = new QLabel("Filters");
	
	histMaskLabel    = new QLabel("Mask");
	histMaskSelect   = new QComboBox();
	histMaskSelect->addItem("none");
	histMask         = new EGSPhant();
	
	histMediumLabel  = new QLabel("Media");
	histMediumView   = new QListWidget();
	histMediumView->setSelectionMode(QAbstractItemView::MultiSelection);
	
	histDoseMinLabel = new QLabel("Min Dose (Gy)");
	histDoseMinEdit  = new QLineEdit("0");
	histDoseMinEdit->setValidator(&allowedPosReals);
	histDoseMaxLabel = new QLabel("Max Dose (Gy)");
	histDoseMaxEdit  = new QLineEdit("0");
	histDoseMaxEdit->setValidator(&allowedPosReals);
	
	histFilterFrame  = new QFrame();
	histFilterLayout = new QGridLayout();
	
	histFilterLayout->addWidget(histFilterLabel , 0, 0, 1, 6);
	histFilterLayout->addWidget(histMaskLabel   , 1, 0, 1, 2);
	histFilterLayout->addWidget(histMaskSelect  , 1, 2, 1, 4);
	histFilterLayout->addWidget(histMediumLabel , 2, 0, 1, 6);
	histFilterLayout->addWidget(histMediumView  , 3, 0, 1, 6);
	
	histFilterLayout->addWidget(histDoseMinLabel, 4, 0, 1, 3);
	histFilterLayout->addWidget(histDoseMinEdit , 4, 3, 1, 3);
	histFilterLayout->addWidget(histDoseMaxLabel, 5, 0, 1, 3);
	histFilterLayout->addWidget(histDoseMaxEdit , 5, 3, 1, 3);
	
	histFilterFrame->setLayout(histFilterLayout);
	histFilterFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histFilterFrame);
	
	// Doses
	histDosesLabel   = new QLabel("Doses");
	histLoadButton   = new QPushButton("Load");
	histDoseSelect   = new QComboBox();
	histDeleteButton = new QPushButton("Delete");
	
	histLoadedView   = new QListWidget();
					 
	histLegendBox    = new QCheckBox("Legend");
	histDiffBox      = new QCheckBox("Differential");
	histLegendBox->setChecked(true);
					 
	histDosesFrame   = new QFrame();
	histDosesLayout  = new QGridLayout();
	
	histDosesLayout->addWidget(histDosesLabel  , 0, 0, 1, 4);
	histDosesLayout->addWidget(histLoadButton  , 1, 0, 1, 1);
	histDosesLayout->addWidget(histDoseSelect  , 1, 1, 1, 3);
	histDosesLayout->addWidget(histLoadedView  , 2, 0, 1, 4);
	histDosesLayout->addWidget(histDeleteButton, 3, 0, 1, 4);
	histDosesLayout->addWidget(histLegendBox   , 4, 0, 1, 2);
	histDosesLayout->addWidget(histDiffBox     , 4, 2, 1, 2);
	
	histDosesFrame->setLayout(histDosesLayout);
	histDosesFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histDosesFrame);
	
	// Added data
	histOutputLabel  = new QLabel("Metrics");
	histOutputBox    = new QComboBox();
	
	histDxLabel      = new QLabel("Dx");
	histDxEdit       = new QLineEdit("10,20,80,90");
	histVxLabel      = new QLabel("Vx");
	histVxEdit       = new QLineEdit("20,40,60,80,100");
	histDxEdit->setValidator(&allowedPercentArrs);
	histVxEdit->setValidator(&allowedPosRealArrs);
	
	histCalcButton   = new QPushButton("Calculate");
	histSaveButton   = new QPushButton("Output metrics");
	histRawButton    = new QPushButton("Output raw data");
	
	histOutputFrame  = new QFrame();
	histOutputLayout = new QGridLayout();
	
	histOutputLayout->addWidget(histOutputLabel , 0, 0, 1, 2);
	histOutputLayout->addWidget(histOutputBox   , 0, 0, 1, 2);
	histOutputLayout->addWidget(histDxLabel     , 1, 0, 1, 2);
	histOutputLayout->addWidget(histDxEdit      , 1, 2, 1, 4);
	histOutputLayout->addWidget(histVxLabel     , 2, 0, 1, 2);
	histOutputLayout->addWidget(histVxEdit      , 2, 2, 1, 4);
	histOutputLayout->addWidget(histCalcButton  , 3, 0, 1, 2);
	histOutputLayout->addWidget(histSaveButton  , 3, 2, 1, 2);
	histOutputLayout->addWidget(histRawButton   , 3, 4, 1, 2);
	
	histOutputFrame->setLayout(histOutputLayout);
	histOutputFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histOutputFrame);
	
	// Profile ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	// Main layout
	mainLayout->addWidget(optionsTab     , 0, 0, 2, 1);
	mainLayout->addWidget(canvasArea     , 0, 1, 4, 4);
	//mainLayout->addWidget(canvasChart    , 0, 1, 4, 4);
	mainLayout->addWidget(rendering      , 3, 0, 2, 1);
	mainLayout->addWidget(canvasInfo     , 4, 1, 1, 1);
	mainLayout->addWidget(saveDataButton , 4, 3, 1, 1);
	mainLayout->addWidget(saveImageButton, 4, 4, 1, 1);
	
	bufferLayout->addWidget(canvasChart, 0, 1, 4, 4); // Hold it in another layout, and swap as needed
	
	mainLayout->setColumnStretch(0, 1);
	mainLayout->setColumnStretch(1, 5);
	mainLayout->setRowStretch(2, 5);
	
	previewFrame->setLayout(previewLayout);
	histoFrame->setLayout(histoLayout);
	profileFrame->setLayout(profileLayout);
	
	optionsTab->addTab(previewFrame, "Preview");
	optionsTab->addTab(histoFrame, "DVH");
	optionsTab->addTab(profileFrame, "Profile");	
	
	setLayout(mainLayout);
}

void doseInterface::connectLayout() {
	connect(optionsTab, SIGNAL(currentChanged(int)),
			this, SLOT(refresh()));
	connect(renderCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
	connect(renderButton, SIGNAL(pressed()),
			this, SLOT(render()));
	
	// Preview ~~~~~~~~~~~~~~
	connect(resolutionScale, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	
	connect(saveImageButton, SIGNAL(pressed()),
			this, SLOT(saveImage()));
	connect(saveDataButton, SIGNAL(pressed()),
			this, SLOT(saveData()));
			
	// Dimensions
	connect(xAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	connect(yAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	connect(zAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	
	connect(vertBoundaryMin, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	connect(vertBoundaryMax, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	connect(horBoundaryMin, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	connect(horBoundaryMax, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	connect(depthMin, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	connect(legendBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(previewCanvasRenderLive()));
	connect(unitsEdit, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
	
	connect(expandToBounds, SIGNAL(pressed()),
			this, SLOT(previewResetBounds()));
	
	// Egsphant
	connect(phantSelect, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadEgsphant()));
	connect(mediaButton, SIGNAL(toggled(bool)),
			this, SLOT(previewPhantRenderLive()));
	connect(mediaButton, SIGNAL(toggled(bool)),
			this, SLOT(previewRefresh()));
	connect(densityButton, SIGNAL(toggled(bool)),
			this, SLOT(previewPhantRenderLive()));
	connect(densityButton, SIGNAL(toggled(bool)),
			this, SLOT(previewRefresh()));
	connect(densityMin, SIGNAL(textEdited(QString)),
			this, SLOT(previewPhantRenderLive()));
	connect(densityMax, SIGNAL(textEdited(QString)),
			this, SLOT(previewPhantRenderLive()));
	
	// Map
	connect(mapDoseBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadMapDose()));
	connect(mapMinDose, SIGNAL(textEdited(QString)),
			this, SLOT(previewMapRenderLive()));
	connect(mapMaxDose, SIGNAL(textEdited(QString)),
			this, SLOT(previewMapRenderLive()));
	connect(mapOpacSlider, SIGNAL(valueChanged(int )),
			this, SLOT(previewMapRenderLive()));
	
	// Isodose
	QSignalMapper* sigMap = new QSignalMapper(this); // Should get deleted as a child of this
	for (int i = 0; i < 3; i++) {
		connect(isoDoseBox[i], SIGNAL(currentIndexChanged(int)),
				sigMap, SLOT(map()));
	}
	sigMap->setMapping(isoDoseBox[0], 0);
	sigMap->setMapping(isoDoseBox[1], 1);
	sigMap->setMapping(isoDoseBox[2], 2);
	
	connect(sigMap, SIGNAL(mapped(int)),
			this, SLOT(loadIsoDose(int)));
			
	// Change all colours
	sigMap = new QSignalMapper(this); // Should get deleted as a child of this
	connect(mapMinButton, SIGNAL(pressed()),
			sigMap, SLOT(map()));
	connect(mapMidButton, SIGNAL(pressed()),
			sigMap, SLOT(map()));
	connect(mapMaxButton, SIGNAL(pressed()),
			sigMap, SLOT(map()));
	for (int i = 0; i < 5; i++)
		connect(isoColourButton[i], SIGNAL(pressed()),
				sigMap, SLOT(map()));
				
	sigMap->setMapping(mapMinButton, 0);
	sigMap->setMapping(mapMidButton, 1);
	sigMap->setMapping(mapMaxButton, 2);
	
	for (int i = 0; i < 5; i++)
		sigMap->setMapping(isoColourButton[i], 3+i);
	
	connect(sigMap, SIGNAL(mapped(int)),
			this, SLOT(previewChangeColor(int)));
			
	// Histogram ~~~~~~~~~~~~
	connect(histPhantSelect, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadFilterEgsphant()));
	connect(histMaskSelect, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadMaskEgsphant()));
	
	connect(histLoadButton, SIGNAL(pressed()),
			this, SLOT(loadHistoDose()));
	connect(histDeleteButton, SIGNAL(pressed()),
			this, SLOT(deleteHistoDose()));
			
	connect(histCalcButton, SIGNAL(pressed()),
			this, SLOT(calcMetrics()));
	connect(histSaveButton, SIGNAL(pressed()),
			this, SLOT(outputMetrics()));
	connect(histRawButton, SIGNAL(pressed()),
			this, SLOT(outputRawData()));
	
	// Profile ~~~~~~~~~~~~~~
}

// Swap panels
void doseInterface::resetLayout() {
	switch(optionsTab->currentIndex()) {
		case 0 :
			mapDoseBox->setCurrentIndex(0);
			isoDoseBox[0]->setCurrentIndex(0);
			isoDoseBox[1]->setCurrentIndex(0);
			isoDoseBox[2]->setCurrentIndex(0);
			phantSelect->setCurrentIndex(0);
			break;
		case 1 :
			histPhantSelect->setCurrentIndex(0);
			break;
		case 2 :
			
			break;
		default :
			// change nothing
			break;
	}
}

// Delete loaded doses, called when repopulating dose
void doseInterface::resetDoses() {
	for (int i = histDoses.size()-1; i >= 0; i--)
		delete histDoses[i];
	histDoses.clear();
}

// Swap image canvas with 
void doseInterface::tabSwap() {

}

// Refresh
void doseInterface::refresh() {
	if (renderCheckBox->isChecked()) {
		renderButton->setDisabled(true);
		render();
	}
	else
		renderButton->setDisabled(false);
	
	switch(optionsTab->currentIndex()) {
		case 0 :
			bufferLayout->addWidget(canvasArea->takeWidget());
			canvasArea->setWidget(canvas);
			previewRefresh();
			resolutionLabel->setDisabled(false);
			resolutionScale->setDisabled(false);
			renderCheckBox->setDisabled(false);
			saveDataButton->setDisabled(true);
			saveImageButton->setDisabled(false);
			break;
		case 1 :
			bufferLayout->addWidget(canvasArea->takeWidget());
			canvasArea->setWidget(canvasChart);
			histoRefresh();
			resolutionLabel->setDisabled(true);
			resolutionScale->setDisabled(true);
			renderCheckBox->setDisabled(true);
			renderCheckBox->setChecked(false);
			saveDataButton->setDisabled(false);
			saveImageButton->setDisabled(true);
			break;
		case 2 :
			bufferLayout->addWidget(canvasArea->takeWidget());
			canvasArea->setWidget(canvasChart);
			profileRefresh();
			resolutionLabel->setDisabled(true);
			resolutionScale->setDisabled(true);
			renderCheckBox->setDisabled(true);
			renderCheckBox->setChecked(false);
			saveDataButton->setDisabled(false);
			saveImageButton->setDisabled(true);
			break;
		default :
			// change nothing
			break;
	}
}

// Render functions
void doseInterface::render() {
	switch(optionsTab->currentIndex()) {
		case 0 :
			previewCanvasRender(); // This will build the canvas and all subimages
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


// Preview save functions
void doseInterface::saveImage() {
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Images (*.png)"));
	
	if (filePath.length() < 1) // No name selected
		return;
	
	if (!filePath.endsWith(".png")) // Doesn't have the right extension
		filePath += ".png";
	
	canvasPic->save(filePath); // Should know it's png based on fileName suffix
}

void doseInterface::saveData() {
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Plot Data (*.csv)"));
	
	if (filePath.length() < 1) // No name selected
		return;
	
	if (!filePath.endsWith(".csv")) // Doesn't have the right extension
		filePath += ".csv";
		
	QFile dataFile(filePath);
	
	if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "Data file error",
		tr("Failed to open the file for data output.  Aborting"));	
		return;
	}
	
	QTextStream out(&dataFile);
	
	// Output data set names
	for (int i = 0; i < savePlotName.size(); i++)
		out << savePlotName[i] << ",,";
	out << "\n";
	
	
	// Output axes labels for each set of names
	for (int i = 0; i < savePlotName.size(); i++)
		out << savePlotX << "," << savePlotY << ",";
	out << "\n";
	
	int index = 0, doneOutput = 0;
	
	do {
		doneOutput = 0;
		for (int i = 0; i < savePlotData.size(); i++) {
			if (savePlotData[i].size() > index) // Only output plots who still have data
				out << savePlotData[i][index].x() << "," << savePlotData[i][index].y() << ",";
			else { // Else output blanks and count the series as done
				out << ",,";
				doneOutput++;
			}
		}
		out << "\n";
		index++;
	} while (doneOutput < savePlotData.size()); // Break when we don't output anything
	
	dataFile.close();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                                Preview                              //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
void doseInterface::previewChangeAxis() {
	if (xAxisButton->isChecked()) {
		vertBoundaryLabel->setText("y range");
		horBoundaryLabel->setText("z range");
		depthLabel->setText("x depth");
	}
	else if (yAxisButton->isChecked()) {
		vertBoundaryLabel->setText("x range");
		horBoundaryLabel->setText("z range");
		depthLabel->setText("y depth");
	}
	else if (zAxisButton->isChecked()) {
		vertBoundaryLabel->setText("x range");
		horBoundaryLabel->setText("y range");
		depthLabel->setText("z depth");
	}
	
	previewCanvasRenderLive();
}

void doseInterface::previewChangeColor(int i) {
	QColor colour;
	switch (i) {
		case 0 :
			colour = QColorDialog::getColor(mapMinButton->palette().color(QPalette::Button));
		break;
		case 1 :
			colour = QColorDialog::getColor(mapMidButton->palette().color(QPalette::Button));
		break;
		case 2 :
			colour = QColorDialog::getColor(mapMaxButton->palette().color(QPalette::Button));
		break;
		case 3 :
		case 4 :
		case 5 :
		case 6 :
		case 7 :
			colour = QColorDialog::getColor(isoColourButton[i-3]->palette().color(QPalette::Button));
		break;
		default:
		// do nothing
		break;
	}
		
    if(colour.isValid())
    {
		switch (i) {
			case 0 :
				mapMinButton->setStyleSheet(QString("QPushButton {background-color: rgb("+
				QString::number(colour.red())+","+
				QString::number(colour.green())+","+
				QString::number(colour.blue())+")}"));
			break;
			case 1 :
				mapMidButton->setStyleSheet(QString("QPushButton {background-color: rgb("+
				QString::number(colour.red())+","+
				QString::number(colour.green())+","+
				QString::number(colour.blue())+")}"));
			break;
			case 2 :
				mapMaxButton->setStyleSheet(QString("QPushButton {background-color: rgb("+
				QString::number(colour.red())+","+
				QString::number(colour.green())+","+
				QString::number(colour.blue())+")}"));
			break;
			case 3 :
			case 4 :
			case 5 :
			case 6 :
			case 7 :
				isoColourButton[i-3]->setStyleSheet(QString("QPushButton {background-color: rgb("+
				QString::number(colour.red())+","+
				QString::number(colour.green())+","+
				QString::number(colour.blue())+")}"));
			break;
			default:
			// do nothing
			break;
		}
		
		if (i < 3)
			previewMapRenderLive();
		else
			previewIsoRenderLive();
    }
}
	
void doseInterface::previewRefresh() {
	if (densityButton->isChecked()) {
		densityLabel->setDisabled(false);
		densityMin->setDisabled(false);
		densityMax->setDisabled(false);
	}
	else {
		densityLabel->setDisabled(true);
		densityMin->setDisabled(true);
		densityMax->setDisabled(true);
	}
}
	
void doseInterface::previewCanvasRenderLive() {if(renderCheckBox->isChecked()) previewCanvasRender();}
void doseInterface::previewCanvasRender() {
	int width  = abs(horBoundaryMax->text().toDouble() - horBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	int height = abs(vertBoundaryMax->text().toDouble() - vertBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	*blackPic = blackPic->scaled(width,height);
	
	// Invoke all subrenders to reflect the change to the axes
	previewPhantRender();
	previewMapRender();
	previewIsoRender();
	
	previewRender();
}

void doseInterface::previewPhantRenderLive() {if(renderCheckBox->isChecked()) previewPhantRender();}
void doseInterface::previewPhantRender() {
	if (phantSelect->currentIndex() < 1) 
		return;
		
	QString axis = xAxisButton->isChecked()?"x axis":(yAxisButton->isChecked()?"y axis":"z axis");
	double horMin = horBoundaryMin->text().toDouble(), horMax = horBoundaryMax->text().toDouble();
	double vertMin = vertBoundaryMin->text().toDouble(), vertMax = vertBoundaryMax->text().toDouble();
	double depth, res;
	
	if (horMin > horMax) {
		depth  = horMin;
		horMin = horMax;
		horMax = depth;
	}
	
	if (vertMin > vertMax) {
		depth   = vertMin;
		vertMin = vertMax;
		vertMax = depth;
	}

	depth = depthMin->text().toDouble();
	res   = resolutionScale->text().toDouble();
		
	if (mediaButton->isChecked()) {
		*phantPic = phant->getEGSPhantPicMed(axis, horMin, horMax, vertMin, vertMax, depth, res);
	}
	else if (densityButton->isChecked()) {
		*phantPic = phant->getEGSPhantPicDen(axis, horMin, horMax, vertMin, vertMax, depth, res,
											 densityMin->text().toDouble(),
											 densityMax->text().toDouble());
	}
	
	previewRenderLive();
}

void doseInterface::previewMapRenderLive() {if(renderCheckBox->isChecked()) previewMapRender();}
void doseInterface::previewMapRender() {
	if (mapDoseBox->currentIndex() < 1)
		return;
	
	QString axis = xAxisButton->isChecked()?"x axis":(yAxisButton->isChecked()?"y axis":"z axis");
	double horMin = horBoundaryMin->text().toDouble(), horMax = horBoundaryMax->text().toDouble();
	double vertMin = vertBoundaryMin->text().toDouble(), vertMax = vertBoundaryMax->text().toDouble();
	double depth, res;
	
	if (horMin > horMax) {
		depth  = horMin;
		horMin = horMax;
		horMax = depth;
	}
	
	if (vertMin > vertMax) {
		depth   = vertMin;
		vertMin = vertMax;
		vertMax = depth;
	}

	depth = depthMin->text().toDouble();
	res   = resolutionScale->text().toDouble();
	
	*mapPic = mapDose->getColourMap(axis, horMin, horMax, vertMin, vertMax, depth, res,
									mapMinDose->text().toDouble(), mapMaxDose->text().toDouble(),
									mapMinButton->palette().color(QPalette::Button),
									mapMidButton->palette().color(QPalette::Button),
									mapMaxButton->palette().color(QPalette::Button));
	
	previewRenderLive();
}

void doseInterface::previewIsoRenderLive() {if(renderCheckBox->isChecked()) previewIsoRender();}
void doseInterface::previewIsoRender() {
	if ((isoDoseBox[0]->currentIndex()+isoDoseBox[1]->currentIndex()+isoDoseBox[2]->currentIndex()) < 1)
		return;
	
	QString axis = xAxisButton->isChecked()?"X":(yAxisButton->isChecked()?"Y":"Z");
	double horMin = horBoundaryMin->text().toDouble(), horMax = horBoundaryMax->text().toDouble();
	double vertMin = vertBoundaryMin->text().toDouble(), vertMax = vertBoundaryMax->text().toDouble();
	double depth, res;
	
	if (horMin > horMax) {
		depth  = horMin;
		horMin = horMax;
		horMax = depth;
	}
	
	if (vertMin > vertMax) {
		depth   = vertMin;
		vertMin = vertMax;
		vertMax = depth;
	}

	depth = depthMin->text().toDouble();
	res   = resolutionScale->text().toDouble();
	
	// Fetch the contours
	QVector <QVector <QLineF> > contour;
	QVector <double> doses;
	
	for (int j = 0; j < 5; j++)
		doses.append(isoColourDose[j]->text().toDouble());
	
	if (isoDoseBox[0]->currentIndex())
		isoDoses[0]->getContour(&solid,  doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
	if (isoDoseBox[1]->currentIndex())
		isoDoses[1]->getContour(&dashed, doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
	if (isoDoseBox[2]->currentIndex())
		isoDoses[2]->getContour(&dotted, doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
	
	previewRenderLive();
}

void doseInterface::previewRenderLive() {if(renderCheckBox->isChecked()) previewRender();}
void doseInterface::previewRender() {
	*canvasPic = *blackPic;
	
	// Choose base canvas
	if (phantSelect->currentIndex()) // Not none
		*canvasPic = *phantPic;
	
	QPainter paint (canvasPic); // Now use it as our canvas
	
	// Add colour map
	paint.setOpacity(double(mapOpacSlider->value())/100.0);
	if (mapDoseBox->currentIndex()) // Not none
		paint.drawImage(0,0,*mapPic);
	paint.setOpacity(1);
		
	// Add isodose contours
    QPen pen;
    pen.setWidth(parent->data->isodoseLineThickness);
	
	for (int i = 0; i < 3; i++)
		if (isoDoseBox[i]->currentIndex()) {			
			// Setup correct pen
			switch (i) {
				case 0:
					pen.setStyle(Qt::SolidLine);
					for (int j = 0; j < 5; j++) {
						pen.setColor(isoColourButton[j]->palette().color(QPalette::Button));
						paint.setPen(pen);
						paint.drawLines(solid[j]);
					}
					break;
				case 1:
					pen.setStyle(Qt::DotLine);
					for (int j = 0; j < 5; j++) {
						pen.setColor(isoColourButton[j]->palette().color(QPalette::Button));
						paint.setPen(pen);
						paint.drawLines(dashed[j]);
					}
					break;
				case 2:
					pen.setStyle(Qt::DashLine);
					for (int j = 0; j < 5; j++) {
						pen.setColor(isoColourButton[j]->palette().color(QPalette::Button));
						paint.setPen(pen);
						paint.drawLines(dotted[j]);
					}
					break;
				default:
					// do nothing
					break;
			}
		}
		
    paint.end();	
		
	canvas->setPixmap(QPixmap::fromImage(*canvasPic));
    canvas->setFixedSize(canvasPic->width(), canvasPic->height());
    canvas->repaint();
}

void doseInterface::loadEgsphant() {
	int i = phantSelect->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local phantom count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirPhants[i]+parent->data->localNamePhants[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading egsphant file");
	connect(phant, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz"))
		phant->loadgzEGSPhantFile(file);
	else if (file.endsWith(".begsphant"))
		phant->loadbEGSPhantFile(file);
	else if (file.endsWith(".egsphant"))
		phant->loadEGSPhantFile(file);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	parent->finishedProgress();
	previewCanvasRenderLive();
}

void doseInterface::loadMapDose() {
	int i = mapDoseBox->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirDoses.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirDoses[i]+parent->data->localNameDoses[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading 3ddose file");
	connect(mapDose, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".b3ddose"))
		mapDose->readBIn(file, 1);
	else if (file.endsWith(".3ddose"))
		mapDose->readIn(file, 1);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type 3ddose or b3ddose.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	parent->finishedProgress();
	previewCanvasRenderLive();
}

void doseInterface::loadIsoDose(int i) {
	int j = isoDoseBox[i]->currentIndex()-1;
	if (j < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (j >= parent->data->localDirDoses.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirDoses[j]+parent->data->localNameDoses[j]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading 3ddose file");
	connect(isoDoses[i], SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".b3ddose"))
		isoDoses[i]->readBIn(file, 1);
	else if (file.endsWith(".3ddose"))
		isoDoses[i]->readIn(file, 1);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type 3ddose or b3ddose.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	parent->finishedProgress();
	previewCanvasRenderLive();
}

void doseInterface::previewResetBounds() {
	double xMin = -10, yMin = -10, zMin = -10, xMax = 10, yMax = 10, zMax = 10;
	bool isReset = false;
	
	if (0 < phantSelect->currentIndex()) {
		if (!isReset) {
			xMin = phant->x[0];
			yMin = phant->y[0];
			zMin = phant->z[0];
			
			xMax = phant->x.last();
			yMax = phant->y.last();
			zMax = phant->z.last();
			
			isReset = true;
		}
		else {
			xMin = xMin>phant->x[0]?phant->x[0]:xMin;
			yMin = yMin>phant->y[0]?phant->y[0]:yMin;
			zMin = zMin>phant->z[0]?phant->z[0]:zMin;
			
			xMax = xMax<phant->x.last()?phant->x.last():xMax;
			yMax = yMax<phant->y.last()?phant->y.last():yMax;
			zMax = zMax<phant->z.last()?phant->z.last():zMax;
		}
	}
	
	if (0 < mapDoseBox->currentIndex()) {
		if (!isReset) {
			xMin = mapDose->cx[0];
			yMin = mapDose->cy[0];
			zMin = mapDose->cz[0];
			
			xMax = mapDose->cx.last();
			yMax = mapDose->cy.last();
			zMax = mapDose->cz.last();
			
			isReset = true;
		}
		else {
			xMin = xMin>mapDose->cx[0]?mapDose->cx[0]:xMin;
			yMin = yMin>mapDose->cy[0]?mapDose->cy[0]:yMin;
			zMin = zMin>mapDose->cz[0]?mapDose->cz[0]:zMin;
			
			xMax = xMax<mapDose->cx.last()?mapDose->cx.last():xMax;
			yMax = yMax<mapDose->cy.last()?mapDose->cy.last():yMax;
			zMax = zMax<mapDose->cz.last()?mapDose->cz.last():zMax;
		}
	}

	for (int i = 0; i < 3; i++)
		if (0 < isoDoseBox[i]->currentIndex()) {
			if (!isReset) {
				xMin = isoDoses[i]->cx[0];
				yMin = isoDoses[i]->cy[0];
				zMin = isoDoses[i]->cz[0];
				
				xMax = isoDoses[i]->cx.last();
				yMax = isoDoses[i]->cy.last();
				zMax = isoDoses[i]->cz.last();
				
				isReset = true;
			}
			else {
				xMin = xMin>isoDoses[i]->cx[0]?isoDoses[i]->cx[0]:xMin;
				yMin = yMin>isoDoses[i]->cy[0]?isoDoses[i]->cy[0]:yMin;
				zMin = zMin>isoDoses[i]->cz[0]?isoDoses[i]->cz[0]:zMin;
				
				xMax = xMax<isoDoses[i]->cx.last()?isoDoses[i]->cx.last():xMax;
				yMax = yMax<isoDoses[i]->cy.last()?isoDoses[i]->cy.last():yMax;
				zMax = zMax<isoDoses[i]->cz.last()?isoDoses[i]->cz.last():zMax;
			}
		}
	
	if (xAxisButton->isChecked()) {
		vertBoundaryMin->setText(QString::number(yMin));
		vertBoundaryMax->setText(QString::number(yMax));
		horBoundaryMin->setText(QString::number(zMin));
		horBoundaryMax->setText(QString::number(zMax));
		
		if (depthMin->text().toDouble() < xMin || depthMin->text().toDouble() > xMax)
			depthMin->setText(QString::number((xMin+xMax)/2.0));		
	}
	else if (yAxisButton->isChecked()) {
		vertBoundaryMin->setText(QString::number(xMin));
		vertBoundaryMax->setText(QString::number(xMax));
		horBoundaryMin->setText(QString::number(zMin));
		horBoundaryMax->setText(QString::number(zMax));
		
		if (depthMin->text().toDouble() < yMin || depthMin->text().toDouble() > yMax)
			depthMin->setText(QString::number((yMin+yMax)/2.0));		
	}
	else if (zAxisButton->isChecked()) {
		vertBoundaryMin->setText(QString::number(xMin));
		vertBoundaryMax->setText(QString::number(xMax));
		horBoundaryMin->setText(QString::number(yMin));
		horBoundaryMax->setText(QString::number(yMax));
		
		if (depthMin->text().toDouble() < zMin || depthMin->text().toDouble() > zMax)
			depthMin->setText(QString::number((zMin+zMax)/2.0));	
	}
	
	previewCanvasRenderLive();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                               Histogram                             //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

void doseInterface::histoRefresh() {
}

void doseInterface::loadFilterEgsphant() {
	localNameMasks.clear();
	localDirMasks.clear();
	histMediumView->clear();
	histMaskSelect->clear();
	histMaskSelect->addItem("none");
	
	int i = histPhantSelect->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local phantom count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirPhants[i]+parent->data->localNamePhants[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading egsphant file");
	connect(histPhant, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz")) {
		histPhant->loadgzEGSPhantFilePlus(file);
		file = file.left(file.size()-12).split("/").last();
	}
	else if (file.endsWith(".begsphant")) {
		histPhant->loadbEGSPhantFilePlus(file);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".egsphant")) {
		histPhant->loadEGSPhantFilePlus(file);
		file = file.left(file.size()-9).split("/").last();
	}
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	// media data	
	histMediumView->clear();
	for (int i = 0; i < histPhant->media.size(); i++)
		histMediumView->addItem(histPhant->media[i]);
	
	// local mask data	
	localNameMasks.clear();
	localDirMasks.clear();
	
	QDirIterator files (parent->data->gui_location+"/database/mask/", {QString(file)+".*.egsphant.gz"},
						QDir::NoFilter, QDirIterator::Subdirectories);
	while(files.hasNext()) {
		files.next();
		localNameMasks << files.fileName();
		localDirMasks << files.path();
	}
	
	histMaskSelect->clear();
	histMaskSelect->addItem("none");
	for (int i = 0; i < localNameMasks.size(); i++)
		histMaskSelect->addItem(localNameMasks[i]);
	
	parent->finishedProgress();
}

void doseInterface::loadMaskEgsphant() {
	int i = histMaskSelect->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= localDirMasks.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected mask index is larger than the local phantom count.  Aborting"));		
		return;
	}
	
	QString file = localDirMasks[i]+localNameMasks[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading mask file");
	connect(histMask, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz")) {
		histMask->loadgzEGSPhantFilePlus(file);
		file = file.left(file.size()-12).split("/").last();
	}
	else if (file.endsWith(".begsphant")) {
		histMask->loadbEGSPhantFilePlus(file);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".egsphant")) {
		histMask->loadEGSPhantFilePlus(file);
		file = file.left(file.size()-9).split("/").last();
	}
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	parent->finishedProgress();
}
	
void doseInterface::loadHistoDose() {
	int i = histDoseSelect->currentIndex();	
	
	if (i >= histDoseSelect->count()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirDoses[i]+parent->data->localNameDoses[i]; // Get file location
	histDoses.append(new Dose());
	
	// Connect the progress bar
	parent->resetProgress("Loading dose file");
	connect(histDoses.last(), SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
	connect(histDoses.last(), SIGNAL(nameProgress(QString)),
			parent, SLOT(nameProgress(QString)));
		
	if (file.endsWith(".b3ddose")) {
		histDoses.last()->readBIn(file, 1);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".3ddose")) {
		histDoses.last()->readIn(file, 1);
		file = file.left(file.size()-9).split("/").last();
	}
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type b3ddose or 3ddose.  Aborting"));
		parent->finishedProgress();
		delete histDoses.last();
		histDoses.removeLast();
		
		return;		
	}
	
	parent->finishedProgress();
	
	// Add listing to loaded dose view
	if (file.endsWith(".b3ddose"))
		histLoadedView->addItem(parent->data->localNameDoses[i].left(parent->data->localNameDoses[i].size()-8));
	else if (file.endsWith(".3ddose"))
		histLoadedView->addItem(parent->data->localNameDoses[i].left(parent->data->localNameDoses[i].size()-7));
	else
		histLoadedView->addItem(parent->data->localNameDoses[i]);
}

void doseInterface::deleteHistoDose() {
	if (histLoadedView->selectedItems().size() != 1) {
		QMessageBox::warning(0, "Deletion error",
		tr("No doses selected.  Please select one to delete."));
		return;
	}
	
	int i = histLoadedView->currentRow();
	delete histDoses[i];
	histDoses.remove(i);
	delete histLoadedView->currentItem();
}

void doseInterface::histoRenderLive() {if(renderCheckBox->isChecked()) histoRender();}
void doseInterface::histoRender() {
	// Return
	if (histDoses.size() == 0) {
		return;
	}
	
	// Check what filters we have
	int filterInfo = 0;
	if (histMaskSelect->currentIndex())
		filterInfo += 1;
	
	QList <QListWidgetItem*> selectedMedia = histMediumView->selectedItems();
	QString allowedMedia = "", allMedia = EGSPHANT_CHARS;
	if (selectedMedia.size()) {
		filterInfo += 2;
		for (int i = 0; i < selectedMedia.size(); i++)
			allowedMedia += allMedia[histMediumView->row(selectedMedia[i])];
	}
	
	double minDose = histDoseMinEdit->text().toDouble(), maxDose = histDoseMaxEdit->text().toDouble();
	if (minDose || maxDose) {
		filterInfo += 4;
	}
	
	// Get sorted dose arrays
	parent->resetProgress("Creating DVH");
			
	QVector <DV> data;
	double volume;
	int count = histDoses.size();
	QVector <QLineSeries*> series;
	QChart* plot = new QChart();
	double increment = 5.0/double(count); // 5% for making plot data, 95% defined implicitly in getDV
	
	// Reset functions holding all the data for later output
	savePlotName.clear();
	savePlotX = savePlotY = "";
	savePlotData.clear();
	QList<QPointF> tempData;
	
	for (int i = 0; i < count; i++) {				
		parent->nameProgress("Filtering data");
		switch (filterInfo) {
			case 1: // Mask with no media
				histDoses[i]->getDV(&data, histMask, &volume, count);
				break;
			case 2: // Media with no mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, count);
				break;
			case 3: // Media and mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, count);
				break;
			case 4: // Dose ranges
				histDoses[i]->getDV(&data, &volume, minDose, maxDose, count);
				break;
			case 5: // Mask with no media and dose ranges
				histDoses[i]->getDV(&data, histMask, &volume, minDose, maxDose, count);
				break;
			case 6: // Media with no mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, minDose, maxDose, count);
				break;
			case 7: // Media and mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, minDose, maxDose, count);
				break;
			default: // #nofilter #nomakeup
				histDoses[i]->getDV(&data, &volume, count);
				break;
		}
		
		// Generate series data
		tempData.clear();
		if (histDiffBox->isChecked()) {
			parent->nameProgress("Building bins arrays");
			series.append(new QLineSeries());
			series.last()->setName(histLoadedView->item(i)->text());
			savePlotName.append(histLoadedView->item(i)->text());
			int binCount = parent->data->histogramBinCount;
			
			// Bin count, maybe make a configuration file option?
			double sInc = (data.last().dose-data[0].dose)/double(binCount);
			double s0 = data[0].dose;
			
			double prev = s0, cur = s0;
			series.last()->append(data[0].dose, 0);
			
			double subIncrement = increment/double(binCount);
			int dataIndex = 0, dataCount = 0;
			for (int i = 1; i <= binCount; i++) {
				cur = s0+sInc*i;
				
				dataCount = 0;
				while (dataIndex < data.size()) {
					if (data[dataIndex].dose > cur)
						break;
					dataCount++;
					dataIndex++;
				}
				
				series.last()->append(prev, dataCount);
				series.last()->append(cur, dataCount);
				
				tempData.append(QPointF((cur+prev)/2.0,double(dataCount)));
				
				prev = cur;
				parent->updateProgress(subIncrement);
			}
			series.last()->append(cur, 0);
			
			plot->addSeries(series.last());
		}
		else {
			parent->nameProgress("Building plot line arrays");
			series.append(new QLineSeries());
			series.last()->setName(histLoadedView->item(i)->text());
			savePlotName.append(histLoadedView->item(i)->text());
						
			int sInc = 1;
			
			// Drop every (n-1)th point, where n is the multiple of full 200s in the data set
			if (data.size() > 200) {
				sInc = data.size()/200.0;
			}
			
			double subIncrement = increment/double(data.size());
				
			for (int i = 0; i < data.size(); i++) {
				if (!(i%sInc)) {// Only add up to 399 points (start skipping at 400+)
					series.last()->append(data[i].dose, 100.0*double(data.size()-i)/double(data.size()));
					tempData.append(QPointF(data[i].dose, 100.0*double(data.size()-i)/double(data.size())));
				}
				parent->updateProgress(subIncrement);
			}
			
			if ((data.size()%sInc)) {// Add end point if it would be skipped
				series.last()->append(data.last().dose, 100.0/double(data.size()));
				tempData.append(QPointF(data.last().dose, 100.0/double(data.size())));
			}
			
			plot->addSeries(series.last());
		}
		savePlotData.append(tempData);
	}
	
	// Fill out plot parameters
	plot->createDefaultAxes();
	plot->axes()[0]->setTitleText("dose / Gy");
	savePlotX = "dose / Gy";
	plot->axes()[0]->setRange(data[0].dose,data.last().dose);
	
	if (histDiffBox->isChecked()) {
		plot->setTitle("Dose Differential Histogram");
		plot->axes()[1]->setTitleText("voxel count");
		savePlotY = "voxel count";
	}
	else {
		plot->setTitle("Dose Volume Histogram");
		plot->axes()[1]->setTitleText("% of total volume");
		plot->axes()[1]->setRange(0,100);
		savePlotY = "% of total volume";
	}
	
	if (!histLegendBox->isChecked()) // Hide legend if it's unwanted
		plot->legend()->hide();
	
	canvasChart->setChart(plot);
	canvasChart->resize(800,600);
    canvasArea->repaint();
	parent->finishedProgress();
}

void doseInterface::calcMetrics() {
	// Return
	if (histDoses.size() == 0) {
		return;
	}
	
	// Check what filters we have
	int filterInfo = 0;
	if (histMaskSelect->currentIndex())
		filterInfo += 1;
	
	QList <QListWidgetItem*> selectedMedia = histMediumView->selectedItems();
	QString allowedMedia = "", allMedia = EGSPHANT_CHARS;
	if (selectedMedia.size()) {
		filterInfo += 2;
		for (int i = 0; i < selectedMedia.size(); i++)
			allowedMedia += allMedia[histMediumView->row(selectedMedia[i])];
	}
	
	double minDose = histDoseMinEdit->text().toDouble(), maxDose = histDoseMaxEdit->text().toDouble();
	if (minDose || maxDose) {
		filterInfo += 4;
	}
	
	// Get sorted dose arrays
	parent->resetProgress("Calculating metrics");
			
	QVector <DV> data;
	double volume;
	int count = histDoses.size();
	
	// Metrics to extract
	QString names, units, average, uncertainty, voxels, volumes;
	QStringList Dx, Vx, temp;
	QVector <double> xD, xV;
	temp = histDxEdit->text().replace(' ',',').split(',');
	for (int i = 0; i < temp.size(); i++) {
		xD.append(temp[i].toDouble());
		Dx.append("");
	}
	
	temp = histVxEdit->text().replace(' ',',').split(',');
	for (int i = 0; i < temp.size(); i++) {
		xV.append(temp[i].toDouble());
		Vx.append("");
	}
	
	// Get dose values	
	for (int i = 0; i < count; i++) {
		parent->nameProgress("Filtering data");
		data.clear(); volume = 0;
		switch (filterInfo) {
			case 1: // Mask with no media
				histDoses[i]->getDV(&data, histMask, &volume, count);
				break;
			case 2: // Media with no mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, count);
				break;
			case 3: // Media and mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, count);
				break;
			case 4: // Dose ranges
				histDoses[i]->getDV(&data, &volume, minDose, maxDose, count);
				break;
			case 5: // Mask with no media and dose ranges
				histDoses[i]->getDV(&data, histMask, &volume, minDose, maxDose, count);
				break;
			case 6: // Media with no mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, minDose, maxDose, count);
				break;
			case 7: // Media and mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, minDose, maxDose, count);
				break;
			default: // #nofilter #nomakeup
				histDoses[i]->getDV(&data, &volume, count);
				break;
		}
		
		parent->nameProgress("Extracting metrics");
		
		// Generate metric data
		double volumeTally = 0, doseTally = 0, doseTallyErr = 0, doseTallyErr2 = 0, countTally = 0;
		int vIndex = 0, dIndex = 0;
		for (int j = 0; j < data.size(); j++) {
			countTally    += 1.0;
			volumeTally   += data[j].vol;
			doseTally     += data[j].dose;
			doseTallyErr  += data[j].dose*data[j].err;
			doseTallyErr2 += (data[j].dose*data[j].err)*(data[j].dose*data[j].err);
			
			// Append Vx values as we go
			if (vIndex < xV.size()) {
				if (data[j].dose > xV[vIndex] && j) {
					Vx[vIndex] += QString::number(volumeTally-data[j].vol).left(11).rightJustified(11,' ')+" "
							   +  "            |";
					vIndex++;
				}
			}
			
			// Append Dx values as we go
			if (dIndex < xD.size()) {
				if (volumeTally/volume*100.0 > xD[dIndex] && j) {
					Dx[dIndex] += QString::number(data[j-1].dose).left(11).rightJustified(11,' ')+" "
							   +  QString::number(data[j-1].err).left(11).rightJustified(11,' ')+" |";
					dIndex++;
				}
			}
		}
		
		for (int j = vIndex; j < xV.size(); j++) 
			Vx[j] += "        n/a         n/a |";
		
		for (int j = dIndex; j < xD.size(); j++) 
			Dx[j] += "        n/a         n/a |";
		
		// Calculate global metrics
		doseTally     /= countTally; // Average dose
		doseTallyErr  /= countTally; // Average uncertainty
		doseTallyErr2 = sqrt(doseTallyErr2/countTally); // Propagated average dose uncertainty
		
		names       += histLoadedView->item(i)->text().left(23).rightJustified(23,' ')+" |";
		units       += "   value    uncertainty |";
		average     += QString::number(doseTally).left(11).rightJustified(11,' ')+" "
		            +QString::number(doseTallyErr2).left(11).rightJustified(11,' ')+" |";
		uncertainty += QString::number(doseTallyErr).left(11).rightJustified(11,' ')+"             |";
		voxels      += QString::number(countTally).left(11).rightJustified(11,' ')+"             |";
		volumes     += QString::number(volume).left(11).rightJustified(11,' ')+"             |";
	}
	
	QString text = QString("Dataset").left(24).rightJustified(24,' ')+"|"+names+"\n";
	text        += QString(" ").left(24).rightJustified(24,' ')+" "+units+"\n";
	text        += QString("Average dose / Gy").left(24).rightJustified(24,' ')+"|"+average+"\n";
	text        += QString("Average uncertainty / Gy").left(24).rightJustified(24,' ')+"|"+uncertainty+"\n";
	text        += QString("Number of voxels").left(24).rightJustified(24,' ')+"|"+voxels+"\n";
	text        += QString("Total volume / cm^3").left(24).rightJustified(24,' ')+"|"+volumes+"\n";
	
	for (int i = 0; i < Dx.size(); i++)
		text += (QString("D")+QString::number(xD[i])+" / Gy").left(24).rightJustified(24,' ')+"|"+Dx[i]+"\n";
	
	for (int i = 0; i < Vx.size(); i++)
		text += (QString("V")+QString::number(xV[i])+" / cm^3").left(24).rightJustified(24,' ')+"|"+Vx[i]+"\n";
	
	parent->finishedProgress();
	
	// Show the log
	log->outputArea->clear();
	log->outputArea->setPlainText(text);
	log->show();
}

void doseInterface::outputMetrics() {
	// Return
	if (histDoses.size() == 0) {
		return;
	}
	
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Images (*.csv)"));
	
	if (filePath.length() < 1) // No name selected
		return;
	
	if (!filePath.endsWith(".csv")) // Doesn't have the right extension
		filePath += ".csv";
	
	QString text = "";
	
	// Check what filters we have
	int filterInfo = 0;
	if (histMaskSelect->currentIndex()) {
		filterInfo += 1;
		text += QString("Data filtered to only include doses within contour ")+histMaskSelect->currentText()+"\n";
	}
	
	QList <QListWidgetItem*> selectedMedia = histMediumView->selectedItems();
	QString allowedMedia = "", allMedia = EGSPHANT_CHARS;
	if (selectedMedia.size()) {
		filterInfo += 2;
		text += QString("Data filtered to only include doses within ")+histPhantSelect->currentText()+" phantom voxels containing:,";
		for (int i = 0; i < selectedMedia.size(); i++) {
			allowedMedia += allMedia[histMediumView->row(selectedMedia[i])];
			text += selectedMedia[i]->text()+",";
		}
		text += QString("\n");
	}
	
	double minDose = histDoseMinEdit->text().toDouble(), maxDose = histDoseMaxEdit->text().toDouble();
	if (minDose || maxDose) {
		text += QString("Data filtered to only include doses within range ")+QString::number(minDose)
		     +  " and "+((minDose < maxDose)?QString::number(maxDose):QString("infinity"))+"\n";
		filterInfo += 4;
	}
	
	if (filterInfo)
		text += QString("\n");
	
	// Get sorted dose arrays
	parent->resetProgress("Calculating metrics");
			
	QVector <DV> data;
	double volume;
	int count = histDoses.size();
	
	// Metrics to extract
	QString names, units, average, uncertainty, voxels, volumes;
	QStringList Dx, Vx, temp;
	QVector <double> xD, xV;
	temp = histDxEdit->text().replace(' ',',').split(',');
	for (int i = 0; i < temp.size(); i++) {
		xD.append(temp[i].toDouble());
		Dx.append("");
	}
	
	temp = histVxEdit->text().replace(' ',',').split(',');
	for (int i = 0; i < temp.size(); i++) {
		xV.append(temp[i].toDouble());
		Vx.append("");
	}
	
	// Get dose values	
	for (int i = 0; i < count; i++) {
		parent->nameProgress("Filtering data");
		data.clear(); volume = 0;
		switch (filterInfo) {
			case 1: // Mask with no media
				histDoses[i]->getDV(&data, histMask, &volume, count);
				break;
			case 2: // Media with no mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, count);
				break;
			case 3: // Media and mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, count);
				break;
			case 4: // Dose ranges
				histDoses[i]->getDV(&data, &volume, minDose, maxDose, count);
				break;
			case 5: // Mask with no media and dose ranges
				histDoses[i]->getDV(&data, histMask, &volume, minDose, maxDose, count);
				break;
			case 6: // Media with no mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, minDose, maxDose, count);
				break;
			case 7: // Media and mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, minDose, maxDose, count);
				break;
			default: // #nofilter #nomakeup
				histDoses[i]->getDV(&data, &volume, count);
				break;
		}
		
		parent->nameProgress("Extracting metrics");
		
		// Generate metric data
		double volumeTally = 0, doseTally = 0, doseTallyErr = 0, doseTallyErr2 = 0, countTally = 0;
		int vIndex = 0, dIndex = 0;
		for (int j = 0; j < data.size(); j++) {
			countTally    += 1.0;
			volumeTally   += data[j].vol;
			doseTally     += data[j].dose;
			doseTallyErr  += data[j].dose*data[j].err;
			doseTallyErr2 += (data[j].dose*data[j].err)*(data[j].dose*data[j].err);
			
			// Append Vx values as we go
			if (vIndex < xV.size()) {
				if (data[j].dose > xV[vIndex] && j) {
					Vx[vIndex] += QString::number(volumeTally-data[j].vol)+",,";
					vIndex++;
				}
			}
			
			// Append Dx values as we go
			if (dIndex < xD.size()) {
				if (volumeTally/volume*100.0 > xD[dIndex] && j) {
					Dx[dIndex] += QString::number(data[j-1].dose)+","+QString::number(data[j-1].err)+",";
					dIndex++;
				}
			}
		}
		
		for (int j = vIndex; j < xV.size(); j++) 
			Vx[j] += "n/a,n/a,";
		
		for (int j = dIndex; j < xD.size(); j++) 
			Dx[j] += "n/a,n/a,";
		
		// Calculate global metrics
		doseTally     /= countTally; // Average dose
		doseTallyErr  /= countTally; // Average uncertainty
		doseTallyErr2 = sqrt(doseTallyErr2/countTally); // Propagated average dose uncertainty
		
		names       += histLoadedView->item(i)->text()+",,";
		units       += "value,uncertainty,";
		average     += QString::number(doseTally)+","+QString::number(doseTallyErr2)+",";
		uncertainty += QString::number(doseTallyErr)+",,";
		voxels      += QString::number(countTally)+",,";
		volumes     += QString::number(volume)+",,";
	}
	
	text        += QString("Dataset,")+names+"\n";
	text        += QString(",")+units+"\n";
	text        += QString("Average dose / Gy,")+average+"\n";
	text        += QString("Average uncertainty / Gy,")+uncertainty+"\n";
	text        += QString("Number of voxels,")+voxels+"\n";
	text        += QString("Total volume / cm^3,")+volumes+"\n";
	
	for (int i = 0; i < Dx.size(); i++)
		text += QString("D")+QString::number(xD[i])+" / Gy,"+Dx[i]+"\n";
	
	for (int i = 0; i < Vx.size(); i++)
		text += QString("V")+QString::number(xV[i])+" / cm^3,"+Vx[i]+"\n";
	
	QFile metricFile(filePath);
	
	if (!metricFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "Metric file error",
		tr("Failed to open the file for metric output.  Aborting"));	
		return;
	}
	
	QTextStream out(&metricFile);
	out << text;
	metricFile.close();
	
	parent->finishedProgress();
}

void doseInterface::outputRawData() {// Return
	if (histDoses.size() == 0) {
		return;
	}
	
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("Images (*.csv)"));
	
	if (filePath.length() < 1) // No name selected
		return;
	
	if (!filePath.endsWith(".csv")) // Doesn't have the right extension
		filePath += ".csv";
	
	QString text = "";
	
	// Check what filters we have
	int filterInfo = 0;
	if (histMaskSelect->currentIndex()) {
		filterInfo += 1;
		text += QString("Data filtered to only include doses within contour ")+histMaskSelect->currentText()+"\n";
	}
	
	QList <QListWidgetItem*> selectedMedia = histMediumView->selectedItems();
	QString allowedMedia = "", allMedia = EGSPHANT_CHARS;
	if (selectedMedia.size()) {
		filterInfo += 2;
		text += QString("Data filtered to only include doses within ")+histPhantSelect->currentText()+" phantom voxels containing:,";
		for (int i = 0; i < selectedMedia.size(); i++) {
			allowedMedia += allMedia[histMediumView->row(selectedMedia[i])];
			text += selectedMedia[i]->text()+",";
		}
		text += QString("\n");
	}
	
	double minDose = histDoseMinEdit->text().toDouble(), maxDose = histDoseMaxEdit->text().toDouble();
	if (minDose || maxDose) {
		text += QString("Data filtered to only include doses within range ")+QString::number(minDose)
		     +  " and "+((minDose < maxDose)?QString::number(maxDose):QString("infinity"))+"\n";
		filterInfo += 4;
	}
	
	if (filterInfo)
		text += QString("\n");
	
	// Get sorted dose arrays
	parent->resetProgress("Outputting raw data");
	
	QVector <DV> data;
	QVector <QStringList> dataColumns;
	int count = histDoses.size();
	double volume;
	dataColumns.resize(count);
	QString names = "";
	
	// Get dose values	
	for (int i = 0; i < count; i++) {
		parent->nameProgress("Filtering data");
		data.clear(); volume = 0;
		switch (filterInfo) {
			case 1: // Mask with no media
				histDoses[i]->getDV(&data, histMask, &volume, count);
				break;
			case 2: // Media with no mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, count);
				break;
			case 3: // Media and mask
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, count);
				break;
			case 4: // Dose ranges
				histDoses[i]->getDV(&data, &volume, minDose, maxDose, count);
				break;
			case 5: // Mask with no media and dose ranges
				histDoses[i]->getDV(&data, histMask, &volume, minDose, maxDose, count);
				break;
			case 6: // Media with no mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, &volume, minDose, maxDose, count);
				break;
			case 7: // Media and mask and dose ranges
				histDoses[i]->getDV(&data, histPhant, allowedMedia, histMask, &volume, minDose, maxDose, count);
				break;
			default: // #nofilter #nomakeup
				histDoses[i]->getDV(&data, &volume, count);
				break;
		}
		
		parent->nameProgress("Building raw output");
		
		// Generate metric data
		names += histLoadedView->item(i)->text()+",,,";
		
		dataColumns[i].clear();
		for (int j = 0; j < data.size(); j++) {
			dataColumns[i]        << QString::number(data[j].dose)+",";
			dataColumns[i].last() += QString::number(data[j].err)+",";
			dataColumns[i].last() += QString::number(data[j].vol)+",";		
		}
	}
	
	text += names + "\n";
	count = 0;
	
	for (int i = 0; i < dataColumns.size(); i++) {
		text += "Dose / Gy,";
		text += "Uncertainty / Gy,";
		text += "Volume / cm^3,";	
		count = (count<dataColumns[i].size()?dataColumns[i].size():count);
	}
	text += "\n";
	
	// Build columns (and fill empty indices with nothing in case there are different dose counts
	for (int i = 0; i < count; i++) {
		for (int j = 0; j < dataColumns.size(); j++) {
			if (dataColumns[j].size() > i)
				text += dataColumns[j][i];
			else
				text += ",,,";
		}
		text += "\n";
	}
	
	QFile dataFile(filePath);
	
	if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(0, "Raw data file error",
		tr("Failed to open the file for raw data output.  Aborting"));	
		return;
	}
	
	QTextStream out(&dataFile);
	out << text;
	dataFile.close();
	
	parent->finishedProgress();	
}
	
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                                Profile                              //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

void doseInterface::profileRefresh() {
}

void doseInterface::profileRenderLive() {if(renderCheckBox->isChecked()) profileRender();}
void doseInterface::profileRender() {
	
}