#pragma once

//OF
#include "ofMain.h"

#include "ofxGemcutterShapeType.h"
#include "ofxGemcutterSettings.h"
#include "mtlUtils.h"
#include "ofxGemcutterVertex.h"

//MSA Libs
#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterPolygon : public ofxMSAInteractiveObject {
	
public:

    // ---
    static ofxGemcutterPolygon* activePolygon;
    static ofxGemcutterPolygon* previousActivePolygon;
    static int activeVertexId;
    static void resetActivePolygonVars();

    // ---
    ofxGemcutterPolygon();
    ~ofxGemcutterPolygon();

    int shapeId;
    int shapeType;
    list<ofxGemcutterVertex*> vertices;
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

    ofxGemcutterVertex* getVertex(int index);
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
