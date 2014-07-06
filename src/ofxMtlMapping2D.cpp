#include "ofxMtlMapping2D.h"
#include "ofxMtlMapping2DSettings.h"
#include "ofxMtlMapping2DGlobal.h"
#include "ofxMtlMapping2DControls.h"
#include "ofxMtlMapping2DInput.h"
#include "ofxMtlMapping2DShapeType.h"
#include "ofxMtlMapping2DShapes.h"

#include "ofxMSAInteractiveObject.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
bool _syphonLoadSettingsAtLaunch = true;
int _syphonNumFrameWhenLastServerAnnounced = -1;

//--------------------------------------------------------------
//--------------------------------------------------------------
list<ofxMtlMapping2DShape*>::iterator ofxMtlMapping2D::iteratorForShapeWithId(int shapeId)
{
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->shapeId == shapeId) {
            return it;
        }
    }
    
    return ofxMtlMapping2DShapes::pmShapes.end();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2D::ofxMtlMapping2D() 
{
}

//--------------------------------------------------------------
ofxMtlMapping2D::~ofxMtlMapping2D() 
{
    // ----
    while(!ofxMtlMapping2DShapes::pmShapes.empty()) delete ofxMtlMapping2DShapes::pmShapes.back(), ofxMtlMapping2DShapes::pmShapes.pop_back();
	ofxMtlMapping2DShapes::pmShapes.clear();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::init(int width, int height, int numSample)
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
    
    // ---
    addListeners();
    
    // ---
    _numSample = numSample;
    _fbo.allocate(width, height, GL_RGBA, _numSample);
    ofxMtlMapping2DGlobal::outputWidth = width;
    ofxMtlMapping2DGlobal::outputHeight = height;
    
    // ---
    // The first time we call ofxMtlMapping2DControls we need to call the init() method
    ofxMtlMapping2DControlsSharedInstance(this).init();
    
    // ---
    ofxMtlMapping2DSettings::infoFont.loadFont("ui/ReplicaBold.ttf", 10);

    // ---
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    setupSyphon();
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    setupVideoPlayer();
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::setModeState(MappingModeState mappingModeState)
{
    _mappingModeState = mappingModeState;
}

//--------------------------------------------------------------
MappingModeState ofxMtlMapping2D::getModeState()
{
    return _mappingModeState;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::update()
{    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    if (_syphonLoadSettingsAtLaunch && (ofGetFrameNum() - _syphonNumFrameWhenLastServerAnnounced) > 0) {
        _syphonLoadSettingsAtLaunch = false;
        ofxMtlMapping2DControlsSharedInstance().loadSyphonSettings();
    }
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
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
    switch (ofxMtlMapping2DGlobal::getEditView()) {
        case MAPPING_CHANGE_TO_INPUT_VIEW:
        {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_INPUT_VIEW);
            
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->setAsIdle();
                shape->inputPolygon->enable();
            }
            
            updateZoomAndOutput();
            
            break;
        }
            
        case MAPPING_CHANGE_TO_OUTPUT_VIEW:
        {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_OUTPUT_VIEW);
            
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->enable();
                
                if(shape->inputPolygon) {
                    // If this Shape is textured and has an 'inputPolygon'
                    shape->inputPolygon->setAsIdle();
                }
            }
            
            updateZoomAndOutput();

            break;
        }
    }
    
    // ---
    // Selected shape with UI
    if (_bSelectedShapeChanged) {
        _bSelectedShapeChanged = false;

        list<ofxMtlMapping2DShape*>::iterator it = iteratorForShapeWithId(_selectedShapeId);
        if(it != ofxMtlMapping2DShapes::pmShapes.end()) {
            ofxMtlMapping2DShape* shape = *it;
            shape->setAsActiveShape(true);
            
            // Put active shape at the top of the list
            ofxMtlMapping2DShapes::pmShapes.push_front(shape);
            ofxMtlMapping2DShapes::pmShapes.erase(it);
        }
    }
    
    // ---
    // Update the Shapes
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        shape->update();
    }
}

