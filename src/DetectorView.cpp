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
#include "SlipGL.h"
#include <iostream>

class SlipPanel;

DetectorView::DetectorView(QWidget *p) : QMainWindow(p)
{
	_gl = new SlipGL(this);
	_gl->show();

	resize(1000, 1000);
}

void DetectorView::resizeEvent(QResizeEvent *event)
{
	_gl->setGeometry(0, 0, width(), height());
}

void DetectorView::setDetector(struct detector *det)
{
	_det = det;
	_gl->preparePanels(_det->n_panels);

	for (int i = 0; i < _det->n_panels; i++)
	{
		struct panel p = _det->panels[i];
		std::cout << "Adding panel: " << p.name << std::endl;
		
		_gl->addPanel(p);
	}
	
	_gl->update();
}

DetectorView::~DetectorView()
{
	delete _gl;
	_gl = NULL;
}
