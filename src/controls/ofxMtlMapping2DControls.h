#pragma once

#include "ofEvents.h"

#include "ofxMtlMapping2D.h"
#include "ofxMtlMapping2DGlobal.h"

#include "ofxUI.h"
#include "ofxMtlMapping2DShapeType.h"

#if defined(USE_OFX_DETECT_DISPLAYS)
#include "ofxDetectDisplays.h"
#endif

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
class ofxMtlMapping2DControls {
    
public:
    static ofxMtlMapping2DControls* sharedInstance();

    void toolsUiEvent(ofxUIEventArgs &event);
    void settingsUiEvent(ofxUIEventArgs &event);

    void shapesListUiEvent(ofxUIEventArgs &event);
    void gridSettingsListUiEvent(ofxUIEventArgs &event);

    void showGridSettingsCanvas();
    void hideGridSettingsCanvas();

    void save();
    void load();
    
    void saveOutputSettings();
    void loadOutputSettings();

    void showInputModeToggle();
    void hideInputModeToggle();

    void enable();
    void disable();

    void toggleVisible();
    bool isVisible();
    bool isEnabled();
    bool isHit(int x, int y);

    void addShapeToList(int shapeID, int shapeType);
    void clearShapesList();

    void setAsActiveShapeWithId(int shapeID, int shapeType);

    void unselectShapesToggles();
    void windowResized();

#if defined(USE_OFX_DETECT_DISPLAYS)
    void displayConfigurationChanged();
    void displaysUiEvent(ofxUIEventArgs &event);
#endif

private:

    ofxMtlMapping2DControls();
    ~ofxMtlMapping2DControls();

    string _rootPath;

    vector<ofxUICanvas*> _uiSuperCanvases;

    ofxUICanvas *_toolsCanvas;
    ofxUISuperCanvas* _settingsUI;
    ofxUIScrollableCanvas *_shapesListCanvas;
    ofxUICanvas *_gridSettingsCanvas;
    
#if defined(USE_OFX_DETECT_DISPLAYS)
    ofxUISuperCanvas* _displaysUI;
    vector<string> _displayNames;
#endif

    void setUIShapeEditingState(bool isOn);
    void resizeShapeList();
    void refreshShapesListForMappingView(MappingEditView currView);

    bool getButtonValue(ofxUICanvas* ui, const string& name);
    
    bool getToggleValue(ofxUICanvas* ui, const string& name);
    void setToggleValue(ofxUICanvas* ui, const string& name, float value);
    
    float getSliderValue(ofxUICanvas* ui, const string& name);
    void setSliderValue(ofxUICanvas* ui, const string& name, float value);

    map<ofxMtlMapping2DShapeType, string> shapeTypesAsString;
    ofImage _fullscreenExpandIcon;
    ofImage _fullscreenContractIcon;
    
};


//--------------------------------------------------------------
ofxMtlMapping2DControls& ofxMtlMapping2DControlsSharedInstance(ofxMtlMapping2D * mtlMapping2D = NULL);
