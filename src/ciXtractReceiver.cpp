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
    float           val;
    
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
                // clamp min-max range
                val = feature->offset + feature->gain * ( message.getArgAsFloat(i) - feature->min ) / ( feature->max - feature->min );
                val = math<float>::clamp( val, 0.0f, 1.0f );
            
                // Damping
                if ( feature->damping > 0.0f && val < feature->data.get()[i] )
                    val = feature->data.get()[i] * feature->damping;

                feature->data.get()[i] = val;
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
    
    FeatureDataRef feature = FeatureDataRef( new FeatureData( { name, std::shared_ptr<float>(), 0, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f } ) );
    mFeatures.push_back( feature );
    
    return feature;
}


XmlTree ciXtractReceiver::getSettingsXml()
{
    XmlTree doc("XtractReceiver", "" );
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        XmlTree node( "feature", "" );
        node.setAttribute( "enum",      mFeatures[k]->name );
        node.setAttribute( "value",     mFeatures[k]->gain );
        node.setAttribute( "min",       mFeatures[k]->min );
        node.setAttribute( "max",       mFeatures[k]->max );
        node.setAttribute( "damping",   mFeatures[k]->damping );
        doc.push_back( node );
    }
    
    return doc;
}

void ciXtractReceiver::loadSettingsXml( XmlTree doc )
{
    string          enumStr;
    FeatureDataRef  fd;
    
    for( XmlTree::Iter nodeIt = doc.find("feature"); nodeIt != doc.end(); ++nodeIt )
    {
        enumStr     = nodeIt->getAttributeValue<string>("enum");

        fd          = getFeatureData( enumStr );
        
        fd->gain    = nodeIt->getAttributeValue<float>("gain");
        fd->offset  = nodeIt->getAttributeValue<float>("offset");
        fd->min     = nodeIt->getAttributeValue<float>("min");
        fd->max     = nodeIt->getAttributeValue<float>("max");
        fd->damping = nodeIt->getAttributeValue<float>("damping");
    }
}

