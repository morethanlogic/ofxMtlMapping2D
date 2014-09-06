#include "ofxGemcutter.h"
#include "ofxGemcutterSettings.h"
#include "ofxGemcutterControls.h"
#include "ofxGemcutterInput.h"
#include "ofxGemcutterShapeType.h"
#include "ofxGemcutterShapes.h"

#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
bool _syphonLoadSettingsAtLaunch = true;
int _syphonNumFrameWhenLastServerAnnounced = -1;

//--------------------------------------------------------------
//--------------------------------------------------------------
list<ofxGemcutterShape*>::iterator ofxGemcutter::iteratorForShapeWithId(int shapeId)
{
    list<ofxGemcutterShape*>::iterator it;
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        if(shape->shapeId == shapeId) {
            return it;
        }
    }
    
    return ofxGemcutterShapes::pmShapes.end();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutter::ofxGemcutter() 
{
}

//--------------------------------------------------------------
ofxGemcutter::~ofxGemcutter() 
{
    // ----
    while(!ofxGemcutterShapes::pmShapes.empty()) delete ofxGemcutterShapes::pmShapes.back(), ofxGemcutterShapes::pmShapes.pop_back();
	ofxGemcutterShapes::pmShapes.clear();
}

//--------------------------------------------------------------
void ofxGemcutter::setup(int width, int height, int numSample)
{
    ofxMSAInteractiveObject::doCoordTransformation = true;
    
    _mappingModeState = MAPPING_LOCKED;
    
    bSaveShapes = false;
    bLoadShapes = false;
    bCreateQuad = false;
    bCreateGrid = false;
    bCreateTriangle = false;
    bCreateMask = false;
    
    _bDeleteShape = false;
    _bSelectedShapeChanged = false;
    _selectedShapeId = -1;
    
    _timeUILastActivation = 0;
    
    // ---
    addListeners();
    
    // ---
    // The first time we call ofxGemcutterControls we need to call the init() method
    ofxGemcutterControlsSharedInstance(this).init();
    
    // ---
    ofxGemcutterGlobal::inputViewOutputPreview.set((ofGetWidth() - ofxGemcutterGlobal::outputWidth)/2, (ofGetHeight() - ofxGemcutterGlobal::outputHeight)/2, ofxGemcutterGlobal::outputWidth, ofxGemcutterGlobal::outputHeight);
    ofxGemcutterGlobal::outputViewOutputPreview.set((ofGetWidth() - ofxGemcutterGlobal::outputWidth)/2, (ofGetHeight() - ofxGemcutterGlobal::outputHeight)/2, ofxGemcutterGlobal::outputWidth, ofxGemcutterGlobal::outputHeight);
    
    _numSample = numSample;
//    zoomScaleToFit(MAPPING_INPUT_VIEW, true);
//    zoomScaleToFit(MAPPING_OUTPUT_VIEW, true);
    
    // ---
    ofxGemcutterSettings::infoFont.loadFont("ui/ReplicaBold.ttf", 10);

    // ---
#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
    setupSyphon();
#endif
    
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
    setupVideoPlayer();
#endif

	// Allocating the fbo right at the beginning. Not allocating it here/now was causing issue on Windows.
	// Not sure why. So for now I will do this.
	_fbo.allocate(ofxGemcutterGlobal::outputWidth, ofxGemcutterGlobal::outputHeight, GL_RGBA32F_ARB, _numSample);
}

//--------------------------------------------------------------
void ofxGemcutter::exit(ofEventArgs &e)
{
    ofxGemcutterControls::sharedInstance()->exit();
}

//--------------------------------------------------------------
void ofxGemcutter::setModeState(MappingModeState mappingModeState)
{
    _mappingModeState = mappingModeState;
}

//--------------------------------------------------------------
MappingModeState ofxGemcutter::getModeState()
{
    return _mappingModeState;
}

//--------------------------------------------------------------
void ofxGemcutter::update()
{
    if (ofxGemcutterGlobal::delayBeforeHiddingUI > 0 && ofGetElapsedTimeMillis() - _timeUILastActivation >= ofxGemcutterGlobal::delayBeforeHiddingUI) {
        ofxGemcutterControls::sharedInstance()->disable();
    }
    
#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
    if (_syphonLoadSettingsAtLaunch && (ofGetFrameNum() - _syphonNumFrameWhenLastServerAnnounced) > 0) {
        _syphonLoadSettingsAtLaunch = false;
        ofxGemcutterControlsSharedInstance().loadSyphonSettings();
    }
#endif
    
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
    updateVideoPlayer();
#endif

    if (_mappingModeState == MAPPING_LOCKED) {
        return;
    }
        
    // ---
    // Edit Mode Actions
    
    // Load and Save mapping
    if (bSaveShapes) {
        saveShapesList();
        return;
    }
    else if (bLoadShapes) {
        loadShapesList();
        return;
    }
    
    // Create new shapes
    else if (bCreateQuad) {
        bCreateQuad = false;
        createQuad();
        return;
    }
    else if (bCreateGrid) {
        bCreateGrid = false;
        createGrid();
        return;
    }
    else if (bCreateTriangle) {
        bCreateTriangle = false;
        createTriangle();
        return;
    }
    else if (bCreateMask) {
        bCreateMask = false;
        createMask();
        return;
    }
    
    else if (_bDeleteShape) {
        _bDeleteShape = false;
        deleteShape();
        return;
    }
    
    // ---
    // Output Mode
    if (ofxGemcutterGlobal::getEditView() == MAPPING_CHANGE_TO_INPUT_VIEW) {
        ofxGemcutterGlobal::setEditView(MAPPING_INPUT_VIEW);
        
        list<ofxGemcutterShape*>::iterator it;
        for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
            ofxGemcutterShape* shape = *it;
            shape->setAsIdle();
            shape->inputPolygon->enable();
        }
        
        updateZoomAndOutput(MAPPING_INPUT_VIEW);

    } else if (ofxGemcutterGlobal::getEditView() == MAPPING_CHANGE_TO_OUTPUT_VIEW) {
        ofxGemcutterGlobal::setEditView(MAPPING_OUTPUT_VIEW);
        
        list<ofxGemcutterShape*>::iterator it;
        for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
            ofxGemcutterShape* shape = *it;
            shape->enable();
            
            if(shape->inputPolygon) {
                // If this Shape is textured and has an 'inputPolygon'
                shape->inputPolygon->setAsIdle();
            }
        }
        
        updateZoomAndOutput(MAPPING_OUTPUT_VIEW);
    }
    
    // ---
    // Selected shape with UI
    if (_bSelectedShapeChanged) {
        _bSelectedShapeChanged = false;

        list<ofxGemcutterShape*>::iterator it = iteratorForShapeWithId(_selectedShapeId);
        if(it != ofxGemcutterShapes::pmShapes.end()) {
            ofxGemcutterShape* shape = *it;
            shape->setAsActiveShape(true);
            
            // Put active shape at the top of the list
            ofxGemcutterShapes::pmShapes.push_front(shape);
            ofxGemcutterShapes::pmShapes.erase(it);
        }
    }
    
    // ---
    // Update the Shapes
    list<ofxGemcutterShape*>::iterator it;
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        shape->update();
    }
}

