#include "ofxGemcutterMask.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterMask::ofxGemcutterMask()
{

}

//--------------------------------------------------------------
ofxGemcutterMask::~ofxGemcutterMask(){

}

//--------------------------------------------------------------
void ofxGemcutterMask::render()
{
    ofBeginShape();
    ofFill();    
    for (int i = 0; i < polyline->size(); i++) {
        ofVertex(polyline->getVertices()[i].x, polyline->getVertices()[i].y);
    }
    ofEndShape(true);
}

//--------------------------------------------------------------
void ofxGemcutterMask::createDefaultShape()
{
    shapeSettings["type"] = "mask";
    
    int xOffset = 0;
    int yOffset = 0;

    //Create a new vertex
    int x = ofxGemcutterGlobal::outputWidth/2;
    int y = 0;
    
    ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    //Create a new vertex
    x = 0;
    y = ofxGemcutterGlobal::outputHeight;
    
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    //Create a new vertex
    x = ofxGemcutterGlobal::outputWidth;
    y = ofxGemcutterGlobal::outputHeight;
    
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
}