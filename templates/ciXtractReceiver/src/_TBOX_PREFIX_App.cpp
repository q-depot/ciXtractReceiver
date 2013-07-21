
#include "cinder/app/AppNative.h"

#include "ciXtractReceiver.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class _TBOX_PREFIX_App : public AppBasic {

public:
	
	void prepareSettings( Settings *settings );
	
	void setup();
	
	void update();
	void draw();
	
    ciXtractReceiverRef                 mXtractReceiver;

    FeatureDataRef 						mSpectrumData;
};


void _TBOX_PREFIX_App::prepareSettings(Settings *settings)
{
	settings->setTitle("ciXtract OSC Receiver");
	settings->setWindowSize( 1280, 720 );
}


void _TBOX_PREFIX_App::setup()
{
    mXtractReceiver = ciXtractReceiver::create();
	
	mSpectrumData = mXtractReceiver->getFeatureData( "spectrum" );
}


void _TBOX_PREFIX_App::update()
{
	
}


void _TBOX_PREFIX_App::draw()
{
	gl::clear( Color::gray( 0.85f ) );
	

}



CINDER_APP_BASIC( _TBOX_PREFIX_App, RendererGl )

