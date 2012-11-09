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
#pragma mark Lifecycle

//--------------------------------------------------------------
void ofxMtlMapping2DInput::update(bool isQuad)
{
    
    if (activePolygon == this && isQuad) {
        list<ofxMtlMapping2DVertex*>::iterator it;
        for (it=vertices.begin(); it!=vertices.end(); it++) {
            ofxMtlMapping2DVertex* vertex = *it;
            
            if(ofxMtlMapping2DVertex::activeVertex == vertex) {
                int i = distance(vertices.begin(), it);
                
                if (i == 0) {
                    getVertex(1)->snapIt(getVertex(1)->x, vertex->y);
                    getVertex(3)->snapIt(vertex->x, getVertex(3)->y);
                } else if (i == 1) {
                    getVertex(0)->snapIt(getVertex(0)->x, vertex->y);
                    getVertex(2)->snapIt(vertex->x, getVertex(2)->y);
                } else if (i == 2) {
                    getVertex(1)->snapIt(vertex->x, getVertex(1)->y);
                    getVertex(3)->snapIt(getVertex(3)->x, vertex->y);
                } else {
                    getVertex(0)->snapIt(vertex->x, getVertex(0)->y);
                    getVertex(2)->snapIt(getVertex(2)->x, vertex->y);
                }
            }
        }
    }
    
    // ----
    _super::update();

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



