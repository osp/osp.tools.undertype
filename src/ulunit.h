/***************************************************************************
 *   Copyright (C) 2007 by Pierre Marchand   *
 *   pierre@moulindetouvois.com   *
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
#ifndef ULUNIT_H
#define ULUNIT_H

/**
	@author Pierre Marchand <pierre@moulindetouvois.com>
	
	In ULUnit, I assume the point is a postscript one.
*/

#include <QString>

class ULUnit
{
	private:
		double value; // value is in point
	public:
		ULUnit();
		~ULUnit();

		ULUnit ( const double& v, QString unit = "pt" ) ;
		ULUnit ( const ULUnit& u );
		ULUnit ( const QString& sv );

		void set ( const double& v, QString unit = "pt" ) ;
		void set ( const ULUnit& u );
		void set ( const QString& sv );

		double pt() const;
		double mm() const;
		
		ULUnit &operator= (const double& v);
		ULUnit &operator= (const ULUnit& u );
		ULUnit &operator= (const QString& sv );
		
		

};

#endif
