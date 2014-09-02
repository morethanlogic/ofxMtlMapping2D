#include "ofxGemcutterQuad.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterQuad::ofxGemcutterQuad()
{

}

//--------------------------------------------------------------
ofxGemcutterQuad::~ofxGemcutterQuad(){

}

//--------------------------------------------------------------
void ofxGemcutterQuad::render()
{    
    glPushMatrix();
    glMultMatrixf(inputPolygon->homoMatrix);
    glBegin(GL_POLYGON);
    
    for (int i = 0; i < inputPolygon->polyline->size(); i++) {
        glTexCoord2f(inputPolygon->polyline->getVertices()[i].x, inputPolygon->polyline->getVertices()[i].y);
        glVertex2f(inputPolygon->polyline->getVertices()[i].x - inputPolygon->boundingBox.x, inputPolygon->polyline->getVertices()[i].y - inputPolygon->boundingBox.y);
    }
    
    
    glEnd();
    glPopMatrix();
}


//--------------------------------------------------------------
void ofxGemcutterQuad::createDefaultShape()
{
    shapeSettings["type"] = "quad";
    
    inputPolygon = new ofxGemcutterInput();
    
    
    int xOffset = 0;
    int yOffset = 0;
    
    //Create a new vertex
    int x = 0;
    int y = 0;
    ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofxGemcutterGlobal::outputWidth;
    y = 0;
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
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = ofxGemcutterGlobal::outputHeight;
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    vertices.push_back(newVertex);
    
    // Input
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    // ----
    inputPolygon->init(shapeId);
}

//--------------------------------------------------------------
void ofxGemcutterQuad::calcHomography()
{
    updatePolyline();
    
    inputPolygon->calcHomography(polyline->getVertices()[0].x, polyline->getVertices()[0].y,
                              polyline->getVertices()[1].x, polyline->getVertices()[1].y,
                              polyline->getVertices()[2].x, polyline->getVertices()[2].y,
                              polyline->getVertices()[3].x, polyline->getVertices()[3].y);
}