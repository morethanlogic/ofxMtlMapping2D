#pragma once

#include "ofEvents.h"

#include "ofxMtlMapping2DControlsInterface.h"
#include "ofxMtlMapping2DMode.h"

#define kControlsMappingPanelWidth 320

#define kSettingMappingSave                 "MAPPING:SAVE"
#define kSettingMappingLoad                 "MAPPING:LOAD"

#define kSettingMappingEditShapes           "MAPPING:EDIT_SHAPES"
#define kSettingMappingCreateNewQuad        "MAPPING:CREATE_NEW_QUAD"
#define kSettingMappingCreateNewTriangle    "MAPPING:CREATE_NEW_TRAINGLE"

#define kSettingMappingShowShapesId         "MAPPING:SHOW_SHAPES_ID"

#define kSettingMappingModeOutput           "MAPPING_MODE:OUTPUT"
#define kSettingMappingModeInput            "MAPPING_MODE:INPUT"

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DControls : public ofxMtlMapping2DControlsInterface
{
    private:
        static ofxMtlMapping2DControls *_mapping2DControls;
        
        bool _saveMapping;
        bool _loadMapping;
        
        bool _editShapes;
        bool _createNewQuad;
        bool _createNewTriangle;
        bool _showShapesId;
    
        bool _mappingModeChanged;
        ofxMtlMapping2DMode _mappingMode;
                            
    public:
        static ofxMtlMapping2DControls * mapping2DControls();

        ofxMtlMapping2DControls(int width, const string& file);
        
        void uiEvent(ofxUIEventArgs &event);
    
        const bool& saveMapping() { return _saveMapping; }
        void resetSaveMapping();
        const bool& loadMapping() { return _loadMapping; }
        void resetLoadMapping();
    
        const bool& editShapes() { return _editShapes; }
        const bool& createNewQuad() { return _createNewQuad; }
        const bool& createNewTriangle() { return _createNewTriangle; }
        void resetCreateNewShape();
        const bool& showShapesId() { return _showShapesId; }
    
        const bool& mappingModeChanged() { return _mappingModeChanged; }
        ofxMtlMapping2DMode mappingMode() { return _mappingMode; }
        void resetMappingChangedFlag();

    
};
