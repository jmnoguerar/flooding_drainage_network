
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <QtGui/QImage>


#include "grid.h"

using namespace std;

#define INVSQRT2 0.70710678122310f
#define EPSILON 0.00001f

#ifndef INFINITY
	#include <limits>
	#define INFINITY std::numeric_limits<float>::infinity();
#endif

//-----------------------------------------------------------------

Grid::Grid(unsigned dimX, unsigned dimY, unsigned cellDimX, unsigned cellDimY)
{
    this->dimX = dimX;
    this->dimY = dimY;
    this->cellDimX = cellDimX;
    this->cellDimY = cellDimY;
    cells = new Cell[dimX * dimY];
}

//-----------------------------------------------------------------

void Grid::loadHGT(const char *filename, unsigned dimX, unsigned dimY, unsigned cellDimX, unsigned cellDimY)
{
    ifstream ifs;
    ifs.exceptions(ifstream::failbit | ifstream::badbit);    
    ifs.open(filename, ifstream::binary);

    this->dimX = dimX;
    this->dimY = dimY;
    this->cellDimX = cellDimX;
    this->cellDimY = cellDimY;
    delete[] cells;
    cells = new Cell[dimX * dimY];

    // Load data
    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
            getCell(c, r)->setZ((ifs.get() << 8) | ifs.get());
            getCell(c, r)->setDA(0.0f);
        }
    }

    // Fill holes
    Cell *cell;
    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
            cell = getCell(c, r);
            if (cell->getZW() > 9000) {
                cell->setZ(getNeighbourMeanZ(c, r));
            }
        }
    }
}

//-----------------------------------------------------------------

double log10Value( double value, double maxValue )
{
	// Input will be between minValue and maxValue
	double minValue = 0;

	//output will be between minv and maxv
	double minv = log10(1.0);
	double maxv = log10(maxValue);

	// Adjustment factor
	double scale = (maxv - minv) / (maxValue - minValue);
	return pow(10.0, minv + (scale * (value - minValue)));
}

void computeColor(int &r, int &g, int &b, double value, double maxValue)
{
	value = maxValue - value/* - 1.0*/;
	double valueLog = log10Value( value, maxValue );
	r = 0;
	//from 255 (min) to 0 (max)
	g = 255 * valueLog / maxValue;
	b = 255;
	return;
}

//-----------------------------------------------------------------

void Grid::savePLY(const char *filename)
{
	ofstream ofs;
	ofs.exceptions(ifstream::failbit | ifstream::badbit);    
	ofs.open(filename);

	ofs << "ply" << endl;
	ofs << "format ascii 1.0" << endl;
	ofs << "element vertex " << dimX * dimY << endl;
	ofs << "property float x" << endl;
	ofs << "property float y" << endl;
	ofs << "property float z"  << endl;
	ofs << "property uchar red" << endl;
	ofs << "property uchar green" << endl;
	ofs << "property uchar blue"  << endl;
	ofs << "end_header" << endl;

	int colorR, colorG, colorB;
	Cell *cell;
	HEIGHT height;

	HEIGHT maxAccumW = getMaxDA();

    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
            cell = getCell(c, r);
            height = cell->getZ();
            
			//assign a color according to the height
			cell->getHeightColor(colorR, colorG, colorB);

			if( height == 0.0f ){
				colorB = 255;
				colorG = 0;
				colorR = 0;

			}else 
				if( cell->isInResult() ){	
					computeColor(colorR, colorG, colorB, cell->getDA(), maxAccumW);
				} 

            ofs << 0.1f * r << " ";
            ofs << 0.1f * c << " ";
            ofs << 0.3f * height / cellDimX << " ";
            ofs << colorR << " " << colorG << " " << colorB << endl;
        }
    }
}


//-----------------------------------------------------------------

