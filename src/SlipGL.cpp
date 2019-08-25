// SlipGL
// Copyright (C) 2017-2018 Helen Ginn
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

#include "SlipGL.h"
#include "SlipPanel.h"
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWindow>
#include <QTimer>
#include <iostream>

void SlipGL::initializeGL()
{
	initializeOpenGLFunctions();

	glClearColor(1.0, 1.0, 1.0, 1.0);

//	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initialisePrograms();
}

SlipGL::SlipGL(QWidget *p, struct detector *d) : QOpenGLWidget(p)
{
	/*
	_timer = new QTimer();
	_timer->setInterval(1);
	connect(_timer, &QTimer::timeout, this, &SlipGL::update);
	*/
	
	_d = d;
	
	const double scale = 0.0008;
	_model = make_mat3x3();
	mat3x3_scale(&_model, scale, scale, scale / 4);
}

void SlipGL::addPanel(struct panel &p)
{
	SlipPanel *spanel = new SlipPanel(_d, p);
	_panels.push_back(spanel);
}

void SlipGL::preparePanels(int n)
{
	_panels.reserve(n);
}

void SlipGL::initialisePrograms()
{
	for (unsigned int i = 0; i < _panels.size(); i++)
	{
		_panels[i]->initialisePrograms();
	}
}

void SlipGL::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (unsigned int i = 0; i < _panels.size(); i++)
	{
		_panels[i]->initialisePrograms();
		_panels[i]->render(this);
	}
}
