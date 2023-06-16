/*
################################################################################
#
#  egs_brachy_GUI data.cpp
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
#include "data.h"
#include "interface.h" // Needed for some compiler variables

//#define DEBUG_BUILDEGSPHANT // Comment out
#define ASSUME_PERMANENT_LDR // Used when there is no specification
#define INTERPOLATE_CONTOURS // Used to create a new contour on slices between two contours from the same structure

int Data::loadDefaults() {
	QProcessEnvironment envVars = QProcessEnvironment::systemEnvironment();
	if (!envVars.contains("EGS_HOME")) // No EGS_HOME defined
		return 101;
	
	eh_location = envVars.value("EGS_HOME");
	eb_location = eh_location+"egs_brachy";
	gui_location = QCoreApplication::applicationDirPath();
	hh_location = envVars.value("HEN_HOUSE");
	ep_location = hh_location+"scripts/bin/egs-parallel";
	
	QFile *file;
    QTextStream *input;
	QString text;
	
	muen_location = eb_location+"/lib/muen/brachy_xcom_1.5MeV.muendat";
	material_location = eb_location+"/lib/media/material.dat";
	transport_location = eb_location+"/lib/transport/low_energy_default";
	mar_location = gui_location+"/database/MAR_defaults.txt";
	metric_location = gui_location+"/database/metric_defaults.txt";
	def_ncase = "1e8";

    file = new QFile(gui_location+"/configuration.txt");

    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        input = new QTextStream(file);
		
        while (!input->atEnd()) {
            text = input->readLine();
            
			// Just look for specific lines, not the most efficient, but
			// as long as the number of lines in the config file stays in
			// the tens, it should be fine
			if (text.left(15).compare("muen location =") == 0)
				muen_location = text.right(text.length()-15).trimmed();
			else if (text.left(19).compare("material location =") == 0)
				material_location = text.right(text.length()-19).trimmed();
			else if (text.left(20).compare("transport location =") == 0)
				transport_location = text.right(text.length()-20).trimmed();
			else if (text.left(11).compare("histories =") == 0)
				def_ncase = text.right(text.length()-11).trimmed();
			else if (text.left(23).compare("checkpoints in series =") == 0)
				def_nbatch = text.right(text.length()-23).trimmed();
			else if (text.left(25).compare("checkpoints in parallel =") == 0)
				def_nchunk = text.right(text.length()-25).trimmed();
			else if (text.left(22).compare("geometry error limit =") == 0)
				def_geomLimit = text.right(text.length()-22).trimmed();
			else if (text.left(25).compare("minimum electron energy =") == 0)
				def_AE = text.right(text.length()-25).trimmed();
			else if (text.left(25).compare("maximum electron energy =") == 0)
				def_UE = text.right(text.length()-25).trimmed();
			else if (text.left(23).compare("minimum photon energy =") == 0)
				def_AP = text.right(text.length()-23).trimmed();
			else if (text.left(23).compare("maximum photon energy =") == 0)
				def_UP = text.right(text.length()-23).trimmed();
			else if (text.left(24).compare("volume correction mode =") == 0)
				def_volCor = text.right(text.length()-24).trimmed();
			else if (text.left(27).compare("volume correction density =") == 0)	
				def_volDen = text.right(text.length()-27).trimmed();
			else if (text.left(33).compare("HU to egsphant conversion table =") == 0)	
				hu_location = gui_location+text.right(text.length()-33).trimmed();
			else if (text.left(27).compare("tissue assignment schemes =") == 0)
				TAS_names = text.right(text.length()-27).trimmed().split(" ", QString::SkipEmptyParts);
			else if (text.left(24).compare("isodose line thickness =") == 0)
				isodoseLineThickness = text.right(text.length()-24).trimmed().toInt();
			else if (text.left(22).compare("histogram bin count =") == 0)
				histogramBinCount = text.right(text.length()-22).trimmed().toInt();
			else if (text.left(24).compare("seed discovery density =") == 0)
				def_seedDisc = text.right(text.length()-24).trimmed();
	    }

        delete input;
    }
    delete file;
	
	// substitute environmental variables
	QStringList envNames = envVars.keys();
	for (int i = 0; i < envNames.size(); i++) {
		if (muen_location.contains(QString("$")+envNames[i]))
			muen_location.replace(QString("$")+envNames[i],envVars.value(envNames[i]));
		if (material_location.contains(QString("$")+envNames[i]))
			material_location.replace(QString("$")+envNames[i],envVars.value(envNames[i]));
		if (transport_location.contains(QString("$")+envNames[i]))
			transport_location.replace(QString("$")+envNames[i],envVars.value(envNames[i]));
	}
	
	// egs_brachy library data
	QDirIterator* files;
	files = new QDirIterator(eb_location+"/lib/geometry/sources/", {"*.geom"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		libNameSources << files->fileName().left(files->fileName().length()-5);
		libDirSources << files->filePath().left(files->filePath().size()-files->fileName().length());
	}
	delete files;
	
	files = new QDirIterator(eb_location+"/lib/geometry/phantoms/", {"*.geom"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		libNamePhants << files->fileName();
		libDirPhants << files->path();
	}
	delete files;
	
	files = new QDirIterator(eb_location+"/lib/geometry/transformations/", QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		if (files->fileName() != "." && files->fileName() != "..") {
			libNameTransforms << files->fileName();
			libDirTransforms << files->path();
		}
	}
	delete files;
	
	files = new QDirIterator(eb_location+"/lib/geometry/eye_plaques/", {"*.geom"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		libNameGeometries << files->fileName().left(files->fileName().length()-5);
		libDirGeometries << files->filePath().left(files->filePath().length()-files->fileName().length());
	}
	delete files;
	
	files = new QDirIterator(eb_location+"/lib/geometry/applicators/", {"*.geom"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		libNameGeometries << files->fileName().left(files->fileName().length()-5);
		libDirGeometries << files->filePath().left(files->filePath().length()-files->fileName().length());
	}
	delete files;
	
	// local GUI data
	if (!QDir(gui_location+"/database/mask/").exists())
		QDir().mkdir(gui_location+"/database/mask/");
	
	if (!QDir(gui_location+"/database/egsphant/").exists())
		QDir().mkdir(gui_location+"/database/egsphant/");
	
	if (!QDir(gui_location+"/database/transformation/").exists())
		QDir().mkdir(gui_location+"/database/transformation/");
	
	if (!QDir(gui_location+"/database/dose/").exists())
		QDir().mkdir(gui_location+"/database/dose/");
	
	files = new QDirIterator(gui_location+"/database/egsphant/", {"*.egsphant","*.egsphant.gz"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup //
	while(files->hasNext()) {
		files->next();
		localNamePhants << files->fileName();
		localDirPhants << files->path();
	}
	delete files;
	
	files = new QDirIterator(gui_location+"/database/transformation/", QDirIterator::Subdirectories);
	while(files->hasNext()) {
		files->next();
		if (files->fileName() != "." && files->fileName() != ".." &&
			!files->fileName().endsWith(".dwell") && !files->fileName().endsWith(".activity") &&
			!files->fileName().endsWith(".log")) {
			localNameTransforms << files->fileName();
			localDirTransforms << files->path();
		}
	}
	delete files;
	
	files = new QDirIterator(gui_location+"/database/dose/", {"*.3ddose","*.3ddose.gz"}, QDir::NoFilter, QDirIterator::Subdirectories);  // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		if (files->fileName() != "." && files->fileName() != "..") {
			localNameDoses << files->fileName();
			localDirDoses << files->path();
		}
	}
	delete files;
	
	// Tissue assignment schemes
	QString medName;
	double medDensity;
	QStringList TAS_reference = TAS_names;
	TAS_names.clear();
	
	files = new QDirIterator(gui_location+"/database/tissue_assignment_scheme/", {"*.txt"}, QDir::NoFilter, QDirIterator::Subdirectories); // #nofilter #nomakeup
	while(files->hasNext()) {
		files->next();
		QFile TAS_file(files->path()+files->fileName());
		if (TAS_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream inp(&TAS_file);
			
			TAS_names.append(files->fileName().left(files->fileName().length()-4));
			media.resize(media.size()+1);
			threshold.resize(threshold.size()+1);
			
			while (!inp.atEnd()) {
				inp >> medName;
				inp >> medDensity;
				media.last().append(medName);
				threshold.last().append(medDensity);
			}
		}
	}
	delete files;
	
	// Reorganize TAS to match reference order, it's O(n^2), hopefully TAS lists stay short
	QString nameTemp;
	QVector <QString> mediaTemp;
	QVector <double> thresholdTemp;
	
	for (int i = 0; i < TAS_reference.size(); i++)
		for (int j = 0; j < TAS_names.size(); j++) {
			if (TAS_reference[i].compare(TAS_names[j]) == 0) {
				nameTemp      = TAS_names[i];
				mediaTemp     = media[i];
				thresholdTemp = threshold[i];
				TAS_names[i]  = TAS_names[j];
				media[i]      = media[j];   
				threshold[i]  = threshold[j];
				TAS_names[j]  = nameTemp;
				media[j]      = mediaTemp;
				threshold[j]  = thresholdTemp;
				break;
			}
		}
	
	// Load metrics
    file = new QFile(metric_location);
	
	if(!file->open(QIODevice::ReadOnly)) {
		QMessageBox::warning(0, "Loading metrics error",
		tr("Could not find ")+metric_location+tr(", no default settings added."));
		metricNames.append("Custom");
	}
	else {
		QTextStream in(file);
		QStringList fields;	
		while(!in.atEnd()) {
			fields = in.readLine().split(" ");
			if (fields.size() == 5) {
				metricNames.append(fields[0]);
				metricDp.append(fields[1].split("=")[1]);
				metricDx.append(fields[2].split("=")[1]);
				metricDcc.append(fields[3].split("=")[1]);
				metricVx.append(fields[4].split("=")[1]);
			}
		}
		metricNames.append("Custom");
	}
	delete file;
	
	return 0;
}

Data::~Data(){
	if (struct_data) delete struct_data;
	if (plan_data) delete plan_data;
}

int Data::buildEgsphant(EGSPhant* phant, QString* log, int contourNum, int defaultTAS,
					    QVector <int>* structIndex, QVector <int>* tasIndex,
					    QVector <EGSPhant*>* makeMasks, double buffer) {
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Building egsphant\n"; std::cout.flush();
	#endif
	
	newProgress("Building egsphant");
	
	QString medIdx = EGSPHANT_CHARS;
	QMap <QString, QChar> mediaIndex;
	QMap <int, int> structToTas;
	
	// Set min-max indices to highest or lowest possible values
	double xminBound = std::numeric_limits<double>::max();
	double yminBound = std::numeric_limits<double>::max();
	double zminBound = std::numeric_limits<double>::max();
	double xmaxBound = std::numeric_limits<double>::lowest();
	double ymaxBound = std::numeric_limits<double>::lowest();
	double zmaxBound = std::numeric_limits<double>::lowest();
	
	// Create a mapping from struct indices to non-default TAS indices for later
	for (int i = 0; i < contourNum; i++) {
		if ((*tasIndex)[i] >= 0)
			structToTas[(*structIndex)[i]] = (*tasIndex)[i];
	}
	
	// Start empty log
	*log = "";
	
	// Get all media names and assign indices and fill in media data in log file
	*log = *log + "--- Media contour and TAS data ---\n";
	*log = *log + "Default TAS used is " + TAS_names[defaultTAS] + ":\n";
	for (int i = 0; i < media[defaultTAS].size(); i++) {
		*log = *log + "  " + media[defaultTAS][i].rightJustified(20,' ') + " " + QString::number(threshold[defaultTAS][i]) + "\n";
		if (!mediaIndex.contains(media[defaultTAS][i]))
			mediaIndex.insert(media[defaultTAS][i],medIdx.at(mediaIndex.size()));
	}
	
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Made the default media array\n"; std::cout.flush();
		for (int i = 0; i < mediaIndex.size(); i++)
			std::cout << mediaIndex.keys()[i].toStdString() << " - " << mediaIndex.values()[i].toLatin1() << "\n";
	#endif
		
	*log = *log + "\nContour TAS defined below in order of descending priority:\n";
	for (int i = 0; i < contourNum; i++) {
		if (tasIndex->at(i) != -1) { // Not default TAS
			
			#if defined(DEBUG_BUILDEGSPHANT)
				std::cout << "  Adding structName[" << structIndex->at(i) << "]="; std::cout.flush();
				std::cout << structName[structIndex->at(i)].toStdString() << " media from TAS"; std::cout.flush();
				std::cout << "[" << tasIndex->at(i) << "]="; std::cout.flush();
				std::cout << TAS_names[tasIndex->at(i)].toStdString() << "\n"; std::cout.flush();
			#endif
			
			*log = *log + "  " + structName[structIndex->at(i)] + " struct is using TAS " + TAS_names[tasIndex->at(i)] + ":\n";
			for (int j = 0; j < media[tasIndex->at(i)].size(); j++) {
				*log = *log + "    " + media[tasIndex->at(i)][j].rightJustified(20,' ') + " " + QString::number(threshold[tasIndex->at(i)][j]) + "\n";
				if (!mediaIndex.contains(media[tasIndex->at(i)][j]))
					mediaIndex.insert(media[tasIndex->at(i)][j],medIdx.at(mediaIndex.size()));
			}
		}
		else {
			
			#if defined(DEBUG_BUILDEGSPHANT)
				std::cout << "  Using default TAS for structName[" << structIndex->at(i) << "]="; std::cout.flush();
				std::cout << structName[structIndex->at(i)].toStdString() << "\n"; std::cout.flush();
			#endif
			
			*log = *log + "  " + structName[structIndex->at(i)] + " struct is using default TAS\n";
		}
	}
	
	for (int i = 0; i < mediaIndex.size(); i++)
		for (int j = 0; j < mediaIndex.size(); j++)
			if (mediaIndex.values()[j] == medIdx.at(i))
				phant->media.append(mediaIndex.keys()[j]); // Set phantom media to those collected into mediaIndex
	
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Made the final media array\n"; std::cout.flush();
		for (int i = 0; i < phant->media.size(); i++)
			std::cout << phant->media[i].toStdString() << " - " << mediaIndex[phant->media[i]].toLatin1() << "\n";
	#endif
	
	*log = *log + "\nFinal media array:\n";
	for (int i = 0; i < phant->media.size(); i++)
		*log = *log + "  " + mediaIndex[phant->media[i]] + " - " + phant->media[i] + "\n";
	
	*log = *log + "----------------------------------\n";
	
	// Fetch HU units map from the file
	*log = *log + "--- HU to density conversion data ---\n";
	QVector <double> HUMap, denMap; // HU to density lookups
	QFile file (hu_location);
	QString tempS = "";
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream input(&file);
		
		while (!input.atEnd()) {
			tempS = input.readLine();
			
			if (tempS.contains(" ")) {
				HUMap << tempS.split(' ', QString::SkipEmptyParts)[0].toDouble();
				denMap << tempS.split(' ', QString::SkipEmptyParts)[1].toDouble();
				*log = *log + "  " + QString::number(HUMap.last()).rightJustified(8,' ') + " " + QString::number(denMap.last()) + "\n";
			}
			else if (tempS.contains("\t")) {
				HUMap << tempS.split('\t', QString::SkipEmptyParts)[0].toDouble();
				denMap << tempS.split('\t', QString::SkipEmptyParts)[1].toDouble();		
				*log = *log + "  " + QString::number(HUMap.last()).rightJustified(8,' ') + " " + QString::number(denMap.last()) + "\n";		
			}
			else {
				return 101;
			}
		}
    }
	else {
		return 102;	
	}
	file.close();
	*log = *log + "-------------------------------------\n";
	
	// Read CT data
	// Sort out all the DICOM data into the following
	double rescaleM = 1, rescaleB = 0, rescaleFlag;
    QVector <QVector <QVector <short int> > > HU;
    QVector <unsigned short int> xPix;
    QVector <unsigned short int> yPix;
    QVector <QVector <double> > imagePos;
    QVector <QVector <double> > xySpacing;
    QVector <double> zSpacing;
	
	*log = *log + "--- Parsing DICOM CT data ---\n";
    double increment = 5./double(CT_data.size()); // First 10% is reading DICOM
	emit newProgressName("Parsing DICOM data");
	
	for (int i = 0; i < CT_data.size(); i++) {
		
		#if defined(DEBUG_BUILDEGSPHANT)
			std::cout << "Parsing slice " << i << " of the CT data..."; std::cout.flush();
		#endif
		
		emit madeProgress(increment);
		
		rescaleFlag = 0;
		Attribute* tempAtt;
		
		// Pixel Spacing (Decimal String), row spacing and then column spacing (in mm)
		tempAtt = CT_data[i]->getEntry(0x0028,0x0030);
		if (tempAtt->tag[0] == 0x0028 && tempAtt->tag[1] == 0x0030) {
			xySpacing.resize(xySpacing.size()+1);
			xySpacing.last().resize(2);

			QString temp = "";
			for (unsigned int s = 0; s < tempAtt->vl; s++) {
				temp.append(tempAtt->vf[s]);
			}

			xySpacing.last()[0] = (temp.split('\\',QString::SkipEmptyParts)[0]).toDouble();
			xySpacing.last()[1] = (temp.split('\\',QString::SkipEmptyParts)[1]).toDouble();
		}
		else
			return 201;
		
		// Slice Thickness (Decimal String, in mm)
		tempAtt = CT_data[i]->getEntry(0x0018,0x0050);
		if (tempAtt->tag[0] == 0x0018 && tempAtt->tag[1] == 0x0050) {
			QString temp = "";
			for (unsigned int s = 0; s < tempAtt->vl; s++) {
				temp.append(tempAtt->vf[s]);
			}

			zSpacing.append(temp.toDouble());
		} 
		else
			return 202;
		
		// Image Position [x,y,z] (Decimal String, in mm)
		tempAtt = CT_data[i]->getEntry(0x0020,0x0032);
		if (tempAtt->tag[0] == 0x0020 && tempAtt->tag[1] == 0x0032) {
			imagePos.resize(imagePos.size()+1);
			imagePos.last().resize(3);

			QString temp = "";
			for (unsigned int s = 0; s < tempAtt->vl; s++) {
				temp.append(tempAtt->vf[s]);
			}

			imagePos.last()[0] = (temp.split('\\',QString::SkipEmptyParts)[0]).toDouble();
			imagePos.last()[1] = (temp.split('\\',QString::SkipEmptyParts)[1]).toDouble();
			imagePos.last()[2] = (temp.split('\\',QString::SkipEmptyParts)[2]).toDouble();
		} 
		else
			return 203;
		
		// Rows
		tempAtt = CT_data[i]->getEntry(0x0028,0x0010);
		if (tempAtt->tag[0] == 0x0028 && tempAtt->tag[1] == 0x0010) {
			if (CT_data[i]->isBigEndian)
				xPix.append((unsigned short int)(((short int)(tempAtt->vf[0]) << 8) +
				(short int)(tempAtt->vf[1])));
			else
				xPix.append((unsigned short int)(((short int)(tempAtt->vf[1]) << 8) +
				(short int)(tempAtt->vf[0])));	
		}
		else
			return 204;
		
		// Columns
		tempAtt = CT_data[i]->getEntry(0x0028,0x0011);
		if (tempAtt->tag[0] == 0x0028 && tempAtt->tag[1] == 0x0011) {
			if (CT_data[i]->isBigEndian)
				yPix.append((unsigned short int)(((short int)(tempAtt->vf[0]) << 8) +
				(short int)(tempAtt->vf[1])));
			else
				yPix.append((unsigned short int)(((short int)(tempAtt->vf[1]) << 8) +
				(short int)(tempAtt->vf[0])));		
		} 
		else
			return 205;
		
		// Rescale HU slope (assuming type is HU)
		tempAtt = CT_data[i]->getEntry(0x0028,0x1053);
		if (tempAtt->tag[0] == 0x0028 && tempAtt->tag[1] == 0x1053) {
			QString temp = "";
			for (unsigned int s = 0; s < tempAtt->vl; s++) {
				temp.append(tempAtt->vf[s]);
			}
			
			rescaleM = temp.toDouble();
			rescaleFlag++;
		}
		
		// Rescale HU intercept (assuming type is HU)
		tempAtt = CT_data[i]->getEntry(0x0028,0x1052);
		if (tempAtt->tag[0] == 0x0028 && tempAtt->tag[1] == 0x1052) {
			QString temp = "";
			for (unsigned int s = 0; s < tempAtt->vl; s++) {
				temp.append(tempAtt->vf[s]);
			}
			
			rescaleB = temp.toDouble();
			rescaleFlag++;
		}
		
		// HU values
		tempAtt = CT_data[i]->getEntry(0x7FE0,0x0010);
		if (tempAtt->tag[0] == 0x7FE0 && tempAtt->tag[1] == 0x0010) {
			HU.resize(HU.size()+1);
			if (HU.size() == xPix.size() && HU.size() == yPix.size()) {
				HU.last().resize(yPix.last());
				for (unsigned int k = 0; k < yPix.last(); k++) {
					HU.last()[k].resize(xPix.last());
				}
				
				short int temp;
				if (CT_data[i]->isBigEndian)
					for (unsigned int s = 0; s < tempAtt->vl; s+=2) {
						temp  = (tempAtt->vf[s+1]);
						temp += (short int)(tempAtt->vf[s]) << 8;
						
						HU.last()[int(int(s/2)/xPix.last())][int(s/2)%xPix.last()] =
							rescaleFlag == 2 ? rescaleM*temp+rescaleB : temp;
					}
				else
					for (unsigned int s = 0; s < tempAtt->vl; s+=2) {
						temp  = (tempAtt->vf[s]);
						temp += (short int)(tempAtt->vf[s+1]) << 8;
						
						HU.last()[int(int(s/2)/xPix.last())][int(s/2)%xPix.last()] =
							rescaleFlag == 2 ? rescaleM*temp+rescaleB : temp;
					}
			}
		}
		else
			return 208;
		
		#if defined(DEBUG_BUILDEGSPHANT)
			std::cout << " parsed!\n"; std::cout.flush();
		#endif
		
    }
	*log = *log + "Extracted all HU data for the " + QString::number(CT_data.size()) + " (" + QString::number(xPix[0]) + "x" + QString::number(yPix[0]) + ") slices\n";
	*log = *log + "-----------------------------\n";
	
	// Build the actual egsphant
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Constructing egsphant dimensions from "
		<< CT_data.size() << " (" << xPix[0] << "x" << yPix[0] << ") slices\n";  std::cout.flush();
	#endif
	
	// Assume first slice matches the rest and set x, y, and z boundaries
	phant->nx = xPix[0];
	phant->ny = yPix[0];
	phant->nz = CT_data.size();
    phant->x.fill(0,phant->nx+1);
    phant->y.fill(0,phant->ny+1);
    phant->z.fill(0,phant->nz+1);
	
	QMap <QString, int> medVol, structVol; // Arrays for holding voxel counts for log file
	for (int i = 0; i < structName.size(); i++)
		structVol[structName[i]] = 0;
	for (int i = 0; i < phant->media.size(); i++)
		medVol[phant->media[i]] = 0;
	
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "  Assigning x and y bounds\n"; std::cout.flush();
	#endif
	
    for (int i = 0; i <= phant->nx; i++)
		phant->x[i] = (imagePos[0][0]+(i-0.5)*xySpacing[0][0])/10.0;
    for (int i = 0; i <= phant->ny; i++)
		phant->y[i] = (imagePos[0][1]+(i-0.5)*xySpacing[0][1])/10.0;

	// Define z bound values
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "  Assigning slice z bounds\n"; std::cout.flush();
	#endif
	
	double prevZ, nextZ;
	nextZ = imagePos[0][2]-zSpacing[0]/2.0;
    for (int i = 0; i < phant->nz; i++) {
		prevZ = nextZ/2.0 + (imagePos[i][2]-zSpacing[i]/2.0)/2.0;
		nextZ = imagePos[i][2]+zSpacing[i]/2.0;
		phant->z[i] = prevZ/10.0;
	}
	phant->z.last() = nextZ/10.0;
		
	// Set up media array
	{
		QVector <char> mz(phant->nz, 0);
		QVector <QVector <char> > my(phant->ny, mz);
		QVector <QVector <QVector <char> > > mx(phant->nx, my);
		phant->m = mx;
	}
		
	// Setup density array
	{
		QVector <double> dz(phant->nz, 0);
		QVector <QVector <double> > dy(phant->ny, dz);
		QVector <QVector <QVector <double> > > dx(phant->nx, dy);
		phant->d = dx;
	}
		
	// Get bounding rectangles over each struct
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Constructing bounds around the structs\n"; std::cout.flush();
	#endif
	
	//*log = *log + "Struct boundaries\n";
	
	QVector <QVector <QRectF> > structRect;
	
	for (int i = 0; i < structPos.size(); i++) {
		structRect.resize(i+1);
		
		#if defined(DEBUG_BUILDEGSPHANT)
			std::cout << "  Struct: " + structName[i].toStdString() + "\n"; std::cout.flush();
		#endif
		
		//*log = *log + "  Struct: " + structName[i] + "\n";
		
		for (int j = 0; j < structPos[i].size(); j++) {
			structRect[i].resize(j+1);
			structRect[i][j] = structPos[i][j].boundingRect();
			
			#if defined(DEBUG_BUILDEGSPHANT)
				std::cout << "    z = " << structZ[i][j] << " : (";
				std::cout << structRect[i][j].top() << ",";
				std::cout << structRect[i][j].left() << "),(";
				std::cout << structRect[i][j].bottom() << ",";
				std::cout << structRect[i][j].right() << ")\n"; std::cout.flush();
			#endif
			
			// Track the exact boundaries of all the structures, remember y is flipped
			if (buffer != -1) {
				if (xminBound > structRect[i][j].left())
					xminBound = structRect[i][j].left();
				if (yminBound > structRect[i][j].top())
					yminBound = structRect[i][j].top();
				if (zminBound > structZ[i][j])
					zminBound = structZ[i][j];
				if (xmaxBound < structRect[i][j].right())
					xmaxBound = structRect[i][j].right();
				if (ymaxBound < structRect[i][j].bottom())
					ymaxBound = structRect[i][j].bottom();
				if (zmaxBound < structZ[i][j])
					zmaxBound = structZ[i][j];
			}
		}
	}
		
	// Push the limits by the boundary buffer
	xminBound -= buffer;
	yminBound -= buffer;
	zminBound -= buffer;
	xmaxBound += buffer;
	ymaxBound += buffer;
	zmaxBound += buffer;
	
	// Arrays that hold the struct numbers and center voxel values to be used
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Assigning density using HU\n"; std::cout.flush();
	#endif
	
	QList<QPoint> zIndex, yIndex;
	QList<QPoint>::iterator p;
	double zMid, yMid, xMid, temp;
	int tempHU = 0, n = 0, q = 0, inStruct = 0;
	
	// Convert HU to density and media
    increment = 15.0/double(phant->nz); // 45% is making the egsphant (15 for density, 30 for media)
	emit newProgressName("Building density arrays");
	
	for (int k = 0; k < phant->nz; k++) { // Z //
		emit madeProgress(increment);
		for (int j = 0; j < phant->ny; j++) { // Y //
			for (int i = 0; i < phant->nx; i++) { // X //
				
				tempHU = HU[k][j][i];
				
				// Linear search because I don't think these arrays every get big
				// get the right density
				for (n = 0; n < HUMap.size()-1; n++)
					if (HUMap[n] <= tempHU && tempHU < HUMap[n+1])
						break;
					
				if (tempHU < HUMap[0])
					n = 0;
				
				temp = interp(tempHU,HUMap[n],HUMap[n+1],denMap[n],denMap[n+1]);
				temp = temp<=0?0.000001:temp; // Set min density to 0.000001
				
				if (temp > phant->maxDensity) // Track max density for images
					phant->maxDensity = temp;
				
				// Assign density
				phant->d[i][j][k] = temp;
			}
		}
	}
	
	// Perform metallic artifact reduction
	emit newProgressName("Metallic artifact reduction");
		
	*log = *log + "--- Metallic artifact reduction ---\n";
	if (do_MAR) {
		*log = *log + "MAR is requested\n";
		*log = *log + QString("  Set all densities outside of range [%1,%2] to %3\n").arg(lowerThresh).arg(upperThresh).arg(marDen);
		*log = *log + QString("  within %1 cm of the source\n").arg(marRad);		
		if (marContourInd != -1) {
			*log = *log + "  only in contour " + marContour + "\n";			
		}
		*log = *log + "\n";	
	}
	else
		*log = *log + "no MAR is requested\n";	
	
	if (do_MAR) {
		// Input reading code
		QFile file (gui_location+"/database/transformation/"+transformFile);
		QString line;
		
		// Variables that will hold dimensions and fetched density
		double xP, yP, zP, dens;
		int minX, minY, minZ, maxX, maxY, maxZ;
		bool inStruct;
		
		// A set (hash table) that we will append all unique values to
		QSet <int> voxels;
		
		if (file.open(QIODevice::ReadOnly)) {
			QTextStream in (&file);
			while(!in.atEnd()) {
				
				// Get the transformation lines from the text
				line = in.readLine();
				if (line.contains("translation =")) {
					line = line.split("=")[1].trimmed();
					xP = line.split(" ")[0].trimmed().toDouble();
					yP = line.split(" ")[1].trimmed().toDouble();
					zP = line.split(" ")[2].trimmed().toDouble();
					
					inStruct = false;
					if (contourNum > 0 && marContourInd != -1) {
						for (int m = 0; m < structZ[structIndex->at(marContourInd)].size(); m++) {
							// If slice j of struct i on the same plane as slice k of the phantom
							if (abs(structZ[structIndex->at(marContourInd)][m] - zP) < 0.1) { // 1 mm threshold
								if (structPos[structIndex->at(marContourInd)][m].containsPoint(QPointF(xP,yP), Qt::OddEvenFill))
									inStruct = true;
							}
						}
					}
					else
						inStruct = true;
					
					if (inStruct) {
						// Get the indices
						minX = phant->getIndex("x axis",xP-marRad);
						minY = phant->getIndex("y axis",yP-marRad);
						minZ = phant->getIndex("z axis",zP-marRad);
						maxX = phant->getIndex("x axis",xP+marRad)+1;
						maxY = phant->getIndex("y axis",yP+marRad)+1;
						maxZ = phant->getIndex("z axis",zP+marRad)+1;
						if (minX < 0 || minY < 0 || minZ < 0 || maxX < 0 || maxY < 0 || maxZ < 0) {
							*log = *log + QString("source position/radius out of bounds error for source [%1,%2,%3], skipping it\n").arg(xP).arg(yP).arg(zP);
							inStruct = false;
						}
						
						// Add all the voxels within marRad to the voxels set
						if (inStruct) {
							for (int k = minZ; k < maxZ; k++) {
								for (int j = minY; j < maxY; j++) {
									for (int i = minX; i < maxX; i++) {
										voxels.insert(i+(j*phant->nx)+(k*phant->nx*phant->ny));
									}
								}
							}
						}
						*log = *log + QString("    MAR performed at source position [%1,%2,%3]\n").arg(xP).arg(yP).arg(zP);
					}
				}
			}
			
			// Now do threshold replacement to all voxels in voxels set
			QSet<int>::iterator it = voxels.begin();
			int count = 0;
			increment = 2.5/voxels.size(); // 2.5%
			int i, j, k;
			
			while (it != voxels.end()) {
				i = *it;
				k = int(i/phant->nx/phant->ny);
				j = int(i/phant->nx)%phant->ny;
				i = i%phant->nx;
				
				dens = phant->getDensity(i,j,k);
				emit madeProgress(increment);
				if (dens < lowerThresh || dens > upperThresh) {
					phant->setDensity(i,j,k,marDen);
					count++;
				}
				it++;
			}
			*log = *log + "\nMAR applied to " + QString::number(count) + " of the " + QString::number(voxels.size()) + " evaluated voxels\n";	
		}
		else {
			*log = *log + "failed to open transport file, MAR aborted\n";
			increment = 5.0; // 5%
			emit madeProgress(increment);
		}
	}
	else {
		increment = 5.0; // 5%
		emit madeProgress(increment);
	}
	
	*log = *log + "-----------------------------------\n";
		
	// Truncate phantom to new boundaries if requested
	if (buffer != -1) {
		*log = *log + "--- Truncating the phantom limits ---\n";
		
		*log = *log + "Initial phantom specifications:\n";
		*log = *log + QString::number(phant->nz) + " z slices ranging from " + QString::number(phant->z[0]) + " to " + QString::number(phant->z.last()) + "\n";
		*log = *log + QString::number(phant->nx) + " x voxels ranging from " + QString::number(phant->x[0]) + " to " + QString::number(phant->x.last()) + "\n";
		*log = *log + QString::number(phant->ny) + " y voxels ranging from " + QString::number(phant->y[0]) + " to " + QString::number(phant->y.last()) + "\n\n";
		
		phant->redefineBounds(xminBound, yminBound, zminBound, xmaxBound, ymaxBound, zmaxBound);
		*log = *log + "Truncated phantom boundaries to contain all structures with a buffer of " + QString::number(buffer) + " cm.\n\n";
				
		*log = *log + "Final phantom specifications:\n";
		*log = *log + QString::number(phant->nz) + " z slices ranging from " + QString::number(phant->z[0]) + " to " + QString::number(phant->z.last()) + "\n";
		*log = *log + QString::number(phant->nx) + " x voxels ranging from " + QString::number(phant->x[0]) + " to " + QString::number(phant->x.last()) + "\n";
		*log = *log + QString::number(phant->ny) + " y voxels ranging from " + QString::number(phant->y[0]) + " to " + QString::number(phant->y.last()) + "\n";
		
		*log = *log + "-----------------------------------\n";
	}
	
	// Set up masks
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Making masks\n"; std::cout.flush();
	#endif
	
	for (int i = 0; i < contourNum; i++) {
		EGSPhant* temp = new EGSPhant;
		temp->makeMask(phant);
		makeMasks->append(temp);
	}
		
	// Convert density to media
	*log = *log + "--- Assigning the egsphant media ---\n";
	emit newProgressName("Building media arrays");
	increment = 30.0/double(phant->nz); // 30%
	bool structAssigned = false;
	
	*log = *log + "Added slices z midpoints:\n\n";
		
	for (int k = 0; k < phant->nz; k++) { // Z //
		emit madeProgress(increment);
		zMid = (phant->z[k]+phant->z[k+1])/2.0;
			
		// Preprocess step to check which structs to look up on this slices\n
		// zIndex is going to have all indices of structRect that we will need to look up
		if (contourNum > 0) {
			zIndex.clear(); // Reset lookup
			for (int l = 0; l < contourNum; l++) {
					// Either find a struct defined within a voxel z
					#if defined(INTERPOLATE_CONTOURS)
						bool foundFlag = false;
						for (int m = 0; m < structZ[structIndex->at(l)].size(); m++) {
							// If slice j of struct i on the same plane as slice k of the phantom
							if (abs(structZ[structIndex->at(l)][m] - zMid) < (phant->z[k+1]-phant->z[k])/2.0) { // && tasIndex->at(l) != -1) { // Don't filter non-default to be able to tally
								zIndex << QPoint(structIndex->at(l),m); // Add it to lookup
								foundFlag = true;
							}
						}
						// Or, if the struct has a z above and below it, transpose the nearest struct in z
						if (!foundFlag) {
							double below = -1000000, above = 1000000, diff; // Hopefully huge lower/upper bounds
							int iBelow = -1, iAbove = -1;
							for (int m = 0; m < structZ[structIndex->at(l)].size(); m++) {
								diff = structZ[structIndex->at(l)][m] - zMid;
								if (diff > 0 && diff < above) {
									above = diff;
									iAbove = m;
								}
								if (diff < 0 && diff > below) {
									below = diff;
									iBelow = m;
								}
							}
							
							// If there is a contour above and below this slice, assign the closest contour
							if (below != -1000000 && above != 1000000) {
								if (above < -below)
									zIndex << QPoint(structIndex->at(l),iAbove); // Add it to lookup
								else
									zIndex << QPoint(structIndex->at(l),iBelow); // Add it to lookup
							}
						}
					#else //#elif
						// Find the first struct within the voxel
						for (int m = 0; m < structZ[structIndex->at(l)].size(); m++) {
							// If slice j of struct i on the same plane as slice k of the phantom
							if (abs(structZ[structIndex->at(l)][m] - zMid) < (phant->z[k+1]-phant->z[k])/2.0) { // && tasIndex->at(l) != -1) { // Don't filter non-default to be able to tally
								zIndex << QPoint(structIndex->at(l),m); // Add it to lookup
							}
						}
					#endif
				}
		}
		
		#if defined(DEBUG_BUILDEGSPHANT)
			std::cout << "  In slice " << k << " checking against " << zIndex.size() << " structs:\n"; std::cout.flush();
			for (int n = 0; n < zIndex.size(); n++) {
				std::cout << "    structName[" << zIndex[n].x() << "]=" << structName[zIndex[n].x()].toStdString() << "\n"; std::cout.flush();
			}
		#endif
		
		for (int j = 0; j < phant->ny; j++) { // Y //
			yMid = (phant->y[j]+phant->y[j+1])/2.0;
		
			// Preprocess step to check which structs to look up on this pixel column\n
			// yIndex is going to have all indices of structPos that we need to look up
			if (zIndex.size() > 0) {
				yIndex.clear(); // Reset lookup
				for (p = zIndex.begin(); p != zIndex.end(); p++) {
					// If column p->y() of struct p->x() is on the same column as slice k,j of the phantom
					if (structRect[p->x()][p->y()].top() <= yMid && yMid <= structRect[p->x()][p->y()].bottom()) {
						yIndex << *p;
					}
				}
			}
			
			#if defined(DEBUG_BUILDEGSPHANT)
				std::cout << "    In y column " << j << " checking against " << yIndex.size() << " structs:\n"; std::cout.flush();
				for (int n = 0; n < yIndex.size(); n++) {
					std::cout << "      structName[" << yIndex[n].x() << "]=" << structName[yIndex[n].x()].toStdString() << "\n"; std::cout.flush();
				}
			#endif
			
			for (int i = 0; i < phant->nx; i++) { // X //
				xMid = (phant->x[i]+phant->x[i+1])/2.0;
				temp = phant->d[i][j][k];
				structAssigned = false;
				
				// Check if we are in a structure
				inStruct = -1;
				if (yIndex.size() > 0) {
					for (p = yIndex.begin(); p != yIndex.end(); p++) { // Check through each
					// If row p->y() of struct p->x() on the same row as slice k,j,i of the phantom
						if (structRect[p->x()][p->y()].left() <= xMid && xMid <= structRect[p->x()][p->y()].right())
							if (structPos[p->x()][p->y()].containsPoint(QPointF(xMid,yMid), Qt::OddEvenFill)) {
								if (!structAssigned) {
									inStruct = p->x();
									structAssigned = true; // Assign first struct found and quit, assumed highest priority
								}
								
								// Count structure volume
								structVol[structName[p->x()]]++;
								
								// Setup mask (revert structIndex to global index)
								(*makeMasks)[p->x()]->m[i][j][k] = 50;
							}
					}
				}
				
				q = defaultTAS; // Default tissue assignment scheme
					
				if (inStruct > -1) { // Change TAS if we are in structure
					// Check to see if a TAS is assigned
					if (structToTas.contains(inStruct))
						q = structToTas[inStruct];
				}		

				// Find the right media in the right TAS
				for (n = 0; n < threshold[q].size()-1; n++)
					if (temp < threshold[q][n])
						break;
				
				// Assign that media
				phant->m[i][j][k] = mediaIndex[media[q][n]].toLatin1();
				
				// Count media volume
				medVol[media[q][n]]++;
			}
		}
		*log = *log + QString::number(zMid) + " ";
	}
	
	// Reverse y-axis - keep image "flipped" and just change the preview images to match to invert y
	//emit newProgressName("Inverting y-axis");
	//increment = 2.5/phant->nz; // 2.5%
	//char tempMed;
	//double tempDen;
	//for (int k = 0; k < phant->nz; k++) {
	//	for (int j = 0; j < int(phant->ny/2); j++) {
	//		nj = phant->ny-1-j;
	//		for (int i = 0; i < phant->nx; i++) {
	//			tempMed = phant->m[i][j][k];
	//			tempDen = phant->d[i][j][k];
	//			phant->m[i][j][k] = phant->m[i][nj][k];
	//			phant->d[i][j][k] = phant->d[i][nj][k];
	//			phant->m[i][nj][k] = tempMed;
	//			phant->d[i][nj][k] = tempDen;
	//		}
	//	}
	//	emit madeProgress(increment);
	//}	
	
	*log = *log + "\n\nEGSPhant building is complete\n";
	
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Egsphant built\n"; std::cout.flush();
	#endif
	
	*log = *log + "-----------------------------\n";
	*log = *log + "--- Outputting voxel counts ---\n";
	
	*log = *log + "Media voxel count:\n";
	for (int i = 0; i < medVol.size(); i++)
		*log = *log + "  " + medVol.keys()[i].left(20).rightJustified(20,' ') + " " + QString::number(medVol.values()[i]).rightJustified(12,' ') + "\n";
	
	*log = *log + "Structure voxel count:\n";
	for (int i = 0; i < structVol.size(); i++)
		*log = *log + "  " + structVol.keys()[i].left(20).rightJustified(20,' ') + " " + QString::number(structVol.values()[i]).rightJustified(12,' ') + "\n";
	
	*log = *log + "-------------------------------\n";
			
	#if defined(DEBUG_BUILDEGSPHANT)
		std::cout << "Log complete\n"; std::cout.flush();
	#endif
	
	return 0;
}

double Data::interp(double x, double x1, double x2, double y1, double y2) {
	return (y2*(x-x1)+y1*(x2-x))/(x2-x1);
}

int Data::parsePlan(QString* log) {
	// This will hold and eventually return errors/information\n
	*log = "";
	
	// Needed variables for parsing the data
	QVector <Attribute *> *att, *att2, *att3;
	QByteArray tempData, tempData2, tempData3;
	QStringList pointData2;
	QString treatDate = "", kermaDate = "";
	QString treatTime = "", kermaTime = "";
	
	// Reset plan data variables to default to know what worked
	treatmentType = "UNKNOWN";
	treatmentTechnique = "UNKNOWN";
	isotopeName = "UNKNOWN";
	seedPos.clear();
	seedTime.clear();
	seedInfo = "UNKNOWN";
	airKerma = halfLife = -1;
	
	#if defined(ASSUME_PERMANENT_LDR)
		treatmentType = "LDR";
		treatmentTechnique = "PERMANENT";
	#endif
	
	*log = *log + "----------------------------------------------\n";
	
	// Treatment Type
	Attribute* tempAtt = plan_data->getEntry(0x300A, 0x0202); // Get att closest to (300A,0202)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0202) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		treatmentType = temp.trimmed();
	}
	else if (!treatmentType.compare("UNKNOWN")) {
		return 101;
	}
	*log = *log + "Treatment type: " + treatmentType + "\n";
	
	// Treatment technique
	tempAtt = plan_data->getEntry(0x300A, 0x0200); // Get att closest to (300A,0200)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0200) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		treatmentTechnique = temp.trimmed();
	}
	else if (!treatmentTechnique.compare("UNKNOWN")) {
		return 102;
	}
	*log = *log + "Treatment technique: " + treatmentTechnique + "\n";
	
	// Treatment time
	tempAtt = plan_data->getEntry(0x300A, 0x0006); // Get att closest to (300A,0006)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0006) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		treatDate = temp.trimmed();
	}
	tempAtt = plan_data->getEntry(0x300A, 0x0007); // Get att closest to (300A,0007)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0007) {
		QString temp = "";
		for (unsigned int s = 0; s < tempAtt->vl; s++) {
			temp.append(tempAtt->vf[s]);
		}
		treatTime = temp.trimmed();
	}
	*log = *log + "Treatment time: " + treatDate.left(4) + "-" + 
		   treatDate.left(6).right(2) + "-" + treatDate.left(8).right(2) + " " +
		   treatTime.left(2) + ":" + treatTime.left(4).right(2) + ":" +
		   treatTime.left(6).right(2) +  "\n";
		
	*log = *log + "----------------------------------------------\n";
		
	// Source sequence
	tempAtt = plan_data->getEntry(0x300A, 0x0210); // Get att closest to (300A,0210)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0210) {
		for (int k = 0; k < tempAtt->seq.items.size(); k++) {
			tempData = QByteArray((char *)tempAtt->seq.items[k]->vf,tempAtt->seq.items[k]->vl);
			QDataStream dataStream(tempData);

			att = new QVector <Attribute *>;
			if (!plan_data->parseSequence(&dataStream, att)) {
				return 103;
			}

			QString tempS   = ""; // Get the air kerma
			QString tempI   = ""; // Get the half life
			QString tempE   = ""; // Get the isotope name
			QString tempInf = ""; // Get seed data
			QString tempD   = ""; // Get date data
			QString tempT   = ""; // Get time data
			for (int l = 0; l < att->size(); l++) {
				if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x022A)
					for (unsigned int s = 0; s < att->at(l)->vl; s++)
						tempS.append(att->at(l)->vf[s]);
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x0228)
					for (unsigned int s = 0; s < att->at(l)->vl; s++)
						tempI.append(att->at(l)->vf[s]);
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x0226)
					for (unsigned int s = 0; s < att->at(l)->vl; s++)
						tempE.append(att->at(l)->vf[s]);
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x0216) {
					if (tempInf != "")
						tempInf = tempInf + " - ";
					
					for (unsigned int s = 0; s < att->at(l)->vl; s++) {
						tempInf.append(att->at(l)->vf[s]);
					}
				}
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x021B) {
					if (tempInf != "")
						tempInf = tempInf + " - ";
					
					for (unsigned int s = 0; s < att->at(l)->vl; s++) {
						tempInf.append(att->at(l)->vf[s]);
					}
				}
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x021C) {
					*log = *log + "Additional seed description:\n" + tempInf + "\n";
				}
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x022C)
					for (unsigned int s = 0; s < att->at(l)->vl; s++)
						tempD.append(att->at(l)->vf[s]);
				else if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x022E)
					for (unsigned int s = 0; s < att->at(l)->vl; s++)
						tempT.append(att->at(l)->vf[s]);
			}

			if (airKerma == -1 && tempS != "")
				airKerma = tempS.toDouble();
			if (halfLife == -1 && tempI != "")
				halfLife = tempI.toDouble();
			if (isotopeName == "UNKNOWN" && tempI != "")
				isotopeName = tempE.trimmed();
			if (seedInfo == "UNKNOWN" && tempI != "")
				seedInfo = tempInf.trimmed();
			if (kermaDate == "" && tempD != "")
				kermaDate = tempD.trimmed();
			if (kermaTime == "" && tempT != "")
				kermaTime = tempT.trimmed();

			for (int l = 0; l < att->size(); l++) {
				delete att->at(l);
			}
			delete att;
		}
	}
	
	// Check to see if all the (needed) fields were found
	//if (isotopeName == "UNKNOWN") {
	//	return 201;
	//}
	*log = *log + "Isotope: " + isotopeName + "\n";
	
	if (airKerma == -1) {
		return 202;
	}
	*log = *log + "Air kerma rate (uGy/h): " + QString::number(airKerma) + "\n"; // At 1 meter
	
	airKerma *= 100*100*0.000001; // Convert to rate: convert uGy to Gy and multiply by d^2 (100 cm)
	
	*log = *log + "Air kerma strength (Gy*cm^2/h): " + QString::number(airKerma) + "\n";
	//if (halfLife == -1) {
	//	return 203;
	//}
	*log = *log + "Half-life (h): " + QString::number(halfLife*24) + "\n";
	
	//if (seedInfo == "UNKNOWN") {
	//	return 204;
	//}
	
	*log = *log + "Measurement time: " + kermaDate.left(4) + "-" + 
		   kermaDate.left(6).right(2) + "-" + kermaDate.left(8).right(2) + " " +
		   kermaTime.left(2) + ":" + kermaTime.left(4).right(2) + ":" +
		   kermaTime.left(6).right(2) +  "\n";
	
	// Removed as it seems plan data auto-adjusts strength for treatment date
	//if (kermaDate != "" && treatDate != "" && kermaTime != "" && treatTime != "") {
	//	tm treat, kerma;
	//	treat.tm_year = treatDate.left(4).toInt()-1900;
	//	kerma.tm_year = kermaDate.left(4).toInt()-1900;
	//	treat.tm_mon = treatDate.left(6).right(2).toInt()-1;
	//	kerma.tm_mon = kermaDate.left(6).right(2).toInt()-1;
	//	treat.tm_mday = treatDate.left(8).right(2).toInt();
	//	kerma.tm_mday = kermaDate.left(8).right(2).toInt();
	//	treat.tm_hour = treatTime.left(2).toInt();
	//	kerma.tm_hour = kermaTime.left(2).toInt();
	//	treat.tm_min = treatTime.left(4).right(2).toInt();
	//	kerma.tm_min = kermaTime.left(4).right(2).toInt();
	//	treat.tm_sec = treatTime.left(6).right(2).toInt();
	//	kerma.tm_sec = kermaTime.left(6).right(2).toInt();
	//	
	//	double timeDiff = double(mktime(&treat)-mktime(&kerma))/3600.0; // Time difference in hours
	//	*log = *log + "Time difference (h): " + QString::number(timeDiff) + "\n";
	//	
	//	airKerma *= pow(2,-timeDiff/(halfLife*24.0));
	//	
	//	*log = *log + "Adjusted air kerma strength (Gy*cm^2/h): " + QString::number(airKerma) + "\n";		
	//}
	
	*log = *log + "Additional seed data: " + seedInfo + "\n";
	*log = *log + "----------------------------------------------\n";
	
	QVector<double> dwellTime;
	QVector<QVector3D> position;
	
	QVector<double> tempDwellTimes;
	QVector<QVector3D> tempPositions;
	double tempTotalDwellTime;
	QVector3D tempPosition;
	
	treatmentTime = 0;
	
	// Application Setup Sequence (looking for control point position)
	tempAtt = plan_data->getEntry(0x300A, 0x0230); // Get att closest to (300A,0230)
	if (tempAtt->tag[0] == 0x300A && tempAtt->tag[1] == 0x0230) {
		for (int k = 0; k < tempAtt->seq.items.size(); k++) {
			tempData = QByteArray((char*)tempAtt->seq.items[k]->vf, tempAtt->seq.items[k]->vl);
			QDataStream dataStream(tempData);

			att = new QVector <Attribute *>;
			if (!plan_data->parseSequence(&dataStream, att)) {
				return 301;
			}

			QString tempS = ""; // Get the channel, it's another nested sequence, so we must go deeper with parseSequence
			for (int l = 0; l < att->size(); l++) {
				if (att->at(l)->tag[0] == 0x300A && att->at(l)->tag[1] == 0x0280) {
					for (int k = 0; k < att->at(l)->seq.items.size(); k++) {
						tempData2 = QByteArray((char*)att->at(l)->seq.items[k]->vf, att->at(l)->seq.items[k]->vl);
						QDataStream dataStream2(tempData2);

						att2 = new QVector <Attribute *>;
						if (!plan_data->parseSequence(&dataStream2, att2)) {
							return 302;
						}
						
						// Here, each seed will have its own subsequence with tag (300A,02D0)
						// Average all the control points 3D positions and get cumulative time
						// and fill dwell time and position as we go
						
						QString tempS = ""; // Get the brachy sequence, it's another nested sequence, so we must go deeper with parseSequence
						for (int m = 0; m < att2->size(); m++) {
							if (att2->at(m)->tag[0] == 0x300A && att2->at(m)->tag[1] == 0x02D0) { // Control sequence
								tempDwellTimes.clear();
								tempPositions.clear();
								tempTotalDwellTime = 0;
					
								for (int k2 = 0; k2 < att2->at(m)->seq.items.size(); k2++) {
									tempData3 = QByteArray((char*)att2->at(m)->seq.items[k2]->vf, att2->at(m)->seq.items[k2]->vl);
									QDataStream dataStream3(tempData3);

									att3 = new QVector <Attribute *>;
									if (!plan_data->parseSequence(&dataStream3, att3)) {
										return 303;
									}

									QString tempS = "";
									QString tempT = "";
									for (int n = 0; n < att3->size(); n++) {
										if (att3->at(n)->tag[0] == 0x300A && att3->at(n)->tag[1] == 0x02D4) { // Seed position
											for (unsigned int s = 0; s < att3->at(n)->vl; s++) {
												tempS.append(att3->at(n)->vf[s]);
											}

											pointData2 = tempS.split('\\');
											
											for (int p = 0; p < pointData2.size(); p+=3) {
												tempPositions.append(QVector3D(pointData2[p].toDouble()/10.0,
																			   pointData2[p+1].toDouble()/10.0,
																			   pointData2[p+2].toDouble()/10.0));
											}
										}
										else if (att3->at(n)->tag[0] == 0x300A && att3->at(n)->tag[1] == 0x02D6) { // Seed time weight

											for (unsigned int s = 0; s < att3->at(n)->vl; s++) {
												tempT.append(att3->at(n)->vf[s]);
											}

											tempDwellTimes.append(tempT.toDouble());
											tempTotalDwellTime += tempT.toDouble();
										}
									}
									for (int n = 0; n < att3->size(); n++) {
										delete att3->at(n);
									}
									delete att3;
								}
							}
						}

						// Convert all the control point data into a seed and a weight
						if (treatmentTechnique == "PERMANENT") {
							tempPosition = tempPositions[0]*tempDwellTimes[0];
							for (int i = 1; i < tempPositions.size(); i++)
								tempPosition += tempPositions[i]*tempDwellTimes[i];
							tempPosition /= tempTotalDwellTime;

							seedPos.append(tempPosition);
							seedTime.append(tempTotalDwellTime);
						}
						
						if (treatmentTechnique != "PERMANENT") { // Measure total time for later calculation
							tempTotalDwellTime = 0;
							for (int i = 1; i < tempPositions.size(); i++) {
								if (tempPositions[i] == tempPositions[i-1] && tempDwellTimes[i] != tempDwellTimes[i-1]) {
									seedPos.append(tempPositions[i]);
									seedTime.append(tempDwellTimes[i]-tempDwellTimes[i-1]);
									tempTotalDwellTime += seedTime.last();
								}
							}

							treatmentTime += tempTotalDwellTime;
						}
						
						for (int n = 0; n < att2->size(); n++) {
							delete att2->at(n);
						}
						delete att2;
					}
				}
			}
		}
	}
	
	// Check to see if seed data was found	
	*log = *log + "Found " + QString::number(seedPos.size()) + " seeds\n";
	treatmentTime /= 3600; // Go from seconds to hours
	if (treatmentTime > 0) {
		*log = *log + "  with " + QString::number(seedTime.size()) + " associated dwell times over " + treatmentTime + "\n";
	}
	
	*log = *log + "----------------------------------------------\n";
	for (int i = 0; i < seedPos.size(); i++) {
		*log = *log + QString("%1) [%2,%3,%4]").arg(i+1, 3)
		.arg(seedPos[i].x(),9,'g',5).arg(seedPos[i].y(),9,'g',5).arg(seedPos[i].z(),9,'g',5);
		
		if (treatmentTechnique != "PERMANENT")
			*log = *log + QString(" - %1").arg(seedTime[i],8,'g',5);
		
		*log = *log + QString("\n");
	}
	*log = *log + "----------------------------------------------\n";
		
	return 0;
}

int Data::outputRTDose(QString dosePath, QString errorPath, Dose* output, QString doseScaling, double n) {	
	static int counter = 1;
	// Generate UID
	QString UID = "1.2.840";                        // Common root
	UID        += ".1111";                          // Organization ID (currently a placeholder)
	UID        += ".1";                             // eb_gui ID
	UID        += "." + QString::number(counter++); // study ID
	
	QString UID2 = UID+".2";                        // series ID
	UID         += ".1";                            // series ID
	
	if (UID.length()%2)
		UID += " ";
	if (UID2.length()%2)
		UID2 += " ";

	// Open the DICOM file
    QFile file(dosePath);
    QFile file2(errorPath);
    if (file.open(QIODevice::WriteOnly) && file2.open(QIODevice::WriteOnly)) {
        unsigned char *dat;
        QDataStream out(&file);
        QDataStream out2(&file2);
        out.setByteOrder(QDataStream::LittleEndian);
		
		emit newProgressName("Outputting DICOM tag data");
		
		// Output the header, can be anything, needs to be 128 characters long
		dat = new unsigned char[128];
		memcpy((void*)dat,(void*)"          This file is written by the eb_gui application intended for use with egs_brachy, written by Martin Martinov.          ",128);
		out.writeRawData((char*)dat, 128);
		out2.writeRawData((char*)dat, 128);
		delete[] dat;
		
		// Output the DICM header
		dat = new unsigned char[4];
		memcpy((void*)dat,(void*)"DICM",4);
		out.writeRawData((char*)dat, 4);
		out2.writeRawData((char*)dat, 4);
		delete[] dat;
		
		int size;
		
		/* Format breakdown
		size: 8 (start of element, 4 for tag, 4 for size and/or VR) + expected data size
		dat: create a data element large enough to hold all the required data
		
		This is the tag definition, below would be for the tag (A1B2,C3D4)
		dat[1] = 0xA1; dat[0] = 0xB2; dat[3] = 0xC3; dat[2] = 0xD4;
		
		Either the dat[4] & dat[5] give you value representation and dat[6] and dat[7] hold size 
		memcpy((void*)(&dat[4]),(void*)"UL",2); dat[6] = size-8; dat[7] = 0;
		
		Or all elements from 4 to 7 give size
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		
		NOTE: Since most sizes are smaller than 255, size is usually only encoded in the
		least byte, with zeros everywhere else.  For sizes larger than 255, ensure that each
		field is filled appropriately for the correct transfer syntax
		*/
		
		// File Meta Information Group Length 0002,0000
		size = 8 + 4;
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x2; dat[3] = 0x0; dat[2] = 0x0;
		memcpy((void*)(&dat[4]),(void*)"UL",2); dat[6] = size-8; dat[7] = 0;
		//dat[8] = 192; dat[9] = 0; dat[10] = 0; dat[11] = 0;
		dat[8] = 72; dat[9] = 0; dat[10] = 0; dat[11] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// File Meta Information Version 0002,0001
		size = 8 + 6;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x2; dat[3] = 0x0; dat[2] = 0x1;
		memcpy((void*)(&dat[4]),(void*)"OB",2); dat[6] = 0; dat[7] = 0;
		dat[8] = 2; dat[9] = 0; dat[10] = 0; dat[11] = 0;
		dat[12] = 0; dat[13] = 1;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Media Storage SOP Class UID 0002,0002
		size = 8 + 30;
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x2; dat[3] = 0x0; dat[2] = 0x2;
		memcpy((void*)(&dat[4]),(void*)"UI",2); dat[6] = size-8; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1.2.840.10008.5.1.4.1.1.481.2",size-8); // RT Dose Storage
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Transfer Syntax UID 0002,0010
		size = 8 + 18;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x2; dat[3] = 0x0; dat[2] = 0x10;
		memcpy((void*)(&dat[4]),(void*)"UI",2); dat[6] = size-8; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1.2.840.10008.1.2",size-8); // Implicit VR Endian: Default Transfer Syntax for DICOM
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Specific Character Set 0008,0005
		size = 8 + 10;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x5;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"ISO_IR 100",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		auto now = std::chrono::system_clock::now();
		std::time_t now_c = std::chrono::system_clock::to_time_t(now);
		struct tm *parts = std::localtime(&now_c);
		
		std::string date = std::to_string(1900+parts->tm_year);
		std::string temp = std::to_string(1+parts->tm_mon);
		if (temp.size() == 1) temp = "0"+temp;
		date = date + temp;
		temp=std::to_string(parts->tm_mday);
		if (temp.size() == 1) temp = "0"+temp;
		date = date + temp;
		
		// Instance Creation Date 0008,0012
		size = 8 + date.size();  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x12;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)date.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		std::string time = std::to_string(parts->tm_hour);
		if (time.size() == 1) time = "0"+time;
		temp = std::to_string(parts->tm_min);
		if (temp.size() == 1) temp = "0"+temp;
		time = time + temp;
		temp = std::to_string(parts->tm_sec);
		if (temp.size() == 1) temp = "0"+temp;
		time = time + temp;
		
		// Instance Creation Time 0008,0013
		size = 8 + time.size();  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x13;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)time.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// SOP Class UID 0008,0016
		size = 8 + 30;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x16;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1.2.840.10008.5.1.4.1.1.481.2",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Accession Number 0008,0050
		size = 8;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x50;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Modality 0008,0060
		size = 8+6;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x0; dat[2] = 0x60;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"RTDOSE",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Study Description 0008,1030
		size = 8+22;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x10; dat[2] = 0x30;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"EGS BRACHY CALCULATION",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Referenced SOP Class UID  0008,1150
		//size = 8 + UID.length();
		//int size2 = 8 + UID2.length();
		//dat = new unsigned char[size];
		//
		//dat[1] = 0x0; dat[0] = 0x8; dat[3] = 0x11; dat[2] = 0x50;
		//dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		//memcpy((void*)(&dat[8]),(void*)UID.toStdString().c_str(),size-8);
		//
		//out.writeRawData((char*)dat, size);
		//memcpy((void*)(&dat[8]),(void*)UID2.toStdString().c_str(),size2-8);
		//
		//out2.writeRawData((char*)dat, size2);
		//delete[] dat;
		//emit madeProgress(0.5);
		
		// Slice Thickness  0008,0050	
		std::string thick = std::to_string((output->cz[1]-output->cz[0])*10);
		if (!(thick.size()%2)) thick = thick+" ";
		size = 8 + thick.size();  
		dat = new unsigned char[size];

		dat[1] = 0x0; dat[0] = 0x18; dat[3] = 0x0; dat[2] = 0x50;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)thick.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Study Instance UID  0020,0000
		size = 8 + UID.length();
		int size2 = 8 + UID2.length();
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x00; dat[2] = 0x00;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)UID.toStdString().c_str(),size-8);
		
		out.writeRawData((char*)dat, size);
		memcpy((void*)(&dat[8]),(void*)UID2.toStdString().c_str(),size2-8);
		
		out2.writeRawData((char*)dat, size2);
		delete[] dat;
		
		// Study ID  0020,0010	
		size = 8+22;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x0; dat[2] = 0x10;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"EGS BRACHY CALCULATION",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Series Number  0020,0011		
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x0; dat[2] = 0x11;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1 ",size-8);
		
		out.writeRawData((char*)dat, size);
		
		memcpy((void*)(&dat[8]),(void*)"2 ",size-8);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Instance Number  0020,0013
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x0; dat[2] = 0x13;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1 ",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Image Position Patient  0020,0032
		std::string position = std::to_string((output->cx[1]+output->cx[0])/2*10)+"\\"+
							   std::to_string((output->cy[1]+output->cy[0])/2*10)+"\\"+
							   std::to_string((output->cz[1]+output->cz[0])/2*10);
		if (!(position.size()%2)) position = position+" ";
		size = 8 + position.size();  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x0; dat[2] = 0x32;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)position.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Image Orientation Patient  0020,0037
		size = 8 + 12;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x0; dat[2] = 0x37;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"1\\0\\0\\0\\1\\0 ",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Position Reference Indicator  0020,1040
		size = 8 + 0;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x20; dat[3] = 0x10; dat[2] = 0x40;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Samples per Pixel  0028,0002
		size = 8 + 2;
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x2;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = 1; dat[9] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Photometric Interpretation  0028,0004
		size = 8+12;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x4;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"MONOCHROME2 ",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Number of Frames  0028,0008
		std::string zCount = std::to_string(output->z);
		if (!(zCount.size()%2)) zCount = zCount+" ";
		size = 8+zCount.size();  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x08;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)zCount.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Frame Increment Pointer  0028,0009
		size = 8+4;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x9;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[9] = 0x30; dat[8] = 0x4; dat[11] = 0x0; dat[10] = 0xc;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Rows  0028,0010
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x10;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = output->x%(1<<8); dat[9] = int(output->x/(1<<8));
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Columns  0028,0011
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x11;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = output->y%(1<<8); dat[9] = int(output->y/(1<<8));
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Pixel Spacing  0028,0030
		std::string xyThick = std::to_string((output->cx[1]-output->cx[0])*10)+"\\"+
							  std::to_string((output->cy[1]-output->cy[0])*10)+" ";
		if (!(xyThick.size()%2)) xyThick = xyThick+" ";
		size = 8+xyThick.size();
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x0; dat[2] = 0x30;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)xyThick.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Bits Allocated  0028,0100
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x1; dat[2] = 0x0;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = 16; dat[9] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Bits Stored  0028,0101
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x1; dat[2] = 0x1;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = 16; dat[9] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// High Bit  0028,0102	
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x1; dat[2] = 0x2;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = 15; dat[9] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Pixel Representation  0028,0103
		size = 8+2;  
		dat = new unsigned char[size];
			
		dat[1] = 0x0; dat[0] = 0x28; dat[3] = 0x1; dat[2] = 0x3;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		dat[8] = 0; dat[9] = 0;
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Dose Units  3004,0002
		size = 8+2;  
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0x2;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"GY",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Dose Type  3004,0004
		size = 8+8;  
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0x4;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"PHYSICAL",size-8);
		
		out.writeRawData((char*)dat, size);
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0x4;
		dat[4] = size-8-2; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"ERROR ",size-8-2);
		
		out2.writeRawData((char*)dat, size-2);
		delete[] dat;
		
		// Dose Comment  3004,0006
		QString DS = QString("Dose Scaling (Gy/history to absolute Gy) = ")+doseScaling;
		if (DS.length()%2)
			DS += " ";
		
		size = 8+DS.length();
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0x6;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)DS.toStdString().c_str(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Dose Summation Type  3004,000A
		size = 8+6;  
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0xA;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"BRACHY",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Grid Frame Offset Vector  3004,000C
		std::string zPlanes = "";
		for (int i = 0; i < output->z-1; i++)
			zPlanes += std::to_string((output->cz[i+1]+output->cz[i])/2*10)+"\\";
		zPlanes += std::to_string((output->cz[output->z+1]+output->cz[output->z])/2*10);
		if (!(zPlanes.size()%2)) zPlanes = zPlanes+" ";
		size = 8 + zPlanes.size();
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0xC;
		dat[4] = (size-8)%(1<<8); dat[5] = int((size-8)/(1<<8)); dat[6] = int((size-8)/(1<<16))%(1<<8); dat[7] = int((size-8)/(1<<24))%(1<<8);
		memcpy((void*)(&dat[8]),(void*)zPlanes.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Dose Grid Scaling  3004,000E
		double scaling = (0xEFFF)/output->getMax();
		output->scale(scaling);
		std::ostringstream oss;
		oss.precision(8);
		oss << scaling;
		std::string scalingString = oss.str();
		if (!(scalingString.size()%2)) scalingString = scalingString+" ";
		size = 8 + scalingString.size();
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0xE;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)scalingString.data(),size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Tissue Heterogeneity Correction  3004,0014
		size = 8+6;
		dat = new unsigned char[size];
		
		dat[1] = 0x30; dat[0] = 0x4; dat[3] = 0x0; dat[2] = 0x14;
		dat[4] = size-8; dat[5] = 0; dat[6] = 0; dat[7] = 0;
		memcpy((void*)(&dat[8]),(void*)"IMAGE ",size-8);
		
		out.writeRawData((char*)dat, size);
		out2.writeRawData((char*)dat, size);
		delete[] dat;
		
		// Pixel Data  7fe0,0010
		dat = new unsigned char[8];
		unsigned long int bSize = output->x*output->y*output->z*2;
		
		dat[1] = 0x7F; dat[0] = 0xE0; dat[3] = 0x0; dat[2] = 0x10;
		dat[4] = bSize%(1<<8); dat[5] = int(bSize/(1<<8))%(1<<8); dat[6] = int(bSize/(1<<16))%(1<<8); dat[7] = int(bSize/(1<<24));
		
		out.writeRawData((char*)dat, 8);
		out2.writeRawData((char*)dat, 8);
		delete[] dat;
		
		emit newProgressName("Outputting DICOM dose data");
		double increment = 50.0/n/output->z;
		
		unsigned short int bDat, eDat;
		for (int k = 0; k < output->z; k++) {
			for (int j = 0; j < output->y; j++) {
				for (int i = 0; i < output->x; i++) {
					bDat = output->val[i][j][k];
					eDat = output->val[i][j][k]*output->err[i][j][k];
					out << bDat;
					out2 << eDat;
				}
			}
			emit madeProgress(increment);
		}
	}
	
	file.close();
	file2.close();
	
    return 1;
}
