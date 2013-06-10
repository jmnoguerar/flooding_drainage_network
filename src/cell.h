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
                  
#ifndef CELL_H
#define CELL_H

#define HEIGHT float
#define MIN_WATER_LEVEL 0.001f

#include <iostream>

/** Class that defines a DEM cell */
class Cell {

public:
    
    /** Constructor */
    Cell();
    
    /** Sets the altitude Z value of the cell */
	inline void setZ(HEIGHT Z) { this->Z = Z;}

    /** Sets the water level W of the cell */
	inline void setW(HEIGHT W){
		if( getZ() > 0.0 )
			this->W = W; 
	}

    /** Sets the accumulated water level DA of the cell */
    inline void setDA(HEIGHT acumWH = 0) { this->DA = acumWH; }

    /** Adds water to the W level of the cell */
    void addW(HEIGHT W);

    /** Returns the altitude Z of the cell */
    inline HEIGHT getZ() { 
		return Z; 
	}

    /** Returns the water level W of the cell */
    inline HEIGHT getW() { return W; }

    /** Returns the altitude ZW of the cell */
    inline HEIGHT getZW() { return Z + W; };

    /** Returns the accumulated water DA of the cell */
    inline HEIGHT getDA() { return DA; }
    
    /** Mark the cell as belonging to the drainage network */
    inline void markAsResult() { inResult = true; }

    /** Unmark the cell as belonging to the drainage network */
    inline void unMarkAsResult() { inResult = false; }
    
    /** Returns whether the cell belongs to the drainage network*/
    inline bool isInResult() { return inResult; }

    /** Returns a color according to the altitude Z of the cell */
    void getHeightColor(int &r, int &g, int &b);

private:
    /** Altitude Z of the center of the cell*/
    HEIGHT Z;
    /** Height W of the water of the cell */
    HEIGHT W;
    /** Accumulated water DA of the cell */
    HEIGHT DA;
    /** Indicates whether the cell belongs to the drainage network */
    bool inResult;

};

#endif
