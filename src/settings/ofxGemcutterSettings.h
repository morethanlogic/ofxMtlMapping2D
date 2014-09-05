#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
class ofxGemcutterSettings {
public:

    // --- available / activated options
    static const bool kIsManuallyCreatingShapeEnabled; // set to false before compiling in order to lock the creating shape capabilities 
    static const bool kIsManuallyAddingDeletingVertexEnabled;

    // ---
    static ofTrueTypeFont infoFont;

    // ---
    static int gridDefaultNbCols;
    static int gridDefaultNbRows;
};