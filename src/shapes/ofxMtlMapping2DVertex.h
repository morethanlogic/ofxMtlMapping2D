#pragma once

//OF
#include "ofMain.h"
#include "ofVec2f.h"

//MSA Libs
#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DVertex : public ofxMSAInteractiveObject {
	
public:
    
    static ofxMtlMapping2DVertex* activeVertex;
    
    ofxMtlMapping2DVertex();
    ~ofxMtlMapping2DVertex();
	
    void kill();
    void update();
    void updateCenter();
    void drawBack();
    void drawTop();
    
    void snapIt(float _x, float _y);
	
    void init(float _x, float _y, int _index = -1);
    
    static ofxMtlMapping2DVertex* getActiveVertex();
    
    virtual void onRollOver(int x, int y);
    virtual void onRollOut();
    virtual void onMouseMove(int x, int y);
    virtual void onDragOver(int x, int y, int button);
    virtual void onDragOutside(int x, int y, int button);
    virtual void onPress(int x, int y, int button);
    virtual void onPressOutside(int x, int y, int button);
    virtual void onRelease(int x, int y, int button);
    virtual void onReleaseOutside(int x, int y, int button);
    
    bool toBeRemoved;
    bool isDefiningTectureCoord;
    ofVec2f center;
    int index;
    bool bIsOnAnEdge;
    int edgeIndex;
    
    void setAsActive();
    
    void left();
    void up();
    void right();
    void down();
	
};
