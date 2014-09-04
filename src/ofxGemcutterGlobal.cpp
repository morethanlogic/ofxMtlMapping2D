#include "ofxGemcutterGlobal.h"

//--------------------------------------------------------------
MappingEditView ofxGemcutterGlobal::_mappingEditView = MAPPING_OUTPUT_VIEW;

//--------------------------------------------------------------
float ofxGemcutterGlobal::delayBeforeHiddingUI = 30000;
float ofxGemcutterGlobal::inputViewZoomFactor = 1.0f;
float ofxGemcutterGlobal::outputViewZoomFactor = 1.0f;
ofRectangle ofxGemcutterGlobal::inputViewOutputPreview;
ofRectangle ofxGemcutterGlobal::outputViewOutputPreview;
ofRectangle ofxGemcutterGlobal::inputViewZoomedCoordSystem;
ofRectangle ofxGemcutterGlobal::outputViewZoomedCoordSystem;

int ofxGemcutterGlobal::outputWidth = 10;
int ofxGemcutterGlobal::outputHeight = 10;
bool ofxGemcutterGlobal::bIsDraggingZone = false;

//--------------------------------------------------------------
void ofxGemcutterGlobal::setEditView(MappingEditView mappingEditView)
{
    _mappingEditView = mappingEditView;
}

//--------------------------------------------------------------
MappingEditView ofxGemcutterGlobal::getEditView()
{
    return _mappingEditView;
}

//--------------------------------------------------------------
float ofxGemcutterGlobal::getCurrZoomFactor()
{
    float zoomFactor;
    
    if (getEditView() == MAPPING_INPUT_VIEW) {
        zoomFactor = inputViewZoomFactor;
    }
    
    else if (getEditView() == MAPPING_OUTPUT_VIEW) {
        zoomFactor = outputViewZoomFactor;
    }
    
    return zoomFactor;
}

//--------------------------------------------------------------
ofVec2f ofxGemcutterGlobal::screenToZoomed(ofVec2f point, float xOffset, float yOffset)
{
    float transformedX;
    float transformedY;
    
    if (_mappingEditView == MAPPING_INPUT_VIEW) {
        transformedX = ofMap(point.x, .0f, ofGetWidth(), inputViewZoomedCoordSystem.x + xOffset, inputViewZoomedCoordSystem.x + xOffset + inputViewZoomedCoordSystem.width);
        transformedY = ofMap(point.y, .0f, ofGetHeight(), inputViewZoomedCoordSystem.y + yOffset, inputViewZoomedCoordSystem.y + yOffset + inputViewZoomedCoordSystem.height);
    }
    
    else if (_mappingEditView == MAPPING_OUTPUT_VIEW) {
        transformedX = ofMap(point.x, .0f, ofGetWidth(), outputViewZoomedCoordSystem.x + xOffset, outputViewZoomedCoordSystem.x + xOffset + outputViewZoomedCoordSystem.width);
        transformedY = ofMap(point.y, .0f, ofGetHeight(), outputViewZoomedCoordSystem.y + yOffset, outputViewZoomedCoordSystem.y + yOffset + outputViewZoomedCoordSystem.height);
    }
    
    return ofVec2f(transformedX, transformedY);
}

