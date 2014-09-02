#include "ofxGemcutterTriangle.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterTriangle::ofxGemcutterTriangle()
{

}

//--------------------------------------------------------------
ofxGemcutterTriangle::~ofxGemcutterTriangle(){

}

//--------------------------------------------------------------
void ofxGemcutterTriangle::render()
{    
    glPushMatrix();
    glBegin(GL_POLYGON);
    
    for (int i = 0; i < inputPolygon->polyline->size(); i++) {
        glTexCoord2f(inputPolygon->polyline->getVertices()[i].x, inputPolygon->polyline->getVertices()[i].y);
        glVertex2f(polyline->getVertices()[i].x, polyline->getVertices()[i].y);
    }
    
    glEnd();
    glPopMatrix();
}

//--------------------------------------------------------------
void ofxGemcutterTriangle::createDefaultShape()
{
    shapeSettings["type"] = "triangle";
    inputPolygon = new ofxGemcutterInput();
    
    int xOffset = 0;
    int yOffset = 0;

    //Create a new vertex
    int x = ofxGemcutterGlobal::outputWidth/2;
    int y = 0;
    ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = ofxGemcutterGlobal::outputHeight;
    
    //Create a new vertex
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofxGemcutterGlobal::outputWidth;
    y = ofxGemcutterGlobal::outputHeight;
    
    //Create a new vertex
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    inputPolygon->init(shapeId);
}