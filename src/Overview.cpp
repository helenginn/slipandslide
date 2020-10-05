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

void Overview::makeDistanceSlider(QWidget *prev)
{
	delete _distanceSlider;
	_distanceSlider = new QSlider(Qt::Horizontal, this);
	_distanceSlider->setGeometry(prev->geometry().left(),
	                             prev->geometry().bottom(), 
	                             prev->width(), 30);
	_distanceSlider->setMinimum(0);
	_distanceSlider->setMaximum(2500);
	_distanceSlider->show();
	
	delete _distanceLabel;
	_distanceLabel = new QLabel("Detector distance: ", this);
	_distanceLabel->setGeometry(prev->geometry().left(),
	                            _distanceSlider->geometry().bottom(),
	                             prev->width(), 30);
	_distanceLabel->show();

	_detView->updateSlider(_distanceSlider);

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
}

void Overview::supplyImagesToPanel(SlipPanel *p)
{
	p->clearImageData();
	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *ptr = &_images[i];
		p->getPeaksFromImage(ptr);
	}
	

}
