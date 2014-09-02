#pragma once

#include "ofxGemcutterShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterMask : public ofxGemcutterShape {
	
public:
    
    ofxGemcutterMask();
    ~ofxGemcutterMask();
    
protected:
    virtual void createDefaultShape();
    virtual void render();

};
