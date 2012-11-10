#pragma once

#include "ofxMtlMapping2DShape.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DQuad : public ofxMtlMapping2DShape {
	
    public:
        
        ofxMtlMapping2DQuad();
        ~ofxMtlMapping2DQuad();
        
    protected:
        virtual void createDefaultShape();
        virtual void render();
        virtual void calcHomography();
};
