#include "ofxMtlMapping2DShapes.h"

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


