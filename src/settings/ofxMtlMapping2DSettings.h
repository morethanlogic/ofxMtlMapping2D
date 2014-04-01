#pragma once

#define USE_OFX_DETECT_DISPLAYS

#include "ofMain.h"

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
};