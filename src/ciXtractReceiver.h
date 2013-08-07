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
#include "cinder/Xml.h"
#include "OscListener.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ciXtractReceiver;
typedef std::shared_ptr<ciXtractReceiver>   ciXtractReceiverRef;


class FeatureData;
typedef std::shared_ptr<FeatureData>    FeatureDataRef;

class FeatureData {
  
public:
    
    static FeatureDataRef create( std::string name, size_t size )
    {
        return FeatureDataRef( new FeatureData( name, size ) );
    };
    
    std::shared_ptr<float>  getData() { return mData; }
    
    size_t getSize() { return mDataSize; }
    
    void setSize( size_t n )
    {
        mDataSize   = n;
        mData       = std::shared_ptr<float>( new float[n] );
    }
    
    std::string getName() { return mName; }
    
    float getGain()     { return mGain; }
    float getOffset()   { return mOffset; }
    float getMin()      { return mMin; }
    float getMax()      { return mMax; }
    float getDamping()  { return mDamping; }
    
    void setGain( float val )       { mGain     = val; }
    void setOffset( float val )     { mOffset   = val; }
    void setMin( float val )        { mMin      = val; }
    void setMax( float val )        { mMax      = val; }
    void setDamping( float val )    { mDamping  = val; }

    float getDataValue( size_t binN )
    {
        if ( mDataSize == 0 )
            return 0.0f;
        
        binN = binN % mDataSize;
        return mData.get()[binN];
    }
    
    
private:
    
    FeatureData( std::string name, size_t size )
    {
        mName       = name;
        mData       = std::shared_ptr<float>( new float[size] );
        mDataSize   = size;
        
        mGain       = 1.0f;
        mOffset     = 0.0f;
        mMin        = 0.0f;
        mMax        = 1.0f;
        mDamping    = 0.0f;
    }
    
    
private:
    
    std::string             mName;
    std::shared_ptr<float>  mData;
    size_t                  mDataSize;
    float                   mGain;
    float                   mOffset;
    float                   mMin;
    float                   mMax;
    float                   mDamping;
};


class ciXtractReceiver {
    
public:
    
    static ciXtractReceiverRef create( uint32_t port = 8000 ) { return ciXtractReceiverRef( new ciXtractReceiver( port ) ); }
    
    ~ciXtractReceiver();
    
    FeatureDataRef getFeatureData( std::string name );
    
    std::vector<FeatureDataRef> getFeatures() { return mFeatures; }

    ci::XmlTree getSettingsXml();

    void        loadSettingsXml( ci::XmlTree doc );
    
    
private:
    
    ciXtractReceiver( uint32_t port );
    
    void receiveData();
    
    
private:
    
    std::vector<FeatureDataRef>     mFeatures;
    
	osc::Listener                   mOscListener;
    uint32_t                        mPort;
    std::thread                     mReceiveDataThread;
    bool                            mRunReceiveData;
    
};

#endif
