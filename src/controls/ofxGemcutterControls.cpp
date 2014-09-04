#include "ofxGemcutterControls.h"

#include "ofxGemcutterSettings.h"
#include "ofxGemcutterShape.h"
#include "ofxGemcutterShapes.h"
#include "ofxGemcutterGrid.h"

#include "ofMain.h"

//--------------------------------------------------------------
//--------------------------------------------------------------

const int kSliderHeight = 16;
const int kSpacerHeight = 20;
const int kToggleSize = 24;
const int kBottomSpacerHeight = 100; // padding to be able to scroll until the end/bottom of the UI canvas
const string uiDataPath = "ui/";
bool _bInitialized = false;

//--------------------------------------------------------------
//--------------------------------------------------------------
static ofxGemcutter * _mtlMapping2D = NULL;

//--------------------------------------------------------------
ofxGemcutterControls* ofxGemcutterControls::sharedInstance()
{
    static ofxGemcutterControls* instance = NULL;
    if (instance == NULL) {
        instance = new ofxGemcutterControls();
    }
    return instance;
}

//--------------------------------------------------------------
ofxGemcutterControls& ofxGemcutterControlsSharedInstance(ofxGemcutter * mtlMapping2D)
{
    //if (mtlMapping2D == NULL && _mtlMapping2D == NULL) {
    //    ofLogError() << "You need to pass a reference to the ofxGemcutter the Controls before going further.";
    //    return;
   // } else 
	if (mtlMapping2D != NULL && _mtlMapping2D == NULL) {
        _mtlMapping2D = mtlMapping2D;
    }
    
    return *ofxGemcutterControls::sharedInstance();
}

