#include "ofxMtlMapping2DControls.h"

#include "ofxMtlMapping2DSettings.h"
#include "ofxMtlMapping2DShape.h"
#include "ofxMtlMapping2DShapes.h"
#include "ofxMtlMapping2DGrid.h"

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------

const int kSliderHeight = 16;
const int kSpacerHeight = 20;
const int kToggleSize = 24;
const int kBottomSpacerHeight = 100; // padding to be able to scroll until the end/bottom of the UI canvas

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DControls * ofxMtlMapping2DControls::_mapping2DControls = NULL;

//--------------------------------------------------------------
ofxMtlMapping2DControls * ofxMtlMapping2DControls::mapping2DControls(ofxMtlMapping2D * mtlMapping2D, string xmlFilePath)
{
    if (_mapping2DControls == NULL) {
        _mapping2DControls = new ofxMtlMapping2DControls(mtlMapping2D, xmlFilePath);
    }
    return _mapping2DControls;
}

//--------------------------------------------------------------
ofxMtlMapping2DControls * ofxMtlMapping2DControls::mapping2DControls()
{
    if (_mapping2DControls == NULL) {
        ofLog(OF_LOG_WARNING, "ofxMtlMapping2DControls uses a default path to an xml file for saving and loading settings!");
        //_mapping2DControls = new ofxMtlMapping2DControls("mapping/controls/mapping.xml");
    }
    return _mapping2DControls;
}

