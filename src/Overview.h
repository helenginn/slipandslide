// slip and slide
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

#ifndef __slipandslide__overview__
#define __slipandslide__overview__

#include <QMainWindow>
#include <crystfel/detector.h>
#include <crystfel/stream.h>
#include <crystfel/image.h>

class QSlider;
class QLabel;
class DetectorView;
class CurveView;
class SlipPanel;

class Overview : public QMainWindow
{
Q_OBJECT

public:
	Overview(QWidget *parent = NULL);
	
	void loadDetector(struct detector *det);
	void loadStream(Stream *stream);
	void updateDistanceLabel(double mm);
	void supplyImagesToPanel(SlipPanel *p);
public slots:

protected:
	void powderGraph(QTabWidget *tab);
	void targetGraph(QTabWidget *tab);
	void makeDistanceSlider(QWidget *prev);
private:
	std::vector<struct image> _images;
	CurveView *_powderView;
	CurveView *_targetView;
	DetectorView *_detView;
	struct detector *_detector;

	QSlider *_distanceSlider;
	QLabel *_distanceLabel;
};

#endif
