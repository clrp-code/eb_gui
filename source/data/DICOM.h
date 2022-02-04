/*
################################################################################
#
#  egs_brachy_GUI DICOM.h
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
#ifndef DICOM_H
#define DICOM_H

#include <QtGui>
#include <iostream>
#include <math.h>

// These need to be declared ahead of time, they are needed for nested sequences
class Sequence;
class SequenceItem;
class Attribute;

// The following two classes are used to hold a sequence of items (and yes, you
// can have nested sequences, cause, you know, why not?)
class Sequence {
public:
    QVector <SequenceItem *> items;
    ~Sequence();
};

class SequenceItem {
public:
    unsigned long int vl; // Value Length
    unsigned char *vf; // Value Field
    Sequence seq; // Contains potential sequences

    SequenceItem(unsigned long int size, unsigned char *data);
    SequenceItem(unsigned long int size, Attribute *data);
    ~SequenceItem();
};

class Attribute {
public:
    unsigned short int tag[2]; // Element Identifier
    QString desc; // Desciption
    unsigned short int vr; // Value Representation
    unsigned long int vl; // Value Length
    unsigned char *vf; // Value Field
    Sequence seq; // Contains potential sequences

    Attribute();
    ~Attribute();
	
	// Comparison to allow for sorted insertion
    int compare(const Attribute *a) {
		if (tag[0] < a->tag[0]) return -1;
		if (tag[0] > a->tag[0]) return 1;
		if (tag[1] < a->tag[1]) return -1;
		if (tag[1] > a->tag[1]) return 1;
		return 0;
	};
};

// These are all defined in database.cpp so as to save alot of recompiling
// hassle
struct Reference {
    unsigned short int tag[2]; // Element Identifier
    QString vr; // Element Identifier
    QString title; // Title of element
};

class database : public QObject {
    Q_OBJECT

public:
    // Contains a list of known attribute entries
    QVector <Reference *> lib;
    // Contains all the acceptable value representations
    QStringList validVR;
    QStringList implicitVR;
	
	Reference binSearch(unsigned short int one, unsigned short int two, int min, int max);

    database();
    ~database();
};

class DICOM : public QObject {
    Q_OBJECT

public:
    // Contains all the data read in from a dicom file sorted into attributes
    QVector <Attribute *> data;
	
    // Pointer to precompiled DICOM library
    database *lib;
	
	// Transfer syntax
    bool isImplicit, isBigEndian;
	
	// z height (default to NaN, only change if slice height tag is found)
	double z = std::nan("1");
	
	// n is slice count, used for sorting
	int n = -1;

	// file location for later lookup if needed
	QString path;

    DICOM(); // Shouldn't be invoked
    DICOM(database*);
    ~DICOM();

    int parse(QString p);
    int readSequence(QDataStream *in, Attribute *att);
    int readDefinedSequence(QDataStream *in, Attribute *att, unsigned long int n = 0);
	
	int parseSequence(QDataStream *in, QVector <Attribute*> *att);
	
	// functions for fetching top level data attributes once loaded in
	int binSearch(unsigned short int one, unsigned short int two, int min, int max);
	Attribute* getEntry(unsigned short int one, unsigned short int two) {
		return data[binSearch(one, two, 0, data.size()-1)];
	};
	
	// functions for fetching sequence attributes the sequences have been parsed
	int binSearch(QVector <Attribute*> *att, unsigned short int one, unsigned short int two, int min, int max);
	Attribute* getSubEntry(QVector <Attribute*> *att, unsigned short int one, unsigned short int two) {
		return (*att)[binSearch(one, two, 0, att->size()-1)];
	};
};

#endif