#pragma once

#include "ofMain.h"
#include "ofxGemcutterVertex.h"
#include "ofxGemcutterPolygon.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterInput : public ofxGemcutterPolygon {
	
public:
    ofxGemcutterInput();
    ~ofxGemcutterInput();

    void update(bool isQuad);

    void calcHomography(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3);

    GLfloat homoMatrix[16];

private:
    typedef ofxGemcutterPolygon _super;
};
