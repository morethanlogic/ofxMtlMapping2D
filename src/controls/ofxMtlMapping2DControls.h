#pragma once

#include "ofEvents.h"

#include "ofxMtlMapping2DMode.h"
#include "ofxUI.h"
#include "ofxMtlMapping2DShapeType.h"

#include "ofxDetectDisplays.h"

#define kControlsMappingToolsPanelWidth 33
#define kControlsMappingShapesListPanelWidth 150

#define kSettingMappingSave                 "MAPPING:SAVE"
#define kSettingMappingLoad                 "MAPPING:LOAD"
#define kSettingMappingSettings             "MAPPING:SETTINGS"

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
        void settingsUiEvent(ofxUIEventArgs &event);
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
        //void toggle();
        void toggleVisible();
        bool isVisible();
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

        static const int kSliderHeight = 16;
        static const int kSpacerHeight = 20;
        static const int kToggleSize = 24;
        static const int kBottomSpacerHeight = 100; // padding to be able to scroll until the end/bottom of the UI canvas
    
        ofxUICanvas *_toolsCanvas;
        string _file;
        
        vector<ofxUICanvas*> _uiSuperCanvases;
        
        bool getButtonValue(ofxUICanvas* ui, const string& name);
        
        bool getToggleValue(ofxUICanvas* ui, const string& name);
        void setToggleValue(ofxUICanvas* ui, const string& name, float value);
        
        float getSliderValue(ofxUICanvas* ui, const string& name);
        void setSliderValue(ofxUICanvas* ui, const string& name, float value);
    
//        bool getToggleValue(const string& name);
//        float getSliderValue(const string& name);
//        ofPoint get2DPadValue(const string& name);
    
    private:
        ofxMtlMapping2DControls(const string& file);
        ~ofxMtlMapping2DControls();

        string _rootPath;

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
    
        ofxUISuperCanvas* _settingsUI;

    
        ofxUIScrollableCanvas *_shapesListCanvas;
        void resizeShapeList();
        void refreshShapesListForMappingMode(ofxMtlMapping2DMode mappingMode);
    
        ofxUICanvas *_gridSettingsCanvas;
    
        ofImage _fullscreenExpandIcon;
        ofImage _fullscreenContractIcon;
    
        ofxDetectDisplays _detectDisplays;

};
