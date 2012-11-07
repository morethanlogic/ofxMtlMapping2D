#pragma once

#include "ofEvents.h"

#include "ofxMtlMapping2DMode.h"
#include "ofxUI.h"

#define kControlsMappingToolsPanelWidth 39
#define kControlsMappingShapesListPanelWidth 150


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
class ofxMtlMapping2DControls
{
    
    public:
        static ofxMtlMapping2DControls * mapping2DControls();

        ofxMtlMapping2DControls(int width, const string& file);
        
        void toolsUiEvent(ofxUIEventArgs &event);
        void shapesListUiEvent(ofxUIEventArgs &event);

    
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

        int shapeCounter;
    
        void save();
        void load();
        
        void enable();
        void disable();
        void toggle();
        bool isEnabled();
        bool isHit(int x, int y);
    
        void addShapeToList(int shapeID);
        void clearShapesList();
        void setAsActiveShapeWithId(int shapeID);
        const int& selectedShapeId() { return _selectedShapeId; }
        void resetSelectedShapeId();

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
        static ofxMtlMapping2DControls *_mapping2DControls;
        
        bool _saveMapping;
        bool _loadMapping;
        
        bool _editShapes;
        bool _createNewQuad;
        bool _createNewTriangle;
        bool _showShapesId;
        int _selectedShapeId;
    
        bool _mappingModeChanged;
        ofxMtlMapping2DMode _mappingMode;
        
        ofxUIScrollableCanvas *_shapesListCanvas;
        void resizeShapeList();

};
