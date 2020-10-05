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

#include "Overview.h"
#include "SlipPanel.h"
#include "DetectorView.h"
#include <FileReader.h>
#include <CurveView.h>
#include <Curve.h>
#include <QTabWidget>
#include <crystfel/image.h>
#include <iostream>
#include <QGuiApplication>
#include <QScreen>
#include <QSlider>
#include <QLabel>

#include <crystfel/stream.h>
#include <crystfel/utils.h>
#include <crystfel/symmetry.h>
#include <crystfel/geometry.h>
#include <crystfel/peaks.h>
#include <crystfel/reflist.h>
#include <crystfel/reflist-utils.h>
#include <crystfel/cell.h>
#include <crystfel/cell-utils.h>

Overview::Overview(QWidget *parent) : QMainWindow(parent)
{
	_detView = NULL;
	_detector = NULL;
	_powderView = NULL;
	_targetView = NULL;
	_distanceSlider = NULL;
	_distanceLabel = NULL;
	_radiusSlider = NULL;
	_radiusLabel = NULL;
	_imageSlider = NULL;
	_imageLabel = NULL;
	_intensitySlider = NULL;
	_intensityLabel = NULL;
	_alphaSlider = NULL;
	_alphaLabel = NULL;
	_betaSlider = NULL;
	_betaLabel = NULL;
	_gammaSlider = NULL;
	_gammaLabel = NULL;
	_horizSlider = NULL;
	_horizLabel = NULL;
	_vertSlider = NULL;
	_vertLabel = NULL;

	setWindowState(Qt::WindowFullScreen);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	
	showFullScreen();
}

void Overview::loadDetector(struct detector *det)
{
	int w = QGuiApplication::primaryScreen()->size().width();
	int h = QGuiApplication::primaryScreen()->size().height();

	_detView = new DetectorView(this);
	_detView->setGeometry(0, 0, w * 2. / 3., h);
	_detView->setOverview(this);
	_detector = det;
	_detView->setDetector(det);
	_detView->show();
}

void Overview::makeSlider(QSlider **handle, QWidget *prev)
{
	delete *handle;
	*handle = new QSlider(Qt::Horizontal, this);
	int w = QGuiApplication::primaryScreen()->size().width();
	w -= prev->geometry().left();

	(*handle)->setGeometry(prev->geometry().left() + w * 1./3.,
	                             prev->geometry().bottom(), 
	                             w * 2/3., 30);
	(*handle)->setMinimum(0);
	(*handle)->setMaximum(2500);
	(*handle)->show();
}

void Overview::makeSliderLabel(QLabel **label, QWidget *prev)
{
	int w = QGuiApplication::primaryScreen()->size().width();
	w -= prev->geometry().left();

	delete *label;
	(*label) = new QLabel("", this);
	(*label)->setGeometry(prev->geometry().left(),
	                            prev->geometry().bottom(),
	                            w * 1/3., 30);
	(*label)->show();

}

void Overview::makeDistanceSlider(QWidget *prev)
{
	makeSlider(&_distanceSlider, prev);
	makeSliderLabel(&_distanceLabel, prev);
	_distanceLabel->setText("Detector distance: ");

	_detView->updateSlider(_distanceSlider);
}

void Overview::makeRadiusSlider(QWidget *prev)
{
	makeSlider(&_radiusSlider, prev);
	makeSliderLabel(&_radiusLabel, prev);
	_radiusSlider->setMinimum(-100);
	_radiusSlider->setMaximum(+100);
	_radiusSlider->setValue(0);
	connect(_radiusSlider, &QSlider::valueChanged, 
	        this, &Overview::handleRadiusSlider);

	_radiusLabel->setText("Radius offset: 0 mm");
}

void Overview::makeBetaSlider(QWidget *prev)
{
	makeSlider(&_betaSlider, prev);
	makeSliderLabel(&_betaLabel, prev);
	_betaSlider->setMinimum(-100);
	_betaSlider->setMaximum(+100);
	_betaSlider->setValue(0);
	connect(_betaSlider, &QSlider::valueChanged, 
	        this, &Overview::handleBetaSlider);

	_betaLabel->setText("Second angle: 0°");
}

void Overview::makeAlphaSlider(QWidget *prev)
{
	makeSlider(&_alphaSlider, prev);
	makeSliderLabel(&_alphaLabel, prev);
	_alphaSlider->setMinimum(-100);
	_alphaSlider->setMaximum(+100);
	_alphaSlider->setValue(0);
	connect(_alphaSlider, &QSlider::valueChanged, 
	        this, &Overview::handleAlphaSlider);

	_alphaLabel->setText("First angle: 0°");
}

