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
	
	delete histPhant;
	delete histMask;
	
	for (int i = 0; i < histDoses.size(); i++)
		delete histDoses[i];
	
	for (int i = 0; i < profDoses.size(); i++)
		delete profDoses[i];
	
	delete bufferLayout;
	delete log;
}

// Layout Settings
void doseInterface::createLayout() {
	mainLayout = new QGridLayout();
	bufferLayout = new QGridLayout();
	QString ttt = ""; // tool tip text
	
	// Shared objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //				    
	canvasArea      = new QScrollArea();
	canvas          = new HoverLabel();
	canvasChart     = new QChartView();
	canvasPic       = new QImage();
	saveDataButton  = new QPushButton("Save data");
	saveDataButton->setDisabled(true);
	ttt = tr("Save plot data into csv file.");
	saveDataButton->setToolTip(ttt);
	saveImageButton = new QPushButton("Save image");
	canvasInfo      = new QLabel("");
	ttt = tr("Save image data into png file.");
	saveImageButton->setToolTip(ttt);
	
	canvasArea->setWidget(canvas);
				    
	rendering       = new QFrame();
	renderingLayout = new QGridLayout();
	renderButton    = new QPushButton("Render");
	ttt = tr("Generate image or plot using the above parameters.");
	rendering->setToolTip(ttt);
	renderCheckBox  = new QCheckBox("Live render");
	ttt = tr("Regenerate image automatically when changing any image parameters\n(can be intensive at high resolutions).");
	renderCheckBox->setToolTip(ttt);
	resolutionLabel = new QLabel("Pixels per cm");
	resolutionScale = new QLineEdit("20");
	ttt = tr("Change image resolution, lower renders faster, higher produces more high quality images.");
	resolutionLabel->setToolTip(ttt);
	resolutionScale->setToolTip(ttt);
	
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
	ttt = tr("The axis along which to slice to generate the images.");
	xAxisButton->setToolTip(ttt);
	yAxisButton->setToolTip(ttt);
	zAxisButton->setToolTip(ttt);
	
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
	ttt = tr("The vertical and horizontal bounds used to generate the image.\nAreas outside of egsphant/3ddose bounds are black/not rendered.");
	vertBoundaryLabel->setToolTip(ttt);
	vertBoundaryMin->setToolTip(ttt);
	vertBoundaryMax->setToolTip(ttt);
	horBoundaryLabel->setToolTip(ttt);
	horBoundaryMin->setToolTip(ttt);
	horBoundaryMax->setToolTip(ttt);
	
	depthLabel        = new QLabel("z depth");
	depthMin          = new QLineEdit("0");
	depthPlusButton   = new QPushButton();
	depthPlusButton->setIcon(this->style()->standardIcon(QStyle::SP_ArrowForward));
	depthMinusButton  = new QPushButton();
	depthMinusButton->setIcon(this->style()->standardIcon(QStyle::SP_ArrowBack));
	depthMin->setValidator(&allowedReals);
	ttt = tr("The depth at which to slice selected egsphant and 3ddoses.");
	depthLabel->setToolTip(ttt);
	depthMin->setToolTip(ttt);
	depthPlusButton->setToolTip(ttt);
	depthMinusButton->setToolTip(ttt);
	
	expandToBounds    = new QPushButton("reset");
	ttt = tr("Find the nearest depth at which any egsphant or 3ddose data is found, then scale vertical "
			 "and horizontal bounds to maximum size for all 3D arrays.");
	expandToBounds->setToolTip(ttt);
	
	legendLabel       = new QLabel("legend");
	legendBox         = new QComboBox();
	legendBox->addItem("none");
	legendBox->addItem("VPM");
	legendBox->addItem("colour map");
	legendBox->addItem("isodose");
	ttt = tr("Add legend to image, and select whether it displays VPM, colour map, or isodose data.");
	legendLabel->setToolTip(ttt);
	legendBox->setToolTip(ttt);
	
	unitsLabel        = new QLabel("Units");
	unitsEdit         = new QLineEdit("Gy");
	ttt = tr("Units to display on legend.");
	unitsLabel->setToolTip(ttt);
	unitsEdit->setToolTip(ttt);
	
	int width  = abs(horBoundaryMax->text().toDouble()-horBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	int height = abs(vertBoundaryMax->text().toDouble()-vertBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	blackPic = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
	blackPic->fill(qRgb(0,0,0));
	
	dimLayout->addWidget(xAxisButton      , 0,  0, 1, 6);
	dimLayout->addWidget(yAxisButton      , 0,  6, 1, 6);
	dimLayout->addWidget(zAxisButton      , 0, 12, 1, 6);
	dimLayout->addWidget(vertBoundaryLabel, 1,  0, 1, 6);
	dimLayout->addWidget(vertBoundaryMin  , 2,  0, 1, 6);
	dimLayout->addWidget(vertBoundaryMax  , 3,  0, 1, 6);
	dimLayout->addWidget(horBoundaryLabel , 1,  6, 1, 6);
	dimLayout->addWidget(horBoundaryMin   , 2,  6, 1, 6);
	dimLayout->addWidget(horBoundaryMax   , 3,  6, 1, 6);
	dimLayout->addWidget(depthLabel       , 1, 12, 1, 6);
	dimLayout->addWidget(depthMinusButton , 2, 12, 1, 1);
	dimLayout->addWidget(depthMin         , 2, 13, 1, 4);
	dimLayout->addWidget(depthPlusButton  , 2, 17, 1, 1);
	dimLayout->addWidget(expandToBounds   , 3, 12, 2, 6);
	dimLayout->addWidget(legendLabel      , 5,  0, 1, 9);
	dimLayout->addWidget(legendBox        , 6,  0, 1, 9);
	dimLayout->addWidget(unitsLabel       , 5,  9, 1, 9);
	dimLayout->addWidget(unitsEdit        , 6,  9, 1, 9);
	
	for (int i = 0; i < 18; i++)
		dimLayout->setColumnStretch(i, 5);
	
	dimFrame->setLayout(dimLayout);
	dimFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(dimFrame);
	
	// Phantom selection
	phantPic      = new QImage(width,height,QImage::Format_ARGB32_Premultiplied);
	phant         = new EGSPhant();
	
	phantFrame    = new QFrame();
	phantLayout   = new QGridLayout();
	phantLabel    = new QLabel("VPM");
	
	mediaButton   = new QRadioButton("media");
	densityButton = new QRadioButton("density");
	ttt = tr("Generate image based on egsphant media assignment or density.");
	mediaButton->setToolTip(ttt);
	densityButton->setToolTip(ttt);
	
	phantSelect   = new QComboBox();
	phantSelect->addItem("none");
	phantSelect->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	ttt = tr("Egsphant to use to generate images.");
	phantSelect->setToolTip(ttt);
	
	densityLabel  = new QLabel("Density range");
	densityMin    = new QLineEdit("0");
	densityMax    = new QLineEdit("3");
	densityMin->setValidator(&allowedPosReals);
	densityMax->setValidator(&allowedPosReals);
	ttt = tr("The minimum (black below) and maximum (white above) thresholds used\nto generate density images.");
	densityLabel->setToolTip(ttt);
	densityMin->setToolTip(ttt);
	densityMax->setToolTip(ttt);
	
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
	ttt = tr("The 3ddose file used to generate the colour map.");
	mapDoseBox->setToolTip(ttt);
	
	mapMinLabel   = new QLabel("min");
	mapMaxLabel   = new QLabel("max");
	
	mapMinDose    = new QLineEdit("20");
	mapMaxDose    = new QLineEdit("100");
	mapMinDose->setValidator(&allowedPosReals);
	mapMaxDose->setValidator(&allowedPosReals);
	ttt = tr("The minimum and maximum values used to generate the linear colour map.");
	mapMinLabel->setToolTip(ttt);
	mapMaxLabel->setToolTip(ttt);
	mapMinDose->setToolTip(ttt);
	mapMaxDose->setToolTip(ttt);
	
	mapMinButton  = new QPushButton();
	mapMidButton  = new QPushButton();
	mapMaxButton  = new QPushButton();
	mapMinButton->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	mapMidButton->setStyleSheet("QPushButton {background-color: rgb(0,255,0)}");
	mapMaxButton->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	ttt = tr("The minimum and maximum colours used for the colour map, as well as a midpoint colour "
			 "for a richer colour spectrum and potentially closer comparisons to isodose contours.");
	mapMinButton->setToolTip(ttt);
	mapMidButton->setToolTip(ttt);
	mapMaxButton->setToolTip(ttt);
	
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
	ttt = tr("The 3ddose files used to generate the solid, dashed, or dotted lines."
			 "  Different line types are much easier to distinguish at higher resolutions.");
	isoDoseLabel.append(new QLabel("solid line")); isoDoseBox.append(new QComboBox());
	isoDoseLabel.last()->setToolTip(ttt); isoDoseBox.last()->setToolTip(ttt);
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	isoDoseLabel.append(new QLabel("dashed line")); isoDoseBox.append(new QComboBox());
	isoDoseLabel.last()->setToolTip(ttt); isoDoseBox.last()->setToolTip(ttt);
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	isoDoseLabel.append(new QLabel("dotted line")); isoDoseBox.append(new QComboBox());
	isoDoseLabel.last()->setToolTip(ttt); isoDoseBox.last()->setToolTip(ttt);
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseBox.last()->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
	
	// Colors 1 - 5
	ttt = tr("The dose value and associated colour used to generate the contours.");
	isoColourDose.append(new QLineEdit("20")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	isoColourDose.last()->setToolTip(ttt);isoColourButton.last()->setToolTip(ttt);
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("40")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,255,255)}");
	isoColourDose.last()->setToolTip(ttt);isoColourButton.last()->setToolTip(ttt);
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("60")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(0,255,0)}");
	isoColourDose.last()->setToolTip(ttt);isoColourButton.last()->setToolTip(ttt);
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("80")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(255,255,0)}");
	isoColourDose.last()->setToolTip(ttt);isoColourButton.last()->setToolTip(ttt);
	isoColourDose.last()->setValidator(&allowedPosReals);
	isoColourDose.append(new QLineEdit("100")); isoColourButton.append(new QPushButton()); isoColourButton.last()->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	isoColourDose.last()->setToolTip(ttt);isoColourButton.last()->setToolTip(ttt);
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
	//isoFrame->setDisabled(true); // Fix/reimplement getContour in dose class
	previewLayout->addWidget(isoFrame);
	
	// Histogram ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Phantom selection
	histPhantLabel  = new QLabel("Virtual patient model");
	histPhant       = new EGSPhant();
	histPhantSelect = new QComboBox();
	histPhantSelect->addItem("none");
	ttt = tr("The VPM used to select structures and media for filtering dose data.");
	histPhantLabel->setToolTip(ttt);
	histPhantSelect->setToolTip(ttt);
	
	histPhantFrame  = new QFrame();
	histPhantLayout = new QGridLayout();
	
	histPhantLayout->addWidget(histPhantLabel , 0, 0, 1, 1);
	histPhantLayout->addWidget(histPhantSelect, 1, 0, 1, 1);
	
	histPhantFrame->setLayout(histPhantLayout);
	histPhantFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histPhantFrame);
	
	// Filters
	histFilterLabel  = new QLabel("Filters");
	
	histMaskLabel    = new QLabel("Structure");
	histMaskSelect   = new QComboBox();
	histMaskSelect->addItem("none");
	histMask         = new EGSPhant();
	ttt = tr("Ignore all dose data not within the selected structure.  This list "
			 "is populated when generating the egsphant using the metrics option.");
	histMaskLabel->setToolTip(ttt);
	histMaskSelect->setToolTip(ttt);
	
	histMediumLabel  = new QLabel("Media");
	histMediumView   = new QListWidget();
	histMediumView->setSelectionMode(QAbstractItemView::MultiSelection);
	ttt = tr("Ignore all dose data not within the selected media.");
	histMediumLabel->setToolTip(ttt);
	histMediumView->setToolTip(ttt);
	
	histDoseMinLabel = new QLabel("Min dose (Gy)");
	histDoseMinEdit  = new QLineEdit("0");
	histDoseMinEdit->setValidator(&allowedPosReals);
	histDoseMaxLabel = new QLabel("Max dose (Gy)");
	histDoseMaxEdit  = new QLineEdit("0");
	histDoseMaxEdit->setValidator(&allowedPosReals);
	ttt = tr("Ignore all dose data not within the selected dose thresholds. "
			 "If max dose is less than or equal to min dose, it is ignored.");
	histDoseMinLabel->setToolTip(ttt);
	histDoseMaxLabel->setToolTip(ttt);
	histDoseMinEdit->setToolTip(ttt);
	histDoseMaxEdit->setToolTip(ttt);
	
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
	ttt = tr("Load doses to use to generate histograms.");
	histDosesLabel->setToolTip(ttt);
	histLoadButton->setToolTip(ttt);
	histDoseSelect->setToolTip(ttt);
	
	histDeleteButton = new QPushButton("Delete");
	ttt = tr("Delete loaded dose.");
	histDeleteButton->setToolTip(ttt);
	
	histLoadedView   = new QListWidget();
	ttt = tr("List of loaded doses.");
	histDeleteButton->setToolTip(ttt);
					 
	histLegendBox    = new QCheckBox("Legend");
	histDiffBox      = new QCheckBox("Differential");
	histLegendBox->setChecked(true);
	ttt = tr("Add legend to plot.");
	histLegendBox->setToolTip(ttt);
	ttt = tr("Generate a non-cumulative histogram of doses.  Bin count is adjustable in the configuration file.");
	histDiffBox->setToolTip(ttt);
					 
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
	for (int i = 0; i < parent->data->metricNames.size(); i++)
		histOutputBox->addItem(parent->data->metricNames[i]);
				
	histDxLabel      = new QLabel("Dx (%)");
	histDxEdit       = new QLineEdit();
	histDccLabel     = new QLabel("Dx (cc)");
	histDccEdit      = new QLineEdit();
	histVxLabel      = new QLabel("Vx");
	histVxEdit       = new QLineEdit();
	histDpLabel      = new QLabel("Prescription (Gy)");
	histDpEdit       = new QLineEdit();
	
	if (parent->data->metricNames.size() > 1) {
		histDxEdit->setText(parent->data->metricDx[0]);
		histDccEdit->setText(parent->data->metricDcc[0]);
		histVxEdit->setText(parent->data->metricVx[0]);
		histDpEdit->setText(parent->data->metricDp[0]);
		histDxLabel->setDisabled(true);
		histDxEdit->setDisabled(true);
		histDccLabel->setDisabled(true);
		histDccEdit->setDisabled(true);
		histVxLabel->setDisabled(true);
		histVxEdit->setDisabled(true);
		histDpLabel->setDisabled(true);
		histDpEdit->setDisabled(true);
	}
	
	histDxEdit->setValidator(&allowedPercentArrs);
	histDccEdit->setValidator(&allowedPosRealArrs);
	histVxEdit->setValidator(&allowedPosRealArrs);
	histDpEdit->setValidator(&allowedPosReals);
	ttt = tr("Calculate Dx (in cc or % of total volume) and Vx (in % of prescription) values, the dose deposited within "
			 " at least x percent of the volume and total volume with at least x dose, respectively.");
	histDxLabel->setToolTip(ttt);
	histDxEdit->setToolTip(ttt);
	histDccLabel->setToolTip(ttt);
	histDccEdit->setToolTip(ttt);
	histVxLabel->setToolTip(ttt);
	histVxEdit->setToolTip(ttt);
	
	ttt = tr("Prescription dose used to calculate Vx metrics.");
	histDpLabel->setToolTip(ttt);
	histDpEdit->setToolTip(ttt);
	
	histCalcButton   = new QPushButton("Calculate");
	ttt = tr("Calculate and display data metrics.");
	histCalcButton->setToolTip(ttt);
	histSaveButton   = new QPushButton("Output metrics");
	ttt = tr("Calculate and save data metrics in csv file.");
	histSaveButton->setToolTip(ttt);
	histRawButton    = new QPushButton("Output raw data");
	ttt = tr("Save the sorted data of all voxels that were not filtered out in csv format.  File sizes may become very large.");
	histRawButton->setToolTip(ttt);
	
	histOutputFrame  = new QFrame();
	histOutputLayout = new QGridLayout();
	
	histOutputLayout->addWidget(histOutputLabel, 0, 0, 1, 2);
	histOutputLayout->addWidget(histOutputBox  , 0, 2, 1, 4);
	histOutputLayout->addWidget(histDxLabel    , 1, 0, 1, 1);
	histOutputLayout->addWidget(histDxEdit     , 1, 1, 1, 2);
	histOutputLayout->addWidget(histDccLabel   , 1, 3, 1, 1);
	histOutputLayout->addWidget(histDccEdit    , 1, 4, 1, 2);
	histOutputLayout->addWidget(histVxLabel    , 2, 0, 1, 1);
	histOutputLayout->addWidget(histVxEdit     , 2, 1, 1, 2);
	histOutputLayout->addWidget(histDpLabel    , 2, 3, 1, 2);
	histOutputLayout->addWidget(histDpEdit     , 2, 5, 1, 1);
	histOutputLayout->addWidget(histCalcButton , 3, 0, 1, 2);
	histOutputLayout->addWidget(histSaveButton , 3, 2, 1, 2);
	histOutputLayout->addWidget(histRawButton  , 3, 4, 1, 2);
	
	for (int i = 0; i < 6; i++)
		histOutputLayout->setColumnStretch(i, 5);
	
	histOutputFrame->setLayout(histOutputLayout);
	histOutputFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	histoLayout->addWidget(histOutputFrame);
	
	// Profile ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Doses
	profDosesLabel   = new QLabel("Doses");
	profLoadButton   = new QPushButton("Load");
	profDoseSelect   = new QComboBox();
	ttt = tr("Load doses to use to generate profiles.");
	profDosesLabel->setToolTip(ttt);
	profLoadButton->setToolTip(ttt);
	profDoseSelect->setToolTip(ttt);
	
	profDeleteButton = new QPushButton("Delete");
	ttt = tr("Delete loaded dose.");
	profDeleteButton->setToolTip(ttt);
	
	profLoadedView   = new QListWidget();
	
	profLegendBox    = new QCheckBox("Legend");
	profLegendBox->setChecked(true);
	ttt = tr("Add legend to plot.");
	profLegendBox->setToolTip(ttt);
	
	profInterpBox    = new QCheckBox("Interpolate");
	ttt = tr("Interpolate doses using the eight neighbour voxel data.");
	profInterpBox->setToolTip(ttt);
	
	profDosesFrame   = new QFrame();
	profDosesLayout  = new QGridLayout();
	
	profDosesLayout->addWidget(profDosesLabel  , 0, 0, 1, 2);
	profDosesLayout->addWidget(profLoadButton  , 1, 0, 1, 1);
	profDosesLayout->addWidget(profDoseSelect  , 1, 1, 1, 3);
	profDosesLayout->addWidget(profLoadedView  , 2, 0, 1, 4);
	profDosesLayout->addWidget(profDeleteButton, 3, 0, 1, 4);
	profDosesLayout->addWidget(profLegendBox   , 0, 2, 1, 2);
	//profDosesLayout->addWidget(profInterpBox   , 4, 2, 1, 2); // moved
	
	profDosesFrame->setLayout(profDosesLayout);
	profDosesFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	profileLayout->addWidget(profDosesFrame);
	
	// Position
	profCoordLabel  = new QLabel("Coordinate");
	
	p0CoordLabel    = new QLabel("Start");
	p1CoordLabel    = new QLabel("Stop");
	xAxisLabel      = new QLabel("x");
	yAxisLabel      = new QLabel("y");
	zAxisLabel      = new QLabel("z");
	
	profx0Edit      = new QLineEdit("0");
	profy0Edit      = new QLineEdit("0");
	profz0Edit      = new QLineEdit("0");
	profx1Edit      = new QLineEdit("1");
	profy1Edit      = new QLineEdit("1");
	profz1Edit      = new QLineEdit("0");
	profx0Edit->setValidator(&allowedReals);
	profy0Edit->setValidator(&allowedReals);
	profz0Edit->setValidator(&allowedReals);
	profx1Edit->setValidator(&allowedReals);
	profy1Edit->setValidator(&allowedReals);
	profz1Edit->setValidator(&allowedReals);
	ttt = tr("Generate a plot by sampling doses from start's (x,y,z) to stop's (x,y,z).");
	p0CoordLabel->setToolTip(ttt);
	p1CoordLabel->setToolTip(ttt);
	xAxisLabel->setToolTip(ttt);
	yAxisLabel->setToolTip(ttt);
	zAxisLabel->setToolTip(ttt);
	profx0Edit->setToolTip(ttt);
	profy0Edit->setToolTip(ttt);
	profz0Edit->setToolTip(ttt);
	profx1Edit->setToolTip(ttt);
	profy1Edit->setToolTip(ttt);
	profz1Edit->setToolTip(ttt);
	
	profResLabel    = new QLabel("Samples per cm");
	profResEdit     = new QLineEdit("10");
	ttt = tr("The number of sample doses to take for each cm of the profile.");
	profResLabel->setToolTip(ttt);
	profResEdit->setToolTip(ttt);
	
	profCoordFrame  = new QFrame();
	profCoordLayout = new QGridLayout();
	
	profCoordLayout->addWidget(profCoordLabel, 0, 0, 1, 2);
	
	profCoordLayout->addWidget(p0CoordLabel  , 0, 2, 1, 2);
	profCoordLayout->addWidget(p1CoordLabel  , 0, 4, 1, 2);
	profCoordLayout->addWidget(xAxisLabel    , 1, 0, 1, 2);
	profCoordLayout->addWidget(yAxisLabel    , 2, 0, 1, 2);
	profCoordLayout->addWidget(zAxisLabel    , 3, 0, 1, 2);
	
	profCoordLayout->addWidget(profx0Edit    , 1, 2, 1, 2);
	profCoordLayout->addWidget(profy0Edit    , 2, 2, 1, 2);
	profCoordLayout->addWidget(profz0Edit    , 3, 2, 1, 2);
	profCoordLayout->addWidget(profx1Edit    , 1, 4, 1, 2);
	profCoordLayout->addWidget(profy1Edit    , 2, 4, 1, 2);
	profCoordLayout->addWidget(profz1Edit    , 3, 4, 1, 2);
	
	profCoordLayout->addWidget(profResLabel  , 4, 0, 1, 2);
	profCoordLayout->addWidget(profResEdit   , 4, 2, 1, 2);
	profCoordLayout->addWidget(profInterpBox , 4, 4, 1, 2);
	
	profCoordFrame->setLayout(profCoordLayout);
	profCoordFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	profileLayout->addWidget(profCoordFrame);
	
	// Egsphant visual
	profPhantLabel    = new QLabel("Cross-section image");
	profPhant         = new EGSPhant();
	profPhantSelect   = new QComboBox();
	profPhantPreview  = new QLabel();
	ttt = tr("Load an egsphant file to create an image showing the profile within the VPM.");
	profPhantLabel->setToolTip(ttt);
	profPhantSelect->setToolTip(ttt);
	profPhantPreview->setToolTip(ttt);
					  
	profPhantProject  = new QLabel("Projected on");
	profPhantAxis     = new QComboBox();
	profPhantAxis->addItem("x axis");
	profPhantAxis->addItem("y axis");
	profPhantAxis->addItem("z axis");
	profPhantAxis->setCurrentIndex(2);
	ttt = tr("Choose axis along which to project the line.");
	profPhantProject->setToolTip(ttt);
	profPhantAxis->setToolTip(ttt);
	
	renderProfPreview = new QPushButton("Generate");
	ttt = tr("Generate image.");
	renderProfPreview->setToolTip(ttt);
	
	saveProfPreview   = new QPushButton("Save");
	ttt = tr("Save image.");
	saveProfPreview->setToolTip(ttt);
	saveProfPreview->setEnabled(false);
	
	profMediaButton   = new QRadioButton("Media");
	profDensityButton = new QRadioButton("Density");
	profMediaButton->setChecked(true);
	ttt = tr("Select VPM media or density to generate image.");
	profMediaButton->setToolTip(ttt);
	profDensityButton->setToolTip(ttt);
	
	profPhantFrame    = new QFrame();
	profPhantLayout   = new QGridLayout();
	
	profPhantLayout->addWidget(profPhantLabel   , 0, 0, 1, 2);
	profPhantLayout->addWidget(profPhantSelect  , 1, 0, 1, 2);
	profPhantLayout->addWidget(profPhantProject , 2, 0, 1, 1);
	profPhantLayout->addWidget(profPhantAxis    , 2, 1, 1, 1);
	profPhantLayout->addWidget(profMediaButton  , 3, 0, 1, 1);
	profPhantLayout->addWidget(profDensityButton, 3, 1, 1, 1);
	profPhantLayout->addWidget(profPhantPreview , 4, 0, 1, 2);
	profPhantLayout->addWidget(renderProfPreview, 5, 0, 1, 1);
	profPhantLayout->addWidget(saveProfPreview  , 5, 1, 1, 1);
	
	profPhantFrame->setLayout(profPhantLayout);
	profPhantFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	profileLayout->addWidget(profPhantFrame);
	profPhantFrame->setDisabled(true); // To be implemented
	
	// Main layout ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
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
	connect(renderButton, SIGNAL(released()),
			this, SLOT(render()));
	
	// Preview ~~~~~~~~~~~~~~
	connect(resolutionScale, SIGNAL(textEdited(QString)),
			this, SLOT(previewCanvasRenderLive()));
			
    connect(canvas, SIGNAL(mouseClicked(int, int)),
            this, SLOT(writePreviewLabel(int, int)));
	
	connect(saveImageButton, SIGNAL(released()),
			this, SLOT(saveImage()));
	connect(saveDataButton, SIGNAL(released()),
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
	
	connect(expandToBounds, SIGNAL(released()),
			this, SLOT(previewResetBounds()));
			
	connect(depthPlusButton, SIGNAL(released()),
			this, SLOT(previewSliceUp()));
	connect(depthMinusButton, SIGNAL(released()),
			this, SLOT(previewSliceDown()));
			
	connect(canvas, SIGNAL(mouseWheelUp()),
			this, SLOT(previewSliceUp()));
	connect(canvas, SIGNAL(mouseWheelDown()),
			this, SLOT(previewSliceDown()));
	
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
	connect(mapMinButton, SIGNAL(released()),
			sigMap, SLOT(map()));
	connect(mapMidButton, SIGNAL(released()),
			sigMap, SLOT(map()));
	connect(mapMaxButton, SIGNAL(released()),
			sigMap, SLOT(map()));
	for (int i = 0; i < 5; i++)
		connect(isoColourButton[i], SIGNAL(released()),
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
	
	connect(histLoadButton, SIGNAL(released()),
			this, SLOT(loadHistoDose()));
	connect(histDeleteButton, SIGNAL(released()),
			this, SLOT(deleteHistoDose()));
			
	connect(histOutputBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadMetrics()));
	connect(histCalcButton, SIGNAL(released()),
			this, SLOT(calcMetrics()));
	connect(histSaveButton, SIGNAL(released()),
			this, SLOT(outputMetrics()));
	connect(histRawButton, SIGNAL(released()),
			this, SLOT(outputRawData()));
	
	// Profile ~~~~~~~~~~~~~~
	connect(profPhantSelect, SIGNAL(currentIndexChanged(int)),
			this, SLOT(loadPreviewEgsphant()));
	connect(renderProfPreview, SIGNAL(released()),
			this, SLOT(showPreviewEgsphant()));
	connect(saveProfPreview, SIGNAL(released()),
			this, SLOT(savePreviewEgsphant()));
			
	connect(profLoadButton, SIGNAL(released()),
			this, SLOT(loadProfDose()));
	connect(profDeleteButton, SIGNAL(released()),
			this, SLOT(deleteProfDose()));
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
	*blackPic = blackPic->scaled(height,width);
	
	// Invoke all subrenders to reflect the change to the axes
	previewPhantRender();
	previewMapRender();
	previewIsoRender();
	
	previewRender();
}

