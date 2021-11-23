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
	delete canvasPic;
	delete blackPic;
	delete phantPic;
	delete phant;
	delete mapPic;
	delete mapDose;
	delete isoPic;
	delete isoDoses[2];
	delete isoDoses[1];
	delete isoDoses[0];
	
	switch(optionsBox->currentIndex()) {
		case 0 :
			delete histoLayout;
			delete profileLayout;			
			break;
		case 1 :
			delete previewLayout;
			delete profileLayout;	
			break;
		case 2 :
			delete previewLayout;
			delete histoLayout;
			break;
		default :
			delete previewLayout;
			delete histoLayout;
			delete profileLayout;
			break;
	}
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
	histoLayout = new QVBoxLayout();
	profileLayout = new QVBoxLayout();
	
	// Preview ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Preview bounds
	blackPic          = new QImage(400,400,QImage::Format_ARGB32);
	blackPic->fill(qRgb(0,0,0));
	
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
	phantPic      = new QImage(400,400,QImage::Format_ARGB32);
	phant         = new EGSPhant();
	
	phantFrame    = new QFrame();
	phantLayout   = new QGridLayout();
	
	mediaButton   = new QRadioButton("media");
	densityButton = new QRadioButton("density");
	phantSelect   = new QComboBox();
	phantSelect->addItem("none");
	
	densityLabel  = new QLabel("Density range");
	densityMin    = new QLineEdit("0");
	densityMax    = new QLineEdit("3");
	densityMin->setValidator(&allowedPosReals);
	densityMax->setValidator(&allowedPosReals);
	
	mediaButton->setChecked(true);
	densityLabel->setDisabled(true);
	densityMin->setDisabled(true);
	densityMax->setDisabled(true);
	
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
	mapPic        = new QImage(400,400,QImage::Format_ARGB32);	
	mapDose       = new Dose();
	
	mapFrame      = new QFrame();
	mapLayout     = new QGridLayout();
				  
	mapDoseBox    = new QComboBox();
	mapDoseBox->addItem("none");
	
	mapMinLabel   = new QLabel("min");
	mapMaxLabel   = new QLabel("max");
	
	mapMinDose    = new QLineEdit("0");
	mapMaxDose    = new QLineEdit("100");
	mapMinDose->setValidator(&allowedPosReals);
	mapMaxDose->setValidator(&allowedPosReals);
	
	mapMinButton  = new QPushButton();
	mapMidButton  = new QPushButton();
	mapMaxButton  = new QPushButton();
	mapMinButton->setStyleSheet("QPushButton {background-color: rgb(0,0,255)}");
	mapMidButton->setStyleSheet("QPushButton {background-color: rgb(0,255,0)}");
	mapMaxButton->setStyleSheet("QPushButton {background-color: rgb(255,0,0)}");
	
	mapLayout->addWidget(mapDoseBox  , 0, 0, 1, 6);
	mapLayout->addWidget(mapMinLabel , 1, 0, 1, 3);
	mapLayout->addWidget(mapMaxLabel , 1, 3, 1, 3);
	mapLayout->addWidget(mapMinDose  , 2, 0, 1, 3);
	mapLayout->addWidget(mapMaxDose  , 2, 3, 1, 3);
	mapLayout->addWidget(mapMinButton, 3, 0, 1, 2);
	mapLayout->addWidget(mapMidButton, 3, 2, 1, 2);
	mapLayout->addWidget(mapMaxButton, 3, 4, 1, 2);
	
	mapFrame->setLayout(mapLayout);
	mapFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	previewLayout->addWidget(mapFrame);
	
	// Isodose selection
	isoPic        = new QImage(400,400,QImage::Format_ARGB32);	
	
	isoFrame       = new QFrame();
	isoLayout      = new QGridLayout();
	
	isoColourLabel = new QLabel("Colours");
	
	// Lines 1 - 3
	isoDoseLabel.append(new QLabel("solid")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseLabel.append(new QLabel("dashed")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	isoDoseLabel.append(new QLabel("dotted")); isoDoseBox.append(new QComboBox());
	isoDoseBox.last()->addItem("none"); isoDoses.append(new Dose());
	
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
	connect(renderCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(refresh()));
	
	// Preview ~~~~~~~~~~~~~~
	// Dimensions
	connect(xAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	connect(xAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewCanvasRenderLive()));
	connect(yAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	connect(yAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewCanvasRenderLive()));
	connect(zAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewChangeAxis()));
	connect(zAxisButton, SIGNAL(toggled(bool)),
			this, SLOT(previewCanvasRenderLive()));
			
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
			
	// Egsphant
	connect(phantSelect, SIGNAL(currentIndexChanged(int)),
			this, SLOT(previewPhantRenderLive()));
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
			this, SLOT(previewMapRenderLive()));
	connect(mapMinDose, SIGNAL(textEdited(QString)),
			this, SLOT(previewMapRenderLive()));
	connect(mapMaxDose, SIGNAL(textEdited(QString)),
			this, SLOT(previewMapRenderLive()));	
	
	// Isodose
	QVector <QComboBox*>   isoDoseBox;
	QVector <QLineEdit*>   isoColourDose;	
			
	// Change all colours
	QSignalMapper* sigMap = new QSignalMapper(this); // Should get deleted as a child of this
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
			this, SLOT(previewChangeColor(int))) ;
}

