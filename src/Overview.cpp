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
#include "Splattice.h"
#include <FileReader.h>
#include <CurveView.h>
#include <Dialogue.h>
#include <Curve.h>
#include <crystfel/image.h>
#include <iostream>
#include <QTabWidget>
#include <QMenuBar>
#include <QGuiApplication>
#include <QScreen>
#include <QMessageBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>

#include <gsl/gsl_linalg.h>
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
	
	_splattice = new Splattice(this);

	makeMenu();

	double w = QGuiApplication::primaryScreen()->size().width();
	double h = QGuiApplication::primaryScreen()->size().height();
	int mh = menuBar()->height();
	
	double cw = std::max(w * 2. / 3., 1000.);
	double hw = std::max(h - mh, 1000.);

	_detView = new DetectorView(this);
	_detView->setGeometry(0, mh, cw, hw);
	_detView->setOverview(this);
	_detView->show();
	
	showFullScreen();
}

void Overview::makeMenu()
{
	QMenu *structure = menuBar()->addMenu(tr("&File"));
	QAction *act = structure->addAction(tr("Load geometry file"));
	connect(act, &QAction::triggered, this, &Overview::loadGeometry);
	act = structure->addAction(tr("Load stream file"));
	connect(act, &QAction::triggered, this, &Overview::loadStreamFile);

	QMenu *splattice = menuBar()->addMenu(tr("&Splattice"));

	act = splattice->addAction(tr("Run splattice"));
	connect(act, &QAction::triggered, 
	        _splattice, &Splattice::runSplattice);
}

void Overview::loadGeometry()
{
	std::string geomstr = openDialogue(this, "Choose geometry file", 
	                                   "CrystFEL geometry file (*.geom)");

	struct detector *det = get_detector_geometry(geomstr.c_str(), NULL);

	if (det == NULL)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("Loading geometry file failed."));
		msgBox.exec();
		return;
	}

	bool already = (_detector != NULL);

	loadDetector(det);
	repredictImages(true);
	
	_geomstr = geomstr;
	
	if (already)
	{
		_detView->updatePowderPattern();
		_detView->updateTargetPattern();
		return;
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
	QWidget *above = splitButton(_gammaLabel);
	refineButtons(above);
}

void Overview::loadStreamFile()
{
	if (_detView->getDetector() == NULL)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("Need to load geometry file first."));
		msgBox.exec();
		return;
	}

	std::string streamstr = openDialogue(this, "Choose geometry file", 
	                                   "CrystFEL geometry file (*.geom)");

	Stream *stream = open_stream_for_read(streamstr.c_str());

	if (stream == NULL)
	{
		QMessageBox msgBox;
		msgBox.setText(tr("Loading stream file failed."));
		msgBox.exec();
		return;
	}

	loadStream(stream);
	close_stream(stream);
}

void Overview::loadDetector(struct detector *det)
{
	_detector = det;
	_detView->setDetector(det);
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
	double b = tick / 200.;
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
	double a = tick / 200.;
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
	double a = tick / 500.;
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
	double a = tick / 500.;
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
	cv->setWindow(-5, -5, 5, 5);
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
		if (read_chunk(stream, next) != 0 )
		{
			break;
		}

		struct image *cur = &_images[_images.size() - 1];
		cur->spectrum = spectrum_generate_gaussian(cur->lambda, cur->bw);
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

	makeImageSlider(_distanceLabel);

	repredictImages(false);

	_detView->updatePowderPattern();
	_detView->updateTargetPattern();
}

void Overview::writeGeometry()
{
	if (_geomstr.length() == 0)
	{
		std::cout << "No geometry file loaded." << std::endl;
	}

	double d = _detView->originalDistance();
	for (int i = 0; i < _detector->n_panels; i++)
	{
		SlipPanel *p = _detView->getPanel(i);
		p->cLenToOffset(d);
	}

	std::string path = getPath(_geomstr);
	std::string name = getFilename(_geomstr);
	std::string newname = path + "/s-and-s-" + name;
	write_detector_geometry_2(_geomstr.c_str(), newname.c_str(),
	                          _detector,
	                          "refined by slip-and-slide algorithm, "\
	                          "J. Synchrotron Rad. (2017). 24, 1152-1162", 1);


	for (int i = 0; i < _detector->n_panels; i++)
	{
		SlipPanel *p = _detView->getPanel(i);
		p->cOffsetToLen(d);
	}
	
	QMessageBox msgBox;
	msgBox.setText(QString::fromStdString("Written out geometry file to " 
	                                      + newname));
	msgBox.exec();
}