#pragma mark -
#pragma mark Draw / Edit Mode
//--------------------------------------------------------------
void ofxMtlMapping2D::draw()
{

    if (_mappingModeState == MAPPING_EDIT) {
        
        if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            ofPushMatrix();
            {
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                ofScale(ofxMtlMapping2DGlobal::inputViewZoomFactor, ofxMtlMapping2DGlobal::inputViewZoomFactor);
                ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
                
                drawFbo();
                
                ofPushMatrix();
                {
                    ofTranslate(ofxMtlMapping2DGlobal::inputViewOutputPreview.x, ofxMtlMapping2DGlobal::inputViewOutputPreview.y);
                    drawAllShapes();
                }
                ofPopMatrix();
                
                // Zoom
                ofSetColor(255, 0, 0);
                ofNoFill();
                ofRect(ofxMtlMapping2DGlobal::inputViewOutputPreview);
                
                ofSetColor(0, 0, 255);
                ofRect(ofxMtlMapping2DGlobal::inputViewZoomedCoordSystem);
                
                ofVec2f transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(ofGetMouseX(), ofGetMouseY()));
                
                ofFill();
                ofSetColor(255, 0, 0);
                ofRect(transformedCoord.x-5, transformedCoord.y-5, 10, 10);
            }
            ofPopMatrix();
            
        }
        
        else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
            ofPushMatrix();
            {
                ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
                ofScale(ofxMtlMapping2DGlobal::outputViewZoomFactor, ofxMtlMapping2DGlobal::outputViewZoomFactor);
                ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
                
                render(false);
                
                ofPushMatrix();
                {
                    ofTranslate(ofxMtlMapping2DGlobal::outputViewOutputPreview.x, ofxMtlMapping2DGlobal::outputViewOutputPreview.y);
                    drawAllShapes();
                }
                ofPopMatrix();
                
                // Zoom
                ofSetColor(255, 0, 0);
                ofNoFill();
                ofRect(ofxMtlMapping2DGlobal::outputViewOutputPreview);
                
                ofSetColor(0, 0, 255);
                ofRect(ofxMtlMapping2DGlobal::outputViewZoomedCoordSystem);
                
                ofVec2f transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(ofGetMouseX(), ofGetMouseY()));
                
                ofFill();
                ofSetColor(255, 0, 0);
                ofRect(transformedCoord.x-5, transformedCoord.y-5, 10, 10);
            }
            ofPopMatrix();
        }
    }
    else {
        render(true);
    }
    
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
void ofxMtlMapping2D::drawAllShapes()
{
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
        if(shape != ofxMtlMapping2DShape::activeShape) {
            shape->draw();
        }
    }
    
    if(ofxMtlMapping2DShape::activeShape) {
        //Draw active shape on top
        ofxMtlMapping2DShape::activeShape->draw();
    }
}

#pragma mark -
#pragma mark FBO
//--------------------------------------------------------------
void ofxMtlMapping2D::bind()
{
    _fbo.begin();
    ofClear(.0f, .0f, .0f, .0f);
    ofClearAlpha();
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    drawSyphon();
#endif
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    drawVideoPlayer();
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::unbind()
{
    _fbo.end();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::drawFbo()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    _fbo.draw(ofxMtlMapping2DGlobal::inputViewOutputPreview);
}

#pragma mark -
#pragma mark Render - Mapping Mode
//--------------------------------------------------------------
void ofxMtlMapping2D::render(bool bIsOutput)
{
    list<ofxMtlMapping2DShape*>::iterator it;
    
    if (!bIsOutput) {
        ofPushMatrix();
        ofTranslate(ofxMtlMapping2DGlobal::outputViewOutputPreview.x, ofxMtlMapping2DGlobal::outputViewOutputPreview.y);
    }
    
    // Textured shapes
    _fbo.getTextureReference().bind();
    ofSetColor(255, 255, 255, 255);
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
        if (shape->shapeType != MAPPING_2D_SHAPE_MASK) {
            shape->render();
        }
    }
    _fbo.getTextureReference().unbind();
    
    // Masks - non textured shapes
    ofSetColor(0, 0, 0, 255);
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
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
void ofxMtlMapping2D::createQuad()
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DQuad();
    newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_QUAD);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createGrid()
{
    ofxMtlMapping2DShape::nextShapeId++;
    
    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DGrid();
    newShape->shapeType = MAPPING_2D_SHAPE_GRID;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_GRID);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createTriangle()
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DTriangle();
    newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_TRIANGLE);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createMask()
{
    ofxMtlMapping2DShape::nextShapeId++;
    
    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DMask();
    newShape->shapeType = MAPPING_2D_SHAPE_MASK;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_MASK);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::selectShapeId(int shapeId)
{
    _selectedShapeId = shapeId;
    _bSelectedShapeChanged = true;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::setLockForShapeId(int shapeId, bool bLocked)
{
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->shapeId == shapeId) {
            shape->shapeSettings["isLocked"] = ofToString(bLocked);
            shape->setLock(bLocked);
            return;
        }
    }
}