//--------------------------------------------------------------
ofxGemcutterControls::ofxGemcutterControls() //ofxGemcutter * mtlMapping2D)
{
    _rootPath = "settings/";
 
    uiColor.set(83, 83, 83, 160);
    uiColorB.set(203, 203, 203, 130);
    
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
    _fullscreenExpandIcon.loadImage(uiDataPath + "expand.png");
    _fullscreenContractIcon.loadImage(uiDataPath + "contract.png");
    _lockedIcon.loadImage(uiDataPath + "locked.png");
    _unlockedIcon.loadImage(uiDataPath + "unlocked.png");
    
    _toolsCanvas->addImageToggle(kSettingMappingFullscreen, uiDataPath + "expand.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    
    // Edit
    _toolsCanvas->addImageToggle(kSettingMappingSettings, uiDataPath + "settings-3.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageToggle(kSettingMappingEditShapes, uiDataPath + "edit.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageButton(kSettingMappingSave, uiDataPath + "file-down.png", &_mtlMapping2D->bSaveShapes, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageButton(kSettingMappingLoad, uiDataPath + "file-up.png", &_mtlMapping2D->bLoadShapes, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    
    // Mapping controls Output / Input
    ofxUISpacer *spacer = new ofxUISpacer(kWidgetWidth, kSpacerHeight);
    spacer->setDrawFill(false);
    
    _toolsCanvas->addWidgetDown(spacer);
    _toolsCanvas->addImageToggle(kSettingMappingModeOutput, uiDataPath + "projo.png", true, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageToggle(kSettingMappingModeInput, uiDataPath + "texture.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    
    // Mapping shape controls
    if (ofxGemcutterSettings::kIsManuallyCreatingShapeEnabled) {
        _toolsCanvas->addWidgetDown(spacer);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewQuad, uiDataPath + "quad.png", &_mtlMapping2D->bCreateQuad, kToggleSize, kToggleSize)->setColorBack(uiColorB);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewGrid, uiDataPath + "grid.png", &_mtlMapping2D->bCreateGrid, kToggleSize, kToggleSize)->setColorBack(uiColorB);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewTriangle, uiDataPath + "triangle.png", &_mtlMapping2D->bCreateTriangle, kToggleSize, kToggleSize)->setColorBack(uiColorB);
        _toolsCanvas->addImageToggle(kSettingMappingCreateNewMask, uiDataPath + "mask.png", &_mtlMapping2D->bCreateMask, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    }
    
    _toolsCanvas->addWidgetDown(spacer);
    _toolsCanvas->addImageButton("ZOOM FIT", uiDataPath + "zoomFit.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageButton("ZOOM IN", uiDataPath + "zoomIn.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageButton("ZOOM OUT", uiDataPath + "zoomOut.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _toolsCanvas->addImageToggle("ZOOM DRAG", uiDataPath + "zoomDrag.png", &ofxGemcutterGlobal::bIsDraggingZone, kToggleSize, kToggleSize)->setColorBack(uiColorB);

    
    ofAddListener(_toolsCanvas->newGUIEvent, this, &ofxGemcutterControls::toolsUiEvent);
    _uiSuperCanvases.push_back(_toolsCanvas);
    
    // ---
    // Output Settings UI
    _settingsUI = new ofxUISuperCanvas("SETTINGS");
    _settingsUI->setColorBack(uiColor);
    
    _settingsUI->addSpacer((_settingsUI->getRect()->width - 10) / 2, 1);
    _settingsUI->addToggle("LOAD FILE ON START", false);
    _settingsUI->addButton("SELECT FILE", false);
    _settingsUI->addTextInput("FILE PATH", "NONE", OFX_UI_FONT_SMALL);
    
    _settingsUI->addSpacer((_settingsUI->getRect()->width - 10) / 2, 1);
    _settingsUI->addLabel("PROJECTION SIZE");
    _settingsUI->addTextInput("PROJ. WIDTH", ofToString(ofGetWidth()));
    _settingsUI->addTextInput("PROJ. HEIGHT", ofToString(ofGetHeight()));

    _settingsUI->autoSizeToFitWidgets();
    ofAddListener(_settingsUI->newGUIEvent, this, &ofxGemcutterControls::settingsUiEvent);
    _settingsUI->disable();
    _uiSuperCanvases.push_back(_settingsUI);
    
    // ---
    // Shapes List UI    
    _shapesListCanvas = new ofxUIScrollableCanvas(kControlsMappingToolsPanelWidth, 0, kControlsMappingShapesListPanelWidth, ofGetHeight());
    _shapesListCanvas->setScrollArea(kControlsMappingToolsPanelWidth, 0, kControlsMappingShapesListPanelWidth, ofGetHeight());
    _shapesListCanvas->setScrollableDirections(false, false);
    _shapesListCanvas->setColorBack(uiColorB);
    _shapesListCanvas->autoSizeToFitWidgets();
    
    ofAddListener(_shapesListCanvas->newGUIEvent, this, &ofxGemcutterControls::shapesListUiEvent);
    _uiSuperCanvases.push_back(_shapesListCanvas);
    
    
    // ---
    // Grid Settings UI
    int gridSettingCanvasWidth = 200.0f;
    _gridSettingsCanvas = new ofxUICanvas();
    _gridSettingsCanvas->setWidth(gridSettingCanvasWidth);
    _gridSettingsCanvas->setColorBack(uiColorB);
    
    _gridSettingsCanvas->addLabel("GRID SETTINGS");
    _gridSettingsCanvas->addIntSlider("NB COLS", 1, 20, &ofxGemcutterSettings::gridDefaultNbCols);
    _gridSettingsCanvas->addIntSlider("NB ROWS", 1, 20, &ofxGemcutterSettings::gridDefaultNbRows);
    
    _gridSettingsCanvas->autoSizeToFitWidgets();
    ofAddListener(_gridSettingsCanvas->newGUIEvent, this, &ofxGemcutterControls::gridSettingsListUiEvent);
    _gridSettingsCanvas->disable();
    _uiSuperCanvases.push_back(_gridSettingsCanvas);
    
    
#if defined(USE_OFX_DETECT_DISPLAYS)
    // ---
    // Output Settings UI
    _currActiveDisplayName = "";
    
    _outputUI = new ofxUISuperCanvas("OUTPUT WINDOW");
    _outputUI->setColorBack(uiColor);
    
    _outputUI->addSpacer(_outputUI->getRect()->width - 10, 2);
    
    _outputUI->addButton("DETECT DISPLAYS", false);
    
    ofAddListener(_outputUI->newGUIEvent, this, &ofxGemcutterControls::outputUiEvent);
    _outputUI->autoSizeToFitWidgets();
    _outputUI->disable();
    _uiSuperCanvases.push_back(_outputUI);
    
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    // ---
    // Video player UI
    _videoPlayIcon.loadImage(uiDataPath + "play.png");
    _videoPauseIcon.loadImage(uiDataPath + "pause.png");
    
    _videoPlayerUI = new ofxUISuperCanvas("VIDEO PLAYER");
    _videoPlayerUI->setColorBack(uiColor);
    
    _videoPlayerUI->addButton("NONE", false);
    _videoPlayerUI->addToggle("LOAD VIDEO ON START", false);
    _videoPlayerUI->addButton("SELECT FILE", false);
    _videoPlayerUI->addTextInput("FILE PATH", "NONE", OFX_UI_FONT_SMALL);
    
    _videoPlayerUI->addSpacer(_videoPlayerUI->getRect()->width - 10, 2);
    _videoPlayerUI->addImageButton("STOP", uiDataPath + "stop.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _videoPlayerUI->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    _videoPlayerUI->addImageToggle("PLAY", uiDataPath + "play.png", false, kToggleSize, kToggleSize)->setColorBack(uiColorB);
    _videoPlayerUI->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    _videoPlayerUI->addMinimalSlider("TRACK", .0f, 100.0f, &_mtlMapping2D->videoPositionInSeconds);
    
    ofAddListener(_videoPlayerUI->newGUIEvent, this, &ofxGemcutterControls::videoPlayerUiEvent);
    _videoPlayerUI->autoSizeToFitWidgets();
    _videoPlayerUI->disable();
    _uiSuperCanvases.push_back(_videoPlayerUI);
#endif
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    // ---
    // Syphon UI
    _syphonUI = new ofxUISuperCanvas("SYPHON SETTINGS");
    
    _syphonUI->setColorBack(uiColor);

    _syphonUI->addButton("NONE", false);
    _syphonUI->addSpacer((_syphonUI->getRect()->width - 10) / 2, 1);
    
    ofAddListener(_syphonUI->newGUIEvent, this, &ofxGemcutterControls::syphonUiEvent);
    _syphonUI->autoSizeToFitWidgets();
    _syphonUI->disable();
    _uiSuperCanvases.push_back(_syphonUI);
#endif

    // ---
#if defined(USE_OFX_DETECT_DISPLAYS)
    #if defined(TARGET_OSX)
        // You should only use the shared instance of ofxDetectDisplays,
        // otherwise event registration will be messed up.
        // Events only works on Mac for now.
        ofAddListener(ofxDetectDisplaysSharedInstance().displayConfigurationChanged, this, &ofxGemcutterControls::displayConfigurationChanged);
    #endif
    displayConfigurationChanged();
#endif
    
    // ---
    updateUIsPosition();
    
}

//--------------------------------------------------------------
void ofxGemcutterControls::init()
{
    // ---
    // Load UI XML files and initialize
    loadSettings();
    
    if (getToggleValue(_toolsCanvas, kSettingMappingModeOutput)) {
        ofxGemcutterGlobal::setEditView(MAPPING_OUTPUT_VIEW);
    
    } else if (getToggleValue(_toolsCanvas, kSettingMappingModeInput)) {
        ofxGemcutterGlobal::setEditView(MAPPING_INPUT_VIEW);
    }
    
    // ---
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingEditShapes))->setValue(false);
    setUIShapeEditingState(false);
    
    disable();
    
    _bHasFocus = false;
    
    _bInitialized = true;
}

//--------------------------------------------------------------
void ofxGemcutterControls::exit()
{
    cout << "ofxGemcutterControls::exit()" << endl;

    saveSettings();
}

//--------------------------------------------------------------
ofxGemcutterControls::~ofxGemcutterControls()
{
    while(!_uiSuperCanvases.empty()) delete _uiSuperCanvases.back(), _uiSuperCanvases.pop_back();
	_uiSuperCanvases.clear();
}


#pragma mark -
#pragma mark Tool Box

//--------------------------------------------------------------
void ofxGemcutterControls::toolsUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    if (name == kSettingMappingFullscreen) {
        bool bGoFullscreen = getToggleValue(_toolsCanvas, name);
        ofSetFullscreen(bGoFullscreen);
        
        
        if (bGoFullscreen && _displayNames.size() == 1) {
            _currActiveDisplayName = _displayNames[0];
            setToggleValue(_outputUI, _currActiveDisplayName, true);        
        } else if (!bGoFullscreen && _displayNames.size() == 1) {
            setToggleValue(_outputUI, _currActiveDisplayName, false);
            _currActiveDisplayName = "";
        }
        
#if defined(USE_OFX_DETECT_DISPLAYS)
        /*
        if (!bGoFullscreen) {
            ofxUIRadio* uiRadio = (ofxUIRadio*) _displaysListUI->getWidget("DISPLAYS");
            
            for (int i=0; i<uiRadio->getToggles().size(); i++) {
                uiRadio->getToggles()[i]->setValue(false);
            }
        }
         */
#endif
        
    }
    
    // --- Settings
    else if (name == kSettingMappingSettings) {
        if(getToggleValue(_toolsCanvas, name)) {
            _settingsUI->enable();
#if defined(USE_OFX_DETECT_DISPLAYS)
            _outputUI->enable();
#endif
   
#if defined(USE_VIDEO_PLAYER_OPTION)
            _videoPlayerUI->enable();
#endif
            
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
            _syphonUI->enable();
#endif
        
        } else {
            _settingsUI->disable();
#if defined(USE_OFX_DETECT_DISPLAYS)
            _outputUI->disable();
#endif

#if defined(USE_VIDEO_PLAYER_OPTION)
            _videoPlayerUI->disable();
#endif
            
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
            _syphonUI->disable();
#endif
        }
    }
    
    // --- Editing
    else if (name == kSettingMappingEditShapes) {
        setUIShapeEditingState(getToggleValue(_toolsCanvas, name));
        
    }
    
    // ---
    else if ((name == kSettingMappingModeOutput || name == kSettingMappingModeInput) && getToggleValue(_toolsCanvas, name)) {
        unselectShapesToggles();
        ofxGemcutterShape::resetActiveShapeVars();
        ofxGemcutterPolygon::resetActivePolygonVars();
        
        setToggleValue(_toolsCanvas, "ZOOM DRAG", false);
        
        if (name == kSettingMappingModeOutput) {
            ofxGemcutterGlobal::setEditView(MAPPING_CHANGE_TO_OUTPUT_VIEW);
            refreshShapesListForMappingView(MAPPING_CHANGE_TO_OUTPUT_VIEW);

            ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setValue(false);
            
            // ---
            if (ofxGemcutterSettings::kIsManuallyCreatingShapeEnabled) {
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(true);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(true);
            }
            
            // ---
            showGridSettingsCanvas();
            
        }
        else if (name == kSettingMappingModeInput) {
            ofxGemcutterGlobal::setEditView(MAPPING_CHANGE_TO_INPUT_VIEW);
            refreshShapesListForMappingView(MAPPING_CHANGE_TO_INPUT_VIEW);

            ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingModeOutput))->setValue(false);
            
            // ---
            if (ofxGemcutterSettings::kIsManuallyCreatingShapeEnabled) {
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(false);
                ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(false);
            }
            
            // ---
            hideGridSettingsCanvas();
            
        }
    }
    
    // ---
    else if (name == "ZOOM FIT" && getButtonValue(_toolsCanvas, name)) {
        _mtlMapping2D->zoomScaleToFit(ofxGemcutterGlobal::getEditView());
    }
    else if (name == "ZOOM IN" && getButtonValue(_toolsCanvas, name)) {
        if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            ofxGemcutterGlobal::inputViewZoomFactor += .05f;
            if (ofxGemcutterGlobal::inputViewZoomFactor > 10.0f) {
                ofxGemcutterGlobal::inputViewZoomFactor = 10.0f;
            }
        } else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
            ofxGemcutterGlobal::outputViewZoomFactor += .05f;
            if (ofxGemcutterGlobal::outputViewZoomFactor > 10.0f) {
                ofxGemcutterGlobal::outputViewZoomFactor = 10.0f;
            }
        }
        
        _mtlMapping2D->updateZoomAndOutput(ofxGemcutterGlobal::getEditView());
    }
    else if (name == "ZOOM OUT" && getButtonValue(_toolsCanvas, name)) {
        if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            ofxGemcutterGlobal::inputViewZoomFactor -= .05f;
            if (ofxGemcutterGlobal::inputViewZoomFactor < .1f) {
                ofxGemcutterGlobal::inputViewZoomFactor = .1f;
            }
        } else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
            ofxGemcutterGlobal::outputViewZoomFactor -= .05f;
            if (ofxGemcutterGlobal::outputViewZoomFactor < .1f) {
                ofxGemcutterGlobal::outputViewZoomFactor = .1f;
            }
        }
        
        _mtlMapping2D->updateZoomAndOutput(ofxGemcutterGlobal::getEditView());
    }
    else if (name == "ZOOM DRAG" && getButtonValue(_toolsCanvas, name)) {
        unselectShapesToggles();
        ofxGemcutterShape::resetActiveShapeVars();
        ofxGemcutterPolygon::resetActivePolygonVars();
    }
}


