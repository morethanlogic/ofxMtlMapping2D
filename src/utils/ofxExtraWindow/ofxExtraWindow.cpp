#include "ofxExtraWindow.h"

#ifdef TARGET_OSX
    #include <Cocoa/Cocoa.h>
    #define GLFW_EXPOSE_NATIVE_COCOA
    #define GLFW_EXPOSE_NATIVE_NSGL
    #include "GLFW/glfw3native.h"
#elif defined(TARGET_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_EXPOSE_NATIVE_WGL
    #include <GLFW/glfw3native.h>
#endif

//--------------------------------------------------------------
//--------------------------------------------------------------

ofxExtraWindow	* ofxExtraWindow::instance;

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxExtraWindow::ofxExtraWindow()
{
    instance = this;
    bIsSetup = false;
}

//--------------------------------------------------------------
ofxExtraWindow::~ofxExtraWindow()
{
    destroyWindow();
}

//--------------------------------------------------------------
void ofxExtraWindow::setup(const char *name, ofRectangle rect, bool undecorated)
{
    setup(name, rect.x, rect.y, rect.width, rect.height, undecorated);
}

//--------------------------------------------------------------
void ofxExtraWindow::setup(const char *name, int xpos, int ypos, int width, int height, bool undecorated)
{
    glfwWindowHint(GLFW_DECORATED, !undecorated);
    _mainWindow = glfwGetCurrentContext();
    _extraWindow = glfwCreateWindow(width, height, name, NULL, _mainWindow);
    //_extraWindow = glfwCreateWindow(width, height, "My Title", glfwGetPrimaryMonitor(), NULL);
    glfwSetWindowPos(_extraWindow, xpos, ypos);
    
    glfwSetKeyCallback(_extraWindow, (GLFWkeyfun)keyboard_cb);
    
    bIsSetup = true;

}

//--------------------------------------------------------------
void ofxExtraWindow::keyboard_cb(GLFWwindow* windowP_, int key, int scancode, int action, int mods) {
    switch (key) {
		case GLFW_KEY_ESCAPE:
            instance->destroyWindow();
            break;
		default:
			break;
	}
}

//--------------------------------------------------------------
void ofxExtraWindow::destroyWindow() {
    bIsSetup = false;
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
void ofxExtraWindow::setPostionAndSize(ofRectangle rect)
{
    
#ifdef TARGET_OSX
    [NSApp setPresentationOptions:NSApplicationPresentationHideMenuBar | NSApplicationPresentationHideDock];
    NSWindow * cocoaWindow = glfwGetCocoaWindow(_extraWindow);
    
    [cocoaWindow setStyleMask:NSBorderlessWindowMask];
    
    glfwSetWindowSize(_extraWindow, rect.width, rect.height);
    glfwSetWindowPos(_extraWindow, rect.x, rect.y);
    
#elif defined(TARGET_WIN32)
    HWND hwnd = glfwGetWin32Window(_extraWindow);
    
    SetWindowLong(hwnd, GWL_EXSTYLE, 0);
    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    
    SetWindowPos(hwnd, HWND_TOPMOST, rect.x, rect.y, rect.width, rect.height, SWP_SHOWWINDOW);

#endif

}