QWidget *Overview::splitButton(QWidget *prev)
{
	/*
	QPushButton *b = new QPushButton("Split panel", this);
	QPushButton *r = b;
	int w = QGuiApplication::primaryScreen()->size().width();
	w -= prev->geometry().left();
	b->setGeometry(prev->geometry().left() + w * 1/2,
	               prev->geometry().bottom(), w * 1/2., 30);
	connect(b, &QPushButton::clicked, _detView, &DetectorView::splitPanel);
	b->show();
	*/

	QPushButton *b = new QPushButton("Write geometry file", this);
	int w = QGuiApplication::primaryScreen()->size().width();
	w -= prev->geometry().left();
	b->setGeometry(prev->geometry().left() + w * 1/2,
	               prev->geometry().bottom(), w * 1/2., 30);
	connect(b, &QPushButton::clicked, this, &Overview::writeGeometry);
	b->show();

	b = new QPushButton("Repredict images", this);
	b->setGeometry(prev->geometry().left(),
	               prev->geometry().bottom(), w * 1/2., 30);
	connect(b, &QPushButton::clicked, this, &Overview::recalculateImages);
	b->show();

	
	return b;
}

void Overview::refineButtons(QWidget *prev)
{
	QPushButton *b = new QPushButton("Refine (intra-panel)", this);
	int w = QGuiApplication::primaryScreen()->size().width();
	w -= prev->geometry().left();
	b->setGeometry(prev->geometry().left(),
	               prev->geometry().bottom(), w * 1/2., 30);
	connect(b, &QPushButton::clicked, _detView, &DetectorView::intraPanel);
	b->show();

	b = new QPushButton("Refine (inter-panel)", this);
	b->setGeometry(prev->geometry().left() + w * 1/2,
	               prev->geometry().bottom(), w * 1/2., 30);
	connect(b, &QPushButton::clicked, _detView, &DetectorView::interPanel);
	b->show();
}

static int locate_peak_on_panel(double x, double y, double z, double k,
                                struct panel *p,
                                double *pfs, double *pss)
{
	double ctt, tta, phi;
	gsl_vector *v;
	gsl_vector *t;
	gsl_matrix *M;
	double fs, ss, one_over_mu;

	/* Calculate 2theta (scattering angle) and azimuth (phi) */
	tta = atan2(sqrt(x*x+y*y), k+z);
	ctt = cos(tta);
	phi = atan2(y, x);

	/* Set up matrix equation */
	M = gsl_matrix_alloc(3, 3);
	v = gsl_vector_alloc(3);
	t = gsl_vector_alloc(3);
	if ( (M==NULL) || (v==NULL) || (t==NULL) ) {
		ERROR("Failed to allocate vectors for prediction\n");
		return 0;
	}

	gsl_vector_set(t, 0, sin(tta)*cos(phi));
	gsl_vector_set(t, 1, sin(tta)*sin(phi));
	gsl_vector_set(t, 2, ctt);

	gsl_matrix_set(M, 0, 0, p->cnx);
	gsl_matrix_set(M, 0, 1, p->fsx);
	gsl_matrix_set(M, 0, 2, p->ssx);
	gsl_matrix_set(M, 1, 0, p->cny);
	gsl_matrix_set(M, 1, 1, p->fsy);
	gsl_matrix_set(M, 1, 2, p->ssy);
	gsl_matrix_set(M, 2, 0, p->clen*p->res);
	gsl_matrix_set(M, 2, 1, p->fsz);
	gsl_matrix_set(M, 2, 2, p->ssz);

	if ( gsl_linalg_HH_solve(M, t, v) ) {
		ERROR("Failed to solve prediction equation\n");
		return 0;
	}

	one_over_mu = gsl_vector_get(v, 0);
	fs = gsl_vector_get(v, 1) / one_over_mu;
	ss = gsl_vector_get(v, 2) / one_over_mu;
	gsl_vector_free(v);
	gsl_vector_free(t);
	gsl_matrix_free(M);

	*pfs = fs;  *pss = ss;

	/* Now, is this on this panel? */
	if ( fs < 0.0 ) return 0;
	if ( fs >= p->w ) return 0;
	if ( ss < 0.0 ) return 0;
	if ( ss >= p->h ) return 0;

	return 1;
}

