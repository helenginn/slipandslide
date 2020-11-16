// slipnslide
// Copyright (C) 2019 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

#ifndef __slipnslide__Speckle__
#define __slipnslide__Speckle__

#include <QWidget>

class Speckle : public QWidget
{
Q_OBJECT
public:
	Speckle(QWidget *parent = NULL);

protected:
	virtual void resizeEvent(QResizeEvent *event);
private:

	SlipGL *_gl;
};

#endif
