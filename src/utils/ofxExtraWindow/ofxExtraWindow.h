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
    
    void setPosition(int x, int y);
    void setPostionAndSize(ofRectangle rect);
    
    int getWidth();
    int getHeight();
    
private:
    GLFWwindow *_mainWindow;
    GLFWwindow *_extraWindow;
    int _width;
    int _height;
};