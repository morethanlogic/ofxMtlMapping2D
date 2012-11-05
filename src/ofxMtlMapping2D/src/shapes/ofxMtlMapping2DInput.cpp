#include "ofxMtlMapping2DInput.h"
#include "homography.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DInput::ofxMtlMapping2DInput()
{
}

//--------------------------------------------------------------
ofxMtlMapping2DInput::~ofxMtlMapping2DInput()
{
}

#pragma mark -
#pragma mark Homography

//--------------------------------------------------------------
void ofxMtlMapping2DInput::calcHomography(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3)
{
    ofPoint src[4];
    ofPoint dst[4];
    
    list<ofxMtlMapping2DVertex*>::iterator it;
    it=vertices.begin();
    ofxMtlMapping2DVertex* vertex = *it;
    src[0].set(vertex->x - (boundingBox.x -15), vertex->y - (boundingBox.y -15), 0);
    
    it++;
    vertex = *it;
    src[1].set(vertex->x - (boundingBox.x -15), vertex->y - (boundingBox.y -15), 0);
    
    it++;
    vertex = *it;
    src[2].set(vertex->x - (boundingBox.x -15), vertex->y - (boundingBox.y -15), 0);
    
    it++;
    vertex = *it;
    src[3].set(vertex->x - (boundingBox.x -15), vertex->y - (boundingBox.y -15), 0);

    dst[0].set(x0, y0, 0);
    dst[1].set(x1, y1, 0);
    dst[2].set(x2, y2, 0);        
    dst[3].set(x3, y3, 0);
    
    findHomography(src, dst, homoMatrix);
}