bool Grid::saveImageDA(const char *filename)
{
	#ifdef QT_CORE_LIB 
		int colorR, colorG, colorB;
		Cell *cell;
		HEIGHT height, accumWH;
		HEIGHT maxAccumW = getMaxDA();

		//data must be 32 bit aligned for QImage
		unsigned char *iDataColor = new unsigned char[4* dimX * dimY];
		if( !iDataColor )
			return false;

		unsigned int posData = 0;

		for (unsigned int r = 0; r < dimY; ++r) {
			for (unsigned int c = 0; c < dimX; ++c) {
				cell = getCell(c, r);

				accumWH = cell->getDA();
				height = cell->getZ();

				if( cell->getZW() == 0.0f ){
					colorB = 255;
					colorG = 0;
					colorR = 0;
				}else if( cell->isInResult() ){	
					computeColor(colorR, colorG, colorB, accumWH, maxAccumW);
				} 
				else{
					colorB = 0;
					colorG = 0;
					colorR = 0;
				}

				iDataColor[posData]   = colorB; //blue
				iDataColor[posData+1] = colorG; //green
				iDataColor[posData+2] = colorR; //red
				iDataColor[posData+3] = 255; //alpha

				posData += 4;
			}
		}

		//create a qimage in memory with the image
		QImage iImageColor = QImage( iDataColor, dimX, dimY, QImage::Format_RGB32 );

		//vertical flip of the image
		//iImageColor = iImageColor.mirrored( false, true );

		//save image
		bool returnValue = iImageColor.save( filename, 0, 100 );
		delete[] iDataColor;
		return returnValue;
	#else
		cout << "Error: images can only be saved with Qt enabled." << endl;
		return false;
	#endif
}

//-----------------------------------------------------------------

void computeColorWater(int &r, int &g, int &b, double value, double maxValue)
{
	if( value == 0.0 ){
		r = 0;
		g = 0;
		b = 0;
	}
	else{
		value = maxValue - value/* - 1.0*/;
		double valueLog = log10Value( value, maxValue );
		r = 0;
		//from 255 (min) to 0 (max)
		g = 255 * valueLog / maxValue;
		b = 255;
	}
}

//-----------------------------------------------------------------

bool Grid::saveImageW(const char *filename)
{
	#ifdef QT_CORE_LIB 

		int colorR, colorG, colorB;
		Cell *cell;
		HEIGHT WH;
		HEIGHT max = getMaxW();
		
		unsigned char *iDataColor = new unsigned char[4* dimX * dimY];
		if( !iDataColor )
			return false;

		unsigned int posData = 0;

		for (unsigned int r = 0; r < dimY; ++r) {
			for (unsigned int c = 0; c < dimX; ++c) {
			
				cell = getCell(c, r);
				WH = cell->getW();
				computeColorWater(colorR, colorG, colorB, WH, max);
				iDataColor[posData]   = colorB; //blue
				iDataColor[posData+1] = colorG; //green
				iDataColor[posData+2] = colorR; //red
				iDataColor[posData+3] = 255; //alpha

				posData += 4;
			}
		}

		//create a qimage in memory with the image
		QImage iImageColor = QImage( iDataColor, dimX, dimY, QImage::Format_RGB32 );

		//vertical flip of the image
		//iImageColor = iImageColor.mirrored( false, true );

		//save image
		bool returnValue = iImageColor.save( filename, 0, 100 );
		delete[] iDataColor;
		return returnValue;
	#else
		cout << "Error: images can only be saved with Qt enabled." << endl;
		return false;
	#endif
}

//-----------------------------------------------------------------

Grid::~Grid()
{
	delete[] cells;
}

//-----------------------------------------------------------------

void Grid::addW(HEIGHT wh)
{
	for (unsigned int r = 0; r < dimY; ++r) {
		for (unsigned int c = 0; c < dimX; ++c) {
			Cell *cell = getCell(c, r);
			cell->setW(wh + cell->getW());
		}
	}
}

//-----------------------------------------------------------------

void Grid::setW(HEIGHT wh)
{
    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
          getCell(c, r)->setW(wh);
        }
    }
}

//-----------------------------------------------------------------

void Grid::setDA(HEIGHT wh)
{
    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
          getCell(c, r)->setDA(wh);
        }
    }
}

//-----------------------------------------------------------------

void Grid::markAsResultDAOver(HEIGHT wh)
{
    Cell *cell;

    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
            cell = getCell(c, r);
            if (cell->getDA() >= wh)
                cell->markAsResult();
        }
    }
}

//-----------------------------------------------------------------

HEIGHT Grid::dry()
{
	Cell *cell, *lowerCell;
	HEIGHT accumMovingWater = 0.0f;

    for (unsigned int r = 0; r < dimY; ++r) {
        for (unsigned int c = 0; c < dimX; ++c) {
            cell = getCell(c, r);
			HEIGHT currentCellW = cell->getW();

			if (currentCellW > 0.0f) {
				lowerCell = getLowerNeighbourCell(c, r);
				if( lowerCell ){
					if( cell->getZW() > lowerCell->getZW() ){
						HEIGHT newW = min(currentCellW, cell->getZW() - lowerCell->getZW());
						cell->addW(-newW);
						accumMovingWater += newW;
					}
				}
				else{
					accumMovingWater += currentCellW;
					cell->setW( 0.0f );
				}
			}
		}
	}
	return accumMovingWater;
}

