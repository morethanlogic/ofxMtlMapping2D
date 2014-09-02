#pragma once

#include "ofxGemcutterShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterTriangle : public ofxGemcutterShape {
	
public:
    
    ofxGemcutterTriangle();
    ~ofxGemcutterTriangle();
    
protected:
    virtual void createDefaultShape();
    virtual void render();

};
