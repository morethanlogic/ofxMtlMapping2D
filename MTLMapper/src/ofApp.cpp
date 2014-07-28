#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofBackground(50);
    
#if defined(TARGET_OSX)
    #if defined(OF_RELEASE)
        ofSetDataPathRoot("../Resources/");
    #endif
    
	ofSetWindowPosition(0, 0);
    ofSetWindowShape(ofGetScreenWidth(), ofGetScreenHeight());
#elif defined(TARGET_WIN32)
	// Not sure why, and I did not Google it yet.
	// But the on Windows we need an offset and to reduce the size of the window a bit to see the chromes.
	ofSetWindowPosition(10, 10); 
    ofSetWindowShape(ofGetScreenWidth()-20, ofGetScreenHeight()-20);
#endif



    // ---
    _mapping = new ofxMtlMapping2D();
    _mapping->setup(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::exit()
{
    delete _mapping;
}

//--------------------------------------------------------------
void ofApp::update(){

    _mapping->update();

}

//--------------------------------------------------------------
void ofApp::draw(){    
    // ---
    _mapping->bind();
    
    //_mapping->chessBoard();
    
    _mapping->unbind();
    
    
    // ---
    _mapping->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}