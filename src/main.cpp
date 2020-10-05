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
#include "Overview.h"
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
	
	Overview o;
	o.loadDetector(det);

	Stream *stream = open_stream_for_read("ginn5_mosflm.stream");
	o.loadStream(stream);

	close_stream(stream);

	int status = app.exec();
	free_detector_geometry(det);

	return status;

	/* show det*/
	/*
	DetectorView detView;
	detView.setDetector(det);
	detView.show();

	std::cout << "Hi" << std::endl;
	
	Stream *stream = open_stream_for_read("ginn5_mosflm.stream");

	std::vector<struct image> images;
	struct image im;
	im.det = det;
	
	std::cout << "Reading ... " << stream << std::endl;
	while (!read_chunk(stream, &im))
	{
		int count = image_feature_count(im.features);
		std::cout << "No. crystals: " << im.n_crystals << " ";
		std::cout << " and peaks: " << count << std::endl;
		images.push_back(im);
		
		detView.imageToPanels(&im);
	}

	std::cout << "Failure to read more" << std::endl;
	
	free(im.filename);
	close_stream(stream);

	int status = app.exec();
	
	free_detector_geometry(det);
	
	return 0;
	*/
}
