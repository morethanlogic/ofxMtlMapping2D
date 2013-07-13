#pragma once

#include "ofxMtlMapping2DShape.h"

class SelectablePoint: public ofPoint {
public:
    SelectablePoint(){
        selected = false;
    }
    bool selected;
};

class BaseQuad{
public:
    int index;
    SelectablePoint TL;
    SelectablePoint TR;
    SelectablePoint BL;
    SelectablePoint BR;
    
    int TL_Index;
    int TR_Index;
    int BL_Index;
    int BR_Index;
};

class InternalQuad : public BaseQuad{
public:
    int x;
    int y;
};

class ControlQuad : public BaseQuad{
public:
    vector<InternalQuad*> internalQuads;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DGrid: public ofxMtlMapping2DShape {
	
    public:
        
        ofxMtlMapping2DGrid();
        ~ofxMtlMapping2DGrid();
    
        void update();
        void draw();
    
    protected:
        virtual void createDefaultShape();
        virtual void render();
    
        void drawUI();
        void onGridChange(int & value);
        void onCoordinatesChange(float & value);
        void updateVertices();
    
        vector<ControlQuad*> controlQuads;
        ofVboMesh internalMesh;
    
        int gridWidth;
        int gridHeight;
    
        ofVec2f gridSize;
        ofVec2f resolution;
    
        ofVec2f coordinatesStart;
        ofVec2f coordinatesEnd;
};
