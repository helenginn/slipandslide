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

#ifndef __Slip__DetectorView__
#define __Slip__DetectorView__

#include "SlipGL.h"
#include <QWidget>
#include <crystfel/detector.h>
#include <crystfel/image.h>
#include <QMouseEvent>

class SlipPanel;
class QSlider;
class Curve;
class Overview;

class DetectorView : public QWidget
{
	Q_OBJECT
	
public:
	DetectorView(QWidget *parent = NULL);
	
	void setDetector(struct detector *det);
	void imageToPanels(struct image *im);
	void updateSlider(QSlider *s);
	void setDistanceAllPanels(double metres);
	
	void setOverview(Overview *over)
	{
		_overview = over;
	}
	
	void setTargetCurve(Curve *curve)
	{
		_targetCurve = curve;
	}
	
	void setPowderCurve(Curve *curve)
	{
		_powderCurve = curve;
	}
	
	SlipPanel *getPanel(int i)
	{
		return _panels[i];
	}

	~DetectorView();
public slots:
	void updateGlobalDetectorDistance();
	void updatePowderPattern();
	void updateTargetPattern();
	
protected:
	void convertCoords(double *x, double *y);

	virtual void resizeEvent(QResizeEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

private:
	struct detector *_det;
	SlipGL *_gl;
	std::vector<SlipPanel *> _panels;
	SlipPanel *_allPanels;
	SlipPanel *_selected;
	
	Overview *_overview;
	Curve *_powderCurve;
	Curve *_targetCurve;
	Qt::MouseButton _mouseButton;
	bool _controlPressed;
	bool _moving;
	double _lastX;
	double _lastY;
};

#endif