// Swap panels
void doseInterface::resetLayout() {
	switch(optionsBox->currentIndex()) {
		case 0 :
			mapDoseBox->setCurrentIndex(0);
			isoDoseBox[0]->setCurrentIndex(0);
			isoDoseBox[1]->setCurrentIndex(0);
			isoDoseBox[2]->setCurrentIndex(0);
			phantSelect->setCurrentIndex(0);
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
	if (mediaButton->isChecked()) {
		densityLabel->setDisabled(true);
		densityMin->setDisabled(true);
		densityMax->setDisabled(true);
	}
	else if (mediaButton->isChecked()) {
		densityLabel->setDisabled(false);
		densityMin->setDisabled(false);
		densityMax->setDisabled(false);
	}
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
	
void doseInterface::previewCanvasRenderLive() {if(renderCheckBox->isChecked()) previewCanvasRender();}
void doseInterface::previewCanvasRender() {
	int width  = abs(horBoundaryMin->text().toDouble() - horBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	int height = abs(vertBoundaryMin->text().toDouble() - vertBoundaryMin->text().toDouble())*resolutionScale->text().toInt();
	delete blackPic;
	blackPic = new QImage(width,height,QImage::Format_ARGB32);
	
	previewRender();
}

void doseInterface::previewPhantRenderLive() {if(renderCheckBox->isChecked()) previewPhantRender();}
void doseInterface::previewPhantRender() {
	QString axis = xAxisButton->isChecked()?"x axis":(yAxisButton->isChecked()?"y axis":"z axis");
	
	if (phantSelect->currentIndex()) {
		if (mediaButton->isChecked()) {
			*phantPic = phant->getEGSPhantPicMed(axis, horBoundaryMin->text().toDouble(), horBoundaryMax->text().toDouble(),
												 vertBoundaryMin->text().toDouble(), vertBoundaryMax->text().toDouble(),
												 depthMin->text().toDouble(), resolutionScale->text().toDouble());
		}
		else if (densityButton->isChecked()) {
			*phantPic = phant->getEGSPhantPicDen(axis, horBoundaryMin->text().toDouble(), horBoundaryMax->text().toDouble(),
												 vertBoundaryMin->text().toDouble(), vertBoundaryMax->text().toDouble(),
												 depthMin->text().toDouble(), resolutionScale->text().toDouble(),
												 densityMin->text().toDouble(), densityMax->text().toDouble());
		}
	}
	
	previewRender();
}

void doseInterface::previewMapRenderLive() {if(renderCheckBox->isChecked()) previewMapRender();}
void doseInterface::previewMapRender() {
	
	
	previewRender();
}

void doseInterface::previewIsoRenderLive() {if(renderCheckBox->isChecked()) previewIsoRender();}
void doseInterface::previewIsoRender() {
	
	
	previewRender();
}

void doseInterface::previewRenderLive() {if(renderCheckBox->isChecked()) previewRender();}
void doseInterface::previewRender() {
	// Choose base canvas
	if (phantSelect->currentIndex())
		*canvasPic = *phantPic;
	else
		*canvasPic = *blackPic;
	
	QPainter paint (canvasPic); // Now use it as our canvas
	
	// Add colour map
	if (mapDoseBox->currentIndex())
		paint.drawImage(0,0,*mapPic);
		
	// Add isodose contours
	if (isoDoseBox[0]->currentIndex()+isoDoseBox[1]->currentIndex()+isoDoseBox[2]->currentIndex())
		paint.drawImage(0,0,*isoPic);
}

void doseInterface::histoRenderLive() {if(renderCheckBox->isChecked()) histoRender();}
void doseInterface::histoRender() {
		
}

void doseInterface::profileRenderLive() {if(renderCheckBox->isChecked()) profileRender();}
void doseInterface::profileRender() {
	
}