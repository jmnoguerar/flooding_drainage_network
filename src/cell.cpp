#include "cell.h"

Cell::Cell()
{
	Z = 0.0f;
	W = 0.0f;
	DA = 0.0f;
	inResult = false;
}

void Cell::addW(HEIGHT W)
{
	if( getZ() > 0.0 ){
		this->W += W;
		if (this->W < MIN_WATER_LEVEL)
			this->W = 0.0f;
		if (W > 0.0f)
			DA += W;
	}
}

void Cell::getHeightColor(int &r, int &g, int &b)
{
	if (inResult || Z <= 0) {
		r = 0;
		g = 0;
		b = 255;
	} else if (Z < 1000) {
		r = 255 * (int) Z / 1000;
		g = 255;
		b = 0;
	} else if (Z < 2000) {
		r = 255;
		g = 255 - 255 * ((int) Z - 1000) / 1000;
		b = 0;
	} else {
		r = 255;
		g = 255 * ((int) Z - 2000) / 1000;
		b = 255 * ((int) Z - 2000) / 1000;
	}
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
}
