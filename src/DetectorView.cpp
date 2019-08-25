// Slip n Slide
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

#include "DetectorView.h"
#include <iostream>

DetectorView::DetectorView(QWidget *p) : QMainWindow(p)
{
	setGeometry(100, 100, 400, 400);

	_gl = new SlipGL();
}

void DetectorView::setDetector(struct detector *det)
{
	_det = det;

	for (int i = 0; i < _det->n_panels; i++)
	{
		struct panel p = _det->panels[i];
		std::cout << "Panel: " << p.name << std::endl;
	}
	
}

DetectorView::~DetectorView()
{
	delete _gl;
	_gl = NULL;
}