#pragma mark -
#pragma mark Draw / Edit Mode
//--------------------------------------------------------------
void ofxGemcutter::draw()
{

    if (_mappingModeState == MAPPING_EDIT) {
        
        if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            ofPushMatrix();
            {
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                ofScale(ofxGemcutterGlobal::inputViewZoomFactor, ofxGemcutterGlobal::inputViewZoomFactor);
                ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
                
                drawFbo();
                
                ofPushMatrix();
                {
                    ofTranslate(ofxGemcutterGlobal::inputViewOutputPreview.x, ofxGemcutterGlobal::inputViewOutputPreview.y);
                    drawAllShapes();
                }
                ofPopMatrix();
                
                // Zoom
                ofSetColor(255, 0, 0);
                ofNoFill();
                ofRect(ofxGemcutterGlobal::inputViewOutputPreview);
                
//                ofSetColor(0, 0, 255);
//                ofRect(ofxGemcutterGlobal::inputViewZoomedCoordSystem);
                
                ofVec2f transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(ofGetMouseX(), ofGetMouseY()));
                
//                ofFill();
//                ofSetColor(255, 0, 0);
//                ofRect(transformedCoord.x-5, transformedCoord.y-5, 10, 10);
            }
            ofPopMatrix();
            
        }
        
        else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
            ofPushMatrix();
            {
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                ofScale(ofxGemcutterGlobal::outputViewZoomFactor, ofxGemcutterGlobal::outputViewZoomFactor);
                ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
                
                render(false);
                
                ofPushMatrix();
                {
                    ofTranslate(ofxGemcutterGlobal::outputViewOutputPreview.x, ofxGemcutterGlobal::outputViewOutputPreview.y);
                    drawAllShapes();
                }
                ofPopMatrix();
                
                // Zoom
                ofSetColor(255, 0, 0);
                ofNoFill();
                ofRect(ofxGemcutterGlobal::outputViewOutputPreview);
                
//                ofSetColor(0, 0, 255);
//                ofRect(ofxGemcutterGlobal::outputViewZoomedCoordSystem);
                
                ofVec2f transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(ofGetMouseX(), ofGetMouseY()));
                
//                ofFill();
//                ofSetColor(255, 0, 0);
//                ofRect(transformedCoord.x-5, transformedCoord.y-5, 10, 10);
            }
            ofPopMatrix();
        }
    }
    else {
        render(true);
    }
    
    ofxGemcutterControls::sharedInstance()->drawBg();
    
    if (_outputWindow.bIsSetup) {
        _outputWindow.begin();
        {
            ofBackground(0);
            render(true);
        }
        _outputWindow.end();
    }
}

//--------------------------------------------------------------
void ofxGemcutter::drawAllShapes()
{
    list<ofxGemcutterShape*>::iterator it;
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        
        if(shape != ofxGemcutterShape::activeShape) {
            shape->draw();
        }
    }
    
    if(ofxGemcutterShape::activeShape) {
        //Draw active shape on top
        ofxGemcutterShape::activeShape->draw();
    }
}

#pragma mark -
#pragma mark FBO
//--------------------------------------------------------------
void ofxGemcutter::bind()
{
    _fbo.begin();
    ofClear(.0f, .0f, .0f, .0f);
    ofClearAlpha();
    
#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
    drawSyphon();
#endif
    
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
    drawVideoPlayer();
#endif
}

//--------------------------------------------------------------
void ofxGemcutter::unbind()
{
    _fbo.end();
}

//--------------------------------------------------------------
void ofxGemcutter::drawFbo()
{
	ofSetColor(255, 255, 255);
    _fbo.draw(ofxGemcutterGlobal::inputViewOutputPreview);
}

#pragma mark -
#pragma mark Render - Mapping Mode
//--------------------------------------------------------------
void ofxGemcutter::render(bool bIsOutput)
{
    list<ofxGemcutterShape*>::iterator it;
    
    if (!bIsOutput) {
        ofPushMatrix();
        ofTranslate(ofxGemcutterGlobal::outputViewOutputPreview.x, ofxGemcutterGlobal::outputViewOutputPreview.y);
    }
    
    // Textured shapes
    _fbo.getTextureReference().bind();
    ofSetColor(255, 255, 255, 255);
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        
        if (shape != ofxGemcutterShape::activeShape && shape->shapeType != MAPPING_2D_SHAPE_MASK) {
            shape->render();
        }
    }
    
    if(ofxGemcutterShape::activeShape) {
        //Render active shape on top
        ofxGemcutterShape::activeShape->render();
    }
    
    _fbo.getTextureReference().unbind();
    
    // Masks - non textured shapes
    ofSetColor(0, 0, 0, 255);
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        
        if (shape->shapeType == MAPPING_2D_SHAPE_MASK) {
            shape->render();
        }
    }
    
    if (!bIsOutput) {
        ofPopMatrix();
    }
}


