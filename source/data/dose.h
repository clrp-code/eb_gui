/*
################################################################################
#
#  egs_brachy_GUI dose.h
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

#ifndef DOSE_H
#define DOSE_H

#include "egsphant.h"

// This structure holds a dose and a volume for basic DVH construction
struct DV {
    double dose;
    double vol;
};

// This structure holds a dose and a volume, as well as mask & media data
struct DVplus {
    double dose;
    double vol;
	char   m;      // medium
	bool   inMask; // returns whether it's in the mask or not
};

class Dose : public QObject {
    Q_OBJECT

signals:
    void madeProgress(double n); // Update the progress bar

public:
    // The constructor uses the n to determine how many .3ddose files will be
    // read in, so as to progress the progress bar accordingly
    Dose(QString path = "", int n = 0);
    Dose(const Dose &d); // Copy constructor, used to make a deep copy
    ~Dose();

    int x, y, z; // The number of x, y and z voxels
    QVector <double> cx, cy, cz; // The actual x, y and z coordinates
    QVector < QVector < QVector <double> > > val; // The values
    QVector < QVector < QVector <double> > > err; // The fractional errors
	
    // Interpolate the dose and error of the point (xp, yp, zp), function passes
    // value to val and error to err, and return val
    double triInterpol(double xp, double yp, double zp, double *val,
                       double *err);

    // Read in a .3ddose file, again with the n to be used by the progress bar
    void readIn(QString path, int n);
    void readBIn(QString path, int n);

    // Save data as a .3ddose file, again n to be used by the progress bar
    void readOut(QString path, int n);
    void readBOut(QString path, int n);

    // Translate the origin by dx, dy and dz
    int translate(double dx, double dy, double dz);

    // Remove the outer layers of voxels
    int strip();

    // Returns the index of the coordinate matrix at val
    int getIndex(QString axis, double val);

    // These functions return dose at a point in real space or at an index
    double getDose(double px, double py, double pz);
    double getError(double px, double py, double pz);
    double getDose(int ix, int iy, int iz);
    double getError(int ix, int iy, int iz);

    // Returns the max dose of this
    double getMax();

    // Get isodose points
    void getContour(QVector <QVector <QLineF> > *con, QVector <double> doses,
                    QString axis, double depth, double ai, double af,
                    double bi, double bf, int res);
    int interp(int x1, int x2, double y1, double y2, double y0);

	// Get colourmap
	QImage getColourMap(QString axis, double ai, double af, double bi, double bf, double d, int res,
						double di, double df, QColor min, QColor mid, QColor max);
	
	// Get sorted dose data for making DVH plots and tally volume
	void getDV(QList <DV> *data, double* volume, int n = 1);
	void getDV(QList <DV> *data, EGSPhant* media, QString allowedChars, double* volume, int n = 1);
	void getDV(QList <DV> *data, EGSPhant* mask, double* volume, int n = 1);
	void getDV(QList <DV> *data, EGSPhant* media, QString allowedChars, EGSPhant* mask, double* volume, int n = 1);
	
	int binarySearch(DV datum, QList <DV> *data, int l, int r);
	
	// Get sorted dose data for making DVH plots and tally volume
	//void getDVplus(QList <DVplus> *data, double* volume);
	//void getDVplus(QList <DVplus> *data, EGSPhant* media, QString allowedChars, double* volume);
	//void getDVplus(QList <DVplus> *data, EGSPhant* mask, double* volume);
	//void getDVplus(QList <DVplus> *data, EGSPhant* media, QString allowedChars, EGSPhant* mask, double* volume);
	
	//int binarySearch(DVplus datum, QList <DVplus> *data, int l, int r);
};

#endif
