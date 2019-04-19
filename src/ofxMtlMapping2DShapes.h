#pragma once

#include "ofMain.h"

#include "ofxMtlMapping2DShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
struct Comparator {
    bool operator()(ofxMtlMapping2DShape* first, ofxMtlMapping2DShape* second) const {
        return first->shapeId > second->shapeId;
    }
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DShapes {
    
    public:
    
        static list<ofxMtlMapping2DShape*> pmShapes;
        static ofxMtlMapping2DShape* shapeWithId(int shapeId);
        static vector<ofPolyline*> getShapesOutputPolylineWithType(int shapeType);
        static vector<ofPolyline*> getShapesInputPolyline();



};