#pragma mark -
#pragma mark Shapes Related
//--------------------------------------------------------------
void ofxGemcutter::createQuad()
{
    ofxGemcutterShape::nextShapeId++;

    ofxGemcutterShape* newShape = new ofxGemcutterQuad();
    newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
    newShape->init(ofxGemcutterShape::nextShapeId, true);
    ofxGemcutterShapes::pmShapes.push_front(newShape);
    
    ofxGemcutterControlsSharedInstance().addShapeToList(ofxGemcutterShape::nextShapeId, MAPPING_2D_SHAPE_QUAD);
}

//--------------------------------------------------------------
void ofxGemcutter::createGrid()
{
    ofxGemcutterShape::nextShapeId++;
    
    ofxGemcutterShape* newShape = new ofxGemcutterGrid();
    newShape->shapeType = MAPPING_2D_SHAPE_GRID;
    newShape->init(ofxGemcutterShape::nextShapeId, true);
    ofxGemcutterShapes::pmShapes.push_front(newShape);
    
    ofxGemcutterControlsSharedInstance().addShapeToList(ofxGemcutterShape::nextShapeId, MAPPING_2D_SHAPE_GRID);
}

//--------------------------------------------------------------
void ofxGemcutter::createTriangle()
{
    ofxGemcutterShape::nextShapeId++;

    ofxGemcutterShape* newShape = new ofxGemcutterTriangle();
    newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
    newShape->init(ofxGemcutterShape::nextShapeId, true);
    ofxGemcutterShapes::pmShapes.push_front(newShape);
    
    ofxGemcutterControlsSharedInstance().addShapeToList(ofxGemcutterShape::nextShapeId, MAPPING_2D_SHAPE_TRIANGLE);
}

//--------------------------------------------------------------
void ofxGemcutter::createMask()
{
    ofxGemcutterShape::nextShapeId++;
    
    ofxGemcutterShape* newShape = new ofxGemcutterMask();
    newShape->shapeType = MAPPING_2D_SHAPE_MASK;
    newShape->init(ofxGemcutterShape::nextShapeId, true);
    ofxGemcutterShapes::pmShapes.push_front(newShape);
    
    ofxGemcutterControlsSharedInstance().addShapeToList(ofxGemcutterShape::nextShapeId, MAPPING_2D_SHAPE_MASK);
}

//--------------------------------------------------------------
void ofxGemcutter::selectShapeId(int shapeId)
{
    _selectedShapeId = shapeId;
    _bSelectedShapeChanged = true;
}

//--------------------------------------------------------------
void ofxGemcutter::setLockForShapeId(int shapeId, bool bLocked)
{
    list<ofxGemcutterShape*>::iterator it;
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        if(shape->shapeId == shapeId) {
            shape->shapeSettings["isLocked"] = ofToString(bLocked);
            shape->setLock(bLocked);
            return;
        }
    }
}


//--------------------------------------------------------------
void ofxGemcutter::deleteShape()
{
    if (ofxGemcutterShape::activeShape) {
        ofxGemcutterControlsSharedInstance().clearShapesList();
        ofxGemcutterShapes::pmShapes.remove(ofxGemcutterShape::activeShape);
        delete ofxGemcutterShape::activeShape;
        ofxGemcutterShape::resetActiveShapeVars();
    
        // Re populate the UI List
        list<ofxGemcutterShape*>::reverse_iterator it;
        for (it=ofxGemcutterShapes::pmShapes.rbegin(); it!=ofxGemcutterShapes::pmShapes.rend(); it++) {
            ofxGemcutterShape* shape = *it;
            ofxGemcutterControlsSharedInstance().addShapeToList(shape->shapeId, shape->shapeType, ofToBool(shape->shapeSettings["isLocked"]));

        }
    }
}

#pragma mark -
#pragma mark Zoom / Output
//--------------------------------------------------------------
void ofxGemcutter::updateZoomAndOutput(MappingEditView view, bool updateOutputResolution)
{
    float zoomFactor;
    ofRectangle *outputPreview;
    ofRectangle *zoomedCoordSystem;
    
    if (view == MAPPING_INPUT_VIEW) {
        zoomFactor = ofxGemcutterGlobal::inputViewZoomFactor;
        outputPreview = &ofxGemcutterGlobal::inputViewOutputPreview;
        zoomedCoordSystem = &ofxGemcutterGlobal::inputViewZoomedCoordSystem;
    }
    
    else if (view == MAPPING_OUTPUT_VIEW) {
        zoomFactor = ofxGemcutterGlobal::outputViewZoomFactor;
        outputPreview = &ofxGemcutterGlobal::outputViewOutputPreview;
        zoomedCoordSystem = &ofxGemcutterGlobal::outputViewZoomedCoordSystem;

    }
    
    if (updateOutputResolution) {
        outputPreview->set((ofGetWidth() - ofxGemcutterGlobal::outputWidth)/2, (ofGetHeight() - ofxGemcutterGlobal::outputHeight)/2, ofxGemcutterGlobal::outputWidth, ofxGemcutterGlobal::outputHeight);
        
        // resize / re-allocate the source FBO
        if (_fbo.getWidth() != ofxGemcutterGlobal::outputWidth || _fbo.getHeight() != ofxGemcutterGlobal::outputHeight) {
            _fbo.allocate(ofxGemcutterGlobal::outputWidth , ofxGemcutterGlobal::outputHeight, GL_RGBA32F_ARB, _numSample);
        }
        
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
        resizeVideo(*outputPreview);
#endif
    
    }
    
    float xLoNew = -(ofGetWidth()/zoomFactor - ofGetWidth())/2;
    float yLoNew = -(ofGetHeight()/zoomFactor - ofGetHeight())/2;
    
    zoomedCoordSystem->set(xLoNew, yLoNew, ofGetWidth()/zoomFactor, ofGetHeight()/zoomFactor);
    
    ofxMSAInteractiveObject::coordSystemRect.set(zoomedCoordSystem->x - outputPreview->x, zoomedCoordSystem->y - outputPreview->y, zoomedCoordSystem->width, zoomedCoordSystem->height);
}

