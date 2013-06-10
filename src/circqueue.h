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


//
//  CircQueue.h
//  drainage
//
//  Created by Antonio Jesús Rueda Ruiz on 18/03/13.
//
//

#ifndef CIRCQUEUE_H
#define CIRCQUEUE_H

#include <vector>

/** This class implements a static circular queue */
template<typename T>
class CircQueue {
	std::vector<T> storage;
	typename std::vector<T>::iterator first, last;

public:
	/** Constructor */
	CircQueue(size_t size = 1) : storage(size) {
		first = last = storage.begin();
	}

	/** Modifies the size of the queue.
	Warning: data are destroyed; iterators are moved to the begining. */
	void resize(size_t size) {
		storage.resize(size);
		first = last = storage.begin();
	}

	/** Insert an element */
	void push(const T &t) {
		*last = t;
		if (++last == storage.end()) last = storage.begin();
	}

	/** Get an element */
	void pop() {
		if (++first == storage.end()) first = storage.begin();
	}

	/** Get the next element of the queue*/
	T &top() {
		return *first;
	}

	/**Clear all the elements of the queue.*/
	void clear() {
		storage.clear();
		first = last = storage.begin();
	}
};


#endif
