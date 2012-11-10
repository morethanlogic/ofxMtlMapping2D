#pragma once

#include "ofxMtlMapping2DShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DTriangle : public ofxMtlMapping2DShape {
	
    public:
        
        ofxMtlMapping2DTriangle();
        ~ofxMtlMapping2DTriangle();
        
    protected:
        virtual void createDefaultShape();
        virtual void render();

};