//--------------------------------------------------------------
ofxMtlMapping2DControls::ofxMtlMapping2DControls(ofxMtlMapping2D * mtlMapping2D, const string& file)
{
    _mtlMapping2D = mtlMapping2D;

    _rootPath = "../../../data/mapping/controls/";
    _file = file;
    
    ofColor uiColor;
    uiColor.set(0, 210, 255, 130);
    ofColor uiColorB;
    uiColorB.set(0, 210, 255, 90);
    
#if defined(USE_OFX_DETECT_DISPLAYS)
#if defined(TARGET_OSX)
    // You should only use the shared instance of ofxDetectDisplays,
    // otherwise event registration will be messed up.
	// Events only works on Mac for now.
    ofAddListener(ofxDetectDisplaysSharedInstance().displayConfigurationChanged, this, &ofxMtlMapping2DControls::displayConfigurationChanged);
#endif
#endif


    
//    int maxDisplayWidth = 0;
//    int maxDisplayHeight = 0;
//    
//    for (int i=0; i<ofxDetectDisplaysSharedInstance().getDisplays().size(); i++) {
//        maxDisplayWidth = MAX(ofxDetectDisplaysSharedInstance().getDisplays()[i]->width, maxDisplayWidth);
//        maxDisplayHeight = MAX(ofxDetectDisplaysSharedInstance().getDisplays()[i]->height, maxDisplayHeight);
//	}
    
    // --- Tool box
    shapeTypesAsString[MAPPING_2D_SHAPE_QUAD] = "quad";
    shapeTypesAsString[MAPPING_2D_SHAPE_GRID] = "grid";
    shapeTypesAsString[MAPPING_2D_SHAPE_TRIANGLE] = "triangle";
    shapeTypesAsString[MAPPING_2D_SHAPE_MASK] = "mask";
    
    // set default values

    static const int kWidgetWidth = kControlsMappingToolsPanelWidth - 16;
    _toolsCanvas = new ofxUICanvas(0, 0, kControlsMappingToolsPanelWidth, ofGetHeight());
    _toolsCanvas->setColorBack(uiColor);
    
    // --- Fullscreen
    _fullscreenExpandIcon.loadImage("GUI/expand.png");
    _fullscreenContractIcon.loadImage("GUI/contract.png");
    
    _toolsCanvas->addImageToggle(kSettingMappingFullscreen, "GUI/expand.png", false, kToggleSize, kToggleSize);
    
    // Edit
    _toolsCanvas->addImageToggle(kSettingMappingSettings, "GUI/settings-3.png", false, kToggleSize, kToggleSize);
    _toolsCanvas->addImageToggle(kSettingMappingEditShapes, "GUI/edit.png", false, kToggleSize, kToggleSize);
    _toolsCanvas->addImageButton(kSettingMappingSave, "GUI/file-down.png", &_mtlMapping2D->bSaveShapes, kToggleSize, kToggleSize);
    _toolsCanvas->addImageButton(kSettingMappingLoad, "GUI/file-up.png", &_mtlMapping2D->bLoadShapes, kToggleSize, kToggleSize);
    
    // Mapping controls Output / Input
    ofxUISpacer *spacer = new ofxUISpacer(kWidgetWidth, kSpacerHeight);
    spacer->setDrawFill(false);
    
    _toolsCanvas->addWidgetDown(spacer);
    _toolsCanvas->addImageToggle(kSettingMappingModeOutput, "GUI/projo.png", true, kToggleSize, kToggleSize);
    _toolsCanvas->addImageToggle(kSettingMappingModeInput, "GUI/texture.png", false, kToggleSize, kToggleSize);
    
    // Mapping shape controls
    if (ofxMtlMapping2DSettings::kIsManuallyCreatingShapeEnabled) {
        _toolsCanvas->addWidgetDown(spacer);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewQuad, "GUI/quad.png", &_mtlMapping2D->bCreateQuad, kToggleSize, kToggleSize);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewGrid, "GUI/grid.png", &_mtlMapping2D->bCreateGrid, kToggleSize, kToggleSize);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewTriangle, "GUI/triangle.png", &_mtlMapping2D->bCreateTriangle, kToggleSize, kToggleSize);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewMask, "GUI/mask.png", &_mtlMapping2D->bCreateMask, kToggleSize, kToggleSize);
    }
    
    ofAddListener(_toolsCanvas->newGUIEvent, this, &ofxMtlMapping2DControls::toolsUiEvent);
    _uiSuperCanvases.push_back(_toolsCanvas);
    
    // ---
    // Output Settings UI
    _settingsUI = new ofxUISuperCanvas("OUTPUT SETTINGS", OFX_UI_FONT_SMALL);
    _settingsUI->setPosition(ofGetWindowWidth() - _settingsUI->getRect()->width, 0);
    _settingsUI->setWidgetFontSize(OFX_UI_FONT_SMALL);
    _settingsUI->setColorBack(uiColor);
    
    _settingsUI->addSpacer();
    _settingsUI->addButton("SAVE", false);
    _settingsUI->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    _settingsUI->addButton("LOAD", false);
    _settingsUI->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
 
#if defined(USE_OFX_DETECT_DISPLAYS)
    _settingsUI->addSpacer();
    _settingsUI->addButton("DETECT DISPLAYS", false);
    _settingsUI->addSpacer();
#endif

    _settingsUI->autoSizeToFitWidgets();
    ofAddListener(_settingsUI->newGUIEvent, this, &ofxMtlMapping2DControls::settingsUiEvent);
    _settingsUI->disable();
    _uiSuperCanvases.push_back(_settingsUI);

#if defined(USE_OFX_DETECT_DISPLAYS)
    // ---
    // Displays Settings UI
    _displaysUI = new ofxUISuperCanvas("DISPLAYS SETTINGS", OFX_UI_FONT_SMALL);
    _displaysUI->setPosition(ofGetWindowWidth() - _displaysUI->getRect()->width, _settingsUI->getRect()->height);
    _displaysUI->setWidgetFontSize(OFX_UI_FONT_SMALL);
    _displaysUI->setColorBack(uiColor);
    
    displayConfigurationChanged();
    
    ofAddListener(_displaysUI->newGUIEvent, this, &ofxMtlMapping2DControls::displaysUiEvent);
    _displaysUI->disable();
    _uiSuperCanvases.push_back(_displaysUI);
