#include "ofxMtlMapping2DQuad.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DQuad::ofxMtlMapping2DQuad()
{

}

//--------------------------------------------------------------
ofxMtlMapping2DQuad::~ofxMtlMapping2DQuad(){

}

//--------------------------------------------------------------
void ofxMtlMapping2DQuad::render()
{    
    glPushMatrix();
    glMultMatrixf(inputPolygon->homoMatrix);
    glBegin(GL_POLYGON);
    
//    int i = 0;
//    list<ofxMtlMapping2DVertex*>::iterator it;
//    for (it=inputPolygon->vertices.begin(); it!=inputPolygon->vertices.end(); it++) {
//        ofxMtlMapping2DVertex* vertex = *it;
//        glTexCoord2f(vertex->center.x, vertex->center.y);
//        glVertex2f(inputPolygon->polyline->getVertices()[i].x - inputPolygon->boundingBox.x, inputPolygon->polyline->getVertices()[i].y - inputPolygon->boundingBox.y);
//        i++;
//    }
    
    for (int i = 0; i < inputPolygon->polyline->size(); i++) {
        glTexCoord2f(inputPolygon->polyline->getVertices()[i].x, inputPolygon->polyline->getVertices()[i].y);
        //glVertex2f(inputPolygon->polyline->getVertices()[i].x, inputPolygon->polyline->getVertices()[i].y);
        glVertex2f(inputPolygon->polyline->getVertices()[i].x - inputPolygon->boundingBox.x, inputPolygon->polyline->getVertices()[i].y - inputPolygon->boundingBox.y);
    }
    
    
    glEnd();
    glPopMatrix();
}


//--------------------------------------------------------------
void ofxMtlMapping2DQuad::createDefaultShape()
{
    shapeSettings["type"] = "quad";
    
    inputPolygon = new ofxMtlMapping2DInput();
    
    
    int xOffset = ofGetWidth()/2.0;
    int yOffset = ofGetHeight()/2.0;
    
    //Create a new vertex
    int x = 0;
    int y = 0;
    ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 100;
    y = 0;
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
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = 100;
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    // ----
    inputPolygon->init(shapeId);
}

//--------------------------------------------------------------
void ofxMtlMapping2DQuad::calcHomography()
{
    updatePolyline();
    
    inputPolygon->calcHomography(polyline->getVertices()[0].x, polyline->getVertices()[0].y,
                              polyline->getVertices()[1].x, polyline->getVertices()[1].y,
                              polyline->getVertices()[2].x, polyline->getVertices()[2].y,
                              polyline->getVertices()[3].x, polyline->getVertices()[3].y);
}