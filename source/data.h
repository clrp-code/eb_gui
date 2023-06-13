/*
################################################################################
#
#  egs_brachy_GUI data.h
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
#ifndef DATA_H
#define DATA_H

#include <QtGui>
#include <sstream>

#include "data/DICOM.h"
#include "data/egsphant.h"
#include "data/input.h"
#include "data/dose.h"

// This class holds all the back-end data available to the interface
// and holds many of the backend members for data manipulation
class Data : public QObject {
private:
    Q_OBJECT // This line is necessary to create custom SIGNALs, ie, functions
             // that can notify a change for the GUI
public:
	// locations
	QString hh_location; // Hen House
	QString eh_location; // Egs Home
	
	QString eb_location; // egs_brachy
	QString gui_location; // eb_gui executable
	QString ep_location; // egs-parallel location
	
	// Default egsinp files
	QString muen_location;
	QString material_location;
	QString transport_location;
	
	// Default settings
	QString mar_location;
	QString metric_location;
	
	// egs_brachy parameters from configuration file
	QString def_ncase;
	QString def_nbatch;
	QString def_nchunk;
	QString def_geomLimit;
	
	QString def_AE;
	QString def_UE;
	QString def_AP;
	QString def_UP;
	
	QString def_volCor, def_volDen, def_seedDisc;
	
	// GUI parameters
	int isodoseLineThickness = 2;
	int histogramBinCount = 20;
	
	// egs_brachy library data
	QStringList libNamePhants;
	QStringList libNameSources;
	QStringList libNameTransforms;
	QStringList libNameGeometries;
	QStringList libDirPhants;
	QStringList libDirSources;
	QStringList libDirTransforms;
	QStringList libDirGeometries;
	
	// local GUI data
	QStringList localNamePhants;
	QStringList localNameTransforms;
	QStringList localNameDoses;
	QStringList localDirPhants;
	QStringList localDirTransforms;
	QStringList localDirDoses;
	
	// tissue assignment schemes (TAS)
	QStringList TAS_names;
	QVector <QVector <QString> > media; // Media names in TAS
	QVector <QVector <double> > threshold; // Density thresholds for TAS
	
	// metallic artifact reduction
	bool do_MAR = false;
	double lowerThresh, upperThresh, marDen, marRad;
	int marContourInd;
	QString marContour, transformFile;
	
	// metric extraction (name, prescription, Dx (%), Dx (cc), Vx (%))
	QStringList metricNames, metricDp, metricDx, metricDcc, metricVx;
	
	// functions
	int loadDefaults();
	
	// destructor
	~Data();
	
	// DICOM data
	database tag_data; // Holds DICOM tag info fetched from:
                       // https://www.dicomlibrary.com/dicom/dicom-tags/
	
	QVector <DICOM*> CT_data; // Holds all CT phantoms
	DICOM* struct_data = 0; // Holds the structure data
	bool struct_loaded = false;
	DICOM* plan_data = 0; // Holds the plan data
	bool plan_loaded = false;
	
	QString hu_location; // File holding the default conversion of HU to density
	
	// DICOM struct/contour data arrays
	QVector <QVector <QPolygonF> > structPos; // Holds actual contour points per slice
	QVector <QVector <double> > structZ; // Holds contour z positions
	QVector <QString> structName; // Holds contour name
	
	// Plan data to import from DICOM
	QString treatmentType; // MANUAL, HDR, MDR, LDR, or PDR
	QString treatmentTechnique; // INTRALUMENARY, INTRACAVITARY, INTERSTITIAL,
								// CONTACT, INTRAVASCULAR, or PERMANENT

	QString seedInfo; // Found seed info

	double airKerma, halfLife; // Air kerma from dicom, half_life
	QString isotopeName; // Corresponds

	double treatmentTime, longestTime; // Total treatment time, longest seed time
	QVector <QVector3D> seedPos; // Transformation file
	QVector <double> seedTime; // All dwell times
	
public:
	// Build egsphant
	int buildEgsphant(EGSPhant* phant, QString* log, int contourNum, int defaultTAS,
					  QVector <int>* structIndex, QVector <int>* tasIndex,
					  QVector <EGSPhant*>* makeMasks, double buffer = -1);
	
	double interp(double x, double x1, double x2, double y1, double y2);
	
	// Parse plan file
	int parsePlan(QString* log);
	
	// Output RT dose
	int outputRTDose(QString dosePath, QString errorPath, Dose* output, QString doseScaling = "", double n = 1);
	
	// signal used for the progress bar
signals:
	void newProgress(QString title);
	void madeProgress(double percent);
	void completedProgress();
	void newProgressName(QString text);
};
#endif