#endif
    
    // ---
    // Shapes List UI    
    _shapesListCanvas = new ofxUIScrollableCanvas(kControlsMappingToolsPanelWidth, 0, kControlsMappingShapesListPanelWidth, ofGetHeight());
    _shapesListCanvas->setScrollArea(kControlsMappingToolsPanelWidth, 0, kControlsMappingShapesListPanelWidth, ofGetHeight());
    _shapesListCanvas->setScrollableDirections(false, true);
    _shapesListCanvas->setColorBack(uiColorB);
    _shapesListCanvas->autoSizeToFitWidgets();
    
    ofAddListener(_shapesListCanvas->newGUIEvent, this, &ofxMtlMapping2DControls::shapesListUiEvent);
    _uiSuperCanvases.push_back(_shapesListCanvas);
    
    
    // ---
    // Grid Settings UI
    int gridSettingCanvasWidth = 200.0f;
    _gridSettingsCanvas = new ofxUICanvas();
    _gridSettingsCanvas->setPosition(kControlsMappingToolsPanelWidth, ofGetHeight() - 90);
    _gridSettingsCanvas->setWidth(gridSettingCanvasWidth);
    _gridSettingsCanvas->setColorBack(uiColorB);
    
    _gridSettingsCanvas->addLabel("GRID SETTINGS");
    _gridSettingsCanvas->addIntSlider("NB COLS", 1, 20, &ofxMtlMapping2DSettings::gridDefaultNbCols);
    _gridSettingsCanvas->addIntSlider("NB ROWS", 1, 20, &ofxMtlMapping2DSettings::gridDefaultNbRows);
    
    _gridSettingsCanvas->autoSizeToFitWidgets();
    ofAddListener(_gridSettingsCanvas->newGUIEvent, this, &ofxMtlMapping2DControls::gridSettingsListUiEvent);
    _gridSettingsCanvas->disable();
    _uiSuperCanvases.push_back(_gridSettingsCanvas);
    

    // ---
    // Load UI XML files and initialize
    load();
    
    if (getToggleValue(_toolsCanvas, kSettingMappingModeOutput)) {
        ofxMtlMapping2DGlobal::setEditView(MAPPING_OUTPUT_VIEW);
    
    } else if (getToggleValue(_toolsCanvas, kSettingMappingModeInput)) {
        ofxMtlMapping2DGlobal::setEditView(MAPPING_INPUT_VIEW);
    }
    
    // ---
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingEditShapes))->setValue(false);
    setUIShapeEditingState(false);
}

//--------------------------------------------------------------
ofxMtlMapping2DControls::~ofxMtlMapping2DControls()
{
    while(!_uiSuperCanvases.empty()) delete _uiSuperCanvases.back(), _uiSuperCanvases.pop_back();
	_uiSuperCanvases.clear();
}


#pragma mark -
#pragma mark Tool Box

//--------------------------------------------------------------
void ofxMtlMapping2DControls::toolsUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    if (name == kSettingMappingFullscreen) {        
        ofSetFullscreen(getToggleValue(_toolsCanvas, name));
    }
    
    // --- Settings
    else if (name == kSettingMappingSettings) {
        if(getToggleValue(_toolsCanvas, name)) {
            _settingsUI->enable();
#if defined(USE_OFX_DETECT_DISPLAYS)
            _displaysUI->enable();
#endif
        } else {
            _settingsUI->disable();
#if defined(USE_OFX_DETECT_DISPLAYS)
            _displaysUI->disable();
#endif
        }
    }
    
    // --- Editing
    else if (name == kSettingMappingEditShapes) {
        setUIShapeEditingState(getToggleValue(_toolsCanvas, name));
        
    }
    
    // ---
    if (getToggleValue(_toolsCanvas, name)) {
        unselectShapesToggles();
        ofxMtlMapping2DShape::resetActiveShapeVars();
        ofxMtlMapping2DPolygon::resetActivePolygonVars();
        
        if (name == kSettingMappingModeOutput) {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_CHANGE_TO_OUTPUT_VIEW);
            refreshShapesListForMappingView(MAPPING_CHANGE_TO_OUTPUT_VIEW);

            ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setValue(false);
            
            // ---
            if (ofxMtlMapping2DSettings::kIsManuallyCreatingShapeEnabled) {
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(true);
            }
            
            // ---
            showGridSettingsCanvas();
        }
        else if (name == kSettingMappingModeInput) {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_CHANGE_TO_INPUT_VIEW);
            refreshShapesListForMappingView(MAPPING_CHANGE_TO_INPUT_VIEW);

            ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeOutput))->setValue(false);
            
            // ---
            if (ofxMtlMapping2DSettings::kIsManuallyCreatingShapeEnabled) {
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(false);
            }
            
            // ---
            hideGridSettingsCanvas();
        }
    }
}


