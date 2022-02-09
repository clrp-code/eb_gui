/*
################################################################################
#
#  egs_brachy_GUI dose.cpp
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

#include "dose.h"

// Comparison function for std containers
bool DV_sorter(const DV& a, const DV& b) {
	return a.dose < b.dose;
}

Dose::Dose(const Dose &d)
    : QObject(0) {
    // Set the number of coordinates the same
    x = d.x;
    y = d.y;
    z = d.z;

    // Go through and fill the same boundaries
    for (int i = 0; i <= x; i++) {
        cx.append(d.cx[i]);
    }
    for (int i = 0; i <= y; i++) {
        cy.append(d.cy[i]);
    }
    for (int i = 0; i <= z; i++) {
        cz.append(d.cz[i]);
    }

    // Go through and fill the same doses and errors
    val.resize(x);
    err.resize(x);
    for (int i = 0; i < d.x; i++) {
        val[i].resize(y);
        err[i].resize(y);
        for (int j = 0; j < d.y; j++) {
            val[i][j].resize(z);
            err[i][j].resize(z);
            for (int k = 0; k < d.z; k++) {
                val[i][j][k] = d.val[i][j][k];
                err[i][j][k] = d.err[i][j][k];
            }
        }
    }
}

Dose::Dose(QString path, int n)
    : QObject(0) {
	if (!n) {
		// do nothing
	}
    if (path.endsWith(".b3ddose")) {
        readBIn(path, n);
    }
    else if (path.endsWith(".3ddose")) {
        readIn(path, n);
    }
}

Dose::~Dose() {
	
}

double Dose::triInterpol(double xp, double yp, double zp, double *val,
                         double *err) {
    // Convert real numbers to indices to see if we are in the phantom
    int xi = getIndex("X", xp);
    int yi = getIndex("Y", yp);
    int zi = getIndex("Z", zp);
    if (zi == -1 || yi == -1 || xi == -1) {
        return *val = *err = -1; // If outside of bounds, return -1
    }
							 
    // All the positions needed (in addition to the ones passed in)
    double x0, y0, z0, x1, y1, z1;

    // Define X
    if (xp < (cx[xi] + cx[xi-1])/2.0) {
        x0 = (cx[xi] + cx[xi-1])/2.0;
        x1 = (cx[xi] + cx[xi+1])/2.0;
    }
    else {
        x0 = (cx[xi] + cx[xi+1])/2.0;
        x1 = (cx[xi+1] + cx[xi+2])/2.0;
    }

    // Define Y
    if (yp < (cy[yi] + cy[yi-1])/2.0) {
        y0 = (cy[yi] + cy[yi-1])/2.0;
        y1 = (cy[yi] + cy[yi+1])/2.0;
    }
    else {
        y0 = (cy[yi] + cy[yi+1])/2.0;
        y1 = (cy[yi+1] + cy[yi+2])/2.0;
    }

    // Define Z
    if (zp < (cz[zi] + cz[zi-1])/2.0) {
        z0 = (cz[zi] + cz[zi-1])/2.0;
        z1 = (cz[zi] + cz[zi+1])/2.0;
    }
    else {
        z0 = (cz[zi] + cz[zi+1])/2.0;
        z1 = (cz[zi+1] + cz[zi+2])/2.0;
    }

    // Precompute lengths
    double ix = xp-x0, fx = x1-xp, iy = yp-y0, fy = y1-yp, iz = zp-z0, fz = z1-zp;
    double vol = 0, weight = 0;

    // Add all 8 values together, weighted by the volume of the rectangular
    // prism formed by the point that was passed in and the one in the opposite
    // corner of the total rectangular prism volume
    *val = *err = 0;

    vol = fx*fy*fz;
    weight += vol*vol;
    *val += getDose(x0,y0,z0)*vol;
    *err += pow(getDose(x0,y0,z0)*getError(x0,y0,z0)*vol,2);

    vol = ix*fy*fz;
    weight += vol*vol;
    *val += getDose(x1,y0,z0)*vol;
    *err += pow(getDose(x1,y0,z0)*getError(x1,y0,z0)*vol,2);

    vol = fx*iy*fz;
    weight += vol*vol;
    *val += getDose(x0,y1,z0)*vol;
    *err += pow(getDose(x0,y1,z0)*getError(x0,y1,z0)*vol,2);

    vol = fx*fy*iz;
    weight += vol*vol;
    *val += getDose(x0,y0,z1)*vol;
    *err += pow(getDose(x0,y0,z1)*getError(x0,y0,z1)*vol,2);

    vol = fx*iy*iz;
    weight += vol*vol;
    *val += getDose(x0,y1,z1)*vol;
    *err += pow(getDose(x0,y1,z1)*getError(x0,y1,z1)*vol,2);

    vol = ix*fy*iz;
    weight += vol*vol;
    *val += getDose(x1,y0,z1)*vol;
    *err += pow(getDose(x1,y0,z1)*getError(x1,y0,z1)*vol,2);

    vol = ix*iy*fz;
    weight += vol*vol;
    *val += getDose(x1,y1,z0)*vol;
    *err += pow(getDose(x1,y1,z0)*getError(x1,y1,z0)*vol,2);

    vol = ix*iy*iz;
    weight += vol*vol;
    *val += getDose(x1,y1,z1)*vol;
    *err += pow(getDose(x1,y1,z1)*getError(x1,y1,z1)*vol,2);

    *val /= (x1-x0)*(y1-y0)*(z1-z0);
    *err = sqrt(*err/weight)/(*val);

    return *val;
}

void Dose::readIn(QString path, int n) {
    // Open the .3ddose file
    QFile *file;
    QTextStream *input;
    file = new QFile(path);

    // Determine the increment size of the status bar this 3ddose file gets
    double increment = 100.0/double(n);

    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        input = new QTextStream(file);

        emit madeProgress(increment*0.005); // Update progress bar
		
        // Read in the number of voxels
        *input >> x;
        *input >> y;
        *input >> z;

        // Resize x appropriately
        cx.resize(x+1);
        val.resize(x);
        err.resize(x);


        // Resize y appropriately
        cy.resize(y+1);
        for (int i = 0; i < x; i++) {
            val[i].resize(y);
            err[i].resize(y);
        }

        // Resize z appropriately
        cz.resize(z+1);
        for (int i = 0; i < x; i++)
            for (int j = 0; j < y; j++) {
                val[i][j].resize(z);
                err[i][j].resize(z);
            }

        emit madeProgress(increment*0.01); // Update progress bar

        // Read in boundaries
        for (int i = 0; i <= x; i++) {
            *input >> cx[i];
        }
        for (int j = 0; j <= y; j++) {
            *input >> cy[j];
        }
        for (int k = 0; k <= z; k++) {
            *input >> cz[k];
        }

        emit madeProgress(increment*0.01); // Update progress bar

        // Resize increments so that the rest of the section of the progress
        // bar is divided by two times the number of z values, so that each
        // time the loops iterate througha new z, the progress bar updates
        increment *= 0.975;
        increment = increment/(2*z);

        // Read in all the doses
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input >> val[i][j][k];
                }

            emit madeProgress(increment); // Update progress bar
        }

        // Read in all the errors
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input >> err[i][j][k];
                }

            emit madeProgress(increment); // Update progress bar
        }

        delete input;
    }
    delete file;
}

void Dose::readBIn(QString path, int n) {
    // Open the .3ddose file
    QFile *file;
    QDataStream *input;
    file = new QFile(path);

    // Determine the increment size of the status bar this 3ddose file gets
    double increment = 100.0/double(n);

    if (file->open(QIODevice::ReadOnly)) {
        input = new QDataStream(file);
		input->setByteOrder(QDataStream::LittleEndian);

        emit madeProgress(increment*0.005); // Update progress bar

        // Insure XYZ format
        unsigned char temp;
        *input >> temp;
        if (temp != 1) {
            delete input;
            delete file;
            return;
        }

        // Read in the number of voxels
        *input >> x;
        *input >> y;
        *input >> z;

        // Resize x appropriately
        cx.resize(x+1);
        val.resize(x);
        err.resize(x);

        // Resize y appropriately
        cy.resize(y+1);
        for (int i = 0; i < x; i++) {
            val[i].resize(y);
            err[i].resize(y);
        }

        // Resize z appropriately
        cz.resize(z+1);
        for (int i = 0; i < x; i++)
            for (int j = 0; j < y; j++) {
                val[i][j].resize(z);
                err[i][j].resize(z);
            }

        emit madeProgress(increment*0.01); // Update progress bar

        // Read in boundaries
        for (int i = 0; i <= x; i++) {
            *input >> cx[i];
        }
        for (int j = 0; j <= y; j++) {
            *input >> cy[j];
        }
        for (int k = 0; k <= z; k++) {
            *input >> cz[k];
        }

        emit madeProgress(increment*0.01); // Update progress bar

        // Resize increments so that the rest of the section of the progress
        // bar is divided by two times the number of z values, so that each
        // time the loops iterate througha new z, the progress bar updates
        increment *= 0.975;
        increment = increment/(2*z);

        // Read in all the doses
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input >> val[i][j][k];
                }

            emit madeProgress(increment); // Update progress bar
        }

        // Read in all the errors
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input >> err[i][j][k];
                }

            emit madeProgress(increment); // Update progress bar
        }

        delete input;
    }
    delete file;
}

void Dose::readOut(QString path, int n) {
    // This function prints out a file in the standard 3ddose format
    QFile *file;
    QTextStream *input;
    file = new QFile(path);

    // Determine the increment size of the status bar this 3ddose file gets
    double increment = 100.0/double(n);

    if (file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        input = new QTextStream(file);

        emit madeProgress(increment*0.005); // Update progress bar

        // Print the number of voxels
        *input << QString::number(x) << tr(" ");
        *input << QString::number(y) << tr(" ");
        *input << QString::number(z) << tr("\n");

        emit madeProgress(increment*0.01); // Update progress bar

        // Print the boundaries
        for (int i = 0; i <= x; i++) {
            *input << QString::number(cx[i]) << tr(" ");
        }
        *input << tr("\n");

        for (int j = 0; j <= y; j++) {
            *input << QString::number(cy[j]) << tr(" ");
        }
        *input << tr("\n");

        for (int k = 0; k <= z; k++) {
            *input << QString::number(cz[k]) << tr(" ");
        }
        *input << tr("\n");

        emit madeProgress(increment*0.01); // Update progress bar

        // Resize increments so that the rest of the section of the progress
        // bar is divided by two times the number of z values, so that each
        // time the loops iterate througha new z, the progress bar updates
        increment *= 0.975;
        increment = increment/(2*z);

        // Read out doses
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input << QString::number(val[i][j][k]) << tr(" ");
                }
            emit madeProgress(increment); // Update progress bar
        }
        *input << tr("\n");

        // Read out errors
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input << QString::number(err[i][j][k]) << tr(" ");
                }
            emit madeProgress(increment); // Update progress bar
        }
        *input << tr("\n\n");

        delete input;
    }
    delete file;
}

void Dose::readBOut(QString path, int n) {
    // This function prints out a file in the standard 3ddose format
    QFile *file;
    QDataStream *input;
    file = new QFile(path);

    // Determine the increment size of the status bar this 3ddose file gets
    double increment = 100.0/double(n);

    if (file->open(QIODevice::WriteOnly)) {
        input = new QDataStream(file);
		input->setByteOrder(QDataStream::LittleEndian);

        emit madeProgress(increment*0.005); // Update progress bar

        *input << (unsigned char)(1);
        // Print the number of voxels
        *input << x << y << z;

        emit madeProgress(increment*0.01); // Update progress bar

        // Print the boundaries
        for (int i = 0; i <= x; i++) {
            *input << cx[i];
        }

        for (int j = 0; j <= y; j++) {
            *input << cy[j];
        }

        for (int k = 0; k <= z; k++) {
            *input << cz[k];
        }

        emit madeProgress(increment*0.01); // Update progress bar

        // Resize increments so that the rest of the section of the progress
        // bar is divided by two times the number of z values, so that each
        // time the loops iterate througha new z, the progress bar updates
        increment *= 0.975;
        increment = increment/(2*z);

        // Read out doses
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input << val[i][j][k];
                }
            emit madeProgress(increment); // Update progress bar
        }

        // Read out errors
        for (int k = 0; k < z; k++) {
            for (int j = 0; j < y; j++)
                for (int i = 0; i < x; i++) {
                    *input << err[i][j][k];
                }
            emit madeProgress(increment); // Update progress bar
        }

        delete input;
    }
    delete file;
}

int Dose::translate(double dx, double dy, double dz) {
    // Change all the values within cx, cy and cz appropriately
    for (int i = 0; i <= x; i++) {
        cx[i] += dx;
    }
    for (int i = 0; i <= y; i++) {
        cy[i] += dy;
    }
    for (int i = 0; i <= z; i++) {
        cz[i] += dz;
    }

    return 1;
}

int Dose::strip() {
    if (x <= 2 || y <= 2 || z <= 2) { // Don't strip if nothing will be left
        return 0;
    }

    // Remove the first and last positions in the coordinates matrices
    cx.remove(x);
    cy.remove(y);
    cz.remove(z);
    cx.remove(0);
    cy.remove(0);
    cz.remove(0);

    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            // Remove the first and last z value of each (x,y) line
            val[i][j].remove(z-1);
            val[i][j].remove(0);
            err[i][j].remove(z-1);
            err[i][j].remove(0);
        }
        // Remove the first and last y value of each x line
        val[i].remove(y-1);
        val[i].remove(0);
        err[i].remove(y-1);
        err[i].remove(0);
    }
    // Remove the first and last x values
    val.remove(x-1);
    val.remove(0);
    err.remove(x-1);
    err.remove(0);

    // Resize the variables that keep track of size
    x -= 2;
    y -= 2;
    z -= 2;

    return 1; // Success
}

int Dose::getIndex(QString axis, double val) {
    // This algorithm checks to see if val is within the outer bounds of axis'
    // coord array, then checks from c[0] to c[x] until it finds a c[n] smaller
    // than val and returns it's index

    int index = -1;
    if (!axis.compare("X")) {
        if (val <= cx[0] || val >= cx[x]) {
            return index;
        }

        for (int i = 0; i <= x; i++)
            if (cx[i] <= val) {
                index = i;
            }
    }
    else if (!axis.compare("Y")) {
        if (val <= cy[0] || val >= cy[y]) {
            return index;
        }

        for (int i = 0; i <= y; i++)
            if (cy[i] <= val) {
                index = i;
            }
    }
    else if (!axis.compare("Z")) {
        if (val <= cz[0] || val >= cz[z]) {
            return index;
        }

        for (int i = 0; i <= z; i++)
            if (cz[i] <= val) {
                index = i;
            }
    }
    return index; // Will return -1 on failure to find
}

double Dose::getDose(int ix, int iy, int iz) {
    if (iz <= -1 || iy <= -1 || ix <= -1 ||
            iz >= z  || iy >= y  || ix >= x) {
        return -1;    // If outside of bounds, return -1
    }

    return val[ix][iy][iz];
}

double Dose::getError(int ix, int iy, int iz) {
    if (iz <= -1 || iy <= -1 || ix <= -1 ||
            iz >= z  || iy >= y  || ix >= x) {
        return -1;    // If outside of bounds, return -1
    }

    return err[ix][iy][iz];
}

double Dose::getDose(double px, double py, double pz) {
    // Convert real numbers to indices and return dose at index
    int ix = getIndex("X", px);
    int iy = getIndex("Y", py);
    int iz = getIndex("Z", pz);
    if (iz == -1 || iy == -1 || ix == -1) {
        return -1;    // If outside of bounds, return -1
    }

    return val[ix][iy][iz];
}

double Dose::getError(double px, double py, double pz) {
    // Convert real numbers to indices and return error at index
    int ix = getIndex("X", px);
    int iy = getIndex("Y", py);
    int iz = getIndex("Z", pz);
    if (iz == -1 || iy == -1 || ix == -1) {
        return -1;    // If outside of bounds, return -1
    }

    return err[ix][iy][iz];
}

double Dose::getMax() {
    double max = val[0][0][0];

    // Iterate through all dose to get largest and smallest dose
    for (int k = 0; k < z; k++)
        for (int j = 0; j < y; j++)
            for (int i = 0; i < x; i++)
                if (val[i][j][k] > max) {
                    max = val[i][j][k];
                }

    return max;
}

int Dose::scale(double factor) {
    if (factor <= 0) { // Scale by positive non-zero number only
        return 0;
    }

    for (int k = 0; k < z; k++)
        for (int j = 0; j < y; j++)
            for (int i = 0; i < x; i++) {
                val[i][j][k] *= factor;    // Multiply each value by factor
            }

    // Since error is fractional, it does not change
    return 1;
}

// Run through all 3ddose values and create lines for each pixel
void Dose::getContour(QVector <QVector <QLineF> > *con,
                      QVector <double> doses, QString axis, double depth,
                      double ai, double af, double bi, double bf,
                      int res) {
    // Use marching squares to determine lines of the contour by looking at
    // rectangles formed by 4 neighbouring doses and determining the contour
    // lines going through the rectangle

    // Find the slice to be used
    con->clear();
    con->resize(doses.size());
    int n = getIndex(axis, depth);
    if (n == -1) {
        return;
    }
	
    int cases = 0, tl = 0, tr = 0, bl = 0, br = 0;
    int fa, fb, flag = false;
    QVector <int> px, py; // Hold voxel midpoint values
    QVector <QVector <double> > d;
    QVector <double> temp;

    px.clear();
    py.clear();
	
    // Setup dose and pixel arrays
    if (!axis.compare("X")) {
        for (int i = 0; i < y; i++)
            if (cy[i] > bi && cy[i+1] < bf) {
                px.append(int(((cy[i]+cy[i+1])/2.0-bi)*double(res)));
                temp.clear();
                for (int j = 0; j < z; j++)
                    if (cz[j] > ai && cz[j+1] < af) {
                        temp.append(val[n][i][j]);
                        if (!flag)
                            py.append(int(((cz[j]+cz[j+1])/2.0-ai)*double(res)));
                    }
                flag++;
                d.append(temp);
            }
    }
    else if (!axis.compare("Y")) {
        for (int i = 0; i < x; i++)
            if (cx[i] > bi && cx[i+1] < bf) {
                px.append(int(((cx[i]+cx[i+1])/2.0-bi)*double(res)));
                temp.clear();
                for (int j = 0; j < z; j++)
                    if (cz[j] > ai && cz[j+1] < af) {
                        temp.append(val[i][n][j]);
                        if (!flag)
                            py.append(int(((cz[j]+cz[j+1])/2.0-ai)*double(res)));
                    }
                flag++;
                d.append(temp);
            }
    }
    else if (!axis.compare("Z")) {
        for (int i = 0; i < x; i++)
            if (cx[i] > bi && cx[i+1] < bf) {
                px.append(int(((cx[i]+cx[i+1])/2.0-bi)*double(res)));
                temp.clear();
                for (int j = 0; j < y; j++)
                    if (cy[j] > ai && cy[j+1] < af) {
                        temp.append(val[i][j][n]);
                        if (!flag)
                            py.append(int(((cy[j]+cy[j+1])/2.0-ai)*double(res)));
                    }
                flag++;
                d.append(temp);
            }
    }
	
    for (int i = 0; i < px.size()-1; i++)
        for (int j = 0; j < py.size()-1; j++)
            for (int p = 0; p < doses.size(); p++) {
                // Cases tells us the number of vertices above the
                // contour dose
                // Bottom Left
                cases  = bl = (d[i][j] > doses[p]);
                // Bottom Right
                cases += br = (d[i+1][j] > doses[p]);
                // Top Left
                cases += tl = (d[i][j+1] > doses[p]);
                // Top Right
                cases += tr = (d[i+1][j+1] > doses[p]);

                if (cases == 1) { // We have 1 vertex within contour, use linear
                    // interpolation to determine line
                    if (bl) {
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i], fb));
                    }
                    else if (br) {
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i+1], fb));
                    }
                    else if (tl) {
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i], fb));
                    }
                    else if (tr) {
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i+1], fb));
                    }
                }
                else if (cases == 2) { // We have 2 vertices within contour, use
                    // linear interpolation to determine line
                    // The first two are simple single line cases
                    if ((bl && br) || (tl && tr)) { // Horizontal line
                        fa = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(px[i], fa, px[i+1], fb));
                    }
                    else if ((bl && tl) || (br && tr)) { // Vertical line
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], fb, py[j+1]));
                    }
                    else if ((bl && tr)) { // First ambiguous case
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i], fb));
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i+1], fb));
                    }
                    else if ((br && tl)) { // Second ambiguous case
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i+1], fb));
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i], fb));
                    }
                }
                else if (cases == 3) { // We have 3 vertices within contour, use
                    // linear interpolation to determine line
                    if (!bl) {
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i], fb));
                    }
                    else if (!br) {
                        fa = interp(px[i], px[i+1], d[i][j], d[i+1][j], doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j], px[i+1], fb));
                    }
                    else if (!tl) {
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1], doses[p]);
                        fb = interp(py[j], py[j+1], d[i][j], d[i][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i], fb));
                    }
                    else if (!tr) {
                        fa = interp(px[i], px[i+1], d[i][j+1], d[i+1][j+1],  doses[p]);
                        fb = interp(py[j], py[j+1], d[i+1][j], d[i+1][j+1], doses[p]);
                        (*con)[p].append(QLineF(fa, py[j+1], px[i+1], fb));
                    }
                } // If we have zero or four, we are either fully inside or outside a contour
            }
}

int Dose::interp(int x1, int x2, double y1, double y2, double y0) {
    return int(x1+(y0-y1)*(x2-x1)/(y2-y1));
}

QImage Dose::getColourMap(QString axis, double ai, double af, double bi, double bf, double d, int res,
						  double di, double df, QColor min, QColor mid, QColor max) {
    // Create a temporary image
    int width  = (af-ai)*res; // Reversed on the image
    int height = (bf-bi)*res; // Reversed on the image
    QImage image(height, width, QImage::Format_ARGB32_Premultiplied);
    double hInc, wInc;
    double h, w, dose = 0;
	double c, weight, invWeight;
	double red, green, blue;

    // Calculate the size (in cm) of pixels, and then the range for grayscaling
    wInc = 1/double(res);
    hInc = 1/double(res);
	c = (df+di)/2.0;

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            // determine the location of the current pixel in the phantom
            h = (double(bi)) + hInc * double(i);
            w = (double(ai)) + wInc * double(j);

            // get the density, which differs based on axis through which image
            // os sliced
            if (!axis.compare("x axis")) {
                dose = getDose(d, h, w);
            }
            else if (!axis.compare("y axis")) {
                dose = getDose(h, d, w);
            }
            else if (!axis.compare("z axis")) {
                dose = getDose(h, w, d);
            }
			
			if (dose < c) {
				dose      = di>dose?di:dose;
				weight    = (dose-di)/(c-di);
				invWeight = 1.0-weight;
				
				red   = (mid.red()  * weight) + (min.red()  * invWeight);
				green = (mid.green()* weight) + (min.green()* invWeight);
				blue  = (mid.blue() * weight) + (min.blue() * invWeight);
			}
			else {
				dose      = df<dose?df:dose;
				weight    = (df-dose)/(df-c);
				invWeight = 1.0-weight;
				
				red   = (mid.red()  * weight) + (max.red()  * invWeight);
				green = (mid.green()* weight) + (max.green()* invWeight);
				blue  = (mid.blue() * weight) + (max.blue() * invWeight);
			}
			
			// Rescale to max brightness
			weight = red>blue?red:blue;
			weight = weight>green?weight:green;
			weight = 255.0/weight;
			
			// finally, paint the pixel
			image.setPixel(i, j, qRgb(red*weight,green*weight,blue*weight));
        }

    return image; // return the image created
}

void Dose::getDV(QVector <DV> *data, double* volume, int n) {
    double increment = 95.0/double(n)/double(z);
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				xLen = (cx[i+1]-cx[i]);
				vol = xLen*yLen*zLen;
				(*volume) += vol;
				data->append({val[i][j][k], err[i][j][k], vol});
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* media, QString allowedChars, double* volume, int n) {
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				xVal = (cx[i]+cx[i+1])/2.0;
				if (allowedChars.contains(media->getMedia(xVal, yVal, zVal))) {
					xLen = (cx[i+1]-cx[i]);
					vol = xLen*yLen*zLen;
					(*volume) += vol;
					data->append({val[i][j][k], err[i][j][k], vol});
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* mask, double* volume, int n) {
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				xVal = (cx[i]+cx[i+1])/2.0;
				if (mask->getMedia(xVal, yVal, zVal) == 50) {
					xLen = (cx[i+1]-cx[i]);
					vol = xLen*yLen*zLen;
					(*volume) += vol;
					data->append({val[i][j][k], err[i][j][k], vol});
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* media, QString allowedChars, EGSPhant* mask, double* volume, int n) {
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				xVal = (cx[i]+cx[i+1])/2.0;
				
				if (allowedChars.contains(media->getMedia(xVal, yVal, zVal)) ||
					mask->getMedia(xVal, yVal, zVal) == 50)
					qDebug() << "Phantom media" << media->getMedia(xVal, yVal, zVal)
							 << "- Mask media" << mask->getMedia(xVal, yVal, zVal);
						 
				if (allowedChars.contains(media->getMedia(xVal, yVal, zVal)))
					qDebug() << "\tProstate media";
				if (mask->getMedia(xVal, yVal, zVal) == 50)
					qDebug() << "\tMask satisfaction";
						 
				if (allowedChars.contains(media->getMedia(xVal, yVal, zVal)) &&
					mask->getMedia(xVal, yVal, zVal) == 50) {
					qDebug() << "\t\tWe are in";
					xLen = (cx[i+1]-cx[i]);
					vol = xLen*yLen*zLen;
					(*volume) += vol;
					data->append({val[i][j][k], err[i][j][k], vol});
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, double* volume, double minDose, double maxDose, int n) {
	if (minDose >= maxDose)
		maxDose = std::numeric_limits<double>::max(); // Set maxDose to max possible dose
    double increment = 95.0/double(n)/double(z);
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				if (minDose <= val[i][j][k] && val[i][j][k] <= maxDose) {
					xLen = (cx[i+1]-cx[i]);
					vol = xLen*yLen*zLen;
					(*volume) += vol;
					data->append({val[i][j][k], err[i][j][k], vol});
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* media, QString allowedChars, double* volume, double minDose, double maxDose, int n) {
	if (minDose >= maxDose)
		maxDose = std::numeric_limits<double>::max(); // Set maxDose to max possible dose
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				if (minDose <= val[i][j][k] && val[i][j][k] <= maxDose) {
				xVal = (cx[i]+cx[i+1])/2.0;
					if (allowedChars.contains(media->getMedia(xVal, yVal, zVal))) {
						xLen = (cx[i+1]-cx[i]);
						vol = xLen*yLen*zLen;
						(*volume) += vol;
						data->append({val[i][j][k], err[i][j][k], vol});
					}
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* mask, double* volume, double minDose, double maxDose, int n) {
	if (minDose >= maxDose)
		maxDose = std::numeric_limits<double>::max(); // Set maxDose to max possible dose
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				if (minDose <= val[i][j][k] && val[i][j][k] <= maxDose) {
					xVal = (cx[i]+cx[i+1])/2.0;
					if (mask->getMedia(xVal, yVal, zVal) == 50) {
						xLen = (cx[i+1]-cx[i]);
						vol = xLen*yLen*zLen;
						(*volume) += vol;
						data->append({val[i][j][k], err[i][j][k], vol});
					}
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDV(QVector <DV> *data, EGSPhant* media, QString allowedChars, EGSPhant* mask, double* volume, double minDose, double maxDose, int n) {
	if (minDose >= maxDose)
		maxDose = std::numeric_limits<double>::max(); // Set maxDose to max possible dose
    double increment = 95.0/double(n)/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol = (*volume) = 0;
	data->clear();
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				if (minDose <= val[i][j][k] && val[i][j][k] <= maxDose) {
					xVal = (cx[i]+cx[i+1])/2.0;
					if (allowedChars.contains(media->getMedia(xVal, yVal, zVal)) &&
						mask->getMedia(xVal, yVal, zVal) == 50) {
						xLen = (cx[i+1]-cx[i]);
						vol = xLen*yLen*zLen;
						(*volume) += vol;
						data->append({val[i][j][k], err[i][j][k], vol});
					}
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	std::sort(data->begin(), data->end(), DV_sorter);
}

void Dose::getDVs(QVector <QVector <DV> > *data, QVector <EGSPhant*> *masks, QVector <double> *volume) {
	emit nameProgress("Filtering data"); // Change progress bar name
	
	if (data->size() != masks->size() && data->size() != volume->size())
		return; // Quit if mask and data array size do not align
	
	double increment = 55.0/double(z);
	double xVal, yVal, zVal;
	double xLen, yLen, zLen;
	double vol;
	DV dataPoint;
	
	for (int i = 0; i < volume->size(); i++) {
		(*volume)[i] = 0;
		(*data)[i].clear();
	}
	
    for (int k = 0; k < z; k++) {
		zVal = (cz[k]+cz[k+1])/2.0;
		zLen = (cz[k+1]-cz[k]);
		emit madeProgress(increment); // Update progress bar
        for (int j = 0; j < y; j++) {
			yVal = (cy[j]+cy[j+1])/2.0;
			yLen = (cy[j+1]-cy[j]);
            for (int i = 0; i < x; i++) {
				xVal = (cx[i]+cx[i+1])/2.0;
				xLen = (cx[i+1]-cx[i]);
				vol = xLen*yLen*zLen;
				dataPoint = {val[i][j][k], err[i][j][k], vol};
				for (int n = 0; n < masks->size(); n++) {
					if ((*masks)[n]->getMedia(xVal, yVal, zVal) == 50) {
						(*volume)[n] += vol;
						(*data)[n].append(dataPoint);
					}
				}
			}
		}
	}
	
	emit nameProgress("Sorting (bar does not update)"); // Change progress bar name
	for (int i = 0; i < data->size(); i++)
		std::sort((*data)[i].begin(), (*data)[i].end(), DV_sorter);
}

QString Dose::getMetricCSV(QVector <DV> *data, double volume, QString name, QString DxStr, QString DccStr, QString VxStr, QString pDStr) {
	QString names, units, average, uncertainty, voxels, volumes, minimum, maximum;
	QStringList Dx, Vx, Dcc, temp;
	QVector <double> xD, xV, ccD;
	double pD, minD = 1000000000, maxD = 0, minE = 0, maxE = 0;
	
	QString text = "";
	
	if (DxStr.length()) {
		temp = DxStr.replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xD.append(temp[i].toDouble());
			Dx.append("");
		}
		std::sort(xD.begin(), xD.end());
	}
	
	if (VxStr.length()) {
		temp = VxStr.replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			xV.append(temp[i].toDouble());
			Vx.append("");
		}
		std::sort(xV.begin(), xV.end());
	}
	
	if (DccStr.length()) {
		temp = DccStr.replace(' ',',').split(',');
		for (int i = 0; i < temp.size(); i++) {
			ccD.append(temp[i].toDouble());
			Dcc.append("");
		}
		std::sort(ccD.begin(), ccD.end());
	}
	
	pD = pDStr.toDouble();
	
	// Generate metric data
	double volumeTally = 0, doseTally = 0, doseTallyErr = 0, doseTallyErr2 = 0, countTally = 0, absError = 0;
	int vIndex = 0, dIndex = xD.size()-1, ccIndex = ccD.size()-1;
	for (int j = 0; j < data->size(); j++) {		
		countTally    += 1.0;
		volumeTally   += data->at(j).vol;
		doseTally     += data->at(j).dose;
		absError       = data->at(j).dose*data->at(j).err;
		doseTallyErr  += absError;
		doseTallyErr2 += absError*absError;
		maxE = maxD<data->at(j).dose?absError:maxE;
		maxD = maxD<data->at(j).dose?data->at(j).dose:maxD;
		minE = minD>data->at(j).dose?absError:minE;
		minD = minD>data->at(j).dose?data->at(j).dose:minD;
		
		// Append Vx values as we go
		if (vIndex < xV.size()) {
			if (data->at(j).dose > (xV[vIndex]*pD/100.0) && j) {
				Vx[vIndex] += QString::number((volume-volumeTally+data->at(j).vol)/volume*100.0)+",,";
				vIndex++;
			}
		}
		
		// Append Dx values as we go
		if (dIndex >= 0) {
			if ((volume-volumeTally)/volume*100.0 < xD[dIndex] && j) {
				Dx[dIndex] += QString::number(data->at(j-1).dose)+","+QString::number(data->at(j-1).dose*data->at(j-1).err)+",";
				dIndex--;
			}
		}
		
		// Append Dcc values as we go
		if (ccIndex >= 0) {
			if ((volume-volumeTally) < ccD[ccIndex] && j) {
				Dcc[ccIndex] += QString::number(data->at(j-1).dose)+","+QString::number(data->at(j-1).dose*data->at(j-1).err)+",";
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
	
	names       = name+",,";
	units       = "value,uncertainty,";
	minimum     = QString::number(minD)+","+QString::number(minE)+",";
	maximum     = QString::number(maxD)+","+QString::number(maxE)+",";
	average     = QString::number(doseTally)+","+QString::number(doseTallyErr2)+",";
	uncertainty = QString::number(doseTallyErr)+",,";
	voxels      = QString::number(countTally)+",,";
	volumes     = QString::number(volume)+",,";
	
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
	
	return text;
}