/*
################################################################################
#
#  egs_brachy_GUI appInterface.cpp
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
#include "appInterface.h"

// Constructors
appInterface::appInterface() {
	parent = (Interface*)parentWidget();
	
	createLayout();
	connectLayout();
}

appInterface::appInterface(Interface* p) {
	parent = p;
	createLayout();
	connectLayout();
}

// Destructor
appInterface::~appInterface() {
}

// Layout Settings
void appInterface::createLayout() {
	mainLayout = new QGridLayout();
	
	setLayout(mainLayout);
}

void appInterface::connectLayout() {
	
}

// Refresh
void appInterface::refresh() {
	mainLayout->addWidget(parent->doseFrame, 1, 2, 1, 1);
}