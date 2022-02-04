/*
################################################################################
#
#  egs_brachy_GUI egsphant.h
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
#ifndef EGSPHANT_H
#define EGSPHANT_H

#include <QtWidgets>
#include <iostream>
#include <math.h>
#include "libraries/gzstream.h"

class EGSPhant : public QObject {
    Q_OBJECT

signals:
	void madeProgress(double percent); // Update the progress bar

public:
    EGSPhant();

    int nx, ny, nz; // these hold the number of voxels
    QVector <double> x, y, z; // these hold the boundaries of the above voxels
    QVector <QVector <QVector <char> > > m; // this holds all the media
    QVector <QVector <QVector <double> > > d; // this holds all the densities
    QVector <QString> media; // this holds all the possible media
    double maxDensity;
	
    void loadEGSPhantFile(QString path);
    void loadEGSPhantFilePlus(QString path);
    void loadbEGSPhantFile(QString path);
    void loadbEGSPhantFilePlus(QString path);
    void loadgzEGSPhantFile(QString path);
	void loadgzEGSPhantFilePlus(QString path);
	
    void savegzEGSPhantFile(QString path);
	void savegzEGSPhantFilePlus(QString path);
	
	void setDensity(int px, int py, int pz, double density);
	
	void redefineBounds(double xi, double yi, double zi, double xf, double yf, double zf);
	
	void makeMask(EGSPhant* mask); // Useful for later analysis

    char getMedia(double px, double py, double pz);
    double getDensity(double px, double py, double pz);
    double getDensity(int px, int py, int pz);
    int getIndex(QString axis, double p);
	
    QImage getEGSPhantPicDen(QString axis, double ai, double af,
                             double bi, double bf, double d, int res,
							 double di, double df);
    QImage getEGSPhantPicMed(QString axis, double ai, double af,
                             double bi, double bf, double d, int res);
};

#endif