void doseInterface::previewPhantRenderLive() {
	if(renderCheckBox->isChecked()) {
		previewPhantRender();
		previewRender();
	}
}

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
}

void doseInterface::previewMapRenderLive() {
	if(renderCheckBox->isChecked()) {
		previewMapRender();
		previewRender();
	}
}

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
}

void doseInterface::previewIsoRenderLive() {
	if(renderCheckBox->isChecked()) {
		previewIsoRender();
		previewRender();
	}
}

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
	QVector <double> doses;
	
	for (int j = 0; j < 5; j++)
		doses.append(isoColourDose[j]->text().toDouble());
	
	if (isoDoseBox[0]->currentIndex())
		isoDoses[0]->getContour(&solid,  doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
	if (isoDoseBox[1]->currentIndex())
		isoDoses[1]->getContour(&dashed, doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
	if (isoDoseBox[2]->currentIndex())
		isoDoses[2]->getContour(&dotted, doses, axis, depth, horMin, horMax, vertMin, vertMax, res);
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
					pen.setStyle(Qt::DashLine);
					for (int j = 0; j < 5; j++) {
						pen.setColor(isoColourButton[j]->palette().color(QPalette::Button));
						paint.setPen(pen);
						paint.drawLines(dashed[j]);
					}
					break;
				case 2:
					pen.setStyle(Qt::DotLine);
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
		
	// Legend
	if (legendBox->currentIndex()) {
		QImage legend = createLegend();
		paint.drawImage(canvasPic->width()-legend.width()-11, 11, legend);
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
		tr("Somehow the selected egsphant index is larger than the local VPM count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirPhants[i]+parent->data->localNamePhants[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading egsphant file");
	connect(phant, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz"))
		phant->loadgzEGSPhantFilePlus(file);
	else if (file.endsWith(".begsphant"))
		phant->loadbEGSPhantFilePlus(file);
	else if (file.endsWith(".egsphant"))
		phant->loadEGSPhantFilePlus(file);
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type egsphant.gz, begsphant, or egsphant.  Aborting"));
		parent->finishedProgress();
		return;		
	}
	
	parent->finishedProgress();
	previewPhantRenderLive();
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
	previewMapRenderLive();
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
	previewIsoRenderLive();
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

void doseInterface::previewSliceUp() {
	double depth = depthMin->text().toDouble(), newDepth = depthMin->text().toDouble(), tempDepth = depthMin->text().toDouble();
	int index;
	bool isReset = false;
	
	QVector <QVector <double> * > depths;
	QVector <int> sizes;
	
	// Add all possible depth indices to depths and sizes
	if (xAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->x);
			sizes.append(phant->nx);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cx);
			sizes.append(mapDose->x);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cx);
				sizes.append(isoDoses[i]->x);				
			}
	}
	else if (yAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->y);
			sizes.append(phant->ny);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cy);
			sizes.append(mapDose->y);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cy);
				sizes.append(isoDoses[i]->y);				
			}
	}
	else if (zAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->z);
			sizes.append(phant->nz);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cz);
			sizes.append(mapDose->z);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cz);
				sizes.append(isoDoses[i]->z);				
			}
	}
	
	for (int i = 0; i < depths.size(); i++) {
		if (depth < depths[i]->at(0)) {// If below, go to first slice
			tempDepth = (depths[i]->at(0)+depths[i]->at(1))/2.0;
		} // And if we aren't already at the final slice
		else if ((depth+0.05) < ((depths[i]->at(sizes[i]-1)+depths[i]->at(sizes[i]))/2.0)) { 
			// Get index
			index = 0;
			for (int j = 0; j < sizes[i]; j++) {
				if (depth > depths[i]->at(j))
					index = j;
				else
					break;
			}
			
			// Get midpoint of current slice
			tempDepth = (depths[i]->at(index)+depths[i]->at(index+1))/2.0;
			
			// If current depth is only 0.5 mm below slice center or above center, get next center
			if ((depth+0.05) >= tempDepth)
				tempDepth = (depths[i]->at(index+1)+depths[i]->at(index+2))/2.0;
		}
		
		// Set newDepth, unless we already have a newDepth closer to the current point
		if (!isReset) {
			newDepth = tempDepth;
			isReset = true;
		}
		else if (abs(tempDepth-depth) < abs(newDepth-depth)) {
			newDepth = tempDepth;
		}
	}
	
	depthMin->setText(QString::number(newDepth));
	
	previewCanvasRenderLive();
}

