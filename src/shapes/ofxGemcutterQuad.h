#pragma once

#include "ofxGemcutterShape.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterQuad : public ofxGemcutterShape {
	
public:
    
    ofxGemcutterQuad();
    ~ofxGemcutterQuad();
    
protected:
    virtual void createDefaultShape();
    virtual void render();
    virtual void calcHomography();
};
