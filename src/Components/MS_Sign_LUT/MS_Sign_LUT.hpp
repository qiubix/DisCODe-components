/*!
 * \file MS_Sign_LUT.hpp
 * \brief
 * \author mstefanc
 * \date 2010-07-05
 */

#ifndef MS_SIGN_LUT_HPP_
#define MS_SIGN_LUT_HPP_

#include "Component_Aux.hpp"
#include "Component.hpp"
//#include "Panel_Empty.hpp"
#include "DataStream.hpp"

#include "Property.hpp"

#include <opencv2/opencv.hpp>
#include <highgui.h>

namespace Processors {
namespace MS_Sign {

using namespace cv;

/*!
 * \class MS_Sign_LUT
 * \brief Example processor class.
 */
class MS_Sign_LUT: public Base::Component
{
public:
	/*!
	 * Constructor.
	 */
	MS_Sign_LUT(const std::string & name = "");

	/*!
	 * Destructor
	 */
	virtual ~MS_Sign_LUT();

        void prepareInterface();

protected:

	/*!
	 * Connects source to given device.
	 */
	bool onInit();

	/*!
	 * Disconnect source from device, closes streams, etc.
	 */
	bool onFinish();

	/*!
	 * Retrieves data from device.
	 */
	bool onStep();

	/*!
	 * Start component
	 */
	bool onStart();

	/*!
	 * Stop component
	 */
	bool onStop();


	/*!
	 * Event handler function.
	 */
	void onNewImage();

	/// Event handler.
	Base::EventHandler <MS_Sign_LUT> h_onNewImage;

	/// Input image
	Base::DataStreamIn <Mat> in_img;

	/// Event raised, when image is processed
    //Base::Event * newImage;

	/// Output data stream - hue part with continous red
	Base::DataStreamOut <Mat> out_hue;

	/// Output data stream - segments
	Base::DataStreamOut <Mat> out_segments;

private:
	cv::Mat hue_img;
	cv::Mat segments;

	Base::Property<int> m_sat_threshold_1;
	Base::Property<int> m_sat_threshold_2;
	Base::Property<int> m_val_threshold_1;
	Base::Property<int> m_val_threshold_2;
};

}//: namespace MS_Sign
}//: namespace Processors


/*
 * Register processor component.
 */
REGISTER_COMPONENT("MS_Sign_LUT", Processors::MS_Sign::MS_Sign_LUT)

#endif /* MS_SIGN_LUT_HPP_ */