//--------------------------------------------------------------
void ofxGemcutter::zoomScaleToFit(MappingEditView view, bool updateFBO)
{
    // Calculate what would be a scale to fit.
    ofRectangle outputPreview;
    ofRectangle targetRect(.0f, .0f, ofGetWidth(), ofGetHeight());
    
    if (view == MAPPING_INPUT_VIEW) {
        outputPreview = ofxGemcutterGlobal::inputViewOutputPreview;
    }
    else if (view == MAPPING_OUTPUT_VIEW) {
        outputPreview = ofxGemcutterGlobal::outputViewOutputPreview;
    }

    outputPreview.scaleTo(targetRect, OF_SCALEMODE_FIT);
    
    // Get the data out and make them compatible with our Zoom and Drag shizzle.
    float zoomFactor = ofxGemcutterGlobal::outputWidth / outputPreview.getWidth();

    outputPreview.x = (targetRect.width - outputPreview.width * zoomFactor)/2;
    outputPreview.y = (targetRect.height - outputPreview.height * zoomFactor)/2;
    
    zoomFactor = 1/zoomFactor;
    
    if (view == MAPPING_INPUT_VIEW) {
        ofxGemcutterGlobal::inputViewZoomFactor = zoomFactor;

        ofxGemcutterGlobal::inputViewOutputPreview.x = outputPreview.x;
        ofxGemcutterGlobal::inputViewOutputPreview.y = outputPreview.y;
    }
    else if (view == MAPPING_OUTPUT_VIEW) {
        ofxGemcutterGlobal::outputViewZoomFactor = zoomFactor;
        ofxGemcutterGlobal::outputViewOutputPreview.x = outputPreview.x;
        ofxGemcutterGlobal::outputViewOutputPreview.y = outputPreview.y;
    }
    
    // Update the Zoom.
    updateZoomAndOutput(view, updateFBO);
}

#pragma mark -
#pragma mark Add / Remove Listeners

//--------------------------------------------------------------
void ofxGemcutter::addListeners()
{
    ofAddListener(ofEvents().mouseMoved, this, &ofxGemcutter::mouseMoved);
	ofAddListener(ofEvents().mousePressed, this, &ofxGemcutter::mousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxGemcutter::mouseDragged);
    ofAddListener(ofEvents().keyPressed, this, &ofxGemcutter::keyPressed);
    ofAddListener(ofEvents().windowResized, this, &ofxGemcutter::windowResized);
    ofAddListener(ofEvents().exit, this, &ofxGemcutter::exit);
}

//--------------------------------------------------------------
void ofxGemcutter::removeListeners()
{
    ofRemoveListener(ofEvents().mouseMoved, this, &ofxGemcutter::mouseMoved);
	ofRemoveListener(ofEvents().mousePressed, this, &ofxGemcutter::mousePressed);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxGemcutter::mouseDragged);
    ofRemoveListener(ofEvents().keyPressed, this, &ofxGemcutter::keyPressed);
    ofRemoveListener(ofEvents().windowResized, this, &ofxGemcutter::windowResized);
    ofRemoveListener(ofEvents().exit, this, &ofxGemcutter::exit);
}

#pragma mark -
#pragma mark Events

//--------------------------------------------------------------
void ofxGemcutter::updateUiTimer()
{
    _timeUILastActivation = ofGetElapsedTimeMillis() ;
    
    if (!ofxGemcutterControls::sharedInstance()->isEnabled()) {
        ofxGemcutterControls::sharedInstance()->toggleVisible();
    }
}

//--------------------------------------------------------------
void ofxGemcutter::windowResized(ofResizeEventArgs &e)
{
    ofxGemcutterControlsSharedInstance().updateUIsPosition();
    updateZoomAndOutput(ofxGemcutterGlobal::getEditView());
}

//--------------------------------------------------------------
void ofxGemcutter::mouseMoved(ofMouseEventArgs &e)
{
    updateUiTimer();
}

