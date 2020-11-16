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

#include "Refine.h"
#include "DetectorView.h"
#include "SlipPanel.h"
#include "Overview.h"
#include "Line.h"
#include "CurveView.h"
#include "Curve.h"
#include <SlipGL.h>
#include <iostream>
#include <QSlider>
#include <QThread>
#include <RefinementNelderMead.h>

#define PAN_SENSITIVITY 3

DetectorView::DetectorView(QWidget *p) : QWidget(p)
{
	_worker = NULL;
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
	_det = NULL;

	resize(1000, 1000);
	setFocus();
}

void DetectorView::resizeEvent(QResizeEvent *event)
{
	_gl->setGeometry(0, 0, width(), height());
}

void DetectorView::setDetector(struct detector *det, bool refresh)
{
	for (size_t i = 0; i < _panels.size(); i++)
	{
		_gl->removeObject(_panels[i]);
	}

	_panels.clear();

	if (_det != NULL)
	{
		free_detector_geometry(_det);
	}

	_det = det;

	_gl->preparePanels(_det->n_panels);
	double ave_d = 0;
	_allPanels = new SlipPanel();
	_allPanels->setOverview(_overview);
	_selected->clearPanels();

	for (int i = 0; i < _det->n_panels; i++)
	{
		struct panel *p = &(_det->panels[i]);

		SlipPanel *spanel = new SlipPanel(p);
		spanel->setOverview(_overview);
		_panels.push_back(spanel);
		_gl->addObject(spanel, false);

		double d = 1000 *  (p->clen + p->coffset);
		ave_d += d;

		_allPanels->addPanel(spanel);
	}
	
	ave_d /= _det->n_panels;

	vec3 centre = make_vec3(0, 0, ave_d);
	std::cout << "Average distance: " << ave_d << std::endl;
	
	vec3 extra = centre;
	vec3_mult(&extra, 1.1);
	vec3 origin = empty_vec3();
	
	Line *l = new Line(origin, extra);
	_gl->addObject(l, false);

	if (!refresh)
	{
		centre.z *= -1;
		_gl->changeCentre(centre);
//		_gl->draggedRightMouse(0, 100 * ave_d);
	}
	
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
		if (_panels[i]->intersectsPolygon(x, y, &z))
		{
			closest = _panels[i];
		}
	}

	if (closest)
	{
		_selected->togglePanel(closest);
		_overview->supplyImagesToPanel(_selected);
		_selected->acceptNudges();
	}
	else
	{
		activePanel()->acceptNudges();
		_selected->clearPanels();
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
	clen *= 1e7;
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
		SlipPanel *p = getPanel(i);
		p->setZ(metres);
		p->updateTmpPanelValues();
		p->updateVertices();
	}
	
	std::cout << -metres * 1000 << " mm." << std::endl;
	
	updatePowderPattern();
	updateTargetPattern();
}

void DetectorView::splitPanel()
{
	if (activePanel()->panelCount() != 1)
	{
		std::cout << "Too many panels selected" << std::endl;
		return;
	}
	
	SlipPanel *p = activePanel()->getPanel(0);
	p->split(_det);

	_gl->clearObjects();
	activePanel()->clearPanels();
	setDetector(_det, true);
}

void DetectorView::updateGlobalDetectorDistance()
{
	QSlider *s = static_cast<QSlider *>(QObject::sender());
	int val = s->value();
	if (val == 0)
	{ 
		return;
	}

	double newlen = (double)val / 1e7;
	setDistanceAllPanels(newlen);
	_overview->updateDistanceLabel(-newlen * 1000);
}

void DetectorView::updateTargetPattern()
{
	if (_targetCurve->getCurveView()->isVisible())
	{
		activePanel()->updateTarget(_targetCurve, true);
	}
}

void DetectorView::updatePowderPattern()
{
	if (_powderCurve->getCurveView()->isVisible())
	{
		activePanel()->updatePowder(_powderCurve, true);
	}
}

SlipPanel *DetectorView::activePanel()
{
	if (_selected->panelCount() > 0)
	{
		return _selected;
	}
	
	return _allPanels;
}

DetectorView::~DetectorView()
{
	delete _gl;
	_gl = NULL;
}

void DetectorView::setOverview(Overview *over)
{
	_overview = over;
	
	for (size_t i = 0; i < _panels.size(); i++)
	{
		getPanel(i)->setOverview(over);
	}

	_selected->setOverview(_overview);
}

void DetectorView::clearPanelScratch()
{
	_allPanels->clearImageData();
	activePanel()->clearImageData();
	
	for (size_t i = 0; i < _panels.size(); i++)
	{
		_panels[i]->clearImageData();
	}

}

void DetectorView::intraPanel()
{
	refinePanel(true);
}

void DetectorView::refinePanel(bool intra)
{
	if (_worker && _worker->isRunning())
	{
		return;
	}
	
	if (!_worker)
	{
		_worker = new QThread();
	}
	
	_refine = new Refine();
	_refine->moveToThread(_worker);
	
	_refine->setPanel(activePanel(), intra);
	
	connect(this, SIGNAL(refine()), _refine, SLOT(refine()));

	connect(_refine, SIGNAL(resultReady()), this, SLOT(handleResults()));
//	connect(_refine, SIGNAL(failed()), this, SLOT(handleError()));
	_worker->start();

	emit refine();
}

void DetectorView::handleResults()
{
	Refine*obj = static_cast<Refine *>(QObject::sender());

	disconnect(this, SIGNAL(refine()), nullptr, nullptr);
	disconnect(obj, SIGNAL(resultReady()), this, SLOT(handleResults()));
	_worker->quit();
	_worker->wait();

	updateTargetPattern();
}

void DetectorView::interPanel()
{
	refinePanel(false);
}

