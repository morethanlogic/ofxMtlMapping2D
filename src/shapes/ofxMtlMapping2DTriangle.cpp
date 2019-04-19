#include "ofxMtlMapping2DTriangle.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DTriangle::ofxMtlMapping2DTriangle()
{

}

//--------------------------------------------------------------
ofxMtlMapping2DTriangle::~ofxMtlMapping2DTriangle(){

}

//--------------------------------------------------------------
void ofxMtlMapping2DTriangle::render()
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
void ofxMtlMapping2DTriangle::createDefaultShape()
{
    shapeSettings["type"] = "triangle";
    inputPolygon = new ofxMtlMapping2DInput();
    
    int xOffset = ofGetWidth()/2.0;
    int yOffset = ofGetHeight()/2.0;

    int x = 50;
    int y = 0;
    
    ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = 100;
    
    //Create a new vertex
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 100;
    y = 100;
    
    //Create a new vertex
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    inputPolygon->init(shapeId);
}