//--------------------------------------------------------------
void ofxGemcutter::mousePressed(ofMouseEventArgs &e)
{
    updateUiTimer();
    
    // ---
    int eX = e.x;
    int eY = e.y;
    int eButton = e.button;
    
    if (ofxGemcutterControlsSharedInstance().isHit(eX, eY) || _mappingModeState == MAPPING_LOCKED) return;

    // ---
    ofVec2f transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(eX, eY));
    eX = transformedCoord.x;
    eY = transformedCoord.y;

    _mouse = ofVec2f(eX, eY);
    _lastMouse = _mouse;
    
    if ( ofxGemcutterGlobal::bIsDraggingZone) return;
    
    if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(e.x, e.y), -ofxGemcutterGlobal::inputViewOutputPreview.x, -ofxGemcutterGlobal::inputViewOutputPreview.y);
    }
    
    else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(e.x, e.y), -ofxGemcutterGlobal::outputViewOutputPreview.x, -ofxGemcutterGlobal::outputViewOutputPreview.y);
    }
    
    eX = transformedCoord.x;
    eY = transformedCoord.y;
    
    // ----
    // A vertex has been selected
    if ((ofxGemcutterVertex::activeVertex && ofxGemcutterVertex::activeVertex->isDragged()) || eButton == 2) {
        return;
    }
    
    // ----
    // Select an existing shape
    list<ofxGemcutterShape*>::iterator it;
    for (it=ofxGemcutterShapes::pmShapes.begin(); it!=ofxGemcutterShapes::pmShapes.end(); it++) {
        ofxGemcutterShape* shape = *it;
        bool grabbedOne = false;
        
        if (!shape->isLocked()) {
            
            if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
                if (shape->inputPolygon || shape->shapeType != MAPPING_2D_SHAPE_MASK) {
                    if(shape->inputPolygon->hitTest(eX, eY)) {
                        grabbedOne = true;
                        shape->inputPolygon->select(eX, eY);
                        shape->inputPolygon->enable();
                    }
                }

            } else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
                if(shape->hitTest(eX, eY)) {
                    grabbedOne = true;
                    shape->select(eX, eY);
                    shape->enable();
                }
            }
            
            if(grabbedOne) {
                // Put active shape at the top of the list
                ofxGemcutterShapes::pmShapes.push_front(shape);
                ofxGemcutterShapes::pmShapes.erase(it);
                
                return;
            }
            
        }
    }
    
    // ----
    if(ofxGemcutterSettings::kIsManuallyAddingDeletingVertexEnabled && ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        // Add vertex to the selected shape
        if(ofxGemcutterShape::activeShape) {
            // Only if the shape is a Mask
            if (ofxGemcutterShape::activeShape->shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxGemcutterShape* shape = ofxGemcutterShape::activeShape;
                if (shape) {
                    ofLog(OF_LOG_NOTICE, "Add vertex to shape %i", shape->shapeId);
                    shape->addPoint(eX, eY);
                } else {
                    ofLog(OF_LOG_NOTICE, "No shape has been selected, can not add a vertex");
                }
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofxGemcutter::mouseDragged(ofMouseEventArgs &e)
{
    updateUiTimer();
    
    // ---
    if (!ofxGemcutterGlobal::bIsDraggingZone) return;
    
    int eX = e.x;
    int eY = e.y;
    int eButton = e.button;

    ofVec2f transformedCoord = ofxGemcutterGlobal::screenToZoomed(ofVec2f(eX, eY));
    eX = transformedCoord.x;
    eY = transformedCoord.y;
    
    _mouse = ofVec2f(eX, eY);
    _mouseVel = _mouse  - _lastMouse;
    _lastMouse = _mouse;
    
    _sensitivityXY = 1.0f;
    _moveX = _mouseVel.x * _sensitivityXY;
    _moveY = _mouseVel.y * _sensitivityXY;
    
    if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        ofxGemcutterGlobal::inputViewOutputPreview.x += _moveX;
        ofxGemcutterGlobal::inputViewOutputPreview.y += _moveY;
    }
    
    else if (ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        ofxGemcutterGlobal::outputViewOutputPreview.x += _moveX;
        ofxGemcutterGlobal::outputViewOutputPreview.y += _moveY;
    }
    
    ofxMSAInteractiveObject::coordSystemRect.x -= _moveX;
    ofxMSAInteractiveObject::coordSystemRect.y -= _moveY;
    
}
//--------------------------------------------------------------
void ofxGemcutter::keyPressed(ofKeyEventArgs &e)
{
    updateUiTimer();
    
    // ---
    if (ofxGemcutterControls::sharedInstance()->hasFocus()) return;
    
    // ----
    switch (e.key) {
        case 9:  // TAB
            ofToggleFullscreen();
            break;
            
        case 'l':
            loadShapesList();
            break;
            
        case 'm':
            //ofxGemcutterControlsSharedInstance().toggleVisible();
            break;
            
        case 's':
            saveShapesList();
            break;
            
        case 356:
            //left
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxGemcutterShape::activeShape && ofxGemcutterShape::activeVertexId >=0) {
                    ofxGemcutterVertex::activeVertex->left();
                }
            }
            break;
                
        case 357: //up
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxGemcutterShape::activeShape && ofxGemcutterShape::activeVertexId >=0) {
                    ofxGemcutterVertex::activeVertex->up();
                }
            }
            break;
        
        case 358: //right
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxGemcutterShape::activeShape && ofxGemcutterShape::activeVertexId >=0) {
                    ofxGemcutterVertex::activeVertex->right();
                }
            }
            break;
                
        case 359: //down
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxGemcutterShape::activeShape && ofxGemcutterShape::activeVertexId >=0) {
                    ofxGemcutterVertex::activeVertex->down();
                }
            }
            break;
            
        case 127:
            _bDeleteShape = true;
            break;
            
        case 8:
            _bDeleteShape = true;
            break;
            
        case 'n':
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxGemcutterShape::activeShape) {
                    ofxGemcutterShape::activeVertexId++;
                    ofxGemcutterShape::activeVertexId %= ofxGemcutterShape::activeShape->vertices.size();
                    
                    //-----
                    if (ofxGemcutterShape::activeShape->vertices.size() > ofxGemcutterShape::activeVertexId)
                    {
                        list<ofxGemcutterVertex*>::iterator it = ofxGemcutterShape::activeShape->vertices.begin();
                        advance(it, ofxGemcutterShape::activeVertexId);
                        ofxGemcutterVertex* vertex = *it;
                        vertex->setAsActive();
                    }
                }
            }
            break;
    }
}


#pragma mark -
#pragma mark Load and Save Shapes List
//--------------------------------------------------------------
void ofxGemcutter::loadShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemLoadDialog();
    
    if (!fileDialogResult.bSuccess) {
        return;
    }
    
    loadXml(fileDialogResult.getPath());
}