void Overview::makeVerticalSlider(QWidget *prev)
{
	makeSlider(&_vertSlider, prev);
	makeSliderLabel(&_vertLabel, prev);
	_vertSlider->setMinimum(-100);
	_vertSlider->setMaximum(+100);
	_vertSlider->setValue(0);
	connect(_vertSlider, &QSlider::valueChanged, 
	        this, &Overview::handleVertSlider);

	_vertLabel->setText("Vertical slide: 0°");
}

void Overview::makeGammaSlider(QWidget *prev)
{
	makeSlider(&_gammaSlider, prev);
	makeSliderLabel(&_gammaLabel, prev);
	_gammaSlider->setMinimum(-100);
	_gammaSlider->setMaximum(+100);
	_gammaSlider->setValue(0);
	connect(_gammaSlider, &QSlider::valueChanged, 
	        this, &Overview::handleGammaSlider);

	_gammaLabel->setText("Swivel slide: 0°");
}

void Overview::makeHorizontalSlider(QWidget *prev)
{
	makeSlider(&_horizSlider, prev);
	makeSliderLabel(&_horizLabel, prev);
	_horizSlider->setMinimum(-100);
	_horizSlider->setMaximum(+100);
	_horizSlider->setValue(0);
	connect(_horizSlider, &QSlider::valueChanged, 
	        this, &Overview::handleHorizSlider);

	_horizLabel->setText("Horizontal slide: 0°");
}

void Overview::makeIntensitySlider(QWidget *prev)
{
	makeSlider(&_intensitySlider, prev);
	_intensitySlider->setMaximum(2000);
	_intensitySlider->setValue(200);
	connect(_intensitySlider, &QSlider::valueChanged, 
	        this, &Overview::handleIntensitySlider);

	makeSliderLabel(&_intensityLabel, prev);
	_intensityLabel->setText("Minimum intensity: 200 ADU");
}

void Overview::makeImageSlider(QWidget *prev)
{
	makeSlider(&_imageSlider, prev);
	_imageSlider->setMaximum(_images.size());
	_imageSlider->setValue(20);
	connect(_imageSlider, &QSlider::valueChanged, 
	        this, &Overview::handleImageSlider);

	makeSliderLabel(&_imageLabel, prev);
	_imageLabel->setText("Up to 20 images");
}

