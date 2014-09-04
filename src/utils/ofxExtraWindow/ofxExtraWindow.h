#pragma once

#include "ofMain.h"
#include <GLFW/glfw3.h>

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxExtraWindow {
    
public:
    
    ofxExtraWindow();
    ~ofxExtraWindow();
    
    void setup(const char *name, int xpos, int ypos, int width, int height, bool undecorated);
    void setup(const char *name, ofRectangle rect, bool undecorated);
    void destroyWindow();
    
    void begin();
    void end();
    
    void setPostionAndSize(ofRectangle rect);
    
    bool bIsSetup;
    
    ofEvent<int> windowClosedEvent;
    
private:
    static ofxExtraWindow	* instance;

    GLFWwindow *_mainWindow;
    GLFWwindow *_extraWindow;
    
    static void 	keyboard_cb(GLFWwindow* windowP_, int key, int scancode, int action, int mods);

};