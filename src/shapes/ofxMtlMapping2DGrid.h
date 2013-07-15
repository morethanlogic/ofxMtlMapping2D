#pragma once

#include "ofxMtlMapping2DShape.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DGrid: public ofxMtlMapping2DShape {
	
    public:
        
        ofxMtlMapping2DGrid();
        ~ofxMtlMapping2DGrid();
    
        void update();
        void draw();
    
        void updateGrid();
    
        int getNbCols() { return gridNbCols; }
        int getNbRows() { return gridNbRows; }
    
    protected:
        virtual void createDefaultShape();
        virtual void render();
    
        void updateVertices();
        void updateGridAndMesh();
        void updateUVMap();
        void drawInternalMesh();
    
        ofVboMesh internalMesh;
    
        int gridWidth;
        int gridHeight;
        int gridNbCols;
        int gridNbRows;
        int gridHorizontalResolution;
        int gridVerticalResolution;
    
        float gridQuadWidth;
        float gridQuadHeight;
        float gridCellWidth;
        float gridCellHeight;
};
