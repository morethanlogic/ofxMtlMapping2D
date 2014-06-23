#include "ofxExtraWindow.h"

//--------------------------------------------------------------
ofxExtraWindow::ofxExtraWindow()
{
    
}

//--------------------------------------------------------------
ofxExtraWindow::~ofxExtraWindow()
{
    destroyWindow();
}

//--------------------------------------------------------------
void ofxExtraWindow::setup(const char *name, ofRectangle rect, bool undecorated)
{
    setup(name, rect.x, rect.y, rect.width/3, rect.height/3, undecorated);
}

//--------------------------------------------------------------
void ofxExtraWindow::setup(const char *name, int xpos, int ypos, int width, int height, bool undecorated)
{
    _width = width;
    _height = height;
    glfwWindowHint(GLFW_DECORATED, !undecorated);
    _mainWindow = glfwGetCurrentContext();
    _extraWindow = glfwCreateWindow(width, height, name, NULL, _mainWindow);
    glfwSetWindowPos(_extraWindow, xpos, ypos);
}

//--------------------------------------------------------------
void ofxExtraWindow::destroyWindow() {
    glfwDestroyWindow(_extraWindow);
}

//--------------------------------------------------------------
void ofxExtraWindow::begin()
{
    int width;
    int height;
    glfwMakeContextCurrent(_extraWindow);
    glfwGetFramebufferSize(_extraWindow, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//--------------------------------------------------------------
void ofxExtraWindow::end()
{
    glfwSwapBuffers(_extraWindow);
    glfwPollEvents();
    glfwMakeContextCurrent(_mainWindow);
}


//------------------------------------------------------------
void ofxExtraWindow::setPosition(int x, int y)
{
    glfwSetWindowPos(_extraWindow, x, y);
}

//------------------------------------------------------------
void ofxExtraWindow::setPostionAndSize(ofRectangle rect)
{
    _width = rect.width;
    _height = rect.height;
    glfwSetWindowSize(_extraWindow, rect.width, rect.height);
    setPosition(rect.x, rect.y);

}

//------------------------------------------------------------
int ofxExtraWindow::getWidth()
{
    return _width;
}

//------------------------------------------------------------
int ofxExtraWindow::getHeight()
{
    return _height;
}