//--------------------------------------------------------------
void ofxMtlMapping2D::deleteShape()
{
    if (ofxMtlMapping2DShape::activeShape) {
        ofxMtlMapping2DControlsSharedInstance().clearShapesList();
        ofxMtlMapping2DShapes::pmShapes.remove(ofxMtlMapping2DShape::activeShape);
        delete ofxMtlMapping2DShape::activeShape;
        ofxMtlMapping2DShape::resetActiveShapeVars();
    
        // Re populate the UI List
        list<ofxMtlMapping2DShape*>::reverse_iterator it;
        for (it=ofxMtlMapping2DShapes::pmShapes.rbegin(); it!=ofxMtlMapping2DShapes::pmShapes.rend(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            ofxMtlMapping2DControlsSharedInstance().addShapeToList(shape->shapeId, shape->shapeType, ofToBool(shape->shapeSettings["isLocked"]));

        }
    }
}

#pragma mark -
#pragma mark Zoom / Output
//--------------------------------------------------------------
void ofxMtlMapping2D::updateZoomAndOutput(bool updateFBO)
{
    float zoomFactor;
    ofRectangle *outputPreview;
    ofRectangle *zoomedCoordSystem;
    
    if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        zoomFactor = ofxMtlMapping2DGlobal::inputViewZoomFactor;
        outputPreview = &ofxMtlMapping2DGlobal::inputViewOutputPreview;
        zoomedCoordSystem = &ofxMtlMapping2DGlobal::inputViewZoomedCoordSystem;
    }
    
    else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        zoomFactor = ofxMtlMapping2DGlobal::outputViewZoomFactor;
        outputPreview = &ofxMtlMapping2DGlobal::outputViewOutputPreview;
        zoomedCoordSystem = &ofxMtlMapping2DGlobal::outputViewZoomedCoordSystem;

    }
    
    if (updateFBO) {
        outputPreview->set((ofGetWidth() - ofxMtlMapping2DGlobal::outputWidth)/2, (ofGetHeight() - ofxMtlMapping2DGlobal::outputHeight)/2, ofxMtlMapping2DGlobal::outputWidth, ofxMtlMapping2DGlobal::outputHeight);
        
        // resize / re-allocate the source FBO
        if (_fbo.getWidth() != ofxMtlMapping2DGlobal::outputWidth || _fbo.getHeight() != ofxMtlMapping2DGlobal::outputHeight) {
            _fbo.allocate(ofxMtlMapping2DGlobal::outputWidth , ofxMtlMapping2DGlobal::outputHeight, GL_RGBA, _numSample);
        }
        
#if defined(USE_VIDEO_PLAYER_OPTION)
        resizeVideo(ofxMtlMapping2DGlobal::inputViewOutputPreview);
#endif
    
    } else if (outputPreview->width == 0 || outputPreview->height == 0) {
        outputPreview->set((ofGetWidth() - ofxMtlMapping2DGlobal::outputWidth)/2, (ofGetHeight() - ofxMtlMapping2DGlobal::outputHeight)/2, ofxMtlMapping2DGlobal::outputWidth, ofxMtlMapping2DGlobal::outputHeight);
    }
    
    float xLoNew = -(ofGetWidth()/zoomFactor - ofGetWidth())/2;
    float yLoNew = -(ofGetHeight()/zoomFactor - ofGetHeight())/2;
    
    zoomedCoordSystem->set(xLoNew, yLoNew, ofGetWidth()/zoomFactor, ofGetHeight()/zoomFactor);
    
    ofxMSAInteractiveObject::coordSystemRect.set(zoomedCoordSystem->x - outputPreview->x, zoomedCoordSystem->y - outputPreview->y, zoomedCoordSystem->width, zoomedCoordSystem->height);
    
