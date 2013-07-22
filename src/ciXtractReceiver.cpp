/*
 *  ciXtractReceiver.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


using namespace ci;
using namespace ci::app;
using namespace std;


#include "ciXtractReceiver.h"
#include <boost/algorithm/string.hpp> 


ciXtractReceiver::ciXtractReceiver( uint32_t port ) : mPort(port)
{    
    try
    {
        mOscListener.setup( mPort );
        
        mReceiveDataThread = std::thread( &ciXtractReceiver::receiveData, this );
    }
    catch ( ... )
    {
        console() << "ciXtractReceiver osc listener NOT initialised!" << endl;
    }
}


ciXtractReceiver::~ciXtractReceiver()
{
    mRunReceiveData = false;
    
    if ( mReceiveDataThread.joinable() )
        mReceiveDataThread.join();
    
    mOscListener.shutdown();
}


void ciXtractReceiver::receiveData()
{
    mRunReceiveData = true;
    
    FeatureDataRef  feature;
    string          name;
    
    while( mRunReceiveData )
    {

        while( mOscListener.hasWaitingMessages() )
        {
            osc::Message message;
            mOscListener.getNextMessage( &message );
        
            name = message.getAddress();
            boost::replace_all( name, "/", "" );
            
            feature = getFeatureData( name );

            if ( feature->size != message.getNumArgs() )
            {
                feature->size = message.getNumArgs();
                feature->data = std::shared_ptr<float>( new float[feature->size] );
            }
            
            
            for (int i = 0; i < message.getNumArgs(); i++)
            {
                feature->data.get()[i] = message.getArgAsFloat(i);
            }
        }
        
//        std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
    }
}


FeatureDataRef ciXtractReceiver::getFeatureData( string name )
{
    for( auto k=0; k < mFeatures.size(); k++ )
        if ( mFeatures[k]->name == name )
            return mFeatures[k];
    
    FeatureDataRef feature = FeatureDataRef( new FeatureData( { name, std::shared_ptr<float>(), 0 } ) );
    mFeatures.push_back( feature );
    
    return feature;
}

