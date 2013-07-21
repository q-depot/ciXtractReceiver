/*
 *  ciXtractReceiver.h
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef CI_XTRACT_RECEIVER
#define CI_XTRACT_RECEIVER

#pragma once

#include "cinder/Thread.h"
#include "OscListener.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ciXtractReceiver;
typedef std::shared_ptr<ciXtractReceiver>   ciXtractReceiverRef;

class ciXtractFeature;
typedef std::shared_ptr<ciXtractFeature>   ciXtractFeatureRef;


struct FeatureData {
    std::string             name;
    std::shared_ptr<float>  data;
    int                     size;
};

typedef std::shared_ptr<FeatureData>    FeatureDataRef;


class ciXtractReceiver {
    
public:
    
    static ciXtractReceiverRef create( uint32_t port = 8000 ) { return ciXtractReceiverRef( new ciXtractReceiver( port ) ); }
    
    ~ciXtractReceiver();
    
    FeatureDataRef getFeatureData( std::string name );
    
//    void enableFeature( xtract_features_ feature );
//    
//    void disableFeature( xtract_features_ feature );
//    
//    void toggleFeature( xtract_features_ feature );
//    
//    std::vector<ciXtractFeatureRef> getFeatures() { return mFeatures; };
//    
//    ciXtractFeatureRef getFeature( xtract_features_ feature );


private:
    
    ciXtractReceiver( uint32_t port );
    
    void receiveData();
    
    
private:
    
    
    std::vector<FeatureDataRef>    mFeatures;
    
	osc::Listener 	mOscListener;
    uint32_t        mPort;
    std::thread     mReceiveDataThread;
    bool            mRunReceiveData;
    
};

#endif
