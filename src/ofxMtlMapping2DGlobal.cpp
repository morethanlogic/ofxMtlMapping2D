#include "ofxMtlMapping2DGlobal.h"

//--------------------------------------------------------------
MappingEditView ofxMtlMapping2DGlobal::_mappingEditView = MAPPING_OUTPUT_VIEW;

//--------------------------------------------------------------
float ofxMtlMapping2DGlobal::inputViewZoomFactor = 1.0f;
float ofxMtlMapping2DGlobal::outputViewZoomFactor = 1.0f;
ofRectangle ofxMtlMapping2DGlobal::inputViewOutputPreview;
ofRectangle ofxMtlMapping2DGlobal::outputViewOutputPreview;
ofRectangle ofxMtlMapping2DGlobal::inputViewZoomedCoordSystem;
ofRectangle ofxMtlMapping2DGlobal::outputViewZoomedCoordSystem;

int ofxMtlMapping2DGlobal::outputWidth = 500;
int ofxMtlMapping2DGlobal::outputHeight = 500;
bool ofxMtlMapping2DGlobal::bIsDraggingZone = false;

//--------------------------------------------------------------
void ofxMtlMapping2DGlobal::setEditView(MappingEditView mappingEditView)
{
    _mappingEditView = mappingEditView;
}

//--------------------------------------------------------------
MappingEditView ofxMtlMapping2DGlobal::getEditView()
{
    return _mappingEditView;
}

//--------------------------------------------------------------
float ofxMtlMapping2DGlobal::getCurrZoomFactor()
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
ofVec2f ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f point, float xOffset, float yOffset)
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

