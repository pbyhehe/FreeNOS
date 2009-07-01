/*
 * Copyright (C) 2009 Niek Linnenbank
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __BITMAP_H
#define __BITMAP_H

#include "Macros.h"
#include "Assert.h"
#include <string.h>

/**
 * Class capable of (un)marking bits inside a binary bitmap.
 */
class BitMap
{
    public:

	/**
	 * Class constructor.
	 * @param cnt Number of bits.
	 */
	BitMap(Size cnt) : count(cnt), free(cnt)
	{
	    map = new u8[(cnt / 8) + 1];
	    memset(map, 0, (cnt / 8) + 1);
	}
    
	/**
	 * Mark a given bit used.
	 * @param bit Bit number to mark used.
	 */
	void mark(Size bit)
	{
	    assert(bit < count);
	    assertRead(map);
	    assertWrite(map);
	
	    /* Only mark if the bit is free. */
	    if (!isMarked(bit))
	    {
		map[bit / 8] |= 1 << (bit % 8);
		free--;
	    }
        }
	
	/**
	 * Marks the next free bit used.
	 * @return Bit number on success and -1 otherwise.
	 */
	Error markNext()
	{
	    Address *ptr;                                
	    Size num = count / sizeof(Address);

    	    /* At least one, and include partially used bytes. */
    	    if (!num || sizeof(Address) % count)
	    {
		num++;
	    }
    	    /* Scan bitmap as fast as possible. */
    	    for (Size i = 0; i < num; i++)
    	    {
        	/* Point to the correct offset. */
        	ptr = ((Address *) (map)) + i;

        	/* Any blocks free? */
        	if (*ptr != (Address) ~ZERO)
        	{
            	    /* Find the first free bit. */
            	    for (Size bit = 0; bit < sizeof(Address) * 8; bit++)
            	    {
            		if (!(*ptr & 1 << bit))
                	{
                    	    *ptr |= (1 << bit);
                	    free--;
			    return bit + (sizeof(Address) * 8 * i);
                	}
            	    }
		}
            }
	    /* No free bits left! */
	    return -1;
        }
	
	/**
	 * Unmarks the given bit.
	 * @param bit Bit number to unmark.
	 */
	void unmark(Size bit)
	{
	    assert(bit < count);
	    assertRead(map);
	    assertWrite(map);
	    
	    /* Only unmark if the bit is marked now. */
	    if (isMarked(bit))
	    {
		map[bit / 8] &= ~(1 << (bit % 8));
		free++;
	    }
	}
    
	/**
	 * Verify if a given bit is marked.
	 * @param bit Bit number to check.
	 * @return True if marked, false otherwise.
	 */
	bool isMarked(Size bit) const
	{
	    assert(bit < count);
	    assertRead(map);
	
	    return map[bit / 8] & (bit % 8);
	}

	/**
	 * Retrieve a pointer to the internal bitmap.
	 * @return Internal bitmap.
	 */
	u8 * getMap() const
	{
	    return map;
	}
    
    private:
    
	/** Total number of bits in the map. */
	Size count;
        
	/** Unmarked bits remaining. */
	Size free;

	/** Bitmap which represents free and used blocks. */
	u8 *map;
};

#endif /* __BITMAP_H */
