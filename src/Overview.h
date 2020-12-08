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
	void supplyAllImages();
	void supplyImagesToPanel(SlipPanel *p);
	void resetSliders();

	QWidget *splitButton(QWidget *prev);
	
	std::vector<struct image> *images()
	{
		return &_images;
	}
public slots:
	void handleImageSlider(int tick);
	void handleIntensitySlider(int tick);
	void handleRadiusSlider(int tick);
	void handleAlphaSlider(int tick);
	void handleBetaSlider(int tick);
	void handleGammaSlider(int tick);
	void handleHorizSlider(int tick);
	void handleVertSlider(int tick);
	void recalculateImages();

	void loadStreamFile();
	void loadGeometry();
	void writeGeometry();

protected:
	void makeMenu();
	void powderGraph(QTabWidget *tab);
	void targetGraph(QTabWidget *tab);

	void makeDistanceSlider(QWidget *prev);
	void makeImageSlider(QWidget *prev);
	void makeIntensitySlider(QWidget *prev);
	void makeRadiusSlider(QWidget *prev);
	void makeAlphaSlider(QWidget *prev);
	void makeBetaSlider(QWidget *prev);
	void makeGammaSlider(QWidget *prev);
	void makeHorizontalSlider(QWidget *prev);
	void makeVerticalSlider(QWidget *prev);
private:
	void makeSlider(QSlider **handle, QWidget *prev);
	void makeSliderLabel(QLabel **label, QWidget *prev);
	void refineButtons(QWidget *prev);
	void repredictImages(bool recalc = true);

	double targetScore();

	std::vector<struct image> _images;
	CurveView *_powderView;
	CurveView *_targetView;
	DetectorView *_detView;
	struct detector *_detector;
	std::string _geomstr;
	Splattice *_splattice;

	QSlider *_distanceSlider;
	QSlider *_imageSlider;
	QSlider *_intensitySlider;
	QSlider *_radiusSlider;
	QSlider *_alphaSlider;
	QSlider *_betaSlider;
	QSlider *_gammaSlider;
	QSlider *_horizSlider;
	QSlider *_vertSlider;
	QLabel *_distanceLabel;
	QLabel *_imageLabel;
	QLabel *_intensityLabel;
	QLabel *_radiusLabel;
	QLabel *_alphaLabel;
	QLabel *_betaLabel;
	QLabel *_gammaLabel;
	QLabel *_horizLabel;
	QLabel *_vertLabel;
};

#endif