void doseInterface::previewSliceDown() {
	double depth = depthMin->text().toDouble(), newDepth = depthMin->text().toDouble(), tempDepth = depthMin->text().toDouble();
	int index;
	bool isReset = false;
	
	QVector <QVector <double> * > depths;
	QVector <int> sizes;
	
	// Add all possible depth indices to depths and sizes
	if (xAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->x);
			sizes.append(phant->nx);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cx);
			sizes.append(mapDose->x);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cx);
				sizes.append(isoDoses[i]->x);				
			}
	}
	else if (yAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->y);
			sizes.append(phant->ny);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cy);
			sizes.append(mapDose->y);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cy);
				sizes.append(isoDoses[i]->y);				
			}
	}
	else if (zAxisButton->isChecked()) {
		// Phantom
		if (0 < phantSelect->currentIndex()) {
			depths.append(&phant->z);
			sizes.append(phant->nz);
		}
		
		// Colour map
		if (0 < mapDoseBox->currentIndex()) {
			depths.append(&mapDose->cz);
			sizes.append(mapDose->z);
		}
		
		// Isodoses
		for (int i = 0; i < 3; i++)
			if (0 < isoDoseBox[i]->currentIndex()) {
				depths.append(&isoDoses[i]->cz);
				sizes.append(isoDoses[i]->z);				
			}
	}
	
	for (int i = 0; i < depths.size(); i++) {
		if (depth > depths[i]->last()) {// If below, go to first slice
			tempDepth = (depths[i]->at(sizes[i]-1)+depths[i]->at(sizes[i]))/2.0;
		} // And if we aren't already at the final slice
		else if ((depth-0.05) > ((depths[i]->at(0)+depths[i]->at(1))/2.0)) { 
			// Get index
			index = 0;
			for (int j = 0; j < sizes[i]; j++) {
				if (depth > depths[i]->at(j))
					index = j;
				else
					break;
			}
			
			// Get midpoint of current slice
			tempDepth = (depths[i]->at(index)+depths[i]->at(index+1))/2.0;
			
			// If current depth is only 0.5 mm above slice center or below center, get next center
			if ((depth-0.05) <= tempDepth)
				tempDepth = (depths[i]->at(index-1)+depths[i]->at(index))/2.0;
		}
		
		// Set newDepth, unless we already have a newDepth closer to the current point
		if (!isReset) {
			newDepth = tempDepth;
			isReset = true;
		}
		else if (abs(tempDepth-depth) < abs(newDepth-depth)) {
			newDepth = tempDepth;
		}
	}
	
	depthMin->setText(QString::number(newDepth));
	
	previewCanvasRenderLive();
}

