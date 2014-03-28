/*
 *  ciXtractReceiver.cpp
 *
 *  Created by Andrea Cuius
 *  Nocte Studio Ltd. Copyright 2013 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#include "cinder/app/App.h"
#include <boost/algorithm/string.hpp> 
#include "ciXtractReceiver.h"

using namespace ci;
using namespace ci::app;
using namespace std;


bool sortFeatures( FeatureDataRef a, FeatureDataRef b ) { return a->getName() < b->getName(); }


ciXtractReceiver::ciXtractReceiver( uint32_t osc_in_port ) : mOscInPort(osc_in_port)
{
    try
    {
        mOscListener.setup( mOscInPort );
        
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
    std::shared_ptr<float>  data, rawData;
    float                   val;
    size_t                  valuesN;
    
    for( size_t k=0; k < mFeatures.size(); k++ )
    {
		mFeatures[k]->mMutex.lock();
		
		if ( mFeatures[k]->getSize() == 0 )
		{
			mFeatures[k]->mMutex.unlock();
			continue;
		}

        valuesN = mFeatures[k]->getSize(); 
        data    = mFeatures[k]->getData();
        rawData = mFeatures[k]->getRawData();

        for( size_t i=0; i < valuesN; i++ )
        {
			if ( !rawData )
				continue;

            // clamp min-max range
            val = ( rawData.get()[i] - mFeatures[k]->getMin() ) / ( mFeatures[k]->getMax() - mFeatures[k]->getMin() );
            
            if ( mFeatures[k]->isLog() )
                val = min( (float)(i + 25) / (float)valuesN, 1.0f ) * 100 * log10( 1.0f + val );
            
            val = mFeatures[k]->getOffset() + mFeatures[k]->getGain() * val;
            
            val = math<float>::clamp( val, 0.0f, 1.0f );
            
            if ( mFeatures[k]->getDamping() > 0.0f )
            {
                if (  val >= data.get()[i] )
                    data.get()[i] = val;
                else
                    data.get()[i] *= mFeatures[k]->getDamping();
            }
            else
                data.get()[i] = val;
        }

		mFeatures[k]->mMutex.unlock();
    }
}


void ciXtractReceiver::receiveData()
{
    mRunReceiveData = true;
    
    FeatureDataRef          feature;
    string                  name;
    std::shared_ptr<float>  rawData;
    int                     argsN;
    
    while( mRunReceiveData )
    {
        while( mOscListener.hasWaitingMessages() )
        {
            osc::Message message;
            mOscListener.getNextMessage( &message );
            
            name = message.getAddress();
            boost::replace_all( name, "/", "" );
            
            feature = getFeatureData( name );
			
			feature->mMutex.lock();

            if ( feature->getSize() != message.getNumArgs() )
                feature->setSize( message.getNumArgs() );
            
            rawData = feature->getRawData();
            argsN   = message.getNumArgs();
            
            for (int i = 0; i < argsN; i++)
                rawData.get()[i] = message.getArgAsFloat(i);

			feature->mMutex.unlock();
		}
        // std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
    }
}


FeatureDataRef ciXtractReceiver::getFeatureData( string name )
{
    for( size_t k=0; k < mFeatures.size(); k++ )
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
    XmlTree doc("XtractSettings", "" );
    doc.setAttribute( "oscInPort", mOscInPort );
    
    for( size_t k=0; k < mFeatures.size(); k++ )
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
 
    if ( mOscInPort != port )
    {
        mOscInPort = port;
        mOscListener.setup( mOscInPort );
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



void ciXtractReceiver::drawData( FeatureDataRef feature, Rectf rect, ColorA plotCol, ColorA bgCol )
{
	if ( !feature )
		return;

    glPushMatrix();
    
    gl::drawString( feature->getName(), rect.getUpperLeft(), plotCol );
    
    rect.y1 += 10;
    
    gl::color( bgCol );
    gl::drawSolidRect( rect );
    
    gl::translate( rect.getUpperLeft() );
    
    std::shared_ptr<float>	data  = feature->getData();
    int                     dataN = feature->getSize();
    float                   min   = feature->getMin();
    float                   max   = feature->getMax();
    float                   step  = rect.getWidth() / dataN;
    float                   h     = rect.getHeight();
    float                   val, barY;
    
    gl::color( plotCol );
    
    glBegin( GL_QUADS );
    
    for( int i = 0; i < dataN; i++ )
    {
        val     = ( data.get()[i] - min ) / ( max - min );
        val     = math<float>::clamp( val, 0.0f, 1.0f );
        barY    = h * val;
        
        glVertex2f( i * step,           h );
        glVertex2f( ( i + 1 ) * step,   h );
        glVertex2f( ( i + 1 ) * step,   h-barY );
        glVertex2f( i * step,           h-barY );
    }
    
    glEnd();

    gl::popMatrices();
}