#if defined(USE_VIDEO_PLAYER_OPTION)
    if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        resizeVideo(ofxMtlMapping2DGlobal::inputViewOutputPreview);
    }
#endif
}

#pragma mark -
#pragma mark Add / Remove Listeners

//--------------------------------------------------------------
void ofxMtlMapping2D::addListeners() {
	ofAddListener(ofEvents().mousePressed, this, &ofxMtlMapping2D::mousePressed);
    ofAddListener(ofEvents().mouseDragged, this, &ofxMtlMapping2D::mouseDragged);
    ofAddListener(ofEvents().keyPressed, this, &ofxMtlMapping2D::keyPressed);
    ofAddListener(ofEvents().windowResized, this, &ofxMtlMapping2D::windowResized);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::removeListeners() {
	ofRemoveListener(ofEvents().mousePressed, this, &ofxMtlMapping2D::mousePressed);
    ofRemoveListener(ofEvents().mouseDragged, this, &ofxMtlMapping2D::mouseDragged);
    ofRemoveListener(ofEvents().keyPressed, this, &ofxMtlMapping2D::keyPressed);
    ofRemoveListener(ofEvents().windowResized, this, &ofxMtlMapping2D::windowResized);

}

#pragma mark -
#pragma mark Events

//--------------------------------------------------------------
void ofxMtlMapping2D::windowResized(ofResizeEventArgs &e)
{
    ofxMtlMapping2DControlsSharedInstance().updateUIsPosition();
    updateZoomAndOutput();
}


//--------------------------------------------------------------
void ofxMtlMapping2D::mousePressed(ofMouseEventArgs &e)
{
    int eX = e.x;
    int eY = e.y;
    int eButton = e.button;
    
    if (ofxMtlMapping2DControlsSharedInstance().isHit(eX, eY))
        return;
    
    if(_mappingModeState == MAPPING_LOCKED)
        return;
    
    // ---
    ofVec2f transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(eX, eY));
    eX = transformedCoord.x;
    eY = transformedCoord.y;

    _mouse = ofVec2f(eX, eY);
    _lastMouse = _mouse;
    
    if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(e.x, e.y), -ofxMtlMapping2DGlobal::inputViewOutputPreview.x, -ofxMtlMapping2DGlobal::inputViewOutputPreview.y);
    }
    
    else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(e.x, e.y), -ofxMtlMapping2DGlobal::outputViewOutputPreview.x, -ofxMtlMapping2DGlobal::outputViewOutputPreview.y);
    }
    
    eX = transformedCoord.x;
    eY = transformedCoord.y;
    
    // ----
    // A vertex has been selected
    if ((ofxMtlMapping2DVertex::activeVertex && ofxMtlMapping2DVertex::activeVertex->isDragged()) || eButton == 2) {
        return;
    }
    
    // ----
    // Select an existing shape
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        bool grabbedOne = false;
        
        if (!shape->isLocked()) {
            
            switch (ofxMtlMapping2DGlobal::getEditView()) {
                case MAPPING_INPUT_VIEW:
                    if (shape->inputPolygon || shape->shapeType != MAPPING_2D_SHAPE_MASK) {
                        if(shape->inputPolygon->hitTest(eX, eY)) {
                            grabbedOne = true;
                            shape->inputPolygon->select(eX, eY);
                            shape->inputPolygon->enable();
                        }
                    }
                    break;
                    
                case MAPPING_OUTPUT_VIEW:
                    if(shape->hitTest(eX, eY)) {
                        grabbedOne = true;
                        shape->select(eX, eY);
                        shape->enable();
                    }
                    break;
            }
            
            if(grabbedOne) {
                // Put active shape at the top of the list
                ofxMtlMapping2DShapes::pmShapes.push_front(shape);
                ofxMtlMapping2DShapes::pmShapes.erase(it);
                
                return;
            }
            
        }
    }
    
    // ----
    if(ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled && ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        // Add vertex to the selected shape
        if(ofxMtlMapping2DShape::activeShape) {
            // Only if the shape is a Mask
            if (ofxMtlMapping2DShape::activeShape->shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxMtlMapping2DShape* shape = ofxMtlMapping2DShape::activeShape;
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
void ofxMtlMapping2D::mouseDragged(ofMouseEventArgs &e)
{
    if (!ofxMtlMapping2DGlobal::bIsDraggingZone) return;
    
    int eX = e.x;
    int eY = e.y;
    int eButton = e.button;

    ofVec2f transformedCoord = ofxMtlMapping2DGlobal::screenToZoomed(ofVec2f(eX, eY));
    eX = transformedCoord.x;
    eY = transformedCoord.y;
    
    _mouse = ofVec2f(eX, eY);
    _mouseVel = _mouse  - _lastMouse;
    _lastMouse = _mouse;
    
    _sensitivityXY = 1.0f;
    _moveX = _mouseVel.x * _sensitivityXY;
    _moveY = _mouseVel.y * _sensitivityXY;
    
    if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        ofxMtlMapping2DGlobal::inputViewOutputPreview.x += _moveX;
        ofxMtlMapping2DGlobal::inputViewOutputPreview.y += _moveY;
    }
    
    else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        ofxMtlMapping2DGlobal::outputViewOutputPreview.x += _moveX;
        ofxMtlMapping2DGlobal::outputViewOutputPreview.y += _moveY;
    }
    
    ofxMSAInteractiveObject::coordSystemRect.x -= _moveX;
    ofxMSAInteractiveObject::coordSystemRect.y -= _moveY;
    
}
//--------------------------------------------------------------
void ofxMtlMapping2D::keyPressed(ofKeyEventArgs &e)
{
    // ----
    switch (e.key) {
        case 9:  // TAB
            ofToggleFullscreen();
            break;
            
        case 'l':
            loadShapesList();
            break;
            
        case 'm':
            ofxMtlMapping2DControlsSharedInstance().toggleVisible();
            break;
            
        case 's':
            saveShapesList();
            break;
            
        case 356:
            //left
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->left();
                }
            }
            break;
                
        case 357: //up
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->up();
                }
            }
            break;
        
        case 358: //right
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->right();
                }
            }
            break;
                
        case 359: //down
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->down();
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
                if(ofxMtlMapping2DShape::activeShape) {
                    ofxMtlMapping2DShape::activeVertexId++;
                    ofxMtlMapping2DShape::activeVertexId %= ofxMtlMapping2DShape::activeShape->vertices.size();
                    
                    //-----
                    if (ofxMtlMapping2DShape::activeShape->vertices.size() > ofxMtlMapping2DShape::activeVertexId)
                    {
                        list<ofxMtlMapping2DVertex*>::iterator it = ofxMtlMapping2DShape::activeShape->vertices.begin();
                        advance(it, ofxMtlMapping2DShape::activeVertexId);
                        ofxMtlMapping2DVertex* vertex = *it;
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
void ofxMtlMapping2D::loadShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemLoadDialog();
    
    if (!fileDialogResult.bSuccess) {
        return;
    }
    
    loadXml(fileDialogResult.getPath());
}

//--------------------------------------------------------------
void ofxMtlMapping2D::loadXml(string xmlFile)
{
    
    // UI
    ofxMtlMapping2DControlsSharedInstance().clearShapesList();
    
    // Delete everything
    while(!ofxMtlMapping2DShapes::pmShapes.empty()) delete ofxMtlMapping2DShapes::pmShapes.back(), ofxMtlMapping2DShapes::pmShapes.pop_back();
    ofxMtlMapping2DShapes::pmShapes.clear();
    ofxMtlMapping2DShape::resetActiveShapeVars();
    
    
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
				ofxMtlMapping2DShape* newShape;
				
				shapeId = _shapesListXML.getAttribute("shape", "id", 0, i);
				
				_shapesListXML.pushTag("shape", i);
				
                //SHAPES SETTINGS
                int numShapeSettingTags = _shapesListXML.getNumTags("setting");
                
                for(int j = 0; j < numShapeSettingTags; j++){
                    string key = _shapesListXML.getAttribute("setting", "key", "nc", j);
                    
                    if (key == "type") {
                        string shapeType = _shapesListXML.getValue("setting", "nan", j);

                        if (shapeType == "quad") {
                            newShape = new ofxMtlMapping2DQuad();
                            newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
                        } else if (shapeType == "grid") {
                            newShape = new ofxMtlMapping2DGrid();
                            newShape->shapeType = MAPPING_2D_SHAPE_GRID;
                        } else if (shapeType == "triangle") {
                            newShape = new ofxMtlMapping2DTriangle();
                            newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
                        } else if (shapeType == "mask") {
                            newShape = new ofxMtlMapping2DMask();
                            newShape->shapeType = MAPPING_2D_SHAPE_MASK;
                        } else {
                            newShape = new ofxMtlMapping2DQuad();
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
                    ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
                    newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
                    newShape->vertices.push_back(newVertex);
                }
                _shapesListXML.popTag();
                
                
                if(newShape->shapeType != MAPPING_2D_SHAPE_MASK) {
                    //INPUT QUADS
                    _shapesListXML.pushTag("inputPolygon", 0);

                    //Create a new vertex
                    newShape->inputPolygon = new ofxMtlMapping2DInput();
                
                    //INPUT VERTICES
                    numVertexItemTags = _shapesListXML.getNumTags("vertex");
                    for (int k = 0; k < numVertexItemTags; k++) {
                        int x = _shapesListXML.getAttribute("vertex", "x", 0, k); 
                        int y = _shapesListXML.getAttribute("vertex", "y", 0, k);
                        
                        //Create a new vertex
                        ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
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
                ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
                
                newShape->setLock(ofToBool(newShape->shapeSettings["isLocked"]));
                ofxMtlMapping2DControlsSharedInstance().addShapeToList(shapeId, newShape->shapeType, newShape->isLocked());
				
				_shapesListXML.popTag();
				
			}
		}
		
		//this pops us out of the <root> tag
		//sets the root back to the xml document
		_shapesListXML.popTag();
	}
    
    ofxMtlMapping2DShape::nextShapeId = shapeId;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::saveShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemSaveDialog("", "Save Shapes List");
    
    if (!fileDialogResult.bSuccess) {
        return;
    }
    
    string mappingXmlFilePath = fileDialogResult.getPath();

    
    list<ofxMtlMapping2DShape*> pmShapesCopy;
    pmShapesCopy.resize (ofxMtlMapping2DShapes::pmShapes.size());
    copy (ofxMtlMapping2DShapes::pmShapes.begin(), ofxMtlMapping2DShapes::pmShapes.end(), pmShapesCopy.begin());
    pmShapesCopy.sort(Comparator());

    ofxXmlSettings newShapesListXML;
	
	newShapesListXML.addTag("root");
	newShapesListXML.pushTag("root", 0);
	
	//Create/Update XML
    list<ofxMtlMapping2DShape*>::reverse_iterator it;
    for (it=pmShapesCopy.rbegin(); it!=pmShapesCopy.rend(); it++) {
        ofxMtlMapping2DShape* shape = *it;
		
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
        list<ofxMtlMapping2DVertex*>::iterator itVertex;
        for (itVertex=shape->vertices.begin(); itVertex!=shape->vertices.end(); itVertex++) {
            ofxMtlMapping2DVertex* vertex = *itVertex;
            
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
                ofxMtlMapping2DVertex* vertex = *itVertex;
                
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
vector<ofPolyline*> ofxMtlMapping2D::getMaskShapes()
{
    return ofxMtlMapping2DShapes::getShapesOutputPolylineWithType(MAPPING_2D_SHAPE_MASK);
}

#pragma mark -
//--------------------------------------------------------------
void ofxMtlMapping2D::chessBoard(int nbOfCol)
{
    ofSetColor(ofColor::white);
    ofFill();
    
    int boardWidth = ofxMtlMapping2DGlobal::outputWidth;
    int boardHeight = ofxMtlMapping2DGlobal::outputHeight;
    
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
#pragma mark Syphon

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)

//--------------------------------------------------------------
void ofxMtlMapping2D::setupSyphon()
{    
    _syphonNumFrameWhenLastServerAnnounced = ofGetFrameNum();

    //setup our directory
	_syphonServerDir.setup();
    //setup our client
    _syphonClient.setup();
    
    //register for our directory's callbacks
    ofAddListener(_syphonServerDir.events.serverAnnounced, this, &ofxMtlMapping2D::serverAnnounced);
    ofAddListener(_syphonServerDir.events.serverRetired, this, &ofxMtlMapping2D::serverRetired);
    
    _syphonDirIdx = -1;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::drawSyphon()
{
    if(_syphonServerDir.isValidIndex(_syphonDirIdx)) {
        ofSetColor(255, 255, 255, 255);
        _syphonClient.draw(0, 0);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2D::openOuputWindow(ofRectangle rect)
{
    if (_outputWindow.bIsSetup) return;
    
    _outputWindow.setup("output window", rect, true);
    _outputWindow.setPostionAndSize(rect);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::closeOutputWindow()
{
    if (!_outputWindow.bIsSetup) return;

    _outputWindow.destroyWindow();
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::selectSyphonServer(int syphonDirIdx)
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
void ofxMtlMapping2D::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    _syphonNumFrameWhenLastServerAnnounced = ofGetFrameNum();
    ofxMtlMapping2DControlsSharedInstance().addSyphonServer(arg.servers);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    ofxMtlMapping2DControlsSharedInstance().removeSyphonServer(arg.servers);
}
#endif


#pragma mark -
#pragma mark Video Player

#if defined(USE_VIDEO_PLAYER_OPTION)

//--------------------------------------------------------------
void ofxMtlMapping2D::setupVideoPlayer()
{
    _bIsVideoStopped = true;
    
#if defined(TARGET_OSX)
    videoPositionInSeconds = .0f;
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::updateVideoPlayer()
{
#if defined(TARGET_OSX)
    _videoPlayer.update();
    
    if (_bIsVideoStopped && _videoPlayer.isLoaded() && _videoPlayer.isPlaying()) {
        _bIsVideoStopped = false;
        if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            resizeVideo(ofxMtlMapping2DGlobal::inputViewOutputPreview);
        }
    }
    videoPositionInSeconds = _videoPlayer.getPosition() * 100;
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::drawVideoPlayer()
{
    if (_bIsVideoStopped) {
        return;
    }
    
    ofSetColor(255);
    
#if defined(TARGET_OSX)
    _videoPlayer.draw(_videoXOffset, _videoYOffset, _videoWidth, _videoHeight);
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::exitVideoPlayer()
{
    _bIsVideoStopped = true;
#if defined(TARGET_OSX)
    _videoPlayer.close();
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::loadVideo(string filePath)
{
#if defined(TARGET_OSX)
    _videoPlayer.loadMovie(filePath);
    _videoPlayer.setLoopState(OF_LOOP_NORMAL);
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::playVideo()
{
#if defined(TARGET_OSX)
    _videoPlayer.setPaused(false);
    _videoPlayer.play();
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::pauseVideo()
{
#if defined(TARGET_OSX)
    _videoPlayer.setPaused(true);
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::stopVideo()
{
    _bIsVideoStopped = true;
    
#if defined(TARGET_OSX)
    _videoPlayer.setPaused(true);
    _videoPlayer.stop();
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
float ofxMtlMapping2D::getVideoDurationInSecond()
{
#if defined(TARGET_OSX)
    return _videoPlayer.getDuration();
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::setVideoPostion(float position)
{
#if defined(TARGET_OSX)
    _videoPlayer.setPosition(position/100);
#elif definfed(TARGET_WIN32)
    
#endif
}

//--------------------------------------------------------------
void ofxMtlMapping2D::resizeVideo(ofRectangle outputRect)
{
    _videoRatio = _videoPlayer.getWidth()/_videoPlayer.getHeight();
    
    if (outputRect.width > outputRect.height && (outputRect.height * _videoRatio) < outputRect.width) {
        _videoWidth = outputRect.height * _videoRatio;
        _videoHeight = outputRect.height;
        _videoXOffset = (outputRect.width - _videoWidth)/2; // + outputRect.x;
        _videoYOffset = 0;//outputRect.y;
        
    } else {
        _videoWidth = outputRect.width;
        _videoHeight = outputRect.width/_videoRatio;
        _videoXOffset = 0;//outputRect.x;
        _videoYOffset = (outputRect.height - _videoHeight)/2;// + outputRect.y;
    }
    
}

#endif


