/***************************************************************************
 *   Copyright (C) 2013 by Antonio Rueda and Jose M. Noguera               *
 *   ajrueda@ujaen.es, jnoguera@ujaen.es                                   *
 *   University of Jaen (Spain)                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
                  
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef QT_CORE_LIB 
	#include <QtCore/QCoreApplication>
	#include <QtCore/QStringList>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include "grid.h"

#ifndef INFINITY
	#include <limits>
	#define INFINITY std::numeric_limits<float>::infinity();
#endif

using namespace std;

//default values

#define INIT_WATER 0.05f
#define DA_THRESHOLD 2.0f
#define END_PERCENT 0.1f

#define FIRST_PASS_WATER 10000.0f
#define FIRST_PASS_END_PERCENT 1.0f


//-----------------------------------------------------------------

int doDry(Grid &grid, bool verbose)
{
	float transfer = +INFINITY;
	int n = 1;
	if( verbose )
		cout << "Iteration: ";
	while (transfer > 1) {
		transfer = grid.dry();
		if( !(n%10) && verbose ){
			cout << n << " (" << transfer << ") ";
			cout.flush();
		}
		++n;
	}    
	return n;
}

//-----------------------------------------------------------------

int doFastWaterTransfer(Grid &grid, float minTransfer, bool verbose)
{
	grid.setupFastWaterTransfer();
	float transfer = +INFINITY;
	int n = 1;
	if( verbose )
		cout << "Iteration: ";
		
	while (transfer > minTransfer && n<10000) {
		transfer = grid.fastWaterTransfer();
		if( !(n%10) && verbose ){
			cout << n << " (" << transfer << ") ";
			cout.flush();
		}
		++n;
	}    
	return n;
}

//-----------------------------------------------------------------

void printHelp( char *args )
{
	cout << "Usage:" << endl;
	cout << "\t" << args << " FILE -x VALUE -y VALUE [parameters]" << endl;
	cout << endl;
	cout << "File:" << endl;
	cout << "\t<s>:\tInput .hgt file" << endl;
	cout << "Options:" << endl;
	cout << "\t-x\tX dimension X of the DEM (mandatory)." << endl;
	cout << "\t-y\tY dimension Y of the DEM (mandatory)." << endl;
	cout << "\t-w\tDepth of the initial water layer W (in millimeters) assigned  to each cell." << endl;
	cout << "\t-da\tMinimum drainage accumulation value DA (in millimeters) for a cell belongs to the drainage network (meters)." << endl;
	cout << "\t-s\t The algorithm stops once the water transferred in an iteration falls bellow this percentage of the total amount of water initially dropped on the DEM (percent 1-100)." << endl;
	cout << "\t-o\t Output file containing the drainage network. Most image format are supported. '.ply' format is also supported." << endl;
	cout << "\t-ow\t Output file containing the depth of the residual water layer W after the algorithm. If this parameter is not set, the data is not saved. Most image format are supported." << endl;
	cout << "\t-f\t Preprocess the terrain to fill the pits." << endl;
	cout << "\t-v\t Verbose. Prints real-time status of the program." << endl;
	cout << "\t-h\t Shows this help and exits." << endl;
	cout << endl;
	cout << "2013 (c) Jose Maria Noguera and Antonio Rueda. University of Jaen." << endl << endl;
}

//-----------------------------------------------------------------

typedef struct {
	float endThreshold;
	float DAThreshold;
	float initW;
	std::string outputW;
	std::string outputDA;
	bool fill;
	bool verbose;
} Parameters;

int init( int argc, char *argv[], Grid &grid, Parameters &param )
{
	if( argc < 2 ){
		printHelp( argv[0] );
		return -1;
	}

	//default values
	unsigned int x = 0;
	unsigned int y = 0;
	param.initW = INIT_WATER;
	param.DAThreshold = DA_THRESHOLD;
	float stopPercent = END_PERCENT;
	param.outputDA = "output_DA.png";
	param.outputW = "";
	param.fill = false;
	param.verbose = false;

	//first argument is the name of the HGT file
	std::string file = argv[1];

	for (int i = 2; i < argc; ++i) {

		if (std::string(argv[i]) == "-x" ) {   
			i++;
			if( i < argc )
				istringstream ( argv[i] ) >> x;
		}

		else if (std::string(argv[i]) == "-y" ) {
			i++;
			if( i < argc )
				istringstream ( argv[i] ) >> y;
		}

		else if (std::string(argv[i]) == "-w" ) {
			i++;
			if( i < argc ){
				istringstream ( argv[i] ) >> param.initW;
				param.initW /= 1000.0;
			}	
		}

		else if (std::string(argv[i]) == "-da" ) {   
			i++;
			if( i < argc ){
				istringstream ( argv[i] ) >> param.DAThreshold;
				param.DAThreshold /= 1000.0;
			}	
		}

		else if (std::string(argv[i]) == "-s" ) {
			i++;
			if( i < argc ){
				istringstream ( argv[i] ) >> stopPercent;
				if( stopPercent < 0.f || stopPercent > 100.0 ){
					cout << "Error: -s parameter must be between 0 and 100" << endl;
					return -1;
				}
			}
		}

		else if (std::string(argv[i]) == "-o" ) {
			i++;
			if( i < argc ){
				param.outputDA = argv[i];
			}	
		}
 
		else if (std::string(argv[i]) == "-ow" ) {   
			i++;
			if( i < argc ){
				param.outputW = argv[i];
			}	
		}

		else if (std::string(argv[i]) == "-f" ) {  
			param.fill = true;
		}

		else if (std::string(argv[i]) == "-v" ) {  
			param.verbose = true;
		}

		else if (std::string(argv[i]) == "-h" ) {  
			printHelp( argv[0] );
			return -1;
		}
		else {
            cout << "Uknown arg:" << argv[i] << endl;
            return -1;
        }
	}

	if( x == 0 || y == 0 ){
		cout << "Error: please, specify the -x and -y parameters" << endl;
		return -1;
	}

	try {
		grid.loadHGT(file.c_str(), x, y, 90, 90);
	} catch(std::exception &e) {
		cerr << "Error loading input DEM file." << endl;
		cout << e.what() << endl;
		return -1;
	}
	param.endThreshold = stopPercent/100.0f * param.initW * grid.getDimX() * grid.getDimY();
	//cout << "end threshold is: " << endThreshold << endl;
	return 0;
}

//-----------------------------------------------------------------

bool isPly( std::string file )
{
	std::string extension = file.substr(file.find_last_of(".") + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	return extension == "ply";
}

//-----------------------------------------------------------------

int main(int argc, char *argv[])
{
	#ifdef QT_CORE_LIB 
		QCoreApplication a(argc, argv);
	#endif

	Grid grid;
	Parameters param;
	int numIter;

	cout << endl;

	if( init( argc, argv, grid, param ) == -1 ){
		exit(1);
	}

	if( param.fill ){
		cout << "Filling DEM..." << endl;
		grid.setW(FIRST_PASS_WATER);
		numIter = doDry( grid, param.verbose );
		cout << endl << "Number of iterations: " << numIter << endl;
	}

	cout << "Computing drainage..." << endl;

	grid.addW(param.initW);
	grid.setDA(0);
	numIter = doFastWaterTransfer( grid, param.endThreshold, param.verbose );
	grid.markAsResultDAOver(param.DAThreshold);

	cout << endl << "Number of iterations: " << numIter << endl;

	try {
		if( isPly(param.outputDA) )
			grid.savePLY( param.outputDA.c_str() );
		else if( !grid.saveImageDA(param.outputDA.c_str()) ){
			cout << "Error saving DA image: " << param.outputDA << "." << endl;
		}

		if( param.outputW != "" ){
			if( !grid.saveImageW( param.outputW.c_str()) )
				cout << "Error saving W image: " << param.outputW << "." << endl;
		}
	} catch(std::exception &e) {
		cout << "Error saving image: " << e.what() << endl;
		return 1;
	}
	return 0;
}
