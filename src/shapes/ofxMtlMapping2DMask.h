#pragma once

#include "ofxMtlMapping2DShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DMask : public ofxMtlMapping2DShape {
	
    public:
        
        ofxMtlMapping2DMask();
        ~ofxMtlMapping2DMask();
        
    protected:
        virtual void createDefaultShape();
        virtual void render();

};
