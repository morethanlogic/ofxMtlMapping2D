#pragma once

//OF
#include "ofMain.h"

#include "ofxGemcutterPolygon.h"
#include "ofxGemcutterSettings.h"
#include "mtlUtils.h"
#include "ofxGemcutterInput.h"
#include "ofxGemcutterVertex.h"

#include "ofxGemcutterGlobal.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterShape : public ofxGemcutterPolygon {
	
public:
    // ----
    static int nextShapeId;
    static ofxGemcutterShape* activeShape;
    static ofxGemcutterShape* previousActiveShape;
    static void resetActiveShapeVars();

    // ----
    ofxGemcutterShape();
    ~ofxGemcutterShape();

    map<string,string> shapeSettings;
    ofxGemcutterInput* inputPolygon;

    void init(int sId, bool defaultShape = false);

    void update();
    void draw();
    void drawID();
    virtual void render() {};

    void setAsActiveShape(bool fromUI = false);
    
    void setLock(bool val);
    bool isLocked();

protected:
    virtual void calcHomography() {};
    virtual void createDefaultShape() {};
    virtual void initShape() {};

private:
    typedef ofxGemcutterPolygon _super;
    
    bool _bIsLocked;


};
