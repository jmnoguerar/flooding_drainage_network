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

#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include "circqueue.h"

/** This class defines the grid that contains the DEM cells */	
class Grid {

public:
	/** Initializes an empty grid */
	Grid(unsigned dimX = 100, unsigned dimY = 100, unsigned cellDimX = 10, unsigned cellDimY = 10);

	/** Initializes a grid from a HGT file */
	void loadHGT(const char *filename, unsigned dimX = 1201, unsigned dimY = 1201, unsigned cellDimX = 90, unsigned cellDimY = 90);

	/** Destrois the grid and clean up memory*/
	~Grid();

	/** Saves the grid to a ASCII file in the PLY format (for example, to load it with MeshLab)*/
	void savePLY(const char *filename);

	/** Saves the DA values of the drainage network to a PNG file.
	Returns true if the image was successfully saved; otherwise returns false.*/
	bool saveImageDA(const char *filename);

	/** Saves the grid to a PNG file. Each cell is coloured according to its W level.
	Returns true if the image was successfully saved; otherwise returns false.*/
	bool saveImageW(const char *filename);

	/** Adds a constant W value to all cells of the DEM */
	void addW(HEIGHT wh);

	/** Sets a constant W value in all cells of the DEM */
	void setW(HEIGHT wh);

	/** Sets a constant DA value in all cells of the DEM*/
	void setDA(HEIGHT wh);

	/** Mark as result cell each cell with a DA value above the provided value*/
	void markAsResultDAOver(HEIGHT wh);

	/**Computes an interation of the algortihm used to fill the pits of the dem
	\return The total water eliminated during this iteration*/
	HEIGHT dry();

	/** Computes an iteration of the algortihm. Fast version: only those cells included in a FIFO are processed.
	Do not mix calls to waterTransfer and fastWaterTransfer
	\return The total water transferred during this iteration*/
	HEIGHT fastWaterTransfer();

	/** Initializes the algorithm to use the fast version with the FIFO. This method should be
	called before any call to fastWaterTransfer*/
	void setupFastWaterTransfer();

	/** Gets X dimention of the grid */
	inline unsigned getDimX() { return dimX; }

	/** Gets Y dimention of the grid */
	inline unsigned getDimY() { return dimY; }

	/** Gets X dimention of the cells of the grid */
	inline unsigned getCellDimX() { return cellDimX; }

	/** Gets Y dimention of the cells of the grid */
	inline unsigned getCellDimY() { return cellDimY; }

private:

	/** Grid dimentions */
	unsigned dimX, dimY;
	/** Cell dimentions */
	unsigned cellDimX, cellDimY;
	/** Cells buffer */
	Cell *cells;
	/** FIFO of unprocessed cells*/
	CircQueue<Cell *> processingCells;

	/**Returns a cell of the grid */
	inline Cell *getCell(unsigned x, unsigned y) {
		return cells + y * dimX + x;
	}

	/**Devuelve la mayor cantidad de agua de las celdas vecinas a x,y pero sin
	contar la celda vecina apuntada por x,y ni la celda vecina apuntada por esta*/
	HEIGHT getMaxWNeigh( unsigned x, unsigned y );

	/**Gets the maximum DA value of the DEM cells*/
	HEIGHT getMaxDA();

	/**Gets the maximum Z value of the DEM cells*/
	HEIGHT getMaxW();

	/**Gets the neighbour cell with the lowest ZW value*/
	Cell *getLowerNeighbourCell(unsigned x, unsigned y);

	/**Gets the neighbour cell with the lowest ZW value*/
	inline Cell *getLowerNeighbourCell(Cell *cell) {
		return getLowerNeighbourCell((unsigned)(cell - cells) % dimX, (unsigned)(cell - cells) / dimX);
}

/** Returns a Z value that is the average Z of the neighbour cells of x,y*/
HEIGHT getNeighbourMeanZ(unsigned x, unsigned y);
};

#endif