//--------------------------------------------------------------
void ofxMtlMapping2DControls::settingsUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
#if defined(USE_OFX_DETECT_DISPLAYS)
    if(name == "DETECT DISPLAYS") {
        if (getButtonValue(_settingsUI, "DETECT DISPLAYS")) {
            cout << "DETECT DISPLAYS" << endl;

            ofxDetectDisplaysSharedInstance().detectDisplays();
            
        } else {
            cout << "BUILD UI DISPLAYS" << endl;
            displayConfigurationChanged();
        }
    }
#endif

}

#if defined(USE_OFX_DETECT_DISPLAYS)
//--------------------------------------------------------------
void ofxMtlMapping2DControls::displaysUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();

    if(name == "DISPLAYS") {
        ofxUIRadio *radio = (ofxUIRadio *) event.widget;
        ofxDetectDisplaysSharedInstance().fullscreenWindowOnDisplay(radio->getValue());
    }
}
#endif

//--------------------------------------------------------------
void ofxMtlMapping2DControls::setUIShapeEditingState(bool isOn)
{
    
    if (!isOn) {
        _mtlMapping2D->setModeState(MAPPING_LOCKED);
        
        // Set all Shapes and their InteractiveObj
        list<ofxMtlMapping2DShape*>::iterator it;
        for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            shape->setAsIdle();
        }
        
        // ---
        ofxMtlMapping2DShape::resetActiveShapeVars();
        ofxMtlMapping2DShape::resetActivePolygonVars();
        
        // ---
        _shapesListCanvas->disable();
        hideGridSettingsCanvas();
        
    } else {
        _mtlMapping2D->setModeState(MAPPING_EDIT);
        
        _shapesListCanvas->enable();
        showGridSettingsCanvas();
    }
    
    // ---
    ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingSave))->setVisible(isOn);
    ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingLoad))->setVisible(isOn);

    ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setVisible(isOn);
    ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeOutput))->setVisible(isOn);
    
    if (ofxMtlMapping2DSettings::kIsManuallyCreatingShapeEnabled) {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(isOn);
    }
}


#pragma mark -
#pragma mark Shapes List Related

//--------------------------------------------------------------
void ofxMtlMapping2DControls::shapesListUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    vector<string> result = ofSplitString(name, " ");
    int shapeId = ofToInt(result[1]);
    string shapeTypeAsString = result[2];
    int shapeType = -1;
    
    map<ofxMtlMapping2DShapeType,string>::iterator it;
    for ( it = shapeTypesAsString.begin() ; it != shapeTypesAsString.end(); it++ ) {
        if (it->second == shapeTypeAsString)
        {
            shapeType = it->first;
            break;
        }
    }
    
    setAsActiveShapeWithId(shapeId, shapeType);
    
    _mtlMapping2D->selectedShapeId = shapeId;
    _mtlMapping2D->bSelectedShapeChanged = true;
}


