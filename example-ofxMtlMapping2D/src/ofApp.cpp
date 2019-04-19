#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(30);
    ofBackground(50);
    
    // ----
    _mapping = new ofxMtlMapping2D();
    _mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");
}

//--------------------------------------------------------------
void ofApp::update(){

    _mapping->update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    // ----
    _mapping->bind();
    
        // draw a test pattern
        _mapping->chessBoard();
    
    _mapping->unbind();
    
    
    //-------- mapping of the towers/shapes
    _mapping->draw();
    
    
    ofSetColor(0);
    ofDrawBitmapString("'m' open the mapping controls.\n", 20, 20);
     
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
