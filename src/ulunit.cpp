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
#include "ulunit.h"

#include <QStringList>

ULUnit::ULUnit()
{
	value = 0.0;
}


ULUnit::~ULUnit()
{
}

ULUnit::ULUnit(const double & v, QString unit)
{
	set(v,unit);
}

ULUnit::ULUnit(const ULUnit& u)
{
	value = u.pt();
}

ULUnit::ULUnit(const QString& sv)
{
	set(sv);
}

void ULUnit::set(const double& v, QString unit)
{
	if(unit == "pt")
	{
		value = v;
	}
	else if(unit == "mm")
	{
		value = v / 0.352777778 ;
	}
		
}

void ULUnit::set(const QString& sv)
{
	QString num;
	QString unit;
	QStringList l = sv.split(" ");
	num = l[0];
	unit = l[1];
	set(num.toDouble(), unit);
}

void ULUnit::set(const ULUnit& u)
{
	value = u.pt();
}

double ULUnit::pt() const
{
	return value;
}

double ULUnit::mm() const
{
	return value * 0.352777778;
}

ULUnit & ULUnit::operator =(const double& v)
{
	set(v);
	return *this;
}

ULUnit & ULUnit::operator =(const ULUnit& u)
{
	set(u);
	return *this;
}

ULUnit & ULUnit::operator =(const QString& sv)
{
	set(sv);
	return *this;
}








