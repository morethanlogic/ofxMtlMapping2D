#pragma once

#include "ofxGemcutterShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterGrid: public ofxGemcutterShape {

public:
    
    ofxGemcutterGrid();
    ~ofxGemcutterGrid();

    void update();
    void draw();

    void updateGrid();

    int getNbCols() { return gridNbCols; }
    int getNbRows() { return gridNbRows; }

    float gridNbCols;
    float gridNbRows;

protected:
    virtual void createDefaultShape();
    virtual void initShape();
    virtual void render();

    void updateVertices();
    void updateGridAndMesh(bool startFresh = false);
    void updateUVMap();
    void drawInternalMesh();

    ofMesh    controlMesh;
    ofVboMesh internalMesh;

    int gridWidth;
    int gridHeight;
    int gridHorizontalResolution;
    int gridVerticalResolution;

    float gridQuadWidth;
    float gridQuadHeight;
    float gridCellWidth;
    float gridCellHeight;
};
