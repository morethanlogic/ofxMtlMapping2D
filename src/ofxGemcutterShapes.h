#pragma once

#include "ofMain.h"

#include "ofxGemcutterShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
struct Comparator {
    bool operator()(ofxGemcutterShape* first, ofxGemcutterShape* second) const {
        return first->shapeId > second->shapeId;
    }
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterShapes {
    
public:

    static list<ofxGemcutterShape*> pmShapes;
    static ofxGemcutterShape* shapeWithId(int shapeId);
    static vector<ofPolyline*> getShapesOutputPolylineWithType(int shapeType);
    static vector<ofPolyline*> getShapesInputPolyline();



};