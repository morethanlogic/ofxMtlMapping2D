#pragma once

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
enum MappingEditView
{
    MAPPING_CHANGE_TO_INPUT_VIEW,
    MAPPING_INPUT_VIEW,
    MAPPING_CHANGE_TO_OUTPUT_VIEW,
    MAPPING_OUTPUT_VIEW
};

//--------------------------------------------------------------
class ofxGemcutterGlobal
{
public:
    static void setEditView(MappingEditView mappingEditView);
    static MappingEditView getEditView();
    
    static float delayBeforeHiddingUI;
    
    // Zoom and Drag of the canvases
    static float inputViewZoomFactor;
    static float outputViewZoomFactor;
    
    static float getCurrZoomFactor();
    
    static ofRectangle inputViewOutputPreview;
    static ofRectangle outputViewOutputPreview;
    
    static ofRectangle inputViewZoomedCoordSystem;
    static ofRectangle outputViewZoomedCoordSystem;

    static ofVec2f screenToZoomed(ofVec2f point, float xOffset = .0f, float yOffset = .0f);
    
    static int outputWidth;
    static int outputHeight;
    
    static bool bIsDraggingZone;


private:

    static MappingEditView _mappingEditView;
};