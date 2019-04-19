#include "ofxMtlMapping2DShapes.h"
#include "ofxMtlMapping2DShapeType.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
list<ofxMtlMapping2DShape*> ofxMtlMapping2DShapes::pmShapes;

//--------------------------------------------------------------
ofxMtlMapping2DShape* ofxMtlMapping2DShapes::shapeWithId(int shapeId)
{
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->shapeId == shapeId) {
            return shape;
        }
    }
    
    return NULL;
}

// TODO: Generate and fill those two vector only 'once' when we start the application or quit the Edit Mode
//--------------------------------------------------------------
vector<ofPolyline*> ofxMtlMapping2DShapes::getShapesOutputPolylineWithType(int shapeType)
{
    vector<ofPolyline*> polylinesToReturn;
    
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->shapeType == shapeType) {
            polylinesToReturn.push_back(shape->polyline);
        }
    }

    return polylinesToReturn;
}

//--------------------------------------------------------------
vector<ofPolyline*> ofxMtlMapping2DShapes::getShapesInputPolyline()
{
    vector<ofPolyline*> polylinesToReturn;
    
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=pmShapes.begin(); it!=pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->inputPolygon->polyline) {
            polylinesToReturn.push_back(shape->inputPolygon->polyline);
        }
    }
    
    return polylinesToReturn;
}