//-----------------------------------------------------------------


void Grid::setupFastWaterTransfer()
{
	unsigned numCells = dimX * dimY;
	processingCells.clear();
	processingCells.resize(numCells + 1);

	Cell *cell = cells;
	for (unsigned int c = 0; c < numCells; ++c) {
		processingCells.push(cell++);
	}

	// Push ending token
	processingCells.push(0);
}

//-----------------------------------------------------------------

HEIGHT Grid::fastWaterTransfer()
{
	HEIGHT movingWater, accumMovingWater = 0.0f;
	Cell *cell, *lowerCell;

	// Iterate until we find the ending token
	while ((cell = processingCells.top()) != 0) {
		processingCells.pop();
		lowerCell = getLowerNeighbourCell(cell);
		movingWater = lowerCell ? min(cell->getW(), 0.5f * (cell->getZW() - lowerCell->getZW())) : cell->getW();

		//condition to avoid very small water transfers
		if (movingWater > EPSILON) {
			//remove water from current cell
			cell->addW(-movingWater);

			if (lowerCell && lowerCell->getZ() > 0.0f) {
				//if the neighbour cell did not have water, we pushed it into the FIFO because it is going to recieve water
				if (lowerCell->getW() < EPSILON)
					processingCells.push(lowerCell);
				lowerCell->addW(+movingWater);
			}
			accumMovingWater += movingWater;
		}
		//if the current cell still has water, we pushed it into the FIFO
		if (cell->getW() > EPSILON)
		processingCells.push(cell);
	}

	//remove ending token and add it again after the new list of cells to process
	processingCells.pop();
	processingCells.push(0);

	return accumMovingWater;
}

//-----------------------------------------------------------------

Cell *Grid::getLowerNeighbourCell(unsigned x, unsigned y)
{
	HEIGHT cellHeight = getCell(x, y)->getZW();
	HEIGHT higherSlope = -INFINITY;
	float slope;
	bool borderCell = (x == 0 || y == 0 || x == dimX - 1 || y == dimY - 1);

	if( borderCell ){
		return NULL;
	}

	Cell *lowerCell = 0;

	int rMin = max(0, (int)y - 1);
	int rMax = min(y + 2, dimY);
	int cMin = max(0, (int)x - 1);
	int cMax = min(x + 2, dimX);

	for (int r = rMin; r < rMax; ++r) {
		for (int c = cMin; c < cMax; ++c) {
			if (c != x || r != y) {
				slope = cellHeight - getCell(c, r)->getZW();
				if (c != x && r != y)
					slope *= INVSQRT2;

				if (slope > higherSlope) {
					higherSlope = slope;
					lowerCell = getCell(c, r);
				}
			}
		}
	}
	return lowerCell;
}

//-----------------------------------------------------------------

HEIGHT Grid::getNeighbourMeanZ(unsigned x, unsigned y)
{ 
	HEIGHT ch, h, mh = 0;
	unsigned nn = 0;

	ch = getCell(x, y)->getZ();

	for (unsigned int r = y - 1; r <= y + 1; ++r) {
		for (unsigned int c = x - 1; c <= x + 1; ++c) {
			if (c >= 0 && r >= 0 && c < dimX && r < dimY && c != x && r != y) {
				if ((h = getCell(c, r)->getZ()) < 9000) {
					mh += h;
					++nn;
				}
			}
		}
	}
	if( nn > 0 )
		return mh / nn;
	return 0;
}

//-----------------------------------------------------------------

HEIGHT Grid::getMaxDA()
{
	Cell *cell;
	HEIGHT max = 0.0;
	for (unsigned int r = 0; r < dimY; ++r) {
		for (unsigned int c = 0; c < dimX; ++c) {
			cell = getCell(c, r);
			HEIGHT a = cell->getDA();
			if( a > max )
				max = a;
		}
	}
	return max;
}

//-----------------------------------------------------------------

HEIGHT Grid::getMaxW()
{
	Cell *cell;
	HEIGHT max = 0.0;
	for (unsigned int r = 0; r < dimY; ++r) {
		for (unsigned int c = 0; c < dimX; ++c) {
			cell = getCell(c, r);
			HEIGHT a = cell->getW();
			if( a > max )
				max = a;
		}
	}
	return max;
}

//-----------------------------------------------------------------
