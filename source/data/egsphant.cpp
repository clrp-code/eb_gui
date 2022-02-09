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
		
		double increment = 45./double(nz); // 45%
		
		// Media
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++)
                    (*out) << m[i][j][k];
			emit madeProgress(increment);
		}
		
		(*out) << "\n";
		
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

void EGSPhant::loadgzEGSPhantFile(QString path) {
	// Ripped fairly whole-cloth from egs_brachy
	igzstream ogin(path.toStdString().c_str());
	std::istream* data = (std::istream*)(&ogin);
	
    // Increment size of the status bar
    double increment;
	
	if (data->good()) {
		int nmed;
		*data >> nmed;
		
		std::string med;
		media.clear(); // in case of reload
		for (int i=0; i < nmed; i++) {
			*data >> med;
            media.append(QString(med.c_str()));
		}
		
		// estepe is ignored!
		double estepe;
		for (int i=0; i < nmed; i++) {
			*data >> estepe;
		}
		
		/* read in all bounds and create the geometry */
		double bound;
		
		*data >> nx >> ny >> nz;
		
		x.clear(); // in case of reload
		for (int i=0; i < nx+1; i++) {
			*data >> bound;
			x.append(bound);
		}
		y.clear(); // in case of reload
		for (int i=0; i < ny+1; i++) {
			*data >> bound;
			y.append(bound);
		}
		z.clear(); // in case of reload
		for (int i=0; i < nz+1; i++) {
			*data >> bound;
			z.append(bound);
		}
		
        {
            QVector <char> mz(nz, 0);
            QVector <QVector <char> > my(ny, mz);
            QVector <QVector <QVector <char> > > mx(nx, my);
            m = mx;
        }
				
		/* now we've got all geometry information so construct our geom */
		// read in region media and set them in the geometry
		increment = 100.0/double(nz); // 100%
		char cur_med;
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
					*data >> cur_med;
					m[i][j][k] = cur_med;
                }
            emit madeProgress(increment); // Update progress bar
        }
	}
}

void EGSPhant::loadgzEGSPhantFilePlus(QString path) {	
	// Ripped fairly whole-cloth from egs_brachy
	igzstream ogin(path.toStdString().c_str());
	std::istream* data = (std::istream*)(&ogin);
	
    // Increment size of the status bar
    double increment;
	
	if (data->good()) {
		int nmed;
		*data >> nmed;
		
		std::string med;
		media.clear(); // in case of reload
		for (int i=0; i < nmed; i++) {
			*data >> med;
            media.append(QString(med.c_str()));
		}
		
		// estepe is ignored!
		double estepe;
		for (int i=0; i < nmed; i++) {
			*data >> estepe;
		}
		
		/* read in all bounds and create the geometry */
		double bound;
		
		*data >> nx >> ny >> nz;
		
		x.clear(); // in case of reload
		for (int i=0; i < nx+1; i++) {
			*data >> bound;
			x.append(bound);
		}
		y.clear(); // in case of reload
		for (int i=0; i < ny+1; i++) {
			*data >> bound;
			y.append(bound);
		}
		z.clear(); // in case of reload
		for (int i=0; i < nz+1; i++) {
			*data >> bound;
			z.append(bound);
		}
		
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
				
		/* now we've got all geometry information so construct our geom */
		// read in region media and set them in the geometry
		increment = 30.0/double(nz); // 30%
		char cur_med;
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
					*data >> cur_med;
                    m[i][j][k] = cur_med;
                }
            emit madeProgress(increment); // Update progress bar
        }
		
		// read in region rhos and set the relative rho value if required
		increment = 70.0/double(nz); // 70%
		double cur_rho;
		maxDensity = 0;
        for (int k = 0; k < nz; k++) {
            for (int j = 0; j < ny; j++)
                for (int i = 0; i < nx; i++) {
					*data >> cur_rho;
                    d[i][j][k] = cur_rho;
                    if (d[i][j][k] > maxDensity) {
                        maxDensity = d[i][j][k];
                    }
                }
            emit madeProgress(increment); // Update progress bar
        }
	}
}

char EGSPhant::getMedia(double px, double py, double pz) {
    int ix, iy, iz;
    ix = iy = iz = -1;
	
	// Check and exit if outside of the bounds
	if (px < x[0] || px > x.last() ||
		py < y[0] || py > y.last() ||
		pz < z[0] || pz > z.last())
		return -1;

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

    return -1; // We are not within our bounds
}