static signed int locate_peak(double x, double y, double z, double k,
                              struct detector *det, double *pfs, double *pss)
{
	int i;

	*pfs = -1;  *pss = -1;

	for ( i=0; i<det->n_panels; i++ ) {

		struct panel *p;

		p = &det->panels[i];

		if ( locate_peak_on_panel(x, y, z, k, p, pfs, pss) ) {

			/* Woohoo! */
			return i;

		}

	}

	return -1;
}

void Overview::recalculateImages()
{
	repredictImages(true);
}

void Overview::repredictImages(bool recalc)
{
	struct detector *det = _detView->getDetector();

	double asx, asy, asz;
	double bsx, bsy, bsz;
	double csx, csy, csz;

	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *im = &_images.at(i);
		im->det = det;
		double knom = 1.0/im->lambda;

		ImageFeatureList *list = im->features;
		for (int j = 0; j < image_feature_count(list); j++)
		{
			struct imagefeature *peak;
			peak = image_get_feature(list, j);
			peak->parent = im;
			
			double fs = peak->fs;
			double ss = peak->ss;

			if (recalc)
			{
				signed int pnum = locate_peak(peak->rx, peak->ry, peak->rz, 
				                              knom, det, &fs, &ss);
				
//				peak->fs = fs;
//				peak->ss = ss;
				peak->p = &det->panels[0];

				if (pnum >= 0)
				{
					peak->p = &det->panels[pnum];
				}
			}

			struct panel *p = peak->p;
			
			double x = (p->cnx  + fs*p->fsx + ss*p->ssx);
			x /= p->res;
			double y = (p->cny  + fs*p->fsy + ss*p->ssy);
			y /= p->res;
			double z = (fs*p->fsz + ss*p->ssz);
			z /= p->res;
			z += (p->clen + p->coffset);

			vec3 v = make_vec3(x, y, z);

			vec3_set_length(&v, knom);

			peak->rx = v.x;
			peak->ry = v.y;
			peak->rz = v.z - knom;
		}

		for (int j = 0; j < im->n_crystals; j++)
		{
			Crystal *cryst = im->crystals[j];
			RefListIterator *it;
			RefList *refs = crystal_get_reflections(cryst);
			Reflection *ref = first_refl(refs, &it);
			UnitCell *cell = crystal_get_cell(cryst);

			cell_get_reciprocal(cell, &asx, &asy, &asz,
			                    &bsx, &bsy, &bsz,
			                    &csx, &csy, &csz);

			while (true)
			{
				ref = (next_refl(ref, it));
				if (ref == NULL)
				{
					break;
				}

				signed int h, k, l;
				get_symmetric_indices(ref, &h, &k, &l);

				double xl = h*asx + k*bsx + l*csx;
				double yl = h*asy + k*bsy + l*csy;
				double zl = h*asz + k*bsz + l*csz;

				double fs, ss;        /* Position on detector */
				signed int p;         /* Panel number */
				p = locate_peak(xl, yl, zl, knom,
				                det, &fs, &ss);
				if (p < 0)
				{
					p = 0;
				}

				set_detector_pos(ref, fs, ss);
				set_panel(ref, &det->panels[p]);
			}
		}
	}
	
	supplyAllImages();
}

void Overview::supplyAllImages()
{
	_detView->clearPanelScratch();

	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *ptr = &_images[i];
		_detView->imageToPanels(ptr);
	}

	for (size_t i = 0; i < _images.size(); i++)
	{
		struct image *ptr = &_images[i];
		_splattice->addImage(ptr);
	}
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
	if (_radiusSlider == NULL)
	{
		return;
	}

	_radiusSlider->setValue(0);
	_alphaSlider->setValue(0);
	_betaSlider->setValue(0);
	_horizSlider->setValue(0);
	_vertSlider->setValue(0);
	_gammaSlider->setValue(0);

}
