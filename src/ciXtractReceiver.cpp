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


bool sortFeatures( FeatureDataRef a, FeatureDataRef b ) { return a->getName() < b->getName(); }


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


void ciXtractReceiver::update()
{
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        for( auto i=0; i < mFeatures[k]->getSize(); i++ )
        {
            if ( mFeatures[k]->getDamping() == 0.0f )
                continue;
            
            mFeatures[k]->getData().get()[i] *= mFeatures[k]->getDamping();
        }
    }
}


void ciXtractReceiver::receiveData()
{
    mRunReceiveData = true;
    
    FeatureDataRef          feature;
    string                  name;
    float                   val;
    std::shared_ptr<float>  data;

    while( mRunReceiveData )
    {

        while( mOscListener.hasWaitingMessages() )
        {
            osc::Message message;
            mOscListener.getNextMessage( &message );
            
            name = message.getAddress();
            boost::replace_all( name, "/", "" );
            
            feature = getFeatureData( name );
            
            if ( feature->getSize() != message.getNumArgs() )
                feature->setSize( message.getNumArgs() );
            
            data    = feature->getData();
            
            for (int i = 0; i < message.getNumArgs(); i++)
            {
                // clamp min-max range
                // val = feature->getOffset() + feature->getGain() * ( message.getArgAsFloat(i) - feature->getMin() ) / ( feature->getMax() - feature->getMin() );
                val = ( message.getArgAsFloat(i) - feature->getMin() ) / ( feature->getMax() - feature->getMin() );
                
                if ( feature->isLog() )
                    val = 10 * log( 1.0f + val );

                val = feature->getOffset() + feature->getGain() * val;
                val = math<float>::clamp( val, 0.0f, 1.0f );
                
                if ( feature->getDamping() == 0.0f || val > data.get()[i] )
                    data.get()[i] = val;
            }
        }
        // std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
    }
}


FeatureDataRef ciXtractReceiver::getFeatureData( string name )
{
    for( auto k=0; k < mFeatures.size(); k++ )
        if ( mFeatures[k]->getName() == name )
            return mFeatures[k];
    
    // If the feature doesn't exists, create one with size 0
    FeatureDataRef feature = FeatureData::create( name, 0 );
    mFeatures.push_back( feature );
    
    std::sort( mFeatures.begin(), mFeatures.end(), sortFeatures );
    
    return feature;
}


XmlTree ciXtractReceiver::getSettingsXml()
{
    XmlTree doc("XtractReceiver", "" );
    doc.setAttribute( "oscInPort", mPort );
    
    for( auto k=0; k < mFeatures.size(); k++ )
    {
        XmlTree node( "feature", "" );
        node.setAttribute( "enum",      mFeatures[k]->getName() );
        node.setAttribute( "gain",      mFeatures[k]->getGain() );
        node.setAttribute( "offset",    mFeatures[k]->getOffset() );
        node.setAttribute( "min",       mFeatures[k]->getMin() );
        node.setAttribute( "max",       mFeatures[k]->getMax() );
        node.setAttribute( "damping",   mFeatures[k]->getDamping() );
        node.setAttribute( "isLog",     mFeatures[k]->isLog() );
        doc.push_back( node );
    }
    
    return doc;
}


void ciXtractReceiver::loadSettingsXml( XmlTree doc )
{
    string          enumStr;
    FeatureDataRef  fd;

    int port = doc.getAttributeValue<int>( "oscInPort" );
 
    if ( mPort != port )
    {
        mPort = port;
        mOscListener.setup( mPort );
    }
    
    for( XmlTree::Iter nodeIt = doc.find("feature"); nodeIt != doc.end(); ++nodeIt )
    {
        enumStr     = nodeIt->getAttributeValue<string>("enum");
        fd          = getFeatureData( enumStr );
        
        fd->setGain( nodeIt->getAttributeValue<float>("gain") );
        fd->setOffset( nodeIt->getAttributeValue<float>("offset") );
        fd->setMin( nodeIt->getAttributeValue<float>("min") );
        fd->setMax( nodeIt->getAttributeValue<float>("max") );
        fd->setDamping( nodeIt->getAttributeValue<float>("damping") );
        fd->setLog( nodeIt->getAttributeValue<bool>("isLog") );
    }
}


