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

#include "Refine.h"
#include "SlipPanel.h"
#include <RefinementNelderMead.h>

Refine::Refine()
{
	_intra = false;
	_p = NULL;

}

void Refine::setPanel(SlipPanel *p, bool intra)
{
	_p = p;
	_intra = intra;
}

void Refine::refine()
{
	if (_intra)
	{
		refineIntra();
	}
	else
	{
		refineInter();
	}
}

void Refine::refineInter()
{
	RefinementNelderMead *nm = new RefinementNelderMead();
	nm->setEvaluationFunction(SlipPanel::getInterScore, _p);
	nm->addParameter(_p, SlipPanel::getHoriz,
	                 SlipPanel::setHoriz, 0.001, 0.000005);
	nm->addParameter(_p, SlipPanel::getVert,
	                 SlipPanel::setVert, 0.001, 0.000005);
//	nm->addParameter(_p, SlipPanel::getGamma,
//	                 SlipPanel::setGamma, 0.001, 0.000005);
	nm->setCycles(40);
	nm->refine();
	
	emit resultReady();
}

void Refine::refineIntra()
{
	RefinementNelderMead *nm = new RefinementNelderMead();
	nm->setEvaluationFunction(SlipPanel::getIntraScore, _p);
	nm->addParameter(_p, SlipPanel::getRadius,
	                 SlipPanel::setRadius, 0.0002, 0.000001);
	nm->addParameter(_p, SlipPanel::getAlpha,
	                 SlipPanel::setAlpha, 0.0005, 0.000001);
	nm->addParameter(_p, SlipPanel::getBeta,
	                 SlipPanel::setBeta, 0.0005, 0.000001);
	nm->setCycles(40);
	nm->refine();
	
	emit resultReady();
}

