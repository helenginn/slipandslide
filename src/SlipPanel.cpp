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

#include <iostream>
#include "SlipPanel.h"
#include "SlipObject.h"

SlipPanel::SlipPanel(struct detector *d, struct panel &p) : SlipObject()
{
	/* local panel copy */
	_p = p;
	_d = d;

	_cnz = (_p.clen + _p.coffset) * _p.res;
	setupVertices();
}

void SlipPanel::setupVertices()
{
	_vertices.clear();
	_indices.clear();
	
	_indices.push_back(0);
	_indices.push_back(1);
	_indices.push_back(2);
	_indices.push_back(2);
	_indices.push_back(1);
	_indices.push_back(3);
	
	Vertex v;
	memset(v.pos, 0, sizeof(Vertex));

	v.color[3] = 1;
	
	v.pos[0] = _p.cnx;
	v.pos[1] = _p.cny;
	v.pos[2] = _cnz;

	std::cout << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << std::endl;
	_vertices.push_back(v);
	
	/* top right */
	v.pos[0] = _p.cnx + _p.w * _p.fsx;
	v.pos[1] = _p.cny + _p.w * _p.fsy;
	v.pos[2] = _cnz  + _p.w * _p.fsz;
	std::cout << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << std::endl;
	_vertices.push_back(v);

	/* bottom left */
	v.pos[0] = _p.cnx + _p.h * _p.ssx;
	v.pos[1] = _p.cny + _p.h * _p.ssy;
	v.pos[2] = _cnz  + _p.h * _p.ssz;
	std::cout << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << std::endl;
	_vertices.push_back(v);
	
	/* bottom right */
	v.pos[0] = _p.cnx + _p.h * _p.ssx + _p.w * _p.fsx;
	v.pos[1] = _p.cny + _p.h * _p.ssy + _p.w * _p.fsy;
	v.pos[2] = _cnz  + _p.h * _p.ssz + _p.w * _p.fsz;
	std::cout << v.pos[0] << " " << v.pos[1] << " " << v.pos[2] << std::endl;
	_vertices.push_back(v);
}