//--------------------------------------------------------------
void ofxGemcutterControls::settingsUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    if (name == "LOAD FILE ON START"  && getToggleValue(_settingsUI, name)) {
        if (!_bInitialized && extraOutputSettings["path"] != "") {
            _mtlMapping2D->loadXml(extraOutputSettings["path"]);
        }
    }
    
    else if ((name == "SELECT FILE" && getButtonValue(_settingsUI, name)) || (_bInitialized && name == "FILE PATH")) {
        
        ofxUITextInput* textInput = (ofxUITextInput*) _settingsUI->getWidget("FILE PATH");
        
        // If the event comes from the TextInput widget, and the TextInput widget does not have the focus
        // it means that the user just presented 'cancel' in the system dialog window.
        if (name == "FILE PATH" && !textInput->isFocused()) {
            return;
        }
        
        ofFileDialogResult fileDialogResult = ofSystemLoadDialog();
        
        textInput->setFocus(false);
        
        if (!fileDialogResult.bSuccess) {
            return;
        }
        
        string path = fileDialogResult.getPath();
        extraOutputSettings["path"] = path;
        textInput->setTextString(fileDialogResult.getName());
        _mtlMapping2D->loadXml(path);
    }

    else if (name == "PROJ. WIDTH" || name == "PROJ. HEIGHT") {
        ofxUITextInput* textInput = (ofxUITextInput*) _settingsUI->getWidget(name);
        
        if (!textInput->isFocused()) {
            _bHasFocus = false;
            
            // Make sure we have an Int value
            if (name == "PROJ. WIDTH") {
                ofxGemcutterGlobal::outputWidth = ofToInt(textInput->getTextString());
                textInput->setTextString(ofToString(ofxGemcutterGlobal::outputWidth));
            } else {
                ofxGemcutterGlobal::outputHeight = ofToInt(textInput->getTextString());
                textInput->setTextString(ofToString(ofxGemcutterGlobal::outputHeight));
            }
            
            // Update Zoomed area and Resize the FBO
            _mtlMapping2D->updateZoomAndOutput(MAPPING_INPUT_VIEW, true);
            _mtlMapping2D->updateZoomAndOutput(MAPPING_OUTPUT_VIEW, true);
        
        } else {
            _bHasFocus = true;
        }
    }
}

