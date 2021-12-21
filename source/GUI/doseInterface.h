/*
################################################################################
#
#  egs_brachy_GUI doseInterface.h
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

#ifndef DOSEINTERFACE_H
#define DOSEINTERFACE_H

#include <QtGui>
#include <QtCharts>
#include <iostream>
#include "../interface.h"

// Forward declaration of Interface to pass to the tab windows
class Interface;

// Declaration of the Main Window class, its variables and its methods
class doseInterface : public QWidget {
private:
    Q_OBJECT // This line is necessary to create custom SLOTs, ie, functions
    // that define what happens when you click on buttons
public:
    doseInterface();
    doseInterface(Interface* p);
    ~doseInterface();

    Interface *parent; // Pointer to the parent to access data with
	
	QRegExpValidator allowedReals;
	QRegExpValidator allowedPosReals;
	QRegExpValidator allowedPosRealArrs;
	QRegExpValidator allowedNats;
	QRegExpValidator allowedPercents;
	QRegExpValidator allowedPercentArrs;
	
	// Shared objects
	QScrollArea *canvasArea;
	QLabel      *canvas;
	QChartView  *canvasChart;
	QImage      *canvasPic;
	QPushButton *saveDataButton;
	QPushButton *saveImageButton;
	QLabel      *canvasInfo;
				
	QFrame      *rendering;
	QGridLayout *renderingLayout;
	QPushButton *renderButton;
	QCheckBox   *renderCheckBox;
	QLabel      *resolutionLabel;
	QLineEdit   *resolutionScale;
	
	QVBoxLayout *previewLayout;
	QFrame      *previewFrame;
	QVBoxLayout *histoLayout;
	QFrame      *histoFrame;
	QVBoxLayout *profileLayout;
	QFrame      *profileFrame;
	
	QTabWidget  *optionsTab;
	
	// Saving plot data for output later
	QList <QString>          savePlotName;
	QString					 savePlotX;
	QString					 savePlotY;
	QList <QList <QPointF> > savePlotData; // Used for later output
	
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                                Preview                              //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Preview bounds
	QImage       *blackPic;
	
	QFrame       *dimFrame;
	QGridLayout  *dimLayout;
	
	QRadioButton *xAxisButton;
	QRadioButton *yAxisButton;
	QRadioButton *zAxisButton;
	
	QLabel       *vertBoundaryLabel;
	QLineEdit    *vertBoundaryMin;
	QLineEdit    *vertBoundaryMax;
	
	QLabel       *horBoundaryLabel;
	QLineEdit    *horBoundaryMin;
	QLineEdit    *horBoundaryMax;
	
	QLabel       *depthLabel;
	QLineEdit    *depthMin;
	
	QPushButton  *expandToBounds;
	QLabel       *legendLabel;
	QComboBox    *legendBox;
	QLabel       *unitsLabel;
	QLineEdit    *unitsEdit;
	
	// Phantom selection
	QImage       *phantPic;
	EGSPhant	 *phant;
	QLabel       *phantLabel;
	
	QFrame       *phantFrame;
	QGridLayout  *phantLayout;
	
	QComboBox    *phantSelect;
	
	QRadioButton *mediaButton;
	QRadioButton *densityButton;
	
	QLabel       *densityLabel;
	QLineEdit    *densityMin;
	QLineEdit    *densityMax;
	
	// Map selection
	QImage      *mapPic;
	Dose		*mapDose;
	QLabel      *mapLabel;
	
	QFrame      *mapFrame;
	QGridLayout *mapLayout;
			    
	QComboBox   *mapDoseBox;
				    
	QLabel      *mapMinLabel;
	QLabel      *mapMaxLabel;
	QLineEdit   *mapMinDose;
	QLineEdit   *mapMaxDose;
	QPushButton *mapMinButton;
	QPushButton *mapMidButton;
	QPushButton *mapMaxButton;
	
	QLabel      *mapOpacLabel;
	QSlider     *mapOpacSlider;	
	
	// Isodose selection
	QVector <QVector <QLineF> > solid;
	QVector <QVector <QLineF> > dashed;
	QVector <QVector <QLineF> > dotted;
	
	QFrame                 *isoFrame;
	QGridLayout            *isoLayout;
	
	QVector <QLabel*>	   isoDoseLabel;
	QVector <QComboBox*>   isoDoseBox;
	
	QVector <Dose*>		   isoDoses;
			               
	QLabel                 *isoColourLabel;
	QVector <QLineEdit*>   isoColourDose;
	QVector <QPushButton*> isoColourButton;
	
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                               Histogram                             //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	// Phantom selection
	QLabel      *histPhantLabel;
	EGSPhant	*histPhant;
	QComboBox   *histPhantSelect;
	
	QFrame      *histPhantFrame;
	QGridLayout *histPhantLayout;
	
	// Filters
	QLabel      *histFilterLabel;
	
	QLabel      *histMaskLabel;
	QComboBox   *histMaskSelect;
	EGSPhant	*histMask;
	
	QStringList localNameMasks;
	QStringList localDirMasks;
	
	QLabel      *histMediumLabel;
	QListWidget *histMediumView;
	
	QLabel      *histDoseMinLabel;
	QLineEdit   *histDoseMinEdit;
	QLabel      *histDoseMaxLabel;
	QLineEdit   *histDoseMaxEdit;
	
	QFrame      *histFilterFrame;
	QGridLayout *histFilterLayout;
	
	// Doses
	QLabel          *histDosesLabel;
	QPushButton     *histLoadButton;
	QComboBox   	*histDoseSelect;
	QPushButton     *histDeleteButton;
	
	QVector <Dose*> histDoses;
	
	QListWidget     *histLoadedView;
	
	QCheckBox       *histLegendBox;
	QCheckBox       *histDiffBox;
				    
	QFrame          *histDosesFrame;
	QGridLayout     *histDosesLayout;
	
	// Added data
	QLabel      *histOutputLabel;
	
	QLabel      *histDxLabel;
	QLineEdit   *histDxEdit;
	QLabel      *histVxLabel;
	QLineEdit   *histVxEdit;
	
	QPushButton *histCalcButton;
	QPushButton *histSaveButton;
				    
	QFrame      *histOutputFrame;
	QGridLayout *histOutputLayout;
	
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//                                Profile                              //
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
	
	
public slots:
	// GUI functions
	// All render commands, with a live wrapper function.  All changes (outside of the render buttons)
	// are connected to the live functions, so they only render the image when live rendering is checked
	void render();
	void saveImage();
	void saveData();
	
	// Preview
    void previewRenderLive();
    void previewRender();
    void previewCanvasRender(); // Change of dimensions
    void previewCanvasRenderLive();
    void previewPhantRender(); // Change of egsphant, change of media/density, change of density range
    void previewPhantRenderLive();
    void previewMapRender(); // Change of dose, change of values, change of colours
    void previewMapRenderLive();
    void previewIsoRender(); // Change of doses, changes of values, change of colours
    void previewIsoRenderLive();
	
	void previewChangeAxis();
	void previewChangeColor(int i);
	void previewResetBounds();
	
	void loadEgsphant();
	void loadMapDose();
	void loadIsoDose(int i);
	
	// Histogram
    void histoRenderLive();
    void histoRender();
	
	void loadFilterEgsphant();
	void loadMaskEgsphant();
	
	void loadHistoDose();
	void deleteHistoDose();
	
	// Profile
    void profileRenderLive();
    void profileRender();
	
	// Use this when someone navigates away from the tab to avoid crashing
	void resetLayout();
	void resetDoses();

public:
// LAYOUT~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    QGridLayout *mainLayout;
    QGridLayout *bufferLayout; // A second layout to hold the unused canvas/chartview

    // Generic GUI methods
    void createLayout();
    void connectLayout();
	
public slots:
    void refresh(); // Reset all on screen values properly
    void tabSwap(); // Reset all on screen values properly
    void previewRefresh();
    void histoRefresh();
    void profileRefresh();
};

#endif