void doseInterface::writePreviewLabel(int i, int j) {	
	QString temp = "";
	
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
	
	double x = 0, y = 0, z = 0;

    // Get the appropriate point
    if (!axis.compare("X")) {
		x = depth;
		y = horMin+i/res;
		z = vertMin+j/res;
    }
    else if (!axis.compare("Y")) {
		x = horMin+i/res;
		y = depth;
		z = vertMin+j/res;
    }
    else if (!axis.compare("Z")) {
		x = vertMin+i/res;
		y = horMin+j/res;
		z = depth;
    }
	
	temp += "Position: ("+QString::number(x,'g',3)+",";
	temp += QString::number(y,'g',3)+","+QString::number(z,'g',3)+")";
	
	// Phantom
	if (phantSelect->currentIndex()) {
		if (phant->getMedia(x,y,z) != -1) {
			temp += " , VPM: ";
			temp += phant->media[QString(EGSPHANT_CHARS).indexOf(phant->getMedia(x,y,z))];
			temp += " ("+QString::number(phant->getDensity(x,y,z),'g',3)+" g/cm^3)";
		}
	}
	
	// Colour map
	if (mapDoseBox->currentIndex()) {
		if (mapDose->getDose(x,y,z) != -1) {
			temp += ", Map dose: ";
			temp += QString::number(mapDose->getDose(x,y,z),'g',3)+"+/-";
			temp += QString::number(mapDose->getDose(x,y,z)*mapDose->getError(x,y,z),'g',3)+" Gy";
		}
	}

    canvasInfo->setText(temp);
    canvasInfo->repaint();
}

