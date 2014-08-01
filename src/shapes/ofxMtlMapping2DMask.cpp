#include "ofxMtlMapping2DMask.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DMask::ofxMtlMapping2DMask()
{

}

//--------------------------------------------------------------
ofxMtlMapping2DMask::~ofxMtlMapping2DMask(){

}

//--------------------------------------------------------------
void ofxMtlMapping2DMask::render()
{
    ofBeginShape();
    ofFill();    
    for (int i = 0; i < polyline->size(); i++) {
        ofVertex(polyline->getVertices()[i].x, polyline->getVertices()[i].y);
    }
    ofEndShape(true);
}

//--------------------------------------------------------------
void ofxMtlMapping2DMask::createDefaultShape()
{
    shapeSettings["type"] = "mask";
    
    int xOffset = 0;
    int yOffset = 0;

    //Create a new vertex
    int x = ofxMtlMapping2DGlobal::outputWidth/2;
    int y = 0;
    
    ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    //Create a new vertex
    x = 0;
    y = ofxMtlMapping2DGlobal::outputHeight;
    
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    //Create a new vertex
    x = ofxMtlMapping2DGlobal::outputWidth;
    y = ofxMtlMapping2DGlobal::outputHeight;
    
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
}