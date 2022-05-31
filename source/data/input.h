/*
################################################################################
#
#  egs_brachy_GUI input.h
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
#ifndef INPUT_H
#define INPUT_H

#include <QtWidgets>
#include <iostream>
#include <math.h>

class EGSInput : public QObject {
public:
	// Get all egsinp variables here
	// run control
	QString RC_ncase, RC_nbatch, RC_nchunk, RC_geomLimit, RC_calculation, RC_egsdatFormat;
	
	// run mode
	QString RM_normal;
	
	// media definition
	QString Med_AE, Med_UE, Med_AP, Med_UP, Med_file;
	
	// geometry definition
	QString phantomFile, sourceGeomFile;
	QVector <QString> additional_geomNames;
	QVector <QString> additional_geomFiles;
	QVector <QString> prio;
	QVector <QVector3D> pos, rot;
	QString discoverPoints;
	
	// volume correction
	QString VC_density, VC_type, VC_bound, VC_thresh;
	
	// source definition
	QString sourceTransFile, sourceSpecFile, sourceShapeFile, sourceSeedFile, sourceDwells;
	
	// scoring options
	QString SO_edep, SO_muenFile, SO_muenMed, SO_scale;
	
	// transport parameters
	QString TP_file;
	
	// Functions
	QString buildGeom();
	QString buildInput();
	
	EGSInput();
};

#endif