QImage doseInterface::createLegend() {
	QStringList labels;
	QVector <QColor> colours;
	int longest = 0;
	int spacing = 12, buffer = 10, textWidth = 10;
	
	if ((legendBox->currentIndex() == 0) ||
		(legendBox->currentIndex() == 1 && !phantSelect->currentIndex()) ||
		(legendBox->currentIndex() == 2 && !mapDoseBox->currentIndex()) ||
		(legendBox->currentIndex() == 3 && !(isoDoseBox[0]->currentIndex() ||
		isoDoseBox[1]->currentIndex() || isoDoseBox[2]->currentIndex())))
		return QImage(); // appropriate legend data isn't loaded, so quit
	
	if (legendBox->currentIndex() == 1) {
		if (mediaButton->isChecked()) {
			double c = 0;
			for (int i = 0; i < phant->media.size(); i++) {
				labels.append(phant->media[i]);
				c = (i+1)*255.0/double(phant->media.size()+1);
				colours.append(QColor(qRgb(c,c,c)));
				longest = int(longest<labels.last().length()?labels.last().length():longest);
			}
		}
		else {
			labels.append(densityMin->text()+" g/cm^3");
			longest = int(longest<labels.last().length()?labels.last().length():longest);
			labels.append(densityMax->text()+" g/cm^3");
			longest = int(longest<labels.last().length()?labels.last().length():longest);
			colours.append(QColor(qRgb(0,0,0)));
			colours.append(QColor(qRgb(255,255,255)));
		}
	}
	else if (legendBox->currentIndex() == 2) {
		labels.append(mapMinDose->text()+" "+unitsEdit->text());
		longest = int(longest<labels.last().length()?labels.last().length():longest);
		labels.append(mapMaxDose->text()+" "+unitsEdit->text());
		longest = int(longest<labels.last().length()?labels.last().length():longest);
		colours.append(QColor(mapMinButton->palette().color(QPalette::Button)));
		colours.append(QColor(mapMidButton->palette().color(QPalette::Button)));
		colours.append(QColor(mapMaxButton->palette().color(QPalette::Button)));
	}
	else if (legendBox->currentIndex() == 3) {
		for (int i = 0; i < isoColourDose.size(); i++) {
			labels.append(isoColourDose[i]->text()+" "+unitsEdit->text());
			longest = int(longest<labels.last().length()?labels.last().length():longest);
			colours.append(QColor(isoColourButton[i]->palette().color(QPalette::Button)));
		}
	}
	
	QImage legend(buffer+spacing+buffer+longest*textWidth+buffer,
				  buffer+(labels.size())*(spacing+buffer),
				  QImage::Format_ARGB32_Premultiplied);
	legend.fill(qRgb(255,255,255)); // Paint white
	
	QPainter painter;
	
	QPen pen;
	pen.setWidth(1);
	pen.setColor(Qt::black);
	
	QFont font;
	font.setWeight(75);
	font.setPointSize(10);
	
	painter.begin(&legend);
	painter.setPen(pen);
	painter.setFont(font);
	
	// Draw outer outline
	painter.drawRect(0, 0, legend.width()-1, legend.height()-1);
	
	if (legendBox->currentIndex() == 1) {
		if (mediaButton->isChecked()) {
			// Draw each box
			for (int i = 0; i < colours.size(); i++) {
				painter.fillRect(buffer, buffer+i*(buffer+spacing), spacing, spacing, colours[i]);
				painter.drawRect(buffer, buffer+i*(buffer+spacing), spacing, spacing);
				painter.drawText(2*buffer+spacing, (i+1)*(buffer+spacing), labels[i]);
			}
		}
		else {
			// Draw labels
			painter.drawText(2*buffer+spacing, buffer+spacing, labels[0]);
			painter.drawText(2*buffer+spacing, 2*(buffer+spacing), labels[1]);
			
			double weight = 0, invWeight = 0, red = 0, green = 0, blue = 0;
			
			// Draw grey values line by line
			for (int i = buffer; i < 2*(buffer+spacing); i++) {
				weight = double(i-buffer)/double(2*(buffer+spacing));
				invWeight = 1.0 - weight;
				red   = (colours[0].red()  * weight) +
						(colours[1].red()  * invWeight);
				green = (colours[0].green()* weight) +
						(colours[1].green()* invWeight);
				blue  = (colours[0].blue() * weight) +
						(colours[1].blue() * invWeight);
				
				pen.setColor(qRgb(red,green,blue));
				painter.setPen(pen);
				painter.drawLine(buffer, i, buffer+spacing, i);
			}
			
			// Draw outline
			pen.setWidth(1);
			pen.setColor(Qt::black);
			painter.setPen(pen);
			painter.drawRect(buffer, buffer, spacing, buffer+2*spacing);
		}
	}
	else if (legendBox->currentIndex() == 2) {
		painter.drawText(2*buffer+spacing, (buffer+spacing), labels[0]);
		painter.drawText(2*buffer+spacing, 2*(buffer+spacing), labels[1]);
				
		double weight = 0, invWeight = 0, red = 0, green = 0, blue = 0;
		for (int i = buffer; i < buffer+spacing+buffer/2; i++) {
			weight = double(i-buffer)/double(spacing+buffer/2);
			invWeight = 1.0 - weight;
			red   = (colours[1].red()  * weight) +
					(colours[0].red()  * invWeight);
			green = (colours[1].green()* weight) +
					(colours[0].green()* invWeight);
			blue  = (colours[1].blue() * weight) +
					(colours[0].blue() * invWeight);
					
			weight = red>blue?red:blue;
			weight = weight>green?weight:green;
			weight = 255.0/weight;
			
			pen.setColor(qRgb(red*weight,green*weight,blue*weight));
			painter.setPen(pen);
			painter.drawLine(buffer, i, buffer+spacing, i);
		}
		
		for (int i = buffer+spacing+buffer/2; i < 2*(buffer+spacing); i++) {
			weight = double(i-(buffer+spacing+buffer/2))/double(spacing+buffer/2);
			invWeight = 1.0 - weight;
			red   = (colours[2].red()  * weight) +
					(colours[1].red()  * invWeight);
			green = (colours[2].green()* weight) +
					(colours[1].green()* invWeight);
			blue  = (colours[2].blue() * weight) +
					(colours[1].blue() * invWeight);
					
			weight = red>blue?red:blue;
			weight = weight>green?weight:green;
			weight = 255.0/weight;
			
			pen.setColor(qRgb(red*weight,green*weight,blue*weight));
			painter.setPen(pen);
			painter.drawLine(buffer, i, buffer+spacing, i);
		}
		
		// Draw outline
		pen.setWidth(1);
		pen.setColor(Qt::black);
		painter.setPen(pen);
		painter.drawRect(buffer, buffer, spacing, buffer+2*spacing);
	}
	else if (legendBox->currentIndex() == 3) {
		// Draw each box
		for (int i = 0; i < colours.size(); i++) {
			painter.fillRect(buffer, buffer+i*(buffer+spacing), spacing, spacing, colours[i]);
			painter.drawRect(buffer, buffer+i*(buffer+spacing), spacing, spacing);
			painter.drawText(2*buffer+spacing, (i+1)*(buffer+spacing), labels[i]);
		}
	}
	
	return legend;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                               Histogram                             //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
void doseInterface::histoRefresh() {
	// Enable the metrics boxes if Custom is selected
	if (histOutputBox->currentText().compare("Custom")) {
		histDxLabel->setDisabled(true);
		histDxEdit->setDisabled(true);
		histDccLabel->setDisabled(true);
		histDccEdit->setDisabled(true);
		histVxLabel->setDisabled(true);
		histVxEdit->setDisabled(true);
		histDpLabel->setDisabled(true);
		histDpEdit->setDisabled(true);
	}
	else {
		histDxLabel->setDisabled(false);
		histDxEdit->setDisabled(false);
		histDccLabel->setDisabled(false);
		histDccEdit->setDisabled(false);
		histVxLabel->setDisabled(false);
		histVxEdit->setDisabled(false);
		histDpLabel->setDisabled(false);
		histDpEdit->setDisabled(false);
	}
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
		tr("Somehow the selected egsphant index is larger than the local VPM count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirPhants[i]+parent->data->localNamePhants[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading egsphant file");
	connect(histPhant, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz")) {
		histPhant->loadgzEGSPhantFile(file);
		file = file.left(file.size()-12).split("/").last();
	}
	else if (file.endsWith(".begsphant")) {
		histPhant->loadbEGSPhantFile(file);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".egsphant")) {
		histPhant->loadEGSPhantFile(file);
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
		tr("Somehow the selected mask index is larger than the local VPM count.  Aborting"));		
		return;
	}
	
	QString file = localDirMasks[i]+localNameMasks[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading mask file");
	connect(histMask, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
		
	if (file.endsWith(".egsphant.gz")) {
		histMask->loadgzEGSPhantFile(file);
		file = file.left(file.size()-12).split("/").last();
	}
	else if (file.endsWith(".begsphant")) {
		histMask->loadbEGSPhantFile(file);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".egsphant")) {
		histMask->loadEGSPhantFile(file);
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
			for (int j = 1; j <= binCount; j++) {
				cur = s0+sInc*j;
				
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
			series.last()->append(0, 100.0);
			tempData.append(QPointF(0, 100.0));
			
			for (int j = 0; j < data.size(); j++) {
				if (!(j%sInc)) {// Only add up to 399 points (start skipping at 400+)
					series.last()->append(data[j].dose, 100.0*double(data.size()-j)/double(data.size()));
					tempData.append(QPointF(data[j].dose, 100.0*double(data.size()-j)/double(data.size())));
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
	
	if (histDiffBox->isChecked()) {
		plot->axes()[0]->setRange(data[0].dose,data.last().dose);
		plot->setTitle("Dose Differential Histogram");
		plot->axes()[1]->setTitleText("voxel count");
		savePlotY = "voxel count";
	}
	else {
		plot->axes()[0]->setRange(0,data.last().dose);
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

void doseInterface::loadMetrics() {
	int i = histOutputBox->currentIndex();
	if (i >= 0 && i < parent->data->metricDp.size()) {
		histDxEdit->setText(parent->data->metricDx[i]);
		histDccEdit->setText(parent->data->metricDcc[i]);
		histVxEdit->setText(parent->data->metricVx[i]);
		histDpEdit->setText(parent->data->metricDp[i]);
	}
	histoRefresh();
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
	QString names, units, average, uncertainty, voxels, volumes, minimum, maximum;
	QStringList Dx, Vx, Dcc, temp;
	QVector <double> xD, xV, ccD;
	double pD, minD = 1000000000, maxD = 0, minE = 0, maxE = 0;
	
	if (histDxEdit->text().length()) {
		temp = histDxEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xD.append(temp[i].toDouble());
			Dx.append("");
		}
		std::sort(xD.begin(), xD.end());
	}
	
	if (histVxEdit->text().length()) {
		temp = histVxEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xV.append(temp[i].toDouble());
			Vx.append("");
		}
		std::sort(xV.begin(), xV.end());
	}
	
	if (histDccEdit->text().length()) {
		temp = histDccEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			ccD.append(temp[i].toDouble());
			Dcc.append("");
		}
		std::sort(ccD.begin(), ccD.end());
	}
	
	pD = histDpEdit->text().toDouble();
	
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
		double volumeTally = 0, doseTally = 0, doseTallyErr = 0, doseTallyErr2 = 0, countTally = 0, absError = 0;
		int vIndex = 0, dIndex = xD.size()-1, ccIndex = ccD.size()-1;
		for (int j = 0; j < data.size(); j++) {
			countTally    += 1.0;
			volumeTally   += data[j].vol;
			doseTally     += data[j].dose;
			absError       = data[j].dose*data[j].err;
			doseTallyErr  += absError;
			doseTallyErr2 += absError*absError;
			maxE = maxD<data[j].dose?absError:maxE;
			maxD = maxD<data[j].dose?data[j].dose:maxD;
			minE = minD>data[j].dose?absError:minE;
			minD = minD>data[j].dose?data[j].dose:minD;
			
			// Append Vx values as we go
			if (vIndex < xV.size()) {
				if (data[j].dose > (xV[vIndex]*pD/100.0) && j) {
					Vx[vIndex] += QString::number((volume-volumeTally+data[j].vol)/volume*100.0).left(11).rightJustified(11,' ')+" "
							   +  "            |";
					vIndex++;
				}
			}
			
			// Append Dx values as we go
			if (dIndex >= 0) {
				if ((volume-volumeTally)/volume*100.0 < xD[dIndex] && j) {
					Dx[dIndex] += QString::number(data[j-1].dose).left(11).rightJustified(11,' ')+" "
							   +  QString::number(data[j-1].dose*data[j-1].err).left(11).rightJustified(11,' ')+" |";
					dIndex--;
				}
			}
			
			// Append Dcc values as we go
			if (ccIndex >= 0) {
				if ((volume-volumeTally) < ccD[ccIndex] && j) {
					Dcc[ccIndex] += QString::number(data[j-1].dose).left(11).rightJustified(11,' ')+" "
							     +  QString::number(data[j-1].dose*data[j-1].err).left(11).rightJustified(11,' ')+" |";
					ccIndex--;
				}
			}
		}
		
		for (int j = vIndex; j < xV.size(); j++) 
			Vx[j] += "        n/a         n/a |";
		
		for (int j = dIndex; j >= 0; j--) 
			Dx[j] += "        n/a         n/a |";
		
		for (int j = ccIndex; j >= 0; j--) 
			Dcc[j] += "        n/a         n/a |";
		
		// Calculate global metrics
		doseTally     /= countTally; // Average dose
		doseTallyErr  /= countTally; // Average uncertainty
		doseTallyErr2 = sqrt(doseTallyErr2/countTally); // Propagated average dose uncertainty
		
		names       += histLoadedView->item(i)->text().left(23).rightJustified(23,' ')+" |";
		units       += "   value    uncertainty |";
		minimum     += QString::number(minD).left(11).rightJustified(11,' ')+" "
		             + QString::number(minE).left(11).rightJustified(11,' ')+" |";
		maximum     += QString::number(maxD).left(11).rightJustified(11,' ')+" "
		             + QString::number(maxE).left(11).rightJustified(11,' ')+" |";
		average     += QString::number(doseTally).left(11).rightJustified(11,' ')+" "
		             + QString::number(doseTallyErr2).left(11).rightJustified(11,' ')+" |";
		uncertainty += QString::number(doseTallyErr).left(11).rightJustified(11,' ')+"             |";
		voxels      += QString::number(countTally).left(11).rightJustified(11,' ')+"             |";
		volumes     += QString::number(volume).left(11).rightJustified(11,' ')+"             |";
	}
	
	QString text = QString("Dataset").left(24).rightJustified(24,' ')+"|"+names+"\n";
	text        += QString(" ").left(24).rightJustified(24,' ')+" "+units+"\n";
	text        += QString("Max dose / Gy").left(24).rightJustified(24,' ')+"|"+maximum+"\n";
	text        += QString("Min dose / Gy").left(24).rightJustified(24,' ')+"|"+minimum+"\n";
	text        += QString("Average dose / Gy").left(24).rightJustified(24,' ')+"|"+average+"\n";
	text        += QString("Average uncertainty / Gy").left(24).rightJustified(24,' ')+"|"+uncertainty+"\n";
	text        += QString("Number of voxels").left(24).rightJustified(24,' ')+"|"+voxels+"\n";
	text        += QString("Total volume / cc").left(24).rightJustified(24,' ')+"|"+volumes+"\n";
	
	for (int i = 0; i < Dx.size(); i++)
		text += (QString("D")+QString::number(xD[i])+" (%) / Gy").left(24).rightJustified(24,' ')+"|"+Dx[i]+"\n";
	
	for (int i = 0; i < Dcc.size(); i++)
		text += (QString("D")+QString::number(ccD[i])+" (cc) / Gy").left(24).rightJustified(24,' ')+"|"+Dcc[i]+"\n";
	
	for (int i = 0; i < Vx.size(); i++)
		text += (QString("V")+QString::number(xV[i])+" ("+QString::number(pD)+" Gy) / %").left(24).rightJustified(24,' ')+"|"+Vx[i]+"\n";
	
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
		text += QString("Data filtered to only include doses within ")+histPhantSelect->currentText()+" VPM voxels containing:,";
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
	QString names, units, average, uncertainty, voxels, volumes, minimum, maximum;
	QStringList Dx, Vx, Dcc, temp;
	QVector <double> xD, xV, ccD;
	double pD, minD = 1000000000, maxD = 0, minE = 0, maxE = 0;
	
	if (histDxEdit->text().length()) {
		temp = histDxEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xD.append(temp[i].toDouble());
			Dx.append("");
		}
		std::sort(xD.begin(), xD.end());
	}
	
	if (histVxEdit->text().length()) {
		temp = histVxEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xV.append(temp[i].toDouble());
			Vx.append("");
		}
		std::sort(xV.begin(), xV.end());
	}
	
	if (histDccEdit->text().length()) {
		temp = histDccEdit->text().replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			ccD.append(temp[i].toDouble());
			Dcc.append("");
		}
		std::sort(ccD.begin(), ccD.end());
	}
	
	pD = histDpEdit->text().toDouble();
	
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
		double volumeTally = 0, doseTally = 0, doseTallyErr = 0, doseTallyErr2 = 0, countTally = 0, absError = 0;
		int vIndex = 0, dIndex = xD.size()-1, ccIndex = ccD.size()-1;
		for (int j = 0; j < data.size(); j++) {			
			countTally    += 1.0;
			volumeTally   += data[j].vol;
			doseTally     += data[j].dose;
			absError       = data[j].dose*data[j].err;
			doseTallyErr  += absError;
			doseTallyErr2 += absError*absError;
			maxE = maxD<data[j].dose?absError:maxE;
			maxD = maxD<data[j].dose?data[j].dose:maxD;
			minE = minD>data[j].dose?absError:minE;
			minD = minD>data[j].dose?data[j].dose:minD;
			
			// Append Vx values as we go
			if (vIndex < xV.size()) {
				if (data[j].dose > (xV[vIndex]*pD/100.0) && j) {
					Vx[vIndex] += QString::number((volume-volumeTally+data[j].vol)/volume*100.0)+",,";
					vIndex++;
				}
			}
			
			// Append Dx values as we go
			if (dIndex >= 0) {
				if ((volume-volumeTally)/volume*100.0 < xD[dIndex] && j) {
					Dx[dIndex] += QString::number(data[j-1].dose)+","+QString::number(data[j-1].dose*data[j-1].err)+",";
					dIndex--;
				}
			}
			
			// Append Dcc values as we go
			if (ccIndex >= 0) {
				if ((volume-volumeTally) < ccD[ccIndex] && j) {
					Dcc[ccIndex] += QString::number(data[j-1].dose)+","+QString::number(data[j-1].dose*data[j-1].err)+",";
					ccIndex--;
				}
			}
		}
		
		for (int j = vIndex; j < xV.size(); j++) 
			Vx[j] += "n/a,n/a,";
		
		for (int j = dIndex; j >= 0; j--) 
			Dx[j] += "n/a,n/a,";
		
		for (int j = ccIndex; j >= 0; j--) 
			Dcc[j] += "n/a,n/a,";
		
		// Calculate global metrics
		doseTally     /= countTally; // Average dose
		doseTallyErr  /= countTally; // Average uncertainty
		doseTallyErr2 = sqrt(doseTallyErr2/countTally); // Propagated average dose uncertainty
		
		names       += histLoadedView->item(i)->text()+",,";
		units       += "value,uncertainty,";
		minimum     += QString::number(minD)+","+QString::number(minE)+",";
		maximum     += QString::number(maxD)+","+QString::number(maxE)+",";
		average     += QString::number(doseTally)+","+QString::number(doseTallyErr2)+",";
		uncertainty += QString::number(doseTallyErr)+",,";
		voxels      += QString::number(countTally)+",,";
		volumes     += QString::number(volume)+",,";
	}
	
	text        += QString("Dataset,")+names+"\n";
	text        += QString(",")+units+"\n";
	text        += QString("Max dose / Gy,")+maximum+"\n";
	text        += QString("Min dose / Gy,")+minimum+"\n";
	text        += QString("Average dose / Gy,")+average+"\n";
	text        += QString("Average uncertainty / Gy,")+uncertainty+"\n";
	text        += QString("Number of voxels,")+voxels+"\n";
	text        += QString("Total volume / cm^3,")+volumes+"\n";
	
	for (int i = 0; i < Dx.size(); i++)
		text += QString("D")+QString::number(xD[i])+" (%) / Gy,"+Dx[i]+"\n";
	
	for (int i = 0; i < Dcc.size(); i++)
		text += QString("D")+QString::number(ccD[i])+" (cc) / Gy,"+Dcc[i]+"\n";
	
	for (int i = 0; i < Vx.size(); i++)
		text += QString("V")+QString::number(xV[i])+" / %,"+Vx[i]+"\n";
	
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
		text += QString("Data filtered to only include doses within ")+histPhantSelect->currentText()+" VPM voxels containing:,";
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
	// Return
	if (profDoses.size() == 0) {
		return;
	}
	
	// Get sorted dose arrays
	QVector <QLineSeries*> series;
	QChart* plot = new QChart();
	int count = profDoses.size();
	
	// Reset functions holding all the data for later output
	savePlotName.clear();
	savePlotX = savePlotY = "";
	savePlotData.clear();
	QList<QPointF> tempData;
	
	// Get increments and total count
	double x0 = profx0Edit->text().toDouble(), y0 = profy0Edit->text().toDouble(),
		   z0 = profz0Edit->text().toDouble(), x1 = profx1Edit->text().toDouble(),
		   y1 = profy1Edit->text().toDouble(), z1 = profz1Edit->text().toDouble();
	
	double rInc = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
	
	int points = rInc*profResEdit->text().toDouble()+1;
	
	double xInc = (x1-x0)/double(points), yInc = (y1-y0)/double(points), zInc = (z1-z0)/double(points);
	rInc /= double(points-1);
	double xi, yi, zi, ri;
	
	double value, error;
	
	for (int i = 0; i < count; i++) {
		// Generate series data
		tempData.clear();
		series.append(new QLineSeries());
		series.last()->setName(profLoadedView->item(i)->text());
		savePlotName.append(profLoadedView->item(i)->text());
		
		if (profInterpBox->isChecked()) {
			for (int j = 0; j < points; j++) {
				xi = x0+xInc*j;
				yi = y0+yInc*j;
				zi = z0+zInc*j;
				ri = rInc*j;
				
				profDoses[i]->triInterpol(xi,yi,zi,&value,&error);
				
				if (value != -1) {
					series.last()->append(ri,value);			
					tempData.append(QPointF(ri,value));
				}
			}
		}
		else {
			for (int j = 0; j < points; j++) {
				xi = x0+xInc*j;
				yi = y0+yInc*j;
				zi = z0+zInc*j;
				ri = rInc*j;
				
				value = profDoses[i]->getDose(xi,yi,zi);
				
				if (value != -1) {
					series.last()->append(ri,value);			
					tempData.append(QPointF(ri,value));
				}
			}
		}
			
		plot->addSeries(series.last());
		savePlotData.append(tempData);
	}
		
	// Fill out plot parameters
	QString xString = QString("(")+QString::number(x0)+" "+QString::number(y0)+" "+QString::number(z0)+") to ";
	xString += QString("(")+QString::number(x1)+" "+QString::number(y1)+" "+QString::number(z1)+")";
	plot->createDefaultAxes();
	plot->axes()[0]->setTitleText(QString("distance from ")+xString+" / cm");
	savePlotX = QString("distance from ")+xString+" / cm";
	plot->axes()[0]->setRange(0,rInc*double(points-1));
	
	plot->setTitle("Dose profile");
	plot->axes()[1]->setTitleText("dose / Gy");
	savePlotY = "dose / Gy";
		
	if (!profLegendBox->isChecked()) // Hide legend if it's unwanted
		plot->legend()->hide();
	
	canvasChart->setChart(plot);
	canvasChart->resize(800,600);
    canvasArea->repaint();
	parent->finishedProgress();	
}
	
