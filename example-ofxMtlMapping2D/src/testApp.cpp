#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    ofBackground(50);
    
    // ----
    _mapping = new ofxMtlMapping2D();
    _mapping->init(ofGetWidth(), ofGetHeight(), "mapping/xml/shapes.xml", "mapping/controls/mapping.xml");
}

//--------------------------------------------------------------
void testApp::update(){

    _mapping->update();

}

//--------------------------------------------------------------
void testApp::draw(){    
    // ----
    _mapping->bind();
    
        // draw a test pattern
        _mapping->chessBoard();
    
    _mapping->unbind();
    
    
    //-------- mapping of the towers/shapes
    _mapping->draw();
    //_mapping->drawFbo();
    

//    ofBeginShape();
//    ofFill();
//    ofSetColor(0, 255, 0);
//    for (int i = 0; i < _mapping->getMaskShapes()[0]->size(); i++) {
//        ofVertex(_mapping->getMaskShapes()[0]->getVertices()[i].x, _mapping->getMaskShapes()[0]->getVertices()[i].y);
//    }
//    ofEndShape(true);
    
    
    // Draw some instructions.
    /*
    ofSetColor(0);
    ofDrawBitmapString("'m' open the mapping controls.\n", 20, 20);
     */
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}