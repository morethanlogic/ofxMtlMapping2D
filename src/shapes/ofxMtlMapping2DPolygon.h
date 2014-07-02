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

    // ---
    static ofxMtlMapping2DPolygon* activePolygon;
    static ofxMtlMapping2DPolygon* previousActivePolygon;
    static int activeVertexId;
    static void resetActivePolygonVars();

    // ---
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
    int getNbOfVertices();

    // ---- ofxMSAInteractiveObject related
    void enable();
    void disable();

    bool hitTest(int tx, int ty) const;

    void onPress(int x, int y, int button);
    void onRelease(int x, int y, int button);
    void onReleaseOutside(int x, int y, int button);

    void mouseDragged(int x, int y, int button);

protected:
    ofPoint _centroid2D;
    ofPoint _grabAnchor;
    bool _bMouseEventEnabled;
    bool _bMouseGrabbed;

    void disableVertices();
    void enableVertices();

    void updatePosition(int xInc, int yInc);
    void updatePolyline();

    virtual void createDefaultShape() {};
};
