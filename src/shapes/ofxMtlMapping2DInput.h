#pragma once

#include "ofMain.h"
#include "ofxMtlMapping2DVertex.h"
#include "ofxMtlMapping2DPolygon.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DInput : public ofxMtlMapping2DPolygon {
	
    public:
        ofxMtlMapping2DInput();
        ~ofxMtlMapping2DInput();
    
        void update(bool isQuad);

        void calcHomography(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

        GLfloat homoMatrix[16];
    
    private:
        typedef ofxMtlMapping2DPolygon _super;
};