//--------------------------------------------------------------
void ofxGemcutter::loadXml(string xmlFile)
{
    
    // UI
    ofxGemcutterControlsSharedInstance().clearShapesList();
    
    // Delete everything
    while(!ofxGemcutterShapes::pmShapes.empty()) delete ofxGemcutterShapes::pmShapes.back(), ofxGemcutterShapes::pmShapes.pop_back();
    ofxGemcutterShapes::pmShapes.clear();
    ofxGemcutterShape::resetActiveShapeVars();
    
    
    //LOAD XML
    // ----
	//the string is printed at the top of the app
	//to give the user some feedback
	string feedBackMessage = "loading " + xmlFile;
	ofLog(OF_LOG_NOTICE, "Status > " + feedBackMessage);
    
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( _shapesListXML.loadFile(xmlFile) ){
		feedBackMessage = xmlFile + " loaded!";
	}else{
		feedBackMessage = "unable to load " + xmlFile + " check data/ folder";
	}
    ofLog(OF_LOG_NOTICE, "Status > " + feedBackMessage);
    
    
    int shapeId = -1;
    
    // ----
	//this is a more advanced use of ofXMLSettings
	//we are going to be reading multiple tags with the same name
	
	//lets see how many <root> </root> tags there are in the xml file
	int numRootTags = _shapesListXML.getNumTags("root");
	int numShapeTags = 0;
	
	//if there is at least one <root> tag we can read the list of cards
	//and then load their associated xml file
	if(numRootTags > 0){
		//we push into the last <root> tag
		//this temporarirly treats the tag as
		//the document root.
		_shapesListXML.pushTag("root", 0);
		
		//we see how many params/items we have stored in <card> tags
		numShapeTags = _shapesListXML.getNumTags("shape");
        ofLog(OF_LOG_NOTICE, "Status > numShapeTags :: " + ofToString(numShapeTags));
		
		if(numShapeTags > 0){			
			for(int i = 0; i < numShapeTags; i++){
				ofxGemcutterShape* newShape;
				
				shapeId = _shapesListXML.getAttribute("shape", "id", 0, i);
				
				_shapesListXML.pushTag("shape", i);
				
                //SHAPES SETTINGS
                int numShapeSettingTags = _shapesListXML.getNumTags("setting");
                
                for(int j = 0; j < numShapeSettingTags; j++){
                    string key = _shapesListXML.getAttribute("setting", "key", "nc", j);
                    
                    if (key == "type") {
                        string shapeType = _shapesListXML.getValue("setting", "nan", j);

                        if (shapeType == "quad") {
                            newShape = new ofxGemcutterQuad();
                            newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
                        } else if (shapeType == "grid") {
                            newShape = new ofxGemcutterGrid();
                            newShape->shapeType = MAPPING_2D_SHAPE_GRID;
                        } else if (shapeType == "triangle") {
                            newShape = new ofxGemcutterTriangle();
                            newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
                        } else if (shapeType == "mask") {
                            newShape = new ofxGemcutterMask();
                            newShape->shapeType = MAPPING_2D_SHAPE_MASK;
                        } else {
                            newShape = new ofxGemcutterQuad();
                            newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
                        }
                    }
                }
                                
                if(numShapeSettingTags > 0) {
                    for(int j = 0; j < numShapeSettingTags; j++){
                        string key = _shapesListXML.getAttribute("setting", "key", "nc", j); 
                        string value = _shapesListXML.getValue("setting", "", j);
                        newShape->shapeSettings[key] = value;                        
                    }
                }	
				
                //OUTPUT VERTICES
                _shapesListXML.pushTag("outputVertices", 0);
                int numVertexItemTags = _shapesListXML.getNumTags("vertex");
                for (int j = 0; j < numVertexItemTags; j++) {
                    int x = _shapesListXML.getAttribute("vertex", "x", 0, j); 
                    int y = _shapesListXML.getAttribute("vertex", "y", 0, j);
                    
                    //Create a new vertex
                    ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
                    newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
                    newShape->vertices.push_back(newVertex);
                }
                _shapesListXML.popTag();
                
                
                if(newShape->shapeType != MAPPING_2D_SHAPE_MASK) {
                    //INPUT QUADS
                    _shapesListXML.pushTag("inputPolygon", 0);

                    //Create a new vertex
                    newShape->inputPolygon = new ofxGemcutterInput();
                
                    //INPUT VERTICES
                    numVertexItemTags = _shapesListXML.getNumTags("vertex");
                    for (int k = 0; k < numVertexItemTags; k++) {
                        int x = _shapesListXML.getAttribute("vertex", "x", 0, k); 
                        int y = _shapesListXML.getAttribute("vertex", "y", 0, k);
                        
                        //Create a new vertex
                        ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
                        newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
                        newVertex->isDefiningTectureCoord = true;
                        newShape->inputPolygon->vertices.push_back(newVertex);
                    }
                    
                    newShape->inputPolygon->init(shapeId);
                    newShape->inputPolygon->disable();
                    
                    _shapesListXML.popTag();
                }
                
                newShape->init(shapeId);
                newShape->disable();
                ofxGemcutterShapes::pmShapes.push_front(newShape);
                
                newShape->setLock(ofToBool(newShape->shapeSettings["isLocked"]));
                ofxGemcutterControlsSharedInstance().addShapeToList(shapeId, newShape->shapeType, newShape->isLocked());
				
				_shapesListXML.popTag();
				
			}
		}
		
		//this pops us out of the <root> tag
		//sets the root back to the xml document
		_shapesListXML.popTag();
	}
    
    ofxGemcutterShape::nextShapeId = shapeId;
}