/*
 bool is_number(const std::string& s)
 {
 std::string::const_iterator it = s.begin();
 while (it != s.end() && std::isdigit(*it)) ++it;
 return !s.empty() && it == s.end();
 }
 */

#if defined(USE_OFX_DETECT_DISPLAYS)
//--------------------------------------------------------------
void ofxGemcutterControls::outputUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();

    if(name == "DETECT DISPLAYS") {
        if (getButtonValue(_outputUI, name)) {
            ofxDetectDisplaysSharedInstance().detectDisplays();

        } else {
            displayConfigurationChanged();
        }
    }
    
    else {
        if (name == _currActiveDisplayName) {
            _currActiveDisplayName = "";
            setToggleValue(_outputUI, name, false);

            if (_displayNames.size() == 1) {
                setToggleValue(_toolsCanvas, kSettingMappingFullscreen, false);
                ofSetFullscreen(false);
            } else {
                _mtlMapping2D->closeOutputWindow();
            }
            return;
        }
        
        if (_displayNames.size() == 1) {
            if (name == _displayNames[0] && getToggleValue(_outputUI, name)) {
                _currActiveDisplayName = name;
                setToggleValue(_toolsCanvas, kSettingMappingFullscreen, true);
                ofSetFullscreen(true);

                return;
            }
        } else {
            for (int i=0; i<_displayNames.size(); i++) {
                if (name == _displayNames[i] && getToggleValue(_outputUI, name)) {
                    _currActiveDisplayName = name;
                    _mtlMapping2D->openOuputWindow(ofxDetectDisplaysSharedInstance().getDisplayBounds(i));

                    return;
                }
            }
        }
        
    }
}

#endif

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
//--------------------------------------------------------------
void ofxGemcutterControls::syphonUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();

    if(name == "NONE") {
        if (getButtonValue(_syphonUI, name)) {
            _mtlMapping2D->selectSyphonServer(-1);
            
            for (int i=0; i<_syphonServersNames.size(); i++) {
                setToggleValue(_syphonUI, _syphonServersNames[i], false);
            }
        }
    }
    else {
        for (int i=0; i<_syphonServersNames.size(); i++) {
            if (name == _syphonServersNames[i] && getToggleValue(_syphonUI, name)) {
                _mtlMapping2D->selectSyphonServer(i);
                return;
            }
        }
    }
}
#endif


#if defined(USE_VIDEO_PLAYER_OPTION)
//--------------------------------------------------------------
void ofxGemcutterControls::videoPlayerUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    if (name == "NONE") {
        // TO DO:: Set this to clear the video settings!
    }
    else if ((name == "SELECT FILE" && getButtonValue(_videoPlayerUI, name)) || (_bInitialized && name == "FILE PATH")) {
        
        ofxUITextInput* textInput = (ofxUITextInput*) _videoPlayerUI->getWidget("FILE PATH");
        
        // If the event comes from the TextInput widget, and the TextInput widget does not have the focus
        // it means that the user just presented 'cancel' in the system dialog window.
        if (name == "FILE PATH" && !textInput->isFocused()) {
            return;
        }
        
        ofFileDialogResult fileDialogResult = ofSystemLoadDialog();
        
        textInput->setFocus(false);
        
        if (!fileDialogResult.bSuccess) {
            return;
        }
        
        string path = fileDialogResult.getPath();
        extraOutputSettings["video_file_path"] = path;
        textInput->setTextString(fileDialogResult.getName());
        _mtlMapping2D->loadVideo(path);
        
        // ---
        ((ofxUIImageToggle *)_videoPlayerUI->getWidget("PLAY"))->setImage(&_videoPlayIcon);
        setToggleValue(_videoPlayerUI, "PLAY", false);
    }
    else if (name == "LOAD VIDEO ON START"  && getToggleValue(_videoPlayerUI, name)) {
        if (!_bInitialized && extraOutputSettings["video_file_path"] != "") {
            _mtlMapping2D->loadVideo(extraOutputSettings["video_file_path"]);
        }
    }
    else if (name == "STOP" && getButtonValue(_videoPlayerUI, name)) {
        _mtlMapping2D->stopVideo();
        
        // ---
        ((ofxUIImageToggle *)_videoPlayerUI->getWidget("PLAY"))->setImage(&_videoPlayIcon);
        setToggleValue(_videoPlayerUI, "PLAY", false);
    }
    else if (name == "PLAY") {
        bool toggleValue = getToggleValue(_videoPlayerUI, name);

        if (toggleValue) {
            _mtlMapping2D->playVideo();
            ((ofxUIImageToggle *)_videoPlayerUI->getWidget("PLAY"))->setImage(&_videoPauseIcon);
        } else {
            _mtlMapping2D->pauseVideo();
            ((ofxUIImageToggle *)_videoPlayerUI->getWidget("PLAY"))->setImage(&_videoPlayIcon);
        }
        
    }
    else if (name == "TRACK") {
        _mtlMapping2D->setVideoPostion(getSliderValue(_videoPlayerUI, name));
    }
}