void Overview::handleBetaSlider(int tick)
{
	double b = tick / 50.;
	std::string str = "Second angle: " + f_to_str(b, 2) + "°";
	_betaLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setBeta(panel, b * M_PI / 180 );
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleAlphaSlider(int tick)
{
	double a = tick / 50.;
	std::string str = "First angle: " + f_to_str(a, 2) + "°";
	_alphaLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setAlpha(panel, a * M_PI / 180 );
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleVertSlider(int tick)
{
	double a = tick / 50.;
	std::string str = "Vertical slide: " + f_to_str(a, 2) + "°";
	_vertLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setVert(panel, a * M_PI / 180 );
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleGammaSlider(int tick)
{
	double a = tick / 50.;
	std::string str = "Swivel slide: " + f_to_str(a, 2) + "°";
	_gammaLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setGamma(panel, a * M_PI / 180 );
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleHorizSlider(int tick)
{
	double a = tick / 50.;
	std::string str = "Horizontal slide: " + f_to_str(a, 2) + "°";
	_horizLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setHoriz(panel, a * M_PI / 180 );
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleRadiusSlider(int tick)
{
	double r = tick / 50.;
	std::string str = "Radius offset: " + f_to_str(r, 2) + " mm";
	_radiusLabel->setText(QString::fromStdString(str));

	SlipPanel *panel = _detView->activePanel();
	SlipPanel::setRadius(panel, r / 1000);
	panel->nudgePanels();
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleIntensitySlider(int tick)
{
	std::string str = "Minimum intensity: " + i_to_str(tick) + " ADU";
	_intensityLabel->setText(QString::fromStdString(str));
	SlipPanel::setMinIntensity(tick);
	supplyImagesToPanel(_detView->activePanel());
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::handleImageSlider(int tick)
{
	std::string str = "Up to " + i_to_str(tick) + " images";
	_imageLabel->setText(QString::fromStdString(str));
	supplyImagesToPanel(_detView->activePanel());
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::powderGraph(QTabWidget *tab)
{
	Curve *c = new Curve(NULL);

	CurveView *cv = new CurveView(this);
	cv->setWindow(-2, -2, 2, 2);
	cv->addCurve(c);
	cv->redraw();
	tab->addTab(cv, "Inter-peak distances");
	cv->show();
	
	delete _powderView;
	_powderView = cv;
	_detView->setPowderCurve(c);
	_detView->updatePowderPattern();
}

void Overview::targetGraph(QTabWidget *tab)
{
	Curve *c = new Curve(NULL);

	CurveView *cv = new CurveView(this);
	cv->setWindow(-2, -2, 2, 2);
	cv->addCurve(c);
	cv->redraw();
	tab->addTab(cv, "Offset to closest prediction");
	cv->show();
	
	delete _targetView;
	_targetView = cv;
	_detView->setTargetCurve(c);
	_detView->updateTargetPattern();
}

void Overview::updateDistanceLabel(double mm)
{
	std::string str = "Detector distance: " + f_to_str(-mm, 3) + " mm";
	_distanceLabel->setText(QString::fromStdString(str));
}

static RefList *apply_max_adu(RefList *list, double max_adu)
{
	RefList *nlist;
	Reflection *refl;
	RefListIterator *iter;

	nlist = reflist_new();
	if ( nlist == NULL ) return NULL;

	for ( refl = first_refl(list, &iter);
	      refl != NULL;
	      refl = next_refl(refl, iter) )
	{
		if ( get_peak(refl) < max_adu ) {
			signed int h, k, l;
			get_indices(refl, &h, &k, &l);
			Reflection *nrefl = add_refl(nlist, h, k, l);
			copy_data(nrefl, refl);
		}
	}
	reflist_free(list);
	return nlist;
}

void Overview::loadStream(Stream *stream)
{
	struct image *next;
	
	_images.resize(_images.size() + 1);
	next = &_images[_images.size() - 1];
	next->det = _detector;

	int n_crystals = 0;
	int n_crystals_seen = 0;
	Crystal **crystals = NULL;
	double max_adu = +INFINITY;

	char *sym_str = NULL;
	SymOpList *sym;

	if ( sym_str == NULL ) sym_str = strdup("1");
	pointgroup_warning(sym_str);
	sym = get_pointgroup(sym_str);

	while (true)
	{
//		ReadStreamFlags f = STREAM_READ_REFLECTIONS | STREAM_READ_UNITCELL;
		if (read_chunk(stream, next) != 0 )
		{
			break;
		}

		struct image *cur = &_images[_images.size() - 1];
		RefList *as;

		for (int i = 0; i<cur->n_crystals; i++)
		{

			Crystal *cr;
			Crystal **crystals_new;
			RefList *cr_refl;
			struct image *image;

			n_crystals_seen++;
			
			crystals_new = (Crystal **)realloc(crystals,
			                      (n_crystals+1)*sizeof(Crystal *));
			if ( crystals_new == NULL ) {
				ERROR("Failed to allocate memory for crystal "
				      "list.\n");
				return;
			}
			crystals = crystals_new;
			crystals[n_crystals] = cur->crystals[i];
			cr = crystals[n_crystals];

			image = (struct image *)malloc(sizeof(struct image));
			if ( image == NULL ) {
				ERROR("Failed to allocatea memory for image.\n");
				return;
			}

			crystal_set_image(cr, image);
			*image = *cur;
			image->n_crystals = 1;
			image->crystals = &crystals[n_crystals];

			/* This is the raw list of reflections */
			cr_refl = crystal_get_reflections(cr);

			cr_refl = apply_max_adu(cr_refl, max_adu);

			as = asymmetric_indices(cr_refl, sym);
			crystal_set_reflections(cr, as);
			crystal_set_user_flag(cr, 0);
			reflist_free(cr_refl);

			n_crystals++;
		}

		_images.resize(_images.size() + 1);
		next = &_images[_images.size() - 1];
		next->det = _detector;
		next->div = NAN;
		next->bw = NAN;
	}
	
	_images.pop_back();
	
	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *ptr = &_images[i];
		_detView->imageToPanels(ptr);
	}
	
	int w = QGuiApplication::primaryScreen()->size().width();
	int h = QGuiApplication::primaryScreen()->size().height();

	QTabWidget *tabs = new QTabWidget(this);
	tabs->setGeometry(w * 2. / 3., 0, w * 1. / 3., h * 1. / 3.);
	tabs->show();

	powderGraph(tabs);
	targetGraph(tabs);
	makeDistanceSlider(tabs);
	makeImageSlider(_distanceLabel);
	makeIntensitySlider(_imageLabel);
	makeRadiusSlider(_intensityLabel);
	makeAlphaSlider(_radiusLabel);
	makeBetaSlider(_alphaLabel);
	makeHorizontalSlider(_betaLabel);
	makeVerticalSlider(_horizLabel);
	makeGammaSlider(_vertLabel);
}

void Overview::supplyImagesToPanel(SlipPanel *p)
{
	p->clearImageData();
	p->setMaxImages(_imageSlider->value());
	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *ptr = &_images[i];
		p->getPeaksFromImage(ptr);
	}
	
	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::resetSliders()
{
	_radiusSlider->setValue(0);
	_alphaSlider->setValue(0);
	_betaSlider->setValue(0);
	_horizSlider->setValue(0);
	_vertSlider->setValue(0);
	_gammaSlider->setValue(0);

}
