#ciXtractReceiver
ciXtractReceiver is a CinderBlock to receive sound data analysed with ciXtract via OSC

The counterpart app is XtractSenderOSC that comes with the ciXtract CinderBlock:

[ciXtract on Github](https://github.com/q-depot/ciXtract)

##Notes

Requires Cinder >= 0.8.5
The source code includes an Xcode example and CinderBlock templates.

##Get the code

cd CINDER_PATH/blocks

git clone git@github.com:q-depot/ciXtractReceiver.git


##How to use it
A working example can be found in the Samples folder, you can also use the TinderBox template to generate a new one.

```c++
#include "ciXtractReceiver.h"

class BasicApp : public AppNative 
{
	…
	ciXtractReceiverRef    mXtractReceiver;
}

void BasicApp::setup() 
{
	mXtract = ciXtractReceiver::create();		// create an instance
}

void BasicApp::update() 
{
	mXtract->update();
}

void BasicApp::draw() 
{
	FeatureDataRef feature;
	feature = mXtract->getFeatureData( mAvailableFeatures[k] );
	ciXtractReceiver::drawData( feature, rect );		// utility to draw the data
}

```

##About LibXtract
> LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

[LibXtract on github](https://github.com/jamiebullock/LibXtract)

[LIBXTRACT: A LIGHTWEIGHT LIBRARY FOR AUDIO FEATURE EXTRACTION](https://s3-eu-west-1.amazonaws.com/papers/LibXtract-_a_lightweight_feature_extraction_library.pdf)

##Supported features

###Vector Features
* XTRACT_SPECTRUM
* XTRACT_AUTOCORRELATION
* ~~XTRACT_AUTOCORRELATION_FFT~~ Doesn't work properly, don't use it.
* XTRACT_HARMONIC_SPECTRUM
* XTRACT_PEAK_SPECTRUM
* XTRACT_SUBBANDS
* XTRACT_MFCC
* XTRACT_BARK_COEFFICIENTS

###Scalar Features

* XTRACT_F0
* XTRACT_FAILSAFE_F0
* XTRACT_WAVELET_F0
* XTRACT_MEAN
* XTRACT_VARIANCE
* XTRACT_STANDARD_DEVIATION
* XTRACT_AVERAGE_DEVIATION
* XTRACT_SKEWNESS
* XTRACT_KURTOSIS
* XTRACT_SPECTRAL_MEAN
* XTRACT_SPECTRAL_VARIANCE
* XTRACT_SPECTRAL_STANDARD_DEVIATION
* XTRACT_SPECTRAL_SKEWNESS
* XTRACT_SPECTRAL_KURTOSIS
* XTRACT_SPECTRAL_CENTROID
* XTRACT_IRREGULARITY_K
* XTRACT_IRREGULARITY_J
* XTRACT_TRISTIMULUS_1
* XTRACT_SMOOTHNESS
* XTRACT_SPREAD
* XTRACT_ZCR
* XTRACT_ROLLOFF
* XTRACT_LOUDNESS
* XTRACT_FLATNESS
* XTRACT_FLATNESS_DB
* XTRACT_TONALITY
* XTRACT_RMS_AMPLITUDE
* XTRACT_SPECTRAL_INHARMONICITY
* XTRACT_POWER
* XTRACT_ODD_EVEN_RATIO
* XTRACT_SHARPNESS
* XTRACT_SPECTRAL_SLOPE
* XTRACT_LOWEST_VALUE
* XTRACT_HIGHEST_VALUE
* XTRACT_SUM
* XTRACT_NONZERO_COUNT
* XTRACT_CREST