//--------------------------------------------------------------
void ofxMtlMapping2DControls::addShapeToList(int shapeID, int shapeType)
{
    _shapesListCanvas->addWidgetDown(new ofxUIToggle(("Shape " + ofToString(shapeID) + " " + shapeTypesAsString.find((ofxMtlMapping2DShapeType)shapeType)->second), false, kToggleSize, kToggleSize));

    resizeShapeList();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::clearShapesList()
{
    _shapesListCanvas->removeWidgets();
    _shapesListCanvas->resetPlacer();
    resizeShapeList();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::refreshShapesListForMappingView(MappingEditView currView)
{
    clearShapesList();
    
    // Re populate the UI List
    ofxMtlMapping2DShapes::pmShapes.sort(Comparator());

    list<ofxMtlMapping2DShape*>::reverse_iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.rbegin(); it!=ofxMtlMapping2DShapes::pmShapes.rend(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
        if (currView == MAPPING_CHANGE_TO_OUTPUT_VIEW || (currView == MAPPING_CHANGE_TO_INPUT_VIEW && shape->shapeType != MAPPING_2D_SHAPE_MASK)) {
            ofxMtlMapping2DControls::mapping2DControls()->addShapeToList(shape->shapeId, shape->shapeType);
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::resizeShapeList()
{
    _shapesListCanvas->autoSizeToFitWidgets();
    _shapesListCanvas->getRect()->setY(.0f);
    _shapesListCanvas->getRect()->setWidth(kControlsMappingShapesListPanelWidth);
    
    float listHeight = _shapesListCanvas->getRect()->height;
    if(listHeight < ofGetHeight()) {
        _shapesListCanvas->getRect()->setHeight(ofGetHeight());
    } else {
        _shapesListCanvas->getRect()->setHeight(listHeight + kBottomSpacerHeight);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::setAsActiveShapeWithId(int shapeID, int shapeType)
{    
    // ---
    for (int i=0; i < _shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE).size(); i++) {
        ofxUIToggle * shapeToggle = (ofxUIToggle *)_shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE)[i];
        if ( shapeToggle->getName() == ("Shape " + ofToString(shapeID) + " " + shapeTypesAsString.find((ofxMtlMapping2DShapeType)shapeType)->second) ) {
            shapeToggle->setValue(true);
        } else {
            shapeToggle->setValue(false);
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::unselectShapesToggles()
{
    for (int i=0; i < _shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE).size(); i++) {
        ofxUIToggle * shapeToggle = (ofxUIToggle *)_shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE)[i];
        shapeToggle->setValue(false);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::windowResized()
{
    if (ofGetWindowMode() == OF_FULLSCREEN) {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setImage(&_fullscreenContractIcon);
    } else {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setImage(&_fullscreenExpandIcon);
    }
    
    _toolsCanvas->setHeight(ofGetHeight());
    _gridSettingsCanvas->setPosition(_toolsCanvas->getRect()->width, ofGetHeight() - 90);
    _settingsUI->setPosition(ofGetWidth() - _settingsUI->getRect()->width, 0);
#if defined(USE_OFX_DETECT_DISPLAYS)
    _displaysUI->setPosition(ofGetWidth() - _displaysUI->getRect()->width, _settingsUI->getRect()->height);
#endif
}

#pragma mark -
#pragma mark Settings UI

#if defined(USE_OFX_DETECT_DISPLAYS)

//--------------------------------------------------------------
void ofxMtlMapping2DControls::displayConfigurationChanged()
{
    _displaysUI->removeWidgets();
    
    vector<string> displayNames;
    for (int i=0; i<ofxDetectDisplaysSharedInstance().getDisplays().size(); i++) {
        displayNames.push_back(ofToString(ofxDetectDisplaysSharedInstance().getDisplays()[i]->width) + "x" + ofToString(ofxDetectDisplaysSharedInstance().getDisplays()[i]->height));
    }
    _displaysUI->addRadio("DISPLAYS", displayNames);
    
    _displaysUI->autoSizeToFitWidgets();
}

#endif

#pragma mark -
#pragma mark Grid settings

//--------------------------------------------------------------
void ofxMtlMapping2DControls::gridSettingsListUiEvent(ofxUIEventArgs &event)
{
    if(ofxMtlMapping2DShape::activeShape) {
        if (ofxMtlMapping2DShape::activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {
            ((ofxMtlMapping2DGrid*)ofxMtlMapping2DShape::activeShape)->updateGrid();
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::showGridSettingsCanvas()
{
    if(isEnabled() && ofxMtlMapping2DShape::activeShape) {
        if (ofxMtlMapping2DShape::activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {
            _gridSettingsCanvas->removeWidgets();
            _gridSettingsCanvas->resetPlacer();
            
            ofxUISlider *nSlider;
            _gridSettingsCanvas->addLabel("GRID SETTINGS");
            nSlider = _gridSettingsCanvas->addSlider("NB COLS", 1.0, 20.0, &(((ofxMtlMapping2DGrid*)ofxMtlMapping2DShape::activeShape)->gridNbCols));
            nSlider->setIncrement(1.0f);
            nSlider = _gridSettingsCanvas->addSlider("NB ROWS", 1.0, 20.0, &(((ofxMtlMapping2DGrid*)ofxMtlMapping2DShape::activeShape)->gridNbRows));
            nSlider->setIncrement(1.0f);
             
            _shapesListCanvas->autoSizeToFitWidgets();

            _gridSettingsCanvas->enable();
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::hideGridSettingsCanvas()
{
    _gridSettingsCanvas->disable();
}

#pragma mark -
#pragma mark Set avalaible options
//--------------------------------------------------------------
void ofxMtlMapping2DControls::showInputModeToggle()
{
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setVisible(true);
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::hideInputModeToggle()
{
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setVisible(false);
}

#pragma mark -
#pragma mark Interface Methods

//--------------------------------------------------------------
bool ofxMtlMapping2DControls::getButtonValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUIButton *)ui->getWidget(name))->getValue();
}


//--------------------------------------------------------------
bool ofxMtlMapping2DControls::getToggleValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUIToggle *)ui->getWidget(name))->getValue();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::setToggleValue(ofxUICanvas* ui, const string& name, float value)
{
    ((ofxUIToggle *)ui->getWidget(name))->setValue(value);
}

//--------------------------------------------------------------
float ofxMtlMapping2DControls::getSliderValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUISlider *)ui->getWidget(name))->getScaledValue();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::setSliderValue(ofxUICanvas* ui, const string& name, float value)
{
    ((ofxUISlider *)ui->getWidget(name))->setValue(value);
}

#pragma mark -
//--------------------------------------------------------------
void ofxMtlMapping2DControls::save()
{
//    for (int i=0; i<_uiSuperCanvases.size(); i++) {
//        _uiSuperCanvases[i]->saveSettings(_rootPath + _uiSuperCanvases[i]->getCanvasTitle()->getLabel() + ".xml");
//    }
    
//    _toolsCanvas->saveSettings(_file);

}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::load()
{
//    for (int i=0; i<_uiSuperCanvases.size(); i++) {
//        _uiSuperCanvases[i]->loadSettings(_rootPath + _uiSuperCanvases[i]->getCanvasTitle()->getLabel() + ".xml");
//    }
    
//    _toolsCanvas->loadSettings(_file);

}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::enable()
{
    for (int i=0; i<_uiSuperCanvases.size(); i++) {
        _uiSuperCanvases[i]->enable();
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::disable()
{
    for (int i=0; i<_uiSuperCanvases.size(); i++) {
        _uiSuperCanvases[i]->disable();
    }
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControls::isEnabled()
{
    return _toolsCanvas->isEnabled();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControls::toggleVisible()
{
    _toolsCanvas->toggleVisible();
    
    if (_toolsCanvas->isVisible() && _mtlMapping2D->getModeState() == MAPPING_EDIT) {
        _shapesListCanvas->enable();
        showGridSettingsCanvas();
    } else {
        _shapesListCanvas->disable();
        hideGridSettingsCanvas();
    }
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControls::isVisible()
{
    return _toolsCanvas->isVisible();
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControls::isHit(int x, int y) {
    
    for (auto& it : _uiSuperCanvases) {
        if (it->isHit(x, y)) {
            return true;
        }
    }
    
    return false;
}
