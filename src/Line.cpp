// slipnslide
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

#include "Line.h"
#include "vec_utils.h"
#include "shaders/fImage.h"
#include "shaders/vari_z.h"

Line::Line(vec3 start, vec3 end)
{
	_start = start;
	_end = end;
	updateVertices();
	
	_fString = fImage();
	_vString = variable_z_vsh();
	_renderType = GL_LINES;
}

void Line::updateVertices()
{
	_vertices.clear();
	_indices.clear();
	
	_indices.push_back(0);
	_indices.push_back(1);


	Vertex v;
	memset(v.pos, 0, sizeof(Vertex));

	v.color[0] = 1;
	v.color[1] = 1;
	v.color[2] = 1;
	v.color[3] = 1;

	pos_from_vec(v.pos, _start);
	_vertices.push_back(v);
	pos_from_vec(v.pos, _end);
	_vertices.push_back(v);
}
