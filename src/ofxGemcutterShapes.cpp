#include "ofxGemcutterShapes.h"
#include "ofxGemcutterShapeType.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
list<ofxGemcutterShape*> ofxGemcutterShapes::pmShapes;

//--------------------------------------------------------------
ofxGemcutterShape* ofxGemcutterShapes::shapeWithId(int shapeId)
{
    list<ofxGemcutterShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        if(shape->shapeId == shapeId) {
            return shape;
        }
    }
    
    return NULL;
}

// TODO: Generate and fill those two vector only 'once' when we start the application or quit the Edit Mode
//--------------------------------------------------------------
vector<ofPolyline*> ofxGemcutterShapes::getShapesOutputPolylineWithType(int shapeType)
{
    vector<ofPolyline*> polylinesToReturn;
    
    list<ofxGemcutterShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        if(shape->shapeType == shapeType) {
            polylinesToReturn.push_back(shape->polyline);
        }
    }

    return polylinesToReturn;
}

//--------------------------------------------------------------
vector<ofPolyline*> ofxGemcutterShapes::getShapesInputPolyline()
{
    vector<ofPolyline*> polylinesToReturn;
    
    list<ofxGemcutterShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        if(shape->inputPolygon->polyline) {
            polylinesToReturn.push_back(shape->inputPolygon->polyline);
        }
    }
    
    return polylinesToReturn;
}