double EGSPhant::getDensity(double px, double py, double pz) {
    int ix, iy, iz;
    ix = iy = iz = -1;

	// Check and exit if outside of the bounds
	if (px < x[0] || px > x.last() ||
		py < y[0] || py > y.last() ||
		pz < z[0] || pz > z.last())
		return -1;

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


void EGSPhant::redefineBounds(double xi, double yi, double zi, double xf, double yf, double zf) {
	// Get the new boundary limits
	int xi2 = getIndex("x axis", xi);
	int yi2 = getIndex("y axis", yi);
	int zi2 = getIndex("z axis", zi);
	int xf2 = getIndex("x axis", xf);
	int yf2 = getIndex("y axis", yf);
	int zf2 = getIndex("z axis", zf);
	
	// Check f is larger than i
	if (xf2 <= xi2 || yf2 <= yi2 || zf2 <= zi2)
		return;
	
	// Bring boundaries to within egsphant limits
	if (xi2 < 0)
		xi2 = 0;
	if (yi2 < 0)
		yi2 = 0;
	if (zi2 < 0)
		zi2 = 0;
	if (xf2 < 0)
		xf2 = nx;
	if (yf2 < 0)
		yf2 = ny;
	if (zf2 < 0)
		zf2 = nz;
	
	// Create the new data variables
    int new_nx = xf2-xi2+1, new_ny = yf2-yi2+1, new_nz = zf2-zi2+1;
    QVector <double> new_x, new_y, new_z;
    QVector <QVector <QVector <char> > > new_m;
    QVector <QVector <QVector <double> > > new_d;
	
	for (int i = xi2; i <= xf2+1; i++)
		new_x.append(x[i]);
	for (int i = yi2; i <= yf2+1; i++)
		new_y.append(y[i]);
	for (int i = zi2; i <= zf2+1; i++)
		new_z.append(z[i]);

	// create the new 3D matrices to hold subdata
	{
		QVector <char> mz(new_nz, 0);
		QVector <QVector <char> > my(new_ny, mz);
		QVector <QVector <QVector <char> > > mx(new_nx, my);
		new_m = mx;
		QVector <double> dz(new_nz, 0);
		QVector <QVector <double> > dy(new_ny, dz);
		QVector <QVector <QVector <double> > > dx(new_nx, dy);
		new_d = dx;
	}
	
	for (int k = zi2; k <= zf2; k++)
		for (int j = yi2; j <= yf2; j++)
			for (int i = xi2; i <= xf2; i++) {
				new_m[i-xi2][j-yi2][k-zi2] = m[i][j][k];
				new_d[i-xi2][j-yi2][k-zi2] = d[i][j][k];
			}
	
	// Now replace all data with the new indices
    nx = new_nx;
	ny = new_ny;
	nz = new_nz;
    x = new_x;
	y = new_y;
	z = new_z;
    m = new_m;
    d = new_d;
}

QImage EGSPhant::getEGSPhantPicMed(QString axis, double ai, double af,
                                   double bi, double bf, double d, int res) {
    // Create a temporary image
    int width  = (af-ai)*res; // Reversed on the image
    int height = (bf-bi)*res; // Reversed on the image
    QImage image(height, width, QImage::Format_ARGB32_Premultiplied);
    double hInc, wInc, cInc;
    double h, w;
	int c;
	char med;
	QString indeces("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    // Calculate the size (in cm) of pixels, and then the range for grayscaling
    wInc = 1/double(res);
    hInc = 1/double(res);
    cInc = 255.0/double(media.size()+1);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            // determine the location of the current pixel in the phantom
            h = (double(bi)) + hInc * double(i);
            w = (double(ai)) + wInc * double(j);

            // get the media, which differs based on axis through which image is
            // sliced
            if (!axis.compare("x axis")) {
                med = getMedia(d, h, w);
            }
            else if (!axis.compare("y axis")) {
                med = getMedia(h, d, w);
            }
            else if (!axis.compare("z axis")) {
                med = getMedia(h, w, d);
            }
			
			c = (indeces.indexOf(med)+1)*cInc;

            // finally, paint the pixel
            image.setPixel(i, j, qRgb(c, c, c));
        }

    return image; // return the image created
}

QImage EGSPhant::getEGSPhantPicDen(QString axis, double ai, double af,
                                   double bi, double bf, double d, int res,
								   double di, double df) {
    // Create a temporary image
    int width  = (af-ai)*res; // Reversed on the image
    int height = (bf-bi)*res; // Reversed on the image
    QImage image(height, width, QImage::Format_ARGB32_Premultiplied);
    double hInc, wInc, cInc;
    double h, w, den = 0;
	int	c;

    // Calculate the size (in cm) of pixels, and then the range for grayscaling
    wInc = 1/double(res);
    hInc = 1/double(res);
    cInc = 255.0/(df-di);

    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            // determine the location of the current pixel in the phantom
            h = (double(bi)) + hInc * double(i);
            w = (double(ai)) + wInc * double(j);

            // get the density, which differs based on axis through which image
            // os sliced
            if (!axis.compare("x axis")) {
                den = getDensity(d, h, w);
            }
            else if (!axis.compare("y axis")) {
                den = getDensity(h, d, w);
            }
            else if (!axis.compare("z axis")) {
                den = getDensity(h, w, d);
            }
			
			// if c is out of bounds, set it to zero
			// else if c is below the minimum density, set it to minimum density
			// else if c is above the maximum density, set it to maximum density
			c = ((den==-1)?0:(den<di?di:(den>df?df:den)))*cInc;

            // finally, paint the pixel
            image.setPixel(i, j, qRgb(c, c, c));
        }

    return image; // return the image created
}