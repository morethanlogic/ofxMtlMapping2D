#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
    
    ofAppGLFWWindow window;
    ofSetupOpenGL(&window, 1000, 800, OF_WINDOW);
	ofRunApp( new ofApp());

}
