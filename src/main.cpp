// Slip n Slide
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

#include <crystfel/detector.h>
#include <crystfel/stream.h>
#include "DetectorView.h"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
	std::cout << "Qt version: " << qVersion() << std::endl;

	QApplication app(argc, argv);
	setlocale(LC_NUMERIC, "C");
	srand(time(NULL));

	/* load geometry file */
	std::string geomstr = "ginn5.geom";
	struct detector *det = get_detector_geometry(geomstr.c_str(), NULL);

	/* show det*/
	DetectorView detView;
	detView.setDetector(det);
	detView.show();

	int status = app.exec();
	
	return status;

	std::cout << "Hi" << std::endl;
	
	Stream *stream = open_stream_for_read("ginn5_mosflm.stream");

	struct image im;
	im.det = det;
	
	std::cout << "Reading ... " << stream << std::endl;
	if (read_chunk(stream, &im))
	{
		std::cout << "Failure to read" << std::endl;
	}
	
	std::cout << "Done" << std::endl;
	std::cout << "Image: " << im.filename << std::endl;
	
	free(im.filename);
	
	close_stream(stream);

	return 0;
}
