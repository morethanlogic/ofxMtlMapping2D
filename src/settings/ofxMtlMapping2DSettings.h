#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
class ofxMtlMapping2DSettings {
    public:
       
        // --- available / activated options
        static const bool kIsManuallyCreatingShapeEnabled;   
        static const bool kIsManuallyAddingDeletingVertexEnabled;
    
        // ---
        static ofTrueTypeFont infoFont;
    
        // ---
        static float gridDefaultNbCols;
        static float gridDefaultNbRows;
};