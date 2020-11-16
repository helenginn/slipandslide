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

#ifndef __Slip__SlipPanel__
#define __Slip__SlipPanel__

#include "SlipObject.h"
#include "vec3.h"
#include <crystfel/detector.h>
#include <crystfel/image.h>

typedef struct
{
	Reflection *ref;
	struct imagefeature *peak;
	vec3 recip;
} RefPeak;

class Curve;
class Overview;

class SlipPanel : public SlipObject
{
public:
	SlipPanel(struct panel *p);
	SlipPanel();
	
	void addPanel(SlipPanel *other);	
	void togglePanel(SlipPanel *other);
	
	void clearPanels();
	
	SlipPanel *getPanel(int i)
	{
		return _subpanels[i];
	}
	
	struct panel *getSinglePanel()
	{
		return _panel;
	}
	
	void clearImageData()
	{
		_peaks.clear();
		_pairs.clear();
		_images.clear();
		_imageStarts.clear();
	}
	
	static void setMaxImages(size_t max)
	{
		_maxImages = max;
	}
	
	static void setMinIntensity(int min)
	{
		_minIntensity = min;
	}
	
	size_t panelCount()
	{
		return _subpanels.size();
	}
	
	std::string shortDesc();
	
	void setSelected(bool sel);

	bool isSelected()
	{
		return _isSelected;
	}
	
	void setOverview(Overview *o)
	{
		_overview = o;
	}
	
	static double getRadius(void *object)
	{
		return static_cast<SlipPanel *>(object)->_radius;
	}
	
	static void setRadius(void *object, double radius)
	{
		static_cast<SlipPanel *>(object)->_radius = radius;
	}
	
	static double getAlpha(void *object)
	{
		return static_cast<SlipPanel *>(object)->_alpha;
	}
	
	static void setAlpha(void *object, double alpha)
	{
		static_cast<SlipPanel *>(object)->_alpha = alpha;
	}
	
	static double getBeta(void *object)
	{
		return static_cast<SlipPanel *>(object)->_beta;
	}
	
	static void setBeta(void *object, double beta)
	{
		static_cast<SlipPanel *>(object)->_beta = beta;
	}
	
	static double getGamma(void *object)
	{
		return static_cast<SlipPanel *>(object)->_gamma;
	}
	
	static void setGamma(void *object, double gamma)
	{
		static_cast<SlipPanel *>(object)->_gamma = gamma;
	}
	
	static double getHoriz(void *object)
	{
		return static_cast<SlipPanel *>(object)->_horiz;
	}
	
	static void setHoriz(void *object, double horiz)
	{
		static_cast<SlipPanel *>(object)->_horiz = horiz;
	}
	
	static double getVert(void *object)
	{
		return static_cast<SlipPanel *>(object)->_vert;
	}
	
	static void setVert(void *object, double vert)
	{
		static_cast<SlipPanel *>(object)->_vert = vert;
	}

	void setZ(double metres);
	void updateTmpPanelValues();
	void updateVertices();
	void createVertices();

	std::vector<SlipPanel *> split(struct detector *det);

	void acceptNudges(SlipPanel *parent = NULL);
	void nudgePanels(SlipPanel *parent = NULL);
	
	void getPeaksFromImage(struct image *im);
	
	void updatePowder(Curve *c, bool refresh = true);
	void updateTarget(Curve *c, bool refresh = true);
	void prepareTarget(bool refresh);

	static double getIntraScore(void *object)
	{
		return static_cast<SlipPanel *>(object)->intraScore();
	}

	static double getInterScore(void *object)
	{
		return static_cast<SlipPanel *>(object)->interScore();
	}

protected:
	struct imagefeature *findClosestPeak(struct image *im,
	                                     struct panel *p,
	                                     double fs, double ss);
	vec3 rayTraceToPanel(struct panel *p, vec3 dir);
	bool isValidPanelMember(struct panel *p);
	double intraScore();
	double interScore();
	void updatePeaks();
	void updatePairs();

	struct panel *panelPtr()
	{
		return _panel;
	}
private:
	void makePanelBackup();
	void restoreFromBackup();
	void nudgePanel(SlipPanel *parent);
	void initialise();
	vec3 centroid();

	vec3 _corner;      /* in mm */
	vec3 _fs;          /* unit vector fast axis */
	vec3 _ss;          /* unit vector slow axis */
	vec3 _centre;      /* x,y in pixels, z in metres */
	double _width;     /* in pixels, fs multiplier */
	double _height;    /* in pixels, ss multiplier */
	
	double _alpha;
	double _beta;
	double _gamma;

	double _radius;
	double _horiz;
	double _vert;
	
	Overview *_overview;
	Curve *_target;

	bool _isSelected;
	bool _single;
	static size_t _maxImages;
	static double _minIntensity;
	struct panel *_panel;
	struct panel *_backup;
	std::vector<SlipPanel *> _subpanels;
	std::vector<double> _xs, _ys;

	std::vector<struct imagefeature> _peaks;
	std::vector<struct image *> _images;
	std::vector<RefPeak> _pairs;
	std::vector<size_t> _imageStarts;
};

#endif
