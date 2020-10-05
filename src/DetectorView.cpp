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
#include "SlipPanel.h"
#include "Overview.h"
#include "Line.h"
#include "CurveView.h"
#include "Curve.h"
#include <SlipGL.h>
#include <iostream>
#include <QSlider>

#define PAN_SENSITIVITY 3

DetectorView::DetectorView(QWidget *p) : QWidget(p)
{
	_mouseButton = Qt::NoButton;
	_allPanels = NULL;
	_selected = new SlipPanel();
	_selected->setSelected(true);
	_controlPressed = false;
	_moving = false;
	_lastX = -1;
	_lastY = -1;
	_gl = new SlipGL(this);
	_gl->show();
	_gl->rotate(0, M_PI, 0);

	resize(1000, 1000);
	setFocus();
}

void DetectorView::resizeEvent(QResizeEvent *event)
{
	_gl->setGeometry(0, 0, width(), height());
}

void DetectorView::setDetector(struct detector *det)
{
	_det = det;
	_gl->preparePanels(_det->n_panels);
	double ave_d = 0;
	_allPanels = new SlipPanel();

	for (int i = 0; i < _det->n_panels; i++)
	{
		struct panel *p = &(_det->panels[i]);

		SlipPanel *spanel = new SlipPanel(p);
		_panels.push_back(spanel);
		_gl->addObject(spanel, false);

		double d = 1000 *  (p->clen + p->coffset);
		ave_d += d;

		_allPanels->addPanel(spanel);
	}
	
	ave_d /= _det->n_panels;

	vec3 centre = make_vec3(0, 0, ave_d);
	std::cout << vec3_desc(centre) << std::endl;
	
	vec3 extra = centre;
	vec3_mult(&extra, 1.1);
	vec3 origin = empty_vec3();
	
	Line *l = new Line(origin, extra);
	_gl->addObject(l, false);

	_gl->changeCentre(centre);
	_gl->draggedRightMouse(0, 100 * ave_d);
	_gl->update();
	

}

void DetectorView::imageToPanels(struct image *im)
{
	for (size_t i = 0; i < _panels.size(); i++)
	{
		_panels[i]->getPeaksFromImage(im);
	}
	
	_allPanels->getPeaksFromImage(im);
}

void DetectorView::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Control)
	{
		_controlPressed = true;
	}
}

void DetectorView::keyReleaseEvent(QKeyEvent *event)
{
	_controlPressed = false;
}


void DetectorView::mousePressEvent(QMouseEvent *e)
{
	_lastX = e->x();
	_lastY = e->y();
	_mouseButton = e->button();
	_moving = false;
}

void DetectorView::mouseReleaseEvent(QMouseEvent *e)
{
	if (_mouseButton == Qt::NoButton || _moving)
	{
		return;
	}

	double x = e->x(); double y = e->y();
	convertCoords(&x, &y);

	SlipPanel *closest = NULL;
	double z = -FLT_MAX;

	for (size_t i = 0; i < _panels.size(); i++)
	{
		if (_panels[i]->intersects(x, y, &z))
		{
			closest = _panels[i];
		}
	}

	if (closest)
	{
		_selected->togglePanel(closest);
		_overview->supplyImagesToPanel(_selected);
		updatePowderPattern();
		updateTargetPattern();
	}
}

void DetectorView::mouseMoveEvent(QMouseEvent *e)
{
	if (_mouseButton == Qt::NoButton)
	{
		return;
	}

	_moving = true;

	double newX = e->x();
	double xDiff = _lastX - newX;
	double newY = e->y();
	double yDiff = _lastY - newY;
	_lastX = newX;
	_lastY = newY;

	if (_mouseButton == Qt::LeftButton)
	{
		if (_controlPressed)
		{
			_gl->panned(xDiff * 2, yDiff * 2);
		}
		else
		{
			_gl->draggedLeftMouse(-xDiff * 4, -yDiff * 4);
		}
	}
	else if (_mouseButton == Qt::RightButton)
	{
		_gl->draggedRightMouse(xDiff * PAN_SENSITIVITY * 3,
		                       yDiff * PAN_SENSITIVITY * 3);
	}
}

void DetectorView::convertCoords(double *x, double *y)
{
	double w = width();
	double h = height();// - menuBar()->height();
;//	*y -= menuBar()->height();

	*x = 2 * *x / w - 1.0;
	*y =  - (2 * *y / h - 1.0);
}

void DetectorView::updateSlider(QSlider *s)
{
	double clen = _det->defaults.clen;
	clen *= 1e5;
	std::cout << "Clen for slider: " <<  clen << std::endl;

	s->setMinimum(clen / 1.5);
	s->setMaximum(clen * 1.5);
	
	s->setValue(clen);
	_overview->updateDistanceLabel(-_det->defaults.clen * 1000);

	connect(s, &QSlider::valueChanged, this,
	        &DetectorView::updateGlobalDetectorDistance);
	connect(s, &QSlider::sliderReleased, this,
	        &DetectorView::updatePowderPattern);
}

void DetectorView::setDistanceAllPanels(double metres)
{
	for (int i = 0; i < _det->n_panels; i++)
	{
		struct panel *p = &(_det->panels[i]);
		p->clen = metres;
		_panels[i]->updateTmpPanelValues();
		_panels[i]->updateVertices();
	}
	
	std::cout << -metres * 1000 << " mm." << std::endl;
	
	updatePowderPattern();
	updateTargetPattern();
}

void DetectorView::updateGlobalDetectorDistance()
{
	QSlider *s = static_cast<QSlider *>(QObject::sender());
	int val = s->value();
	if (val == 0)
	{ 
		return;
	}

	double newlen = (double)val / 1e5;
	setDistanceAllPanels(newlen);
	_overview->updateDistanceLabel(-newlen * 1000);
}

void DetectorView::updateTargetPattern()
{
	if (_targetCurve->getCurveView()->isVisible())
	{
		if (_selected->panelCount() > 0)
		{
			_selected->updateTarget(_targetCurve, true);
		}
		else
		{
			_allPanels->updateTarget(_targetCurve, true);
		}
	}
}

void DetectorView::updatePowderPattern()
{
	if (_powderCurve->getCurveView()->isVisible())
	{
		if (_selected->panelCount() > 0)
		{
			_selected->updatePowder(_powderCurve, true);
		}
		else
		{
			_allPanels->updatePowder(_powderCurve, true);
		}
	}
}

DetectorView::~DetectorView()
{
	delete _gl;
	_gl = NULL;
}
