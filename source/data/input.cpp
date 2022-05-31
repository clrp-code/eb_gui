/*
################################################################################
#
#  egs_brachy_GUI input.cpp
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
#include "input.h"

QString EGSInput::buildGeom() {
	QString output = "";
	
	// Define simulation geoms
	output = output + ":start geometry definition:\n";
		
	// Phantom
	if (phantomFile.endsWith(".egsphant") || phantomFile.endsWith(".egsphant.gz")) {
		output = output + "\t:start geometry:\n";
		output = output + "\t\tname          = phantom\n";
		output = output + "\t\tlibrary       = egs_glib\n";
		output = output + "\t\ttype          = egsphant\n";
		output = output + "\t\tegsphant file = " + phantomFile + "\n";
		output = output + "\t\tdensity file  = " + Med_file + "\n";
		output = output + "\t:stop geometry:\n";
	}
	else {
		output = output + "\t:start geometry:\n";
		output = output + "\t\tname         = phantom\n";
		output = output + "\t\tlibrary      = egs_glib\n";
		output = output + "\t\tinclude file = " + phantomFile + "\n";
		output = output + "\t:stop geometry:\n";
	}
	
	output = output + "\n";
	
	// Sources
	output = output + "\t:start geometry:\n";
	output = output + "\t\tname         = lib_source\n";
	output = output + "\t\tlibrary      = egs_glib\n";
	output = output + "\t\tinclude file = " + sourceGeomFile + "\n";
	output = output + "\t:stop geometry:\n";
	
	output = output + "\n";
	
	// Autoenvelope (this sucks to look at)
	output = output + "\t:start geometry:\n";
	output = output + "\t\tname          = " + (additional_geomNames.size()?"penultimate_geom":"ultimate_geom") + "\n";
	output = output + "\t\tlibrary       = egs_autoenvelope\n";
	if (RM_normal.compare("normal") != 0)
		output = output + "\t\ttype          = EGS_ASwitchedEnvelope\n";
	output = output + "\t\tbase geometry = phantom\n";
	
	output = output + "\t\t:start inscribed geometry:\n";
	output = output + "\t\t\tinscribed geometry name = lib_source\n";
	
	output = output + "\t\t\t:start transformations:\n";
	output = output + "\t\t\t\tinclude file = " + sourceTransFile + "\n";
	output = output + "\t\t\t:stop transformations:\n";
	
	output = output + "\t\t\t:start region discovery:\n";
	output = output + "\t\t\t\taction                           = discover\n";
	output = output + "\t\t\t\tdensity of random points (cm^-3) = " + discoverPoints + "\n";
	output = output + "\t\t\t\tinclude file                     = " + sourceShapeFile + "\n";
	output = output + "\t\t\t:stop region discovery:\n";
	
	output = output + "\t\t:stop inscribed geometry:\n";
	
	output = output + "\t:stop geometry:\n";
	
	// Additional geometries
	if (additional_geomNames.size()) {
		for (int i = 0; i < additional_geomNames.size(); i++) {
			output = output + "\n";
			
			output = output + "\t:start geometry:\n";
			output = output + "\t\tname         = " + additional_geomNames[i] + "\n";
			output = output + "\t\tlibrary      = egs_glib\n";
			output = output + "\t\tinclude file = " + additional_geomFiles[i] + "\n";
			output = output + "\t\t:start transformation:\n";
			output = output + "\t\t\ttranslation = " + QString::number(pos[i].x()) + " " +
							  QString::number(pos[i].y()) + " " + QString::number(pos[i].z()) + "\n";
			output = output + "\t\t\trotation    = " + QString::number(rot[i].x()) + " " +
							  QString::number(rot[i].y()) + " " + QString::number(rot[i].z()) + "\n";
			output = output + "\t\t:stop transformation:\n";
			output = output + "\t:stop geometry:\n";
		}
		
		// Union of all geometries
		output = output + "\n";
		
		output = output + "\t:start geometry:\n";
		output = output + "\t\tname       = ultimate_geom\n";
		output = output + "\t\tlibrary    = egs_gunion\n";
		output = output + "\t\tgeometries = penultimate_geom";
		for (int i = 0; i < prio.size(); i++)
			output = output + " " + additional_geomNames[i];
		output = output + "\n";
		output = output + "\t\tpriorities = 1" ;
		for (int i = 0; i < prio.size(); i++)
			output = output + " " + prio[i];
		output = output + "\n";
		output = output + "\t:stop geometry:\n";
	}
	
	output = output + "\n";
	
	if (RM_normal.compare("normal") != 0) {
		output = output + "\tsource geometries        = lib_source\n";
		output = output + "\tphantom geometries       = phantom\n";
		output = output + "\tsimulation geometry      = ultimate_geom\n";
		output = output + "\tsource envelope geometry = " + (additional_geomNames.size()?"penultimate_geom":"ultimate_geom") + "\n";
	}
	else {
		output = output + "\tsource geometries   = lib_source\n";
		output = output + "\tphantom geometries  = phantom\n";
		output = output + "\tsimulation geometry = ultimate_geom\n";		
	}
	
	output = output + ":stop geometry definition:\n";
	
	return output;
}

QString EGSInput::buildInput() {
	QString output = "";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	// run control
	output = output + ":start run control:\n";
	output = output + "\tncase                = " + RC_ncase + "\n";
	output = output + "\tnbatch               = " + RC_nbatch + "\n";
	output = output + "\tnchunk               = " + RC_nchunk + "\n";
	output = output + "\tcalculation          = " + RC_calculation + "\n";
	output = output + "\tgeometry error limit = " + RC_geomLimit + "\n";
	output = output + "\tegsdat file format   = " + RC_egsdatFormat + "\n";
	output = output + ":stop run control:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// run mode
	output = output + ":start run mode:\n";
	output = output + "\trun mode = " + RM_normal + "\n";
	output = output + ":stop run mode:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// media definition
	output = output + ":start media definition:\n";
	output = output + "\tAE                 = " + Med_AE + "\n";
	output = output + "\tUE                 = " + Med_UE + "\n";
	output = output + "\tAP                 = " + Med_AP + "\n";
	output = output + "\tUP                 = " + Med_UP + "\n";
	output = output + "\tmaterial data file = " + Med_file + "\n";
	output = output + ":stop media definition:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// geometry definition
	output = output + buildGeom();
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// volume correction
	output = output + ":start volume correction:\n";
	output = output + "\t:start source volume correction:\n";
	output = output + "\t\t correction type                  = " + VC_type + "\n";
	output = output + "\t\t density of random points (cm^-3) = " + VC_density + "\n";
	output = output + "\t\t include file                     = " + VC_bound + "\n";
	output = output + "\t\t total coverage threshold %       = " + VC_thresh + "\n";
	output = output + "\t:stop source volume correction:\n";
	output = output + ":stop volume correction:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// source definition
	output = output + ":start source definition:\n";
	output = output + "\t:start source:\n";
	output = output + "\t\tname         = lib_source\n";
	output = output + "\t\tlibrary      = egs_isotropic_source\n";
	output = output + "\t\tcharge       = 0\n";
	output = output + "\t\tinclude file = " + sourceSeedFile + "\n";
	output = output + "\t\t:start spectrum:\n";
	output = output + "\t\t\ttype          = tabulated spectrum\n";
	output = output + "\t\t\tspectrum file = " + sourceSpecFile + "\n";
	output = output + "\t\t:stop spectrum:\n";
	output = output + "\t:stop source:\n";
	output = output + "\t:start transformations:\n";
	output = output + "\t\tinclude file = " + sourceTransFile + "\n";
	output = output + "\t:stop transformations:\n";
	if (sourceDwells.length())
		output = output + "\tsource weights = " + sourceDwells + "\n";
	output = output + "\tsimulation source = lib_source\n";
	output = output + ":stop source definition:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// scoring options
	output = output + ":start scoring options:\n";
	output = output + "\tscore energy deposition = " + SO_edep + "\n";
	output = output + "\tmuen file               = " + SO_muenFile + "\n";
	output = output + "\tmuen for media          = " + SO_muenMed + "\n";
	if (SO_scale != 1)
		output = output + "\tdose scaling factor     = " + SO_scale + "\n";
	output = output + ":stop scoring options:\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	// transport parameters
	output = output + "include file = " + TP_file + "\n";
	output = output + "#----------------------------------------------------------------------------------------------------\n";
	
	return output;
}

EGSInput::EGSInput() { // These should all be overwritten
	// run control
	RC_ncase        = "100000000";
	RC_nbatch       = "1";
	RC_nchunk       = "1";
	RC_geomLimit    = "250";
	RC_calculation  = "first";
	RC_egsdatFormat = "gzip";
	
	// run mode
	RM_normal = "normal";
	
	// media definition
	Med_AE   = "0.512";
	Med_UE   = "2.012";
	Med_AP   = "0.001";
	Med_UP   = "1.5";
	Med_file = "";
	
	// geometry definition
	phantomFile    = "";
	sourceGeomFile = "";
	additional_geomNames.clear();
	additional_geomFiles.clear();
	prio.clear();
	pos.clear();
	rot.clear();
	discoverPoints = "100000000";
	
	// volume correction
	VC_type    = "correct";
	VC_density = "100000000";
	VC_bound   = "";
	
	// source definition
	sourceTransFile = "";
	sourceSpecFile  = "";
	sourceShapeFile = "";
	
	// scoring options
	SO_edep     = "no";
	SO_muenFile = "";
	SO_muenMed  = "";
	
	// transport parameters
	TP_file = "";
}