//--------------------------------------------------------------
void ofxGemcutter::saveShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemSaveDialog("", "Save Shapes List");
    
    if (!fileDialogResult.bSuccess) {
        return;
    }
    
    string mappingXmlFilePath = fileDialogResult.getPath();

    
    list<ofxGemcutterShape*> pmShapesCopy;
    pmShapesCopy.resize (ofxGemcutterShapes::pmShapes.size());
    copy (ofxGemcutterShapes::pmShapes.begin(), ofxGemcutterShapes::pmShapes.end(), pmShapesCopy.begin());
    pmShapesCopy.sort(Comparator());

    ofxXmlSettings newShapesListXML;
	
	newShapesListXML.addTag("root");
	newShapesListXML.pushTag("root", 0);
	
	//Create/Update XML
    list<ofxGemcutterShape*>::reverse_iterator it;
    for (it=pmShapesCopy.rbegin(); it!=pmShapesCopy.rend(); it++) {
        ofxGemcutterShape* shape = *it;
		
		int tagNum = newShapesListXML.addTag("shape");
		newShapesListXML.addAttribute("shape", "id", shape->shapeId, tagNum);
		newShapesListXML.pushTag("shape", tagNum);
		
        //Shape settings
        map<string,string>::iterator itShape;
        for ( itShape=shape->shapeSettings.begin() ; itShape != shape->shapeSettings.end(); itShape++ ) {
            int tagNum = newShapesListXML.addTag("setting");            
            newShapesListXML.addAttribute("setting", "key", (*itShape).first, tagNum);
            newShapesListXML.setValue("setting", (*itShape).second, tagNum);
        }
		
        //Output Vertex/Vertices
        tagNum = newShapesListXML.addTag("outputVertices");
        newShapesListXML.pushTag("outputVertices", tagNum);
        list<ofxGemcutterVertex*>::iterator itVertex;
        for (itVertex=shape->vertices.begin(); itVertex!=shape->vertices.end(); itVertex++) {
            ofxGemcutterVertex* vertex = *itVertex;
            
            int tagNum = newShapesListXML.addTag("vertex");
            newShapesListXML.addAttribute("vertex", "x", (int)vertex->center.x, tagNum);
            newShapesListXML.addAttribute("vertex", "y", (int)vertex->center.y, tagNum);
        }
        newShapesListXML.popTag();
        
        if(shape->shapeType != MAPPING_2D_SHAPE_MASK) {
            //Input Quads
            tagNum = newShapesListXML.addTag("inputPolygon");
            newShapesListXML.pushTag("inputPolygon", tagNum);
            //Vertices
            for (itVertex=shape->inputPolygon->vertices.begin(); itVertex!=shape->inputPolygon->vertices.end(); itVertex++) {
                ofxGemcutterVertex* vertex = *itVertex;
                
                int tagNum = newShapesListXML.addTag("vertex");
                newShapesListXML.addAttribute("vertex", "x", (int)vertex->center.x, tagNum);
                newShapesListXML.addAttribute("vertex", "y", (int)vertex->center.y, tagNum);
            }
            newShapesListXML.popTag();
		}
		newShapesListXML.popTag();
    }
	
	//Save to file
	newShapesListXML.saveFile(mappingXmlFilePath);
    ofLog(OF_LOG_NOTICE, "Status > settings saved to xml!");

}

#pragma mark -
#pragma mark Getters
//--------------------------------------------------------------
vector<ofPolyline*> ofxGemcutter::getMaskShapes()
{
    return ofxGemcutterShapes::getShapesOutputPolylineWithType(MAPPING_2D_SHAPE_MASK);
}

#pragma mark -
//--------------------------------------------------------------
void ofxGemcutter::chessBoard(int nbOfCol)
{
    ofSetColor(ofColor::white);
    ofFill();
    
    int boardWidth = ofxGemcutterGlobal::outputWidth;
    int boardHeight = ofxGemcutterGlobal::outputHeight;
    
    float squareSize = boardWidth / nbOfCol;
    int nbOfRow = ceil(boardHeight / squareSize);
    for (int colId = 0; colId < nbOfCol; colId++) {
        for (int rowId = 0; rowId < nbOfRow; rowId++) {
            if ((colId + rowId) % 2 == 0) {
                ofSetColor(ofColor::white);
            } else {
                ofSetColor(ofColor::black);
            }
            
            ofRect(colId * squareSize, rowId * squareSize, squareSize, squareSize);
        }
    }
    
    //Frame
    ofNoFill();
    ofSetColor(ofColor::yellow);
    glLineWidth(8.0f);
    ofRect(.0f, .0f, boardWidth, boardHeight);
    glLineWidth(1.0f);
    
    ofFill();
    ofSetColor(ofColor::red);
    ofRect(60.0f, .0f, 20.0f, 20.0f);
    ofSetColor(ofColor::green);
    ofRect(80.0f, .0f, 20.0f, 20.0f);
    ofSetColor(ofColor::blue);
    ofRect(100.0f, .0f, 20.0f, 20.0f);
    
    ofSetColor(ofColor::white);
}

#pragma mark -
#pragma mark Output window

//--------------------------------------------------------------
void ofxGemcutter::openOuputWindow(ofRectangle rect)
{
    if (!_outputWindow.bIsSetup) {
        _outputWindow.setup("output window", rect, true);
        ofAddListener(_outputWindow.windowClosedEvent, this, &ofxGemcutter::outputWindowClosedEvent);
    }
    _outputWindow.setPostionAndSize(rect);
}

//--------------------------------------------------------------
void ofxGemcutter::closeOutputWindow()
{
    if (!_outputWindow.bIsSetup) return;

    ofRemoveListener(_outputWindow.windowClosedEvent, this, &ofxGemcutter::outputWindowClosedEvent);
    _outputWindow.destroyWindow();
    
}

