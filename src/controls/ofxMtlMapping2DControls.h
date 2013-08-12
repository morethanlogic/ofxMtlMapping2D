#pragma once

#include "ofEvents.h"

#include "ofxMtlMapping2DMode.h"
#include "ofxUI.h"
#include "ofxMtlMapping2DShapeType.h"

#define kControlsMappingToolsPanelWidth 39
#define kControlsMappingShapesListPanelWidth 150

#define kSettingMappingSave                 "MAPPING:SAVE"
#define kSettingMappingLoad                 "MAPPING:LOAD"

#define kSettingMappingFullscreen           "MAPPING:FULLSCREEN"
#define kSettingMappingEditShapes           "MAPPING:EDIT_SHAPES"
#define kSettingMappingCreateNewQuad        "MAPPING:CREATE_NEW_QUAD"
#define kSettingMappingCreateNewGrid        "MAPPING:CREATE_NEW_GRID"
#define kSettingMappingCreateNewTriangle    "MAPPING:CREATE_NEW_TRIANGLE"
#define kSettingMappingCreateNewMask        "MAPPING:CREATE_NEW_MASK"

#define kSettingMappingShowShapesId         "MAPPING:SHOW_SHAPES_ID"

#define kSettingMappingModeOutput           "MAPPING_MODE:OUTPUT"
#define kSettingMappingModeInput            "MAPPING_MODE:INPUT"


//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2DControls
{
    
    public:
        static ofxMtlMapping2DControls * mapping2DControls(string xmlFilePath);
        static ofxMtlMapping2DControls * mapping2DControls();

    
        void toolsUiEvent(ofxUIEventArgs &event);
        void shapesListUiEvent(ofxUIEventArgs &event);
        void gridSettingsListUiEvent(ofxUIEventArgs &event);

    
        const bool& saveMapping() { return _saveMapping; }
        void resetSaveMapping();
        const bool& loadMapping() { return _loadMapping; }
        void resetLoadMapping();
    
        const bool& editShapes() { return _editShapes; }
        const bool& createNewQuad() { return _createNewQuad; }
        const bool& createNewGrid() { return _createNewGrid; }
        const bool& createNewTriangle() { return _createNewTriangle; }
        const bool& createNewMask() { return _createNewMask; }
        void resetCreateNewShape();
        const bool& showShapesId() { return _showShapesId; }
    
        const bool& mappingModeChanged() { return _mappingModeChanged; }
        ofxMtlMapping2DMode mappingMode() { return _mappingMode; }
        void resetMappingChangedFlag();
    
        void showGridSettingsCanvas();
        void hideGridSettingsCanvas();

        int shapeCounter;
    
        void save();
        void load();
    
        void showInputModeToggle();
        void hideInputModeToggle();
    
        void enable();
        void disable();
        void toggle();
        bool isEnabled();
        bool isHit(int x, int y);
    
        void addShapeToList(int shapeID, int shapeType);
        void clearShapesList();
        const bool& selectedShapeChanged() { return _selectedShapeChanged; }
        void resetSelectedShapeChangedFlag();
        const int& selectedShapeId() { return _selectedShapeId; }
        void setAsActiveShapeWithId(int shapeID, int shapeType);
    
        void unselectShapesToggles();
        void windowResized();

    protected:
        ofxUIScrollableCanvas *_toolsCanvas;
        string _file;
        
        static const int kSliderHeight = 16;
        static const int kSpacerHeight = 20;
        static const int kToggleSize = 24;
        static const int kBottomSpacerHeight = 100; // padding to be able to scroll until the end/bottom of the UI canvas
        
        bool getToggleValue(const string& name);
        float getSliderValue(const string& name);
        ofPoint get2DPadValue(const string& name);
        
    private:
        ofxMtlMapping2DControls(int width, const string& file);

        static ofxMtlMapping2DControls *_mapping2DControls;
        map<ofxMtlMapping2DShapeType, string> shapeTypesAsString;

        void setUIShapeEditingState(bool isOn);
        bool _saveMapping;
        bool _loadMapping;
        
        bool _editShapes;
        bool _createNewQuad;
        bool _createNewGrid;
        bool _createNewTriangle;
        bool _createNewMask;
        bool _showShapesId;
        bool _selectedShapeChanged;
        int _selectedShapeId;
    
        bool _mappingModeChanged;
        ofxMtlMapping2DMode _mappingMode;
        
        ofxUIScrollableCanvas *_shapesListCanvas;
        void resizeShapeList();
        void refreshShapesListForMappingMode(ofxMtlMapping2DMode mappingMode);
    
        ofxUICanvas *_gridSettingsCanvas;
    
        ofImage _fullscreenExpandIcon;
        ofImage _fullscreenContractIcon;

};
