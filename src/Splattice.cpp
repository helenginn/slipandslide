// Slip n slide
// Copyright (C) 2020 Helen Ginn, Monarch Jayant
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

#include "Splattice.h"
#include <vec3.h>

Splattice::Splattice(Overview *view)
{
	_view = view;

}

void Splattice::addImage(struct image *im)
{
	ImageFeatureList *list = im->features;

	for (int i = 0; i < image_feature_count(list); i++)
	{
		struct imagefeature *peak;
		peak = image_get_feature(list, i);
		peak->parent = im;
		
		double fs = peak->fs;
		double ss = peak->ss;
		double k = 1e-10 / im->lambda; /* inverse Angs */
		panel *p = peak->p;

		/* Calculate 3D position of given position, in m */ 
		double x = (p->cnx  + fs*p->fsx + ss*p->ssx);
		x /= p->res;
		double y = (p->cny  + fs*p->fsy + ss*p->ssy);
		y /= p->res;
		double z = (fs*p->fsz + ss*p->ssz);
		z /= p->res;
		z += (p->clen + p->coffset);
		
		vec3 v = make_vec3(x, y, z);

		vec3_set_length(&v, k);

		// in inverse Angstroms
		peak->rx = v.x;
		peak->ry = v.y;
		peak->rz = v.z - k;
		
		_peaks.push_back(peak);
	}
}

void Splattice::runSplattice()
{

}