//--------------------------------------------------------------
void ofxGemcutter::outputWindowClosedEvent(int & i)
{
    ofRemoveListener(_outputWindow.windowClosedEvent, this, &ofxGemcutter::outputWindowClosedEvent);
    ofxGemcutterControls::sharedInstance()->resetDisplaySelection();
}


#pragma mark -
#pragma mark Syphon

#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)

//--------------------------------------------------------------
void ofxGemcutter::setupSyphon()
{    
    _syphonNumFrameWhenLastServerAnnounced = ofGetFrameNum();

    //setup our directory
	_syphonServerDir.setup();
    //setup our client
    _syphonClient.setup();
    
    //register for our directory's callbacks
    ofAddListener(_syphonServerDir.events.serverAnnounced, this, &ofxGemcutter::serverAnnounced);
    ofAddListener(_syphonServerDir.events.serverRetired, this, &ofxGemcutter::serverRetired);
    
    _syphonDirIdx = -1;
}

//--------------------------------------------------------------
void ofxGemcutter::drawSyphon()
{
    if(_syphonServerDir.isValidIndex(_syphonDirIdx)) {
        ofSetColor(255, 255, 255, 255);
        _syphonClient.draw(0, 0);
    }
}

//--------------------------------------------------------------
void ofxGemcutter::selectSyphonServer(int syphonDirIdx)
{
    _syphonDirIdx = syphonDirIdx;
    if(_syphonDirIdx > _syphonServerDir.size() - 1) {
        _syphonDirIdx = 0;
    }
    
    int appServerId = -1;
//#if defined(USE_SECOND_WINDOW_OPTION)
//    for (int i=0; i < _syphonServerDir.getServerList().size(); i++) {
//        if (_syphonServerDir.getDescription(i).serverName == ofToString(kSyphonOutputServerName)) {
//            appServerId = i;
//            break;
//        }
//    }
//#endif
    
    if(_syphonServerDir.isValidIndex(_syphonDirIdx)) {
        if (appServerId != -1 && appServerId <= _syphonDirIdx) {
            _syphonDirIdx++;
        }
    
        _syphonClient.set(_syphonServerDir.getDescription(_syphonDirIdx));
    }
}

//--------------------------------------------------------------
void ofxGemcutter::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    _syphonNumFrameWhenLastServerAnnounced = ofGetFrameNum();
    ofxGemcutterControlsSharedInstance().addSyphonServer(arg.servers);
}

//--------------------------------------------------------------
void ofxGemcutter::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    ofxGemcutterControlsSharedInstance().removeSyphonServer(arg.servers);
}
#endif


#pragma mark -
#pragma mark Video Player

#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)

//--------------------------------------------------------------
void ofxGemcutter::setupVideoPlayer()
{
    _bIsVideoStopped = true;
    videoPositionInSeconds = .0f;
}

//--------------------------------------------------------------
void ofxGemcutter::updateVideoPlayer()
{
    _videoPlayer.update();

#if defined(TARGET_WIN32)
	if (_videoPlayer.isLoaded() && _videoPlayer.isFrameNew()) {
        if(!tex.isAllocated()) tex.allocate(_videoPlayer.getPixelsRef());
        else tex.loadData(_videoPlayer.getPixelsRef());
    }
#endif

    if (_bIsVideoStopped && _videoPlayer.isLoaded() && _videoPlayer.isPlaying()) {
        _bIsVideoStopped = false;
        resizeVideo(ofxGemcutterGlobal::inputViewOutputPreview);
    }
    videoPositionInSeconds = _videoPlayer.getPosition() * 100;
}

//--------------------------------------------------------------
void ofxGemcutter::drawVideoPlayer()
{
    if (_bIsVideoStopped) {
        return;
    }
  
    ofSetColor(255);

#if defined(TARGET_OSX)
    _videoPlayer.draw(_videoRect.x, _videoRect.y, _videoRect.width, _videoRect.height);
#elif defined(TARGET_WIN32)
    tex.draw(_videoRect.x, _videoRect.y, _videoRect.width, _videoRect.height);
#endif
}

//--------------------------------------------------------------
void ofxGemcutter::exitVideoPlayer()
{
    _bIsVideoStopped = true;
    _videoPlayer.close();
}

//--------------------------------------------------------------
void ofxGemcutter::loadVideo(string filePath)
{
    _videoPlayer.loadMovie(filePath);
    _videoPlayer.setLoopState(OF_LOOP_NORMAL);
}

//--------------------------------------------------------------
void ofxGemcutter::playVideo()
{
    if (!_videoPlayer.isLoaded()) {
        loadVideo(ofxGemcutterControls::sharedInstance()->getVideoFilePath());
    }
    
    _videoPlayer.setPaused(false);
    _videoPlayer.play();
}

//--------------------------------------------------------------
void ofxGemcutter::pauseVideo()
{
    _videoPlayer.setPaused(true);
}

//--------------------------------------------------------------
void ofxGemcutter::stopVideo()
{
    _bIsVideoStopped = true;
    
    _videoPlayer.setPaused(true);
    _videoPlayer.stop();
}

//--------------------------------------------------------------
float ofxGemcutter::getVideoDurationInSecond()
{
    return _videoPlayer.getDuration();
}

//--------------------------------------------------------------
void ofxGemcutter::setVideoPostion(float position)
{
    _videoPlayer.setPosition(position/100);
}

//--------------------------------------------------------------
void ofxGemcutter::resizeVideo(ofRectangle targetRect)
{
    _videoRect.set(.0f, .0f, _videoPlayer.getWidth(), _videoPlayer.getHeight());
    _videoRect.scaleTo(targetRect, OF_SCALEMODE_FIT);
    
    _videoRect.x = (targetRect.width - _videoRect.width)/2;
    _videoRect.y = (targetRect.height - _videoRect.height)/2;
}

#endif