//--------------------------------------------------------------
string ofxGemcutterControls::getVideoFilePath()
{
    return extraOutputSettings["video_file_path"];
}

#endif

//--------------------------------------------------------------
void ofxGemcutterControls::setUIShapeEditingState(bool isOn)
{
    
    if (!isOn) {
        _mtlMapping2D->setModeState(MAPPING_LOCKED);
        
        // Set all Shapes and their InteractiveObj
        list<ofxGemcutterShape*>::iterator it;
        for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
            ofxGemcutterShape* shape = *it;
            shape->setAsIdle();
        }
        
        // ---
        ofxGemcutterShape::resetActiveShapeVars();
        ofxGemcutterShape::resetActivePolygonVars();
        
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
    
    if (ofxGemcutterSettings::kIsManuallyCreatingShapeEnabled) {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewQuad))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewGrid))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewTriangle))->setVisible(isOn);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingCreateNewMask))->setVisible(isOn);
    }
    
    // ---
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM FIT"))->setVisible(isOn);
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM IN"))->setVisible(isOn);
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM OUT"))->setVisible(isOn);
    ((ofxUIImageToggle *)_toolsCanvas->getWidget("ZOOM DRAG"))->setVisible(isOn);
}


#pragma mark -
#pragma mark Shapes List Related

