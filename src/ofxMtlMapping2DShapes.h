#pragma once

#include "ofMain.h"

#include "ofxMtlMapping2DShape.h"

//--------------------------------------------------------------
class ofxMtlMapping2DShapes {
    
    public:
    
        static list<ofxMtlMapping2DShape*> pmShapes;
        static ofxMtlMapping2DShape* shapeWithId(int shapeId);


};