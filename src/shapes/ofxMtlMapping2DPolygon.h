#pragma once

//OF
#include "ofMain.h"

#include "ofxMtlMapping2DShapeType.h"
#include "ofxMtlMapping2DSettings.h"
#include "mtlUtils.h"
#include "ofxMtlMapping2DVertex.h"

//MSA Libs
#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DPolygon : public ofxMSAInteractiveObject {
	
    public:
    
        // ----
        static ofxMtlMapping2DPolygon* activePolygon;
        static ofxMtlMapping2DPolygon* previousActivePolygon;
        static int activeVertexId;
        static void resetActivePolygonVars();

        // ----
        ofxMtlMapping2DPolygon();
        ~ofxMtlMapping2DPolygon();
    
        int shapeId;
        int shapeType;
        list<ofxMtlMapping2DVertex*> vertices;
        ofPolyline *polyline;
        ofRectangle boundingBox;

        void init(int sId, bool defaultShape = false);
        
        void update();
        void draw();
        void addPoint(int x, int y);
        void drawID();
        virtual void render() {};
        void setAsActive();
        void setAsIdle();
    
        void select(int x, int y);

    
        ofxMtlMapping2DVertex* getVertex(int index);

        // ---- ofxMSAInteractiveObject related
        void enable();
        void disable();
    
        virtual bool hitTest(int tx, int ty);

        virtual void onRollOver(int x, int y);
        virtual void onRollOut();
        virtual void onMouseMove(int x, int y);
        virtual void onDragOver(int x, int y, int button);
        virtual void onDragOutside(int x, int y, int button);
        virtual void onRelease(int x, int y, int button);
        virtual void onReleaseOutside(int x, int y, int button);
 
    protected:
        ofPoint _centroid2D;
        ofPoint _grabAnchor;

        void disableVertices();
        void enableVertices();

        void updatePosition(int xInc, int yInc);
        void updatePolyline();

        virtual void createDefaultShape() {};
};