//--------------------------------------------------------------
void ofxGemcutterControls::shapesListUiEvent(ofxUIEventArgs &event)
{
    string name = event.widget->getName();
    
    vector<string> result = ofSplitString(name, " ");
    string action = result[0];
    int shapeId = ofToInt(result[1]);
    string shapeTypeAsString = result[2];
    int shapeType = -1;
    
    if (action == "Select") {
        map<ofxGemcutterShapeType,string>::iterator it;
        for ( it = shapeTypesAsString.begin() ; it != shapeTypesAsString.end(); it++ ) {
            if (it->second == shapeTypeAsString)
            {
                shapeType = it->first;
                break;
            }
        }
        
        //setAsActiveShapeWithId(shapeId, shapeType);
        
        for (int i=0; i < _shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE).size(); i++) {
            ofxUIToggle * shapeToggle = (ofxUIToggle *)_shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE)[i];
            
            string nameB = shapeToggle->getName();
            
            vector<string> resultB = ofSplitString(nameB, " ");
            string actionB = resultB[0];
            int sId = ofToInt(resultB[1]);
            
            if (actionB == "Select") {
                if ( sId != shapeId ) {
                    shapeToggle->setValue(false);
                }
            }
        }
        
        
        _mtlMapping2D->selectShapeId(shapeId);
    }
    else if (action == "Lock") {
        ofxUIImageToggle* uiImageToggle = ((ofxUIImageToggle *)_shapesListCanvas->getWidget(name));
        
        // Lock / Unlock this shape
        if (getToggleValue(_shapesListCanvas, name)) {
            _mtlMapping2D->setLockForShapeId(shapeId, true);
            uiImageToggle->setImage(&_lockedIcon);
        } else {
            _mtlMapping2D->setLockForShapeId(shapeId, false);
            uiImageToggle->setImage(&_unlockedIcon);
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::addShapeToList(int shapeID, int shapeType, bool bLocked)
{
    //_shapesListCanvas->addWidgetDown(new ofxUIToggle(("Shape " + ofToString(shapeID) + " " + shapeTypesAsString.find((ofxGemcutterShapeType)shapeType)->second), false, kToggleSize, kToggleSize));
    _shapesListCanvas->addToggle("Select " + ofToString(shapeID) + " " + shapeTypesAsString.find((ofxGemcutterShapeType)shapeType)->second, false, 16, 16)->setLabelVisible(false);
    _shapesListCanvas->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    ofxUIImageToggle* uiImageToggle = _shapesListCanvas->addImageToggle("Lock " + ofToString(shapeID) + " " + shapeTypesAsString.find((ofxGemcutterShapeType)shapeType)->second, "", bLocked, 16, 16);
    uiImageToggle->setColorBack(uiColorB);
    uiImageToggle->setLabelVisible(false);
    _shapesListCanvas->addLabel(shapeTypesAsString.find((ofxGemcutterShapeType)shapeType)->second  + " " +  ofToString(shapeID));
    
    if (bLocked) {
        uiImageToggle->setImage(&_lockedIcon);
    } else {
        uiImageToggle->setImage(&_unlockedIcon);
    }
    
    _shapesListCanvas->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    resizeShapeList();
}

//--------------------------------------------------------------
void ofxGemcutterControls::clearShapesList()
{
    _shapesListCanvas->removeWidgets();
    _shapesListCanvas->resetPlacer();
    resizeShapeList();
}

//--------------------------------------------------------------
void ofxGemcutterControls::refreshShapesListForMappingView(MappingEditView currView)
{
    clearShapesList();
    
    // Re populate the UI List
    ofxGemcutterShapes::pmShapes.sort(Comparator());

    list<ofxGemcutterShape*>::reverse_iterator it;
    for (it=ofxGemcutterShapes::pmShapes.rbegin(); it!=ofxGemcutterShapes::pmShapes.rend(); it++) {
        ofxGemcutterShape* shape = *it;
        
        if (currView == MAPPING_CHANGE_TO_OUTPUT_VIEW || (currView == MAPPING_CHANGE_TO_INPUT_VIEW && shape->shapeType != MAPPING_2D_SHAPE_MASK)) {
            addShapeToList(shape->shapeId, shape->shapeType, ofToBool(shape->shapeSettings["isLocked"]));
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::resizeShapeList()
{
    _shapesListCanvas->autoSizeToFitWidgets();
    _shapesListCanvas->getRect()->setY(.0f);
    _shapesListCanvas->getRect()->setWidth(kControlsMappingShapesListPanelWidth);
    
    float listHeight = _shapesListCanvas->getRect()->height;
    if(listHeight < ofGetHeight()) {
        //_shapesListCanvas->getRect()->setHeight(ofGetHeight());
        _shapesListCanvas->setScrollableDirections(false, false);
    } else {
        //_shapesListCanvas->getRect()->setHeight(listHeight + kBottomSpacerHeight);
        _shapesListCanvas->setScrollableDirections(false, true);

    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::setAsActiveShapeWithId(int shapeID, int shapeType)
{    
    // ---
    for (int i=0; i < _shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE).size(); i++) {
        ofxUIToggle * shapeToggle = (ofxUIToggle *)_shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE)[i];
        
        string name = shapeToggle->getName();
        
        vector<string> result = ofSplitString(name, " ");
        string action = result[0];
        int sId = ofToInt(result[1]);
        
        if (action == "Select") {
            if ( sId == shapeID ) {
                shapeToggle->setValue(true);
            } else {
                shapeToggle->setValue(false);
            }
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::unselectShapesToggles()
{
    for (int i=0; i < _shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE).size(); i++) {
        ofxUIToggle * shapeToggle = (ofxUIToggle *)_shapesListCanvas->getWidgetsOfType(OFX_UI_WIDGET_TOGGLE)[i];
        shapeToggle->setValue(false);
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::updateUIsPosition()
{
    if (ofGetWindowMode() == OF_FULLSCREEN) {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setImage(&_fullscreenContractIcon);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setValue(true);
    } else {
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setImage(&_fullscreenExpandIcon);
        ((ofxUIImageToggle *)_toolsCanvas->getWidget(kSettingMappingFullscreen))->setValue(false);
    }
    
    float togglePadding = 6.0f;
    float yOffset = ofGetHeight() - (5 * (kToggleSize + togglePadding));
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM FIT"))->getRect()->setY(yOffset);
    yOffset += kToggleSize + togglePadding;
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM IN"))->getRect()->setY(yOffset);
    yOffset += kToggleSize + togglePadding;
    ((ofxUIImageButton *)_toolsCanvas->getWidget("ZOOM OUT"))->getRect()->setY(yOffset);
    yOffset += kToggleSize + togglePadding;
    ((ofxUIImageToggle *)_toolsCanvas->getWidget("ZOOM DRAG"))->getRect()->setY(yOffset);
    
    _toolsCanvas->setHeight(ofGetHeight());
    _gridSettingsCanvas->setPosition(_toolsCanvas->getRect()->width, ofGetHeight() - 90);
    
    // Update the position of the UIs
    float maxWidth = _settingsUI->getRect()->width;
    _settingsUI->setPosition(ofGetWidth() - _settingsUI->getRect()->width, 0);
#if defined(USE_OFX_DETECT_DISPLAYS)
    maxWidth = MAX(maxWidth, _outputUI->getRect()->width);
    _outputUI->setPosition(ofGetWidth() - _outputUI->getRect()->width, _settingsUI->getRect()->height + 5);
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    #if defined(USE_OFX_DETECT_DISPLAYS)
        _videoPlayerUI->setPosition(ofGetWidth() - _videoPlayerUI->getRect()->width, _outputUI->getRect()->y + _outputUI->getRect()->height + 5);
    #elif
        _videoPlayerUI->setPosition(ofGetWidth() - _videoPlayerUI->getRect()->width, _settingsUI->getRect()->height + 5);
    #endif
    
    maxWidth = MAX(maxWidth, _videoPlayerUI->getRect()->width);
#endif
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    #if defined(USE_VIDEO_PLAYER_OPTION)
        _syphonUI->setPosition(ofGetWidth() - _syphonUI->getRect()->width, _videoPlayerUI->getRect()->y + _videoPlayerUI->getRect()->height + 5);
    #else
        #if defined(USE_OFX_DETECT_DISPLAYS)
            _syphonUI->setPosition(ofGetWidth() - _syphonUI->getRect()->width, _outputUI->getRect()->y + _outputUI->getRect()->height + 5);
        #elif
            _syphonUI->setPosition(ofGetWidth() - _syphonUI->getRect()->width, _settingsUI->getRect()->height + 5);
        #endif
    #endif
    
    maxWidth = MAX(maxWidth, _syphonUI->getRect()->width);

#endif
    
    // Update the width of the UIs.
    _settingsUI->setWidth(maxWidth);
#if defined(USE_OFX_DETECT_DISPLAYS)
    _outputUI->setWidth(maxWidth);
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    _videoPlayerUI->setWidth(maxWidth);
#endif
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    _syphonUI->setWidth(maxWidth);
#endif

}

#pragma mark -
#pragma mark Settings UI

#if defined(USE_OFX_DETECT_DISPLAYS)

//--------------------------------------------------------------
void ofxGemcutterControls::displayConfigurationChanged()
{
    if (ofxDetectDisplaysSharedInstance().getDisplays().size() == _displayNames.size()) {
        return;
    }
    
    // Reset
    ofSetFullscreen(false);
    setToggleValue(_toolsCanvas, kSettingMappingFullscreen, false);
    _mtlMapping2D->closeOutputWindow();
    _currActiveDisplayName = "";
    
    // Clear the actual display list
    _outputUI->removeWidget("DISPLAYS");
    _displayNames.clear();
    
    for (int i=0; i<ofxDetectDisplaysSharedInstance().getDisplays().size(); i++) {
        _displayNames.push_back(ofToString(ofxDetectDisplaysSharedInstance().getDisplays()[i]->width) + "x" + ofToString(ofxDetectDisplaysSharedInstance().getDisplays()[i]->height) + " - UID:" + ofxDetectDisplaysSharedInstance().getDisplays()[i]->UID);
    }
    
    _outputUI->addRadio("DISPLAYS", _displayNames);
    _outputUI->autoSizeToFitWidgets();
    _outputUI->setWidth(_outputUI->getRect()->width);
    
    updateUIsPosition();
}

//--------------------------------------------------------------
void ofxGemcutterControls::resetDisplaySelection()
{
    setToggleValue(_outputUI, _currActiveDisplayName, false);
    _currActiveDisplayName = "";
}
#endif

#pragma mark -
#pragma mark Grid settings

//--------------------------------------------------------------
void ofxGemcutterControls::gridSettingsListUiEvent(ofxUIEventArgs &event)
{
    if(ofxGemcutterShape::activeShape) {
        if (ofxGemcutterShape::activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {
            ((ofxGemcutterGrid*)ofxGemcutterShape::activeShape)->updateGrid();
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::showGridSettingsCanvas()
{
    if(isEnabled() && ofxGemcutterShape::activeShape) {
        if (ofxGemcutterShape::activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {
            _gridSettingsCanvas->removeWidgets();
            _gridSettingsCanvas->resetPlacer();
            
            ofxUISlider *nSlider;
            _gridSettingsCanvas->addLabel("GRID SETTINGS");
            nSlider = _gridSettingsCanvas->addSlider("NB COLS", 1.0, 20.0, &(((ofxGemcutterGrid*)ofxGemcutterShape::activeShape)->gridNbCols));
            nSlider->setIncrement(1.0f);
            nSlider = _gridSettingsCanvas->addSlider("NB ROWS", 1.0, 20.0, &(((ofxGemcutterGrid*)ofxGemcutterShape::activeShape)->gridNbRows));
            nSlider->setIncrement(1.0f);
             
            _shapesListCanvas->autoSizeToFitWidgets();

            _gridSettingsCanvas->enable();
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::hideGridSettingsCanvas()
{
    _gridSettingsCanvas->disable();
}

#pragma mark -
#pragma mark Set avalaible options
//--------------------------------------------------------------
void ofxGemcutterControls::showInputModeToggle()
{
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setVisible(true);
}

//--------------------------------------------------------------
void ofxGemcutterControls::hideInputModeToggle()
{
    ((ofxUIToggle *)_toolsCanvas->getWidget(kSettingMappingModeInput))->setVisible(false);
}

#pragma mark -
#pragma mark Interface Methods

//--------------------------------------------------------------
bool ofxGemcutterControls::getButtonValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUIButton *)ui->getWidget(name))->getValue();
}


//--------------------------------------------------------------
bool ofxGemcutterControls::getToggleValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUIToggle *)ui->getWidget(name))->getValue();
}

//--------------------------------------------------------------
void ofxGemcutterControls::setToggleValue(ofxUICanvas* ui, const string& name, float value)
{
    ((ofxUIToggle *)ui->getWidget(name))->setValue(value);
}

//--------------------------------------------------------------
float ofxGemcutterControls::getSliderValue(ofxUICanvas* ui, const string& name)
{
    return ((ofxUISlider *)ui->getWidget(name))->getScaledValue();
}

//--------------------------------------------------------------
void ofxGemcutterControls::setSliderValue(ofxUICanvas* ui, const string& name, float value)
{
    ((ofxUISlider *)ui->getWidget(name))->setValue(value);
}

#pragma mark -
//--------------------------------------------------------------
void ofxGemcutterControls::saveSettings()
{
    _settingsUI->saveSettings(_rootPath + _settingsUI->getCanvasTitle()->getLabel() + ".xml");
#if defined(USE_OFX_DETECT_DISPLAYS)
    _outputUI->saveSettings(_rootPath + _outputUI->getCanvasTitle()->getLabel() + ".xml");
#endif
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    _syphonUI->saveSettings(_rootPath + _syphonUI->getCanvasTitle()->getLabel() + ".xml");
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    _videoPlayerUI->saveSettings(_rootPath + _videoPlayerUI->getCanvasTitle()->getLabel() + ".xml");
#endif
    
    saveExtraSettings();
}

//--------------------------------------------------------------
void ofxGemcutterControls::loadSettings()
{
    loadExtraSettings();
    
    _settingsUI->loadSettings(_rootPath + _settingsUI->getCanvasTitle()->getLabel() + ".xml");
#if defined(USE_OFX_DETECT_DISPLAYS)
    _outputUI->loadSettings(_rootPath + _outputUI->getCanvasTitle()->getLabel() + ".xml");
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    loadVideoPlayerSettings();
#endif

    updateUIsPosition();
}

//--------------------------------------------------------------
void ofxGemcutterControls::enable()
{
    for (int i=0; i<_uiSuperCanvases.size(); i++) {
        _uiSuperCanvases[i]->enable();
    }
}

//--------------------------------------------------------------
void ofxGemcutterControls::disable()
{
    for (int i=0; i<_uiSuperCanvases.size(); i++) {
        _uiSuperCanvases[i]->disable();
    }
}

//--------------------------------------------------------------
bool ofxGemcutterControls::isEnabled()
{
    return _toolsCanvas->isEnabled();
}

//--------------------------------------------------------------
void ofxGemcutterControls::toggleVisible()
{
    _toolsCanvas->toggleVisible();
    
    if (_toolsCanvas->isVisible() && _mtlMapping2D->getModeState() == MAPPING_EDIT) {
        _shapesListCanvas->enable();
        showGridSettingsCanvas();
    } else {
        _shapesListCanvas->disable();
        hideGridSettingsCanvas();
    }
    
    // --- Settings
    if(_toolsCanvas->isVisible() && getToggleValue(_toolsCanvas, kSettingMappingSettings)) {
        _settingsUI->enable();
#if defined(USE_OFX_DETECT_DISPLAYS)
        _outputUI->enable();
#endif
        
#if defined(USE_VIDEO_PLAYER_OPTION)
        _videoPlayerUI->enable();
#endif
        
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
        _syphonUI->enable();
#endif
        
    } else {
        _settingsUI->disable();
#if defined(USE_OFX_DETECT_DISPLAYS)
        _outputUI->disable();
#endif
        
#if defined(USE_VIDEO_PLAYER_OPTION)
        _videoPlayerUI->disable();
#endif
        
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
        _syphonUI->disable();
#endif
    }
    
    
}

//--------------------------------------------------------------
bool ofxGemcutterControls::isVisible()
{
    return _toolsCanvas->isVisible();
}

//--------------------------------------------------------------
bool ofxGemcutterControls::isHit(int x, int y) {
    
    for (auto& it : _uiSuperCanvases) {
        if (it->isHit(x, y)) {
            return true;
        }
    }
    
    return false;
}

//--------------------------------------------------------------
bool ofxGemcutterControls::hasFocus()
{
    return _bHasFocus;
}


//--------------------------------------------------------------
void ofxGemcutterControls::saveExtraSettings()
{
    ofxXmlSettings xmlSettings;
    
	xmlSettings.addTag("root");
	xmlSettings.pushTag("root", 0);
    
    map<string,string>::iterator itSettings;
    for ( itSettings=extraOutputSettings.begin() ; itSettings != extraOutputSettings.end(); itSettings++ ) {
        int tagNum = xmlSettings.addTag("setting");
        xmlSettings.addAttribute("setting", "key", (*itSettings).first, tagNum);
        xmlSettings.setValue("setting", (*itSettings).second, tagNum);
    }
		
	//Save to file
	xmlSettings.saveFile(_rootPath + "extraSettings.xml");
    ofLogNotice() << "Status > settings saved to xml!";
    
}

//--------------------------------------------------------------
void ofxGemcutterControls::loadExtraSettings()
{
    
    ofxXmlSettings xmlSettings;
    string xmlFile = _rootPath + "extraSettings.xml";
    string feedBackMessage = "";
    
	//we load our settings file
	if( xmlSettings.loadFile(xmlFile) ){
		feedBackMessage = xmlFile + " loaded!";
	}else{
		feedBackMessage = "unable to load " + xmlFile + " check data/ folder";
	}
    ofLogNotice() << "Status > " << feedBackMessage;
    
    // ---
	int numRootTags = xmlSettings.getNumTags("root");
	
	if(numRootTags > 0){
		// ---
        xmlSettings.pushTag("root", 0);
		
        //Settings
        int numSettingTags = xmlSettings.getNumTags("setting");
        
        if(numSettingTags > 0) {
            for(int j = 0; j < numSettingTags; j++){
                string key = xmlSettings.getAttribute("setting", "key", "nc", j);
                string value = xmlSettings.getValue("setting", "", j);
                extraOutputSettings[key] = value;
            }
        }
				
		// ---
		xmlSettings.popTag();
	}
}


#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
#pragma mark -
#pragma mark Syphon
//--------------------------------------------------------------
void ofxGemcutterControls::addSyphonServer(vector<ofxSyphonServerDescription> servers)
{
    for( auto& syphonServerDir : servers ){
        string serverName = syphonServerDir.serverName;
        string appName = syphonServerDir.appName;
        
        
        if (serverName.empty()) {
            serverName = "N.C.";
        }
        
        if (appName.empty()) {
            appName = "N.C.";
        }
        
        string serverID = serverName; // + " - " + appName;

        list<string>::iterator it;
        for (it=_syphonServersList.begin(); it!=_syphonServersList.end(); it++) {
            if (*it == serverID) {
                return;
            }
        }
        
        _syphonServersList.push_back(serverID);

    }
    
    updateSyphonServersList();
}

//--------------------------------------------------------------
void ofxGemcutterControls::removeSyphonServer(vector<ofxSyphonServerDescription> servers)
{
    for( auto& syphonServerDir : servers ){
        string serverName = syphonServerDir.serverName;
        string appName = syphonServerDir.appName;
        
        if (serverName.empty()) {
            serverName = "N.C.";
        }
        
        if (appName.empty()) {
            appName = "N.C.";
        }
        
        string serverID = serverName;// + " - " + appName;

        list<string>::iterator it;
        for (it=_syphonServersList.begin(); it!=_syphonServersList.end(); it++) {
            if (*it == serverID) {
                _syphonServersList.remove(serverID);
            } else {
                // do nothing for now
            }
        }
    }
    
    updateSyphonServersList();
}

//--------------------------------------------------------------
void ofxGemcutterControls::updateSyphonServersList()
{
    _syphonUI->removeWidget("SERVERS");
    _syphonServersNames.clear();
    
    list<string>::iterator it;
    for (it=_syphonServersList.begin(); it!=_syphonServersList.end(); it++) {
        cout << "updateSyphonServersList: " << *it << endl;

        _syphonServersNames.push_back(*it);
    }
    
    _syphonUI->addRadio("SERVERS", _syphonServersNames);
    _syphonUI->autoSizeToFitWidgets();
    updateUIsPosition();
}

//--------------------------------------------------------------
void ofxGemcutterControls::loadSyphonSettings()
{
    _syphonUI->loadSettings(_rootPath + _syphonUI->getCanvasTitle()->getLabel() + ".xml");
    
    updateUIsPosition();
}

#endif

#if defined(USE_VIDEO_PLAYER_OPTION)
//--------------------------------------------------------------
void ofxGemcutterControls::loadVideoPlayerSettings()
{
    _videoPlayerUI->loadSettings(_rootPath + _videoPlayerUI->getCanvasTitle()->getLabel() + ".xml");
    
    updateUIsPosition();
}
#endif


