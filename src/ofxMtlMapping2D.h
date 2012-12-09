#pragma once

// OF
#include "ofMain.h"

// Addons
#include "ofxXmlSettings.h"

//Mapping
#include "ofxMtlMapping2DVertex.h"
#include "ofxMtlMapping2DQuad.h"
#include "ofxMtlMapping2DTriangle.h"
#include "ofxMtlMapping2DMask.h"
#include "ofxMtlMapping2DShape.h"

#include "mtlUtils.h"


//========================================================================
class ofxMtlMapping2D {
    
    public:    
        ofxMtlMapping2D();
        virtual ~ofxMtlMapping2D();
        
        void init(int width, int height, string mappingXmlFilePath = "mapping/xml/shapes.xml", string uiXmlFilePath = "mapping/controls/mapping.xml");
        void update();
        
        void bind();
        void unbind();
        void drawFbo();
        void draw();

        void mousePressed(int x, int y, int button);
        void keyPressed(int key);
    
        void chessBoard(int nbOfCol = 10);

    private:
        string _mappingXmlFilePath;
        ofFbo _fbo;
        ofxXmlSettings _shapesListXML;
        list<ofxMtlMapping2DShape*>::iterator iteratorForShapeWithId(int shapeId);

        void render();
    
        void createQuad(float _x, float _y);
        void createTriangle(float _x, float _y);
        void createMask(float _x, float _y);
        void deleteShape();
    
        void loadShapesList();
        void saveShapesList();
};