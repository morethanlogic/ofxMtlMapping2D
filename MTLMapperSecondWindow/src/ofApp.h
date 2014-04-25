#pragma once

#include "ofMain.h"

#if defined(TARGET_OSX)
#include "ofxSyphon.h"

#elif defined(TARGET_WIN32)

#endif

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
#if defined(TARGET_OSX)
    void setupOSX();
    
    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverUpdated(ofxSyphonServerDirectoryEventArgs &args);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
    
    ofxSyphonServerDirectory dir;
    ofxSyphonClient client;
    int dirIdx;
    
    void setServer();
    
#elif defined(TARGET_WIN32)
    void setupWin();
    
#endif
    

};
