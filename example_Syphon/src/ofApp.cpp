#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    ofBackground(50);

    // ofxMTLMapping2D
    _mapping = new ofxMtlMapping2D();
    _mapping->init(ofGetWidth(), ofGetHeight());
    
    // Syphon
    //setup our directory
	_syphonServerDir.setup();
    //setup our client
    _syphonClient.setup();
    
    //register for our directory's callbacks
    ofAddListener(_syphonServerDir.events.serverAnnounced, this, &ofApp::serverAnnounced);
    // not yet implemented
    //ofAddListener(dir.events.serverUpdated, this, &testApp::serverUpdated);
    ofAddListener(_syphonServerDir.events.serverRetired, this, &ofApp::serverRetired);
    
    _syphonDirIdx = -1;
}

//--------------------------------------------------------------
void ofApp::update()
{
    _mapping->update();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // ---
    _mapping->bind();
    {
        if(_syphonServerDir.isValidIndex(_syphonDirIdx)) {
            ofSetColor(255, 255, 255, 255);
            _syphonClient.draw(0, 0);
        } else {
            // draw a test pattern
            _mapping->chessBoard();
        }
    }
    _mapping->unbind();
    
    
    // --- mapping of the towers/shapes
    _mapping->draw();

}

//--------------------------------------------------------------
void ofApp::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& _syphonServerDir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced") << " Server Name: " << _syphonServerDir.serverName << " | App Name: " << _syphonServerDir.appName;
    }
    _syphonDirIdx = 0;
}

//--------------------------------------------------------------
void ofApp::serverUpdated(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& _syphonServerDir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Updated") << " Server Name: "<< _syphonServerDir.serverName << " | App Name: " <<_syphonServerDir.appName;
    }
    _syphonDirIdx = 0;
}

//--------------------------------------------------------------
void ofApp::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& _syphonServerDir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired") << " Server Name: " <<_syphonServerDir.serverName << " | App Name: " << _syphonServerDir.appName;
    }
    _syphonDirIdx = 0;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 32) {
        _syphonDirIdx++;
        if(_syphonDirIdx > _syphonServerDir.size() - 1)
            _syphonDirIdx = 0;
        
        _syphonClient.set(_syphonServerDir.getDescription(_syphonDirIdx));
        string serverName = _syphonClient.getServerName();
        string appName = _syphonClient.getApplicationName();
        
        if(serverName == ""){
            serverName = "null";
        }
        if(appName == ""){
            appName = "null";
        }
        ofSetWindowTitle(serverName + ":" + appName);
    }

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