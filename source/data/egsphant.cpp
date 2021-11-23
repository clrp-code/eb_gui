/*
################################################################################
#
#  egs_brachy_GUI egsphant.cpp
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
#include "egsphant.h"

EGSPhant::EGSPhant() {
    nx = ny = nz = 0;
}

// Output gz egsphant
void EGSPhant::savegzEGSPhantFilePlus(QString path) { // Progress percentages assume GUI construction
	// Ripped fairly whole-cloth from egs_brachy
	ogzstream ogout(path.toStdString().c_str());
	std::ostream* out = (std::ostream*)(&ogout);
	if (out->good()) {
		// Media count
		(*out) << media.size() << "\n";
		
		// Media names
		for (int i=0; i < media.size(); i++)
			(*out) << media[i].toStdString() << "\n";

		// (unused) ESTEP per media
		for (int i=0; i < media.size(); i++)
			(*out) << " 0.5";
		(*out) << "\n";
		
		// dimensions
		(*out) << nx << " " << ny << " " << nz << "\n";

		// Boundaries
		for (int i=0; i < nx; i++)
			(*out) << x[i] << " ";
		(*out) << x.last() << "\n";
		
		for (int i=0; i < ny; i++)
			(*out) << y[i] << " ";
		(*out) << y.last() << "\n";
		
		for (int i=0; i < nz; i++)
			(*out) << z[i] << " ";
		(*out) << z.last() << "\n";
		
		double increment = 10./double(nz); // 10%
		
		// Media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++)
                    (*out) << m[i][j][k];
			emit madeProgress(increment);
		}
		
		(*out) << "\n";
		
		increment = 35./double(nz); // 35%
		
		// Density
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++)
                    (*out) << d[i][j][k] << " ";
			emit madeProgress(increment);
		}
		
        ogout.close();
	}
}

// Output gz mask
void EGSPhant::savegzEGSPhantFile(QString path) {
	// Ripped fairly whole-cloth from egs_brachy
	ogzstream ogout(path.toStdString().c_str());
	std::ostream* out = (std::ostream*)(&ogout);
	if (out->good()) {
		// Media count
		(*out) << media.size() << "\n";
		
		// Media names
		for (int i=0; i < media.size(); i++)
			(*out) << media[i].toStdString() << "\n";

		// (unused) ESTEP per media
		for (int i=0; i < media.size(); i++)
			(*out) << " 0.5";
		(*out) << "\n";

		// Boundaries
		for (int i=0; i < nx; i++)
			(*out) << x[i] << " ";
		(*out) << x.last() << "\n";
		
		for (int i=0; i < ny; i++)
			(*out) << y[i] << " ";
		(*out) << y.last() << "\n";
		
		for (int i=0; i < nz; i++)
			(*out) << z[i] << " ";
		(*out) << z.last() << "\n";
		
		// Media
        for (int k = 0; k < nz; k++)
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++)
                    (*out) << m[i][j][k];
		
        ogout.close();
	}
}

// Make a mask template from another EGSPhant
void EGSPhant::makeMask(EGSPhant* mask) {
    nx = mask->nx;
	ny = mask->ny;
	nz = mask->nz;
    x = mask->x;
	y = mask->y;
	z = mask->z;
    maxDensity = mask->maxDensity;
	{
		// Set all media to OTHER
		QVector <char> mz(nz, 49);
		QVector <QVector <char> > my(ny, mz);
		QVector <QVector <QVector <char> > > mx(nx, my);
		m = mx;
		
		// Remove densities from here as they aren't needed for masks
		//QVector <double> dz(nz, 0);
		//QVector <QVector <double> > dy(ny, dz);
		//QVector <QVector <QVector <double> > > dx(nx, dy);
		//d = dx;
	}
    media << "OTHER" << "TARGET";
}

void EGSPhant::loadEGSPhantFile(QString path) {
    QFile file(path);

    // Increment size of the status bar
    double increment;

    // Open up the file specified at path
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream input(&file);
        QString line = input.readLine();

        // read in the number of media
        int num = line.trimmed().toInt();
        media.resize(num);

        // read the media into an array
        for (int i = 0; i < num; i++) {
            media[i] = input.readLine().trimmed();
        }

        // skim over the the ESTEP info
        line = input.readLine().trimmed();

        // read in the dimensions of the egsphant file and
        // store the size and resize the matrices holding the boundaries
        input.skipWhiteSpace();
        input >> nx;
        x.fill(0,nx+1);
        input.skipWhiteSpace();
        input >> ny;
        y.fill(0,ny+1);
        input.skipWhiteSpace();
        input >> nz;
        z.fill(0,nz+1);

        // resize the 3D matrix to hold all densities
        {
            QVector <char> mz(nz, 0);
            QVector <QVector <char> > my(ny, mz);
            QVector <QVector <QVector <char> > > mx(nx, my);
            m = mx;
            QVector <double> dz(nz, 0);
            QVector <QVector <double> > dy(ny, dz);
            QVector <QVector <QVector <double> > > dx(nx, dy);
            d = dx;
        }

        // read in all the boundaries of the phantom
        input.skipWhiteSpace();
        for (int i = 0; i <= nx; i++) {
            input >> x[i];
        }
        for (int i = 0; i <= ny; i++) {
            input >> y[i];
        }
        for (int i = 0; i <= nz; i++) {
            input >> z[i];
        }

        // Determine the increment this egsphant file gets
        increment = 100./double(nz-1);

        // Read in all the media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input.skipWhiteSpace();
                    input >> m[i][j][k];
                }
            emit madeProgress(increment); // Update progress bar
        }

        file.close();
    }
}

void EGSPhant::loadEGSPhantFilePlus(QString path) {
    QFile file(path);

    // Increment size of the status bar
    double increment;

    // Open up the file specified at path
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream input(&file);
        QString line = input.readLine();

        // read in the number of media
        int num = line.trimmed().toInt();
        media.resize(num);

        // read the media into an array
        for (int i = 0; i < num; i++) {
            media[i] = input.readLine().trimmed();
        }

        // skim over the the ESTEP info
        line = input.readLine().trimmed();

        // read in the dimensions of the egsphant file
        input.skipWhiteSpace();
        input >> nx;
        x.fill(0,nx+1);
        input.skipWhiteSpace();
        input >> ny;
        y.fill(0,ny+1);
        input.skipWhiteSpace();
        input >> nz;
        z.fill(0,nz+1);

        // resize the 3D matrix to hold all densities
        {
            QVector <char> mz(nz, 0);
            QVector <QVector <char> > my(ny, mz);
            QVector <QVector <QVector <char> > > mx(nx, my);
            m = mx;
            QVector <double> dz(nz, 0);
            QVector <QVector <double> > dy(ny, dz);
            QVector <QVector <QVector <double> > > dx(nx, dy);
            d = dx;
        }

        // read in all the boundaries of the phantom
        input.skipWhiteSpace();
        for (int i = 0; i <= nx; i++) {
            input >> x[i];
        }
        for (int i = 0; i <= ny; i++) {
            input >> y[i];
        }
        for (int i = 0; i <= nz; i++) {
            input >> z[i];
        }

        // Determine the increment this egsphant file gets
        increment = 100./double(nz-1);

        // Read in all the media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input.skipWhiteSpace();
                    input >> m[i][j][k];
                }
            emit madeProgress(increment/100.0*10.0); // Update progress bar
        }

        // Read in all the densities
        maxDensity = 0;
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input.skipWhiteSpace();
                    input >> d[i][j][k];
                    if (d[i][j][k] > maxDensity) {
                        maxDensity = d[i][j][k];
                    }

                }
            emit madeProgress(increment/100.0*90.0); // Update progress bar
        }

        file.close();
    }
}

void EGSPhant::loadbEGSPhantFile(QString path) {
    QFile file(path);

    // Increment size of the status bar
    double increment;

    // Open up the file specified at path
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream input(&file);
		input.setByteOrder(QDataStream::LittleEndian);
		
        // read in the number of media
        unsigned char num;
        input >> num;
        media.resize(num);

        // read the media into an array
        char *temp2;
        for (int i = 0; i < num; i++) {
            input >> temp2;
            media[i] = QString(temp2);
        }

        // skim over the the ESTEP info
        double temp;
        for (int i = 0; i < num; i++) {
            input >> temp;
        }

        // read in the dimensions of the egsphant file
        // store the size and resize the matrices holding the boundaries
        input >> nx;
        x.fill(0,nx+1);
        input >> ny;
        y.fill(0,ny+1);
        input >> nz;
        z.fill(0,nz+1);

        // resize the 3D matrix to hold all densities
        {
            QVector <char> mz(nz, 0);
            QVector <QVector <char> > my(ny, mz);
            QVector <QVector <QVector <char> > > mx(nx, my);
            m = mx;
            QVector <double> dz(nz, 0);
            QVector <QVector <double> > dy(ny, dz);
            QVector <QVector <QVector <double> > > dx(nx, dy);
            d = dx;
        }

        // read in all the boundaries of the phantom
        for (int i = 0; i <= nx; i++) {
            input >> x[i];
        }
        for (int i = 0; i <= ny; i++) {
            input >> y[i];
        }
        for (int i = 0; i <= nz; i++) {
            input >> z[i];
        }

        // Determine the increment this egsphant file gets
        increment = 100./double(nz-1);

        // Read in all the media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input >> num;
                    m[i][j][k] = num;
                }
            emit madeProgress(increment); // Update progress bar
        }

        file.close();
    }
}

void EGSPhant::loadbEGSPhantFilePlus(QString path) {
    QFile file(path);

    // Increment size of the status bar
    double increment;

    // Open up the file specified at path
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream input(&file);
		input.setByteOrder(QDataStream::LittleEndian);
		
        // read in the number of media
        unsigned char num;
        input >> num;
        media.resize(num);

        // read the media into an array
        char *temp2;
        for (int i = 0; i < num; i++) {
            input >> temp2;
            media[i] = QString(temp2);
        }

        // skim over the the ESTEP info
        double temp;
        for (int i = 0; i < num; i++) {
            input >> temp;
        }

        // read in the dimensions of the egsphant file
        // store the size and resize the matrices holding the boundaries
        input >> nx;
        x.fill(0,nx+1);
        input >> ny;
        y.fill(0,ny+1);
        input >> nz;
        z.fill(0,nz+1);

        // resize the 3D matrix to hold all densities
        {
            QVector <char> mz(nz, 0);
            QVector <QVector <char> > my(ny, mz);
            QVector <QVector <QVector <char> > > mx(nx, my);
            m = mx;
            QVector <double> dz(nz, 0);
            QVector <QVector <double> > dy(ny, dz);
            QVector <QVector <QVector <double> > > dx(nx, dy);
            d = dx;
        }

        // read in all the boundaries of the phantom
        for (int i = 0; i <= nx; i++) {
            input >> x[i];
        }
        for (int i = 0; i <= ny; i++) {
            input >> y[i];
        }
        for (int i = 0; i <= nz; i++) {
            input >> z[i];
        }

        // Determine the increment this egsphant file gets
        increment = 100./double(nz-1);

        // Read in all the media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input >> num;
                    m[i][j][k] = num;
                }
            emit madeProgress(increment/100.0*50.0); // Update progress bar
        }

        // Read in all the densities
        maxDensity = 0;
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
                    input >> d[i][j][k];
                    if (d[i][j][k] > maxDensity) {
                        maxDensity = d[i][j][k];
                    }
                }
            emit madeProgress(increment/100.0*50.0); // Update progress bar
        }

        file.close();
    }
}

char EGSPhant::getMedia(double px, double py, double pz) {
    int ix, iy, iz;
    ix = iy = iz = -1;

    // Find the index of the boundary that is less than px, py and pz
    for (int i = 0; i < nx; i++)
        if (px <= x[i+1]) {
            ix = i;
            break;
        }
    if (px < x[0]) {
        ix = -1;
    }

    for (int i = 0; i < ny; i++)
        if (py <= y[i+1]) {
            iy = i;
            break;
        }
    if (py < y[0]) {
        iy = -1;
    }

    for (int i = 0; i < nz; i++)
        if (pz <= z[i+1]) {
            iz = i;
            break;
        }
    if (pz < z[0]) {
        iz = -1;
    }

    // This is to insure that no area outside the vectors is accessed
    if (ix < nx && ix >= 0 && iy < ny && iy >= 0 && iz < nz && iz >= 0) {
        return m[ix][iy][iz];
    }

    return 0; // We are not within our bounds
}

double EGSPhant::getDensity(double px, double py, double pz) {
    int ix, iy, iz;
    ix = iy = iz = -1;

    // Find the index of the boundary that is less than px, py and pz
    for (int i = 0; i < nx; i++)
        if (px <= x[i+1]) {
            ix = i;
            break;
        }
    if (px < x[0]) {
        ix = -1;
    }

    for (int i = 0; i < ny; i++)
        if (py <= y[i+1]) {
            iy = i;
            break;
        }
    if (py < y[0]) {
        iy = -1;
    }

    for (int i = 0; i < nz; i++)
        if (pz <= z[i+1]) {
            iz = i;
            break;
        }
    if (pz < z[0]) {
        iz = -1;
    }

    // This is to insure that no area outside the vectors is accessed
    if (ix < nx && ix >= 0 && iy < ny && iy >= 0 && iz < nz && iz >= 0) {
        return d[ix][iy][iz];
    }

    return -1; // We are not within our bounds
}

double EGSPhant::getDensity(int px, int py, int pz) {
    // This is to insure that no area outside the vectors is accessed
    if (px < nx && px >= 0 && py < ny && py >= 0 && pz < nz && pz >= 0) {
        return d[px][py][pz];
    }

    return -1; // We are not within our bounds
}


void EGSPhant::setDensity(int px, int py, int pz, double density) {
    // This is to insure that no area outside the vectors is accessed
    if (px < nx && px >= 0 && py < ny && py >= 0 && pz < nz && pz >= 0) {
        d[px][py][pz] = density;
    }
}

int EGSPhant::getIndex(QString axis, double p) {
    int index = -1;

    if (!axis.compare("x axis")) {
        for (int i = 0; i < nx; i++)
            if (p < x[i+1]) {
                index = i;
                break;
            }
        if (p < x[0]) {
            index = -1;
        }
    }
    else if (!axis.compare("y axis")) {
        for (int i = 0; i < ny; i++)
            if (p < y[i+1]) {
                index = i;
                break;
            }
        if (p < y[0]) {
            index = -1;
        }
    }
    else if (!axis.compare("z axis")) {
        for (int i = 0; i < nz; i++)
            if (p < z[i+1]) {
                index = i;
                break;
            }
        if (p < z[0]) {
            index = -1;
        }
    }

    return index; // -1 if we are out of bounds
}

QImage EGSPhant::getEGSPhantPicMed(QString axis, double ai, double af,
                                   double bi, double bf, double d, int res) {
    // Create a temporary image
    int width  = (af-ai)*res;
    int height = (bf-bi)*res;
    QImage image(height, width, QImage::Format_ARGB32);
    double hInc, wInc, cInc;
    double h, w, c = 0;

    // Calculate the size (in cm) of pixels, and then the range for grayscaling
    wInc = 1/double(res);
    hInc = 1/double(res);
    cInc = 255.0/media.size()+1;

    for (int i = height-1; i >= 0; i--)
        for (int j = 0; j < width; j++) {
            // determine the location of the current pixel in the phantom
            h = (double(bi)) + hInc * double(i);
            w = (double(ai)) + wInc * double(j);

            // get the media, which differs based on axis through which image is
            // sliced
            if (!axis.compare("x axis")) {
                c = getMedia(d, h, w) - 48;
            }
            else if (!axis.compare("y axis")) {
                c = getMedia(h, d, w) - 48;
            }
            else if (!axis.compare("z axis")) {
                c = getMedia(h, w, d) - 48;
            }
			
			// if c is out of bounds, set it to zero
			// else if c is an upper-case letter, remove the 7 ascii chars between 9 and A
			// else if c is a lower-case letter, remove the 13 non-alphanumeric chars between 9 and a
			c = (c==-1)?0:(c<10?c:(c<36?c-7:c-13));

            // finally, paint the pixel
            image.setPixel(i, j, qRgb(int(cInc*c), int(cInc*c), int(cInc*c)));
        }

    return image; // return the image created
}

QImage EGSPhant::getEGSPhantPicDen(QString axis, double ai, double af,
                                   double bi, double bf, double d, int res,
								   double di, double df) {
    // Create a temporary image
    int width  = (af-ai)*res;
    int height = (bf-bi)*res;
    QImage image(height, width, QImage::Format_RGB32);
    double hInc, wInc, cInc;
    double h, w, c = 0;

    // Calculate the size (in cm) of pixels, and then the range for grayscaling
    wInc = 1/double(res);
    hInc = 1/double(res);
    cInc = 255.0/(df-di);

    for (int i = height-1; i >= 0; i--)
        for (int j = 0; j < width; j++) {
            // determine the location of the current pixel in the phantom
            h = (double(bi)) + hInc * double(i);
            w = (double(ai)) + wInc * double(j);

            // get the density, which differs based on axis through which image
            // os sliced
            if (!axis.compare("x axis")) {
                c = getDensity(d, h, w);
            }
            else if (!axis.compare("y axis")) {
                c = getDensity(h, d, w);
            }
            else if (!axis.compare("z axis")) {
                c = getDensity(h, w, d);
            }
			
			// if c is out of bounds, set it to zero
			// else if c is below the minimum density, set it to minimum density
			// else if c is above the maximum density, set it to maximum density
			c = (c==-1)?0:(c<di?di:(c>df?df:c));

            // finally, paint the pixel
            image.setPixel(i, j, qRgb(int(cInc*c), int(cInc*c), int(cInc*c)));
        }

    return image; // return the image created
}
