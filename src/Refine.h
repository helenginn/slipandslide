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

#ifndef __slipnslide__Refine__
#define __slipnslide__Refine__

#include <QObject>

class SlipPanel;
class DetectorView;

class Refine : public QObject
{
Q_OBJECT
public:
	Refine();

	void setDetectorView(DetectorView *v)
	{
		_view = v;
	}

	void setPanel(SlipPanel *p, bool intra);
signals:
	void resultReady();
public slots:
	void refine();
private:
	void refineIntra();
	void refineInter();

	bool _intra;
	SlipPanel *_p;
	DetectorView *_view;
};

#endif
