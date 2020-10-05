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

#ifndef __slipnslide__variz__
#define __slipnslide__variz__

inline std::string variable_z_vsh()
{
	std::string str =
	"attribute vec3 normal;\n"\
	"attribute vec3 position;\n"\
	"attribute vec4 color;\n"\
	"\n"\
	"varying vec4 vColor;\n"\
	"varying vec4 vPos;\n"\
	"\n"\
	"uniform mat4 projection;\n"\
	"uniform mat4 model;\n"\
	"uniform vec3 focus;\n"\
	"\n"\
	"void main()\n"\
	"{\n"\
	"   vec4 pos = vec4(position[0], position[1], position[2], 1.0);\n"\
	"   vec4 modelPos = model * pos;\n"\
	"   gl_Position = projection * modelPos;\n"\
	"	vColor = color;\n"\
	"	vPos = pos;\n"\
	"}";
	return str;
}

inline std::string variable_z_fsh()
{
	std::string str =
	"varying vec4 vColor;\n"\
	"varying vec4 vPos;\n"\
	"\n"\
	"\n"\
	"vec3 rgb2hsv(vec3 c)\n"\
	"{\n"\
	"vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"\
	"vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"\
	"vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"\
	"\n"\
	"float d = q.x - min(q.w, q.y);\n"\
	"float e = 1.0e-10;\n"\
	"return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"\
	"}\n"\
	"\n"\
	"vec3 hsv2rgb(vec3 c)\n"\
	"{\n"\
	"vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"\
	"vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"\
	"return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"\
	"}\n"\
	"\n"\
	"void main()\n"\
	"{\n"\
	"	gl_FragColor = vColor;\n"\
	"	float var = mod(-vPos[2], 1.0);\n"\
	"	vec3 c = hsv2rgb(vec3(var, 1.0, vColor[0]));\n"\
	"	gl_FragColor = vec4(c, 1.0);\n"\
	"}\n";
	return str;
}

#endif