void doseInterface::loadPreviewEgsphant() {	
	int i = profPhantSelect->currentIndex()-1;
	if (i < 0) {return;} // Exit if none is selected or box is empty in setup
	
	if (i >= parent->data->localDirPhants.size()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected egsphant index is larger than the local VPM count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirPhants[i]+parent->data->localNamePhants[i]; // Get file location
	
	// Connect the progress bar
	parent->resetProgress("Loading egsphant file");
	connect(profPhant, SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
	
	if (file.endsWith(".egsphant.gz")) {
		profPhant->loadgzEGSPhantFilePlus(file);
		file = file.left(file.size()-12).split("/").last();
	}
	else if (file.endsWith(".begsphant")) {
		profPhant->loadbEGSPhantFilePlus(file);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".egsphant")) {
		profPhant->loadEGSPhantFilePlus(file);
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

void doseInterface::loadProfDose() {
	int i = profDoseSelect->currentIndex();	
	
	if (i >= profDoseSelect->count()) {
		QMessageBox::warning(0, "Index error",
		tr("Somehow the selected dose index is larger than the local dose count.  Aborting"));		
		return;
	}
	
	QString file = parent->data->localDirDoses[i]+parent->data->localNameDoses[i]; // Get file location
	profDoses.append(new Dose());
	
	// Connect the progress bar
	parent->resetProgress("Loading dose file");
	connect(profDoses.last(), SIGNAL(madeProgress(double)),
			parent, SLOT(updateProgress(double)));
	connect(profDoses.last(), SIGNAL(nameProgress(QString)),
			parent, SLOT(nameProgress(QString)));
		
	if (file.endsWith(".b3ddose")) {
		profDoses.last()->readBIn(file, 1);
		file = file.left(file.size()-10).split("/").last();
	}
	else if (file.endsWith(".3ddose")) {
		profDoses.last()->readIn(file, 1);
		file = file.left(file.size()-9).split("/").last();
	}
	else {
		QMessageBox::warning(0, "File error",
		tr("Selected file is not of type b3ddose or 3ddose.  Aborting"));
		parent->finishedProgress();
		delete profDoses.last();
		profDoses.removeLast();
		
		return;		
	}
	
	parent->finishedProgress();
	
	// Add listing to loaded dose view
	if (file.endsWith(".b3ddose"))
		profLoadedView->addItem(parent->data->localNameDoses[i].left(parent->data->localNameDoses[i].size()-8));
	else if (file.endsWith(".3ddose"))
		profLoadedView->addItem(parent->data->localNameDoses[i].left(parent->data->localNameDoses[i].size()-7));
	else
		profLoadedView->addItem(parent->data->localNameDoses[i]);
}

void doseInterface::deleteProfDose() {
	if (profLoadedView->selectedItems().size() != 1) {
		QMessageBox::warning(0, "Deletion error",
		tr("No doses selected.  Please select one to delete."));
		return;
	}
	
	int i = profLoadedView->currentRow();
	delete profDoses[i];
	profDoses.remove(i);
	delete profLoadedView->currentItem();
}

void doseInterface::showPreviewEgsphant() {
	// Get positions
	double x0 = profx0Edit->text().toDouble(), y0 = profy0Edit->text().toDouble(),
		   z0 = profz0Edit->text().toDouble(), x1 = profx1Edit->text().toDouble(),
		   y1 = profy1Edit->text().toDouble(), z1 = profz1Edit->text().toDouble();
	
	QString axis = profPhantAxis->currentText();
	
	double horLeft, horRight, verBot, verTop, depth;
	
	// Assign the right points to start with in cm
	switch (profPhantAxis->currentIndex()) {
		case 0 :
		horLeft  = y0;
		horRight = y1;
		verBot   = z0;
		verTop   = z1;
		depth    = (x0+x1)/2.0;
		break;
		case 1 :
		horLeft  = x0;
		horRight = x1;
		verBot   = z0;
		verTop   = z1;
		depth    = (y0+y1)/2.0;
		break;
		default:
		horLeft  = x0;
		horRight = x1;
		verBot   = y0;
		verTop   = y1;
		depth    = (z0+z1)/2.0;
		break;
	}
	
	double density = abs(horRight-horLeft);
	if (abs(verTop-verBot) > density)
		density = abs(verTop-verBot);
	
	double horStart = (horRight+horLeft)/2.0-density/2.0*1.1; // Get the bottom length corner coord
	double verStart = (verTop+verBot)/2.0-density/2.0*1.1; // Get the bottom length corner coord
	
	density /= 180.0; // Use this to break the longer length into 180 pixels to get conversion
	
	// Get line coords in terms of pixels
	QPoint lineStart((horLeft-horStart)*density,(verBot-verStart)*density);
	QPoint lineStop((horRight-horStart)*density,(verTop-verStart)*density);
	
	// Draw mini-image
	QImage* pic = new QImage(200,200,QImage::Format_ARGB32_Premultiplied);
	if (profMediaButton->isChecked()) {
		int cInc = 255.0/double(profPhant->media.size()+1), c;
		double h, w;
		char med;
		QString indeces("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
		for (int i = 0; i < 200; i++)
			for (int j = 0; j < 200; j++) {
				w = horStart+double(i)/density;
				h = verStart+double(j)/density;
				
				if (!axis.compare("x axis"))
					med = profPhant->getMedia(depth, h, w);
				else if (!axis.compare("y axis"))
					med = profPhant->getMedia(h, depth, w);
				else if (!axis.compare("z axis"))
					med = profPhant->getMedia(h, w, depth);
				
				c = (indeces.indexOf(med)+1)*cInc;

				pic->setPixel(i, 199-j, qRgb(c, c, c));
			}
	}
	else {
		int cInc = 255.0/double(profPhant->maxDensity), c;
		double h, w;
		for (int i = 0; i < 200; i++)
			for (int j = 0; j < 200; j++) {
				w = horStart+double(i)/density;
				h = verStart+double(j)/density;
				
				if (!axis.compare("x axis"))
					c = profPhant->getDensity(depth, h, w);
				else if (!axis.compare("y axis"))
					c = profPhant->getDensity(h, depth, w);
				else if (!axis.compare("z axis"))
					c = profPhant->getDensity(h, w, depth);
				else
					c = 0;
				c *= cInc;

				pic->setPixel(i, 199-j, qRgb(c, c, c));
			}
	}
	
	// Draw line
	QPainter paint(pic);
    paint.setPen(Qt::red);

    paint.drawLine(lineStart.x(), 199-lineStart.y(), lineStop.x(), 199-lineStop.y());
    paint.end();

	// Output to GUI	
	profPhantPreview->setPixmap(QPixmap::fromImage((*pic)));
    profPhantPreview->setFixedSize(canvasPic->width(), canvasPic->height());
    profPhantPreview->repaint();
	delete pic;
	saveProfPreview->setEnabled(true);
}

void doseInterface::savePreviewEgsphant() {
	
}

/*******************************************************************************
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Hover Label Class~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*******************************************************************************/
void HoverLabel::mousePressEvent(QMouseEvent *event) {
    if (event->x() > 0 && event->x() < width() && event->y() > 0 && event->y() < height()) {
        emit mouseClicked(event->x(),event->y());
    }
    event->accept();
}

void HoverLabel::wheelEvent(QWheelEvent *event) {
    if (event->delta() > 0 && event->x() > 0 && event->x() < width() && event->y() > 0 && event->y() < height()) {
        emit mouseWheelUp();
    }
    else if (event->x() > 0 && event->x() < width() && event->y() > 0 && event->y() < height()) {
        emit mouseWheelDown();
    }
    event->accept();
}