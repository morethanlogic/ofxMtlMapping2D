#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("MTLMapper Output");
    ofSetFrameRate(60);
    ofSetVerticalSync(true);

#if defined(TARGET_OSX)
    setupOSX();
#elif defined(TARGET_WIN32)
    setupWin();
#endif
}

//--------------------------------------------------------------
void ofApp::update()
{

}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0, 0, 0);
    ofColor(255, 255, 255, 255);
    ofEnableAlphaBlending();

#if defined(TARGET_OSX)
    if(dir.isValidIndex(dirIdx)) {
        client.draw(0, 0);
    }
#elif defined(TARGET_WIN32)
    setupWin();
#endif

}

#if defined(TARGET_OSX)
//--------------------------------------------------------------
void ofApp::setupOSX()
{
    //setup our directory
	dir.setup();
    //setup our client
    client.setup();
    
    //register for our directory's callbacks
    ofAddListener(dir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    ofAddListener(dir.events.serverRetired, this, &ofApp::serverRetired);
    
    dirIdx = -1;
}

//--------------------------------------------------------------
void ofApp::setServer()
{
    for (int i=0; i < dir.getServerList().size(); i++) {
        if (dir.getDescription(i).serverName == "MTLMapperOutput") {
            client.set(dir.getDescription(i));
            dirIdx = i;
            return;
        }
    }
    
    dirIdx = -1;
}

//--------------------------------------------------------------
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    
    setServer();
}

//--------------------------------------------------------------
void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& dir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired")<<" Server Name: "<<dir.serverName <<" | App Name: "<<dir.appName;
    }
    
    setServer();
}

#elif defined(TARGET_WIN32)
//--------------------------------------------------------------
void ofApp::setupWin()
{
    
}

#endif


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
