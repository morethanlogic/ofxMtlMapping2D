#pragma once

#include "ofMain.h"
#include "ofxMtlMapping2DDefines.h"

//--------------------------------------------------------------
class ofxMtlMapping2DSettings {
public:

    // --- available / activated options
    static const bool kIsManuallyCreatingShapeEnabled; // set to false before compiling in order to lock the creating shape capabilities 
    static const bool kIsManuallyAddingDeletingVertexEnabled;

    // ---
    static ofTrueTypeFont infoFont;

    // ---
    static int gridDefaultNbCols;
    static int gridDefaultNbRows;
    
    static float zoomFactor;

    static float xLoNew;
    static float yLoNew;
    static float xHiNew;
    static float yHiNew;
};