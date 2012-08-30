/*!
 * \file MS_Blueball_Decide.cpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#include <memory>
#include <string>

#include "MS_Blueball_Decide.hpp"
#include "Logger.hpp"

#include "Types/Ellipse.hpp"

namespace Processors {
namespace MS_Blueball {

// OpenCV writes hue in range 0..180 instead of 0..360
#define H(x) (x>>1)

MS_Blueball_Decide::MS_Blueball_Decide(const std::string & name) : Base::Component(name)
{
	LOG(LTRACE) << "Hello MS_Blueball_Decide\n";
	blobs_ready = hue_ready = false;
}

MS_Blueball_Decide::~MS_Blueball_Decide()
{
	LOG(LTRACE) << "Good bye MS_Blueball_Decide\n";
}

bool MS_Blueball_Decide::onInit()
{
	LOG(LTRACE) << "MS_Blueball_Decide::initialize\n";

	h_onNewImage.setup(this, &MS_Blueball_Decide::onNewImage);
	registerHandler("onNewImage", &h_onNewImage);

	h_onNewBlobs.setup(this, &MS_Blueball_Decide::onNewBlobs);
	registerHandler("onNewBlobs", &h_onNewBlobs);

	h_onNewCameraInfo.setup(this, &MS_Blueball_Decide::onNewCameraInfo);
	registerHandler("onNewCameraInfo", &h_onNewCameraInfo);

	// Register input streams.
	registerStream("in_blobs", &in_blobs);
	registerStream("in_hue", &in_hue);
	registerStream("in_cameraInfo", &in_cameraInfo);

	found = registerEvent("Found");
	notFound = registerEvent("NotFound");
	newImage = registerEvent("newImage");

	// Register output streams.
	registerStream("out_balls", &out_balls);
	registerStream("out_imagePosition", &out_imagePosition);

	return true;
}

bool MS_Blueball_Decide::onFinish()
{
	LOG(LTRACE) << "MS_Blueball_Decide::finish\n";

	return true;
}

bool MS_Blueball_Decide::onStep()
{
	LOG(LTRACE) << "MS_Blueball_Decide::step\n";

	blobs_ready = hue_ready = false;

	try {
		int id = 0;
		int i;
		Types::Blobs::Blob currentBlob;
		Types::DrawableContainer Blueballs;
		// Check whether there is any blue blob detected.

		if (blobs.GetNumBlobs() <0) {
			LOG(LTRACE) << "Blue blob not found.\n";

			// Disregarding the fact - write output stream.
			out_balls.write(Blueballs);
			// Raise events.
			notFound->raise();
			newImage->raise();
			return true;
		}

		blobs.GetNthBlob(Types::Blobs::BlobGetArea(), 0, currentBlob);


		// get blob bounding rectangle and ellipse
		CvBox2D r2 = currentBlob.GetEllipse();

		std::cout << "Center: " << r2.center.x << "," << r2.center.y << "\n";
		++id;

		Types::Ellipse* tmpball = new Types::Ellipse(Point(r2.center.x, r2.center.y), Size(r2.size.width, r2.size.height), r2.angle);

		// Add to list.
		Blueballs.add(tmpball);

		// Write blueball list to stream.
		out_balls.write(Blueballs);



		Types::ImagePosition imagePosition;
		double maxPixels = std::max(cameraInfo.size().width, cameraInfo.size().height);
		// Change coordinate system hence it will return coordinates from (-1,1), center is 0.
		imagePosition.elements[0] = (r2.center.x - cameraInfo.size().width / 2) / maxPixels;
		imagePosition.elements[1] = (r2.center.y - cameraInfo.size().height / 2) / maxPixels;
		// Size-related coordinate.
		imagePosition.elements[2] = 0;
		// Rotation - in case of blueball - zero.
		imagePosition.elements[3] = 0;

		// Write to stream.
		out_imagePosition.write(imagePosition);
		// Raise events.
		found->raise();
		newImage->raise();

		return true;
	} catch (...) {
		LOG(LERROR) << "MS_Blueball_Decide::onNewImage failed\n";
		return false;
	}
}

bool MS_Blueball_Decide::onStop()
{
	return true;
}

bool MS_Blueball_Decide::onStart()
{
	return true;
}

void MS_Blueball_Decide::onNewImage()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewImage\n";

	hue_ready = true;
	hue_img = in_hue.read();
	hue_img = hue_img.clone();
	if (blobs_ready && hue_ready)
		onStep();
}

void MS_Blueball_Decide::onNewBlobs()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewBlobs\n";

	blobs = in_blobs.read();
	onStep();
}

void MS_Blueball_Decide::onNewCameraInfo()
{
	LOG(LTRACE) << "MS_Blueball_Decide::onNewCameraInfo\n";

	cameraInfo = in_cameraInfo.read();
	onStep();
}


}//: namespace MS_Blueball
}//: namespace Processors
