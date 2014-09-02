#include "ofxGemcutterShape.h"
#include "ofxGemcutterControls.h"
#include "ofxGemcutterGrid.h"

int ofxGemcutterShape::nextShapeId = 0;
ofxGemcutterShape*  ofxGemcutterShape::activeShape = NULL;
ofxGemcutterShape*  ofxGemcutterShape::previousActiveShape = NULL;

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxGemcutterShape::resetActiveShapeVars(){
    ofxGemcutterShape::activeShape = NULL;
    ofxGemcutterShape::previousActiveShape = NULL;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterShape::ofxGemcutterShape()
{
    inputPolygon = NULL;
}

//--------------------------------------------------------------
ofxGemcutterShape::~ofxGemcutterShape()
{
    // ----
    delete inputPolygon;
}


#pragma mark -
#pragma mark Lifecycle
//--------------------------------------------------------------
void ofxGemcutterShape::init(int sId, bool defaultShape)
{

    _bIsLocked = false;

    if (!defaultShape) {
        initShape();
    }
    
    _super::init(sId, defaultShape);
    
    calcHomography();
}

//--------------------------------------------------------------
void ofxGemcutterShape::update()
{
    _super::update();
    
    
    if(activePolygon != this && activePolygon != inputPolygon) {
        return;
    }
    
    if(activePolygon == this || (activePolygon == inputPolygon && inputPolygon) || (activePolygon == this && !inputPolygon && shapeType == MAPPING_2D_SHAPE_MASK)) {
        setAsActiveShape();
        
        // ---- recalculate the homography transformation matrix (for textured Shapes - for now only Quads) .
        calcHomography();
    }
    
    if (activeShape == this) {
        if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            if (shapeType == MAPPING_2D_SHAPE_QUAD || shapeType == MAPPING_2D_SHAPE_GRID) {
                inputPolygon->update(true);
            } else {
                inputPolygon->update(false);
            }
        }
	}
}


//--------------------------------------------------------------
void ofxGemcutterShape::draw()
{		
    //--------
    ofPushStyle();
    ofEnableAlphaBlending();
        
    // ---- OUTPUT MODE
    if(ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        _super::draw();

    // ---- INPUT MODE
    } else if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (inputPolygon && shapeType != MAPPING_2D_SHAPE_MASK) {
            inputPolygon->draw();
        }
    }
    
    drawID();
    
	ofDisableAlphaBlending();
    ofPopStyle();
}


//--------------------------------------------------------------
void ofxGemcutterShape::drawID() 
{
    // ---- OUTPUT MODE
    if(ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        _super::drawID();
    
    // ---- INPUT MODE
    } else if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (inputPolygon && shapeType != MAPPING_2D_SHAPE_MASK) {
            inputPolygon->drawID();
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterShape::setAsActiveShape(bool fromUI)
{
    // ---- OUTPUT MODE
    if(ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        if (activeShape != this) {
            // ----
            if (shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxGemcutterControlsSharedInstance().hideInputModeToggle();
            } else {
                ofxGemcutterControlsSharedInstance().showInputModeToggle();
            }
            
            // ----
            previousActiveShape = activeShape;
            activeShape = this;
            
            // Update UI
            if (fromUI) {
                if (!activeShape->isLocked()) {
                    setAsActive();
                }
            } else {
                ofxGemcutterControlsSharedInstance().setAsActiveShapeWithId(shapeId, shapeType);
            }
            
            // Is a grid
            if (activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {                
                ofxGemcutterControlsSharedInstance().showGridSettingsCanvas();
            } else {
               ofxGemcutterControlsSharedInstance().hideGridSettingsCanvas();
            }
        }
    
    // ---- INPUT MODE
    } else if (ofxGemcutterGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (activeShape != this) {
            previousActiveShape = activeShape;
            activeShape = this;
            
            // Update UI
            if (fromUI) {
                if (!activeShape->isLocked()) {
                    inputPolygon->setAsActive();
                }
            } else {
                ofxGemcutterControlsSharedInstance().setAsActiveShapeWithId(shapeId, shapeType);
            }
        }
    }
}

//--------------------------------------------------------------
void ofxGemcutterShape::setLock(bool val)
{
    _bIsLocked = val;
    
    if (_bIsLocked) {
        setAsIdle();
        if (inputPolygon) {
            inputPolygon->setAsIdle();
        }
    }
//    else if (!_bIsLocked && activePolygon == this) {
//        setAsActive();
//    }
}

//--------------------------------------------------------------
bool ofxGemcutterShape::isLocked()
{
    return _bIsLocked;
}
