#pragma once

//OF
#include "ofMain.h"
#include "ofVec2f.h"

//MSA Libs
#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxGemcutterVertex : public ofxMSAInteractiveObject {
	
public:

    static ofxGemcutterVertex* activeVertex;

    ofxGemcutterVertex();
    ~ofxGemcutterVertex();

    void kill();
    void update();
    void updateCenter();
    void drawBack();
    void drawTop();

    void snapIt(float _x, float _y);

    void init(float _x, float _y, int _index = -1);

    static ofxGemcutterVertex* getActiveVertex();

    void onPress(int x, int y, int button);
    void onPressOutside(int x, int y, int button);
    void onRelease(int x, int y, int button);
    void onReleaseOutside(int x, int y, int button);

    void mouseDragged(int x, int y, int button);

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
    
    bool isDragged();

private:
    bool _bMouseGrabbed;

	
};
