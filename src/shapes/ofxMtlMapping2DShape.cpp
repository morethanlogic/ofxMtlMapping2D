#include "ofxMtlMapping2DShape.h"
#include "ofxMtlMapping2DGlobal.h"
#include "ofxMtlMapping2DControls.h"
#include "ofxMtlMapping2DGrid.h"

int ofxMtlMapping2DShape::nextShapeId = 0;
ofxMtlMapping2DShape*  ofxMtlMapping2DShape::activeShape = NULL;
ofxMtlMapping2DShape*  ofxMtlMapping2DShape::previousActiveShape = NULL;

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxMtlMapping2DShape::resetActiveShapeVars(){
    ofxMtlMapping2DShape::activeShape = NULL;
    ofxMtlMapping2DShape::previousActiveShape = NULL;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DShape::ofxMtlMapping2DShape()
{
    inputPolygon = NULL;
}

//--------------------------------------------------------------
ofxMtlMapping2DShape::~ofxMtlMapping2DShape()
{
    // ----
    delete inputPolygon;
}


#pragma mark -
#pragma mark Lifecycle
//--------------------------------------------------------------
void ofxMtlMapping2DShape::init(int sId, bool defaultShape)
{

    _bIsLocked = false;

    if (!defaultShape) {
        initShape();
    }
    
    _super::init(sId, defaultShape);
    
    calcHomography();
}

//--------------------------------------------------------------
void ofxMtlMapping2DShape::update()
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
        if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
            if (shapeType == MAPPING_2D_SHAPE_QUAD || shapeType == MAPPING_2D_SHAPE_GRID) {
                inputPolygon->update(true);
            } else {
                inputPolygon->update(false);
            }
        }
	}
}


//--------------------------------------------------------------
void ofxMtlMapping2DShape::draw()
{		
    //--------
    ofEnableAlphaBlending();
        
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        _super::draw();

    // ---- INPUT MODE
    } else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (inputPolygon && shapeType != MAPPING_2D_SHAPE_MASK) {
            inputPolygon->draw();
        }
    }
    
    drawID();
    
	ofDisableAlphaBlending();
}


//--------------------------------------------------------------
void ofxMtlMapping2DShape::drawID() 
{
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        _super::drawID();
    
    // ---- INPUT MODE
    } else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (inputPolygon && shapeType != MAPPING_2D_SHAPE_MASK) {
            inputPolygon->drawID();
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DShape::setAsActiveShape(bool fromUI)
{
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        if (activeShape != this) {
            // ----
            if (shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxMtlMapping2DControlsSharedInstance().hideInputModeToggle();
            } else {
                ofxMtlMapping2DControlsSharedInstance().showInputModeToggle();
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
                ofxMtlMapping2DControlsSharedInstance().setAsActiveShapeWithId(shapeId, shapeType);
            }
            
            // Is a grid
            if (activeShape->shapeType == MAPPING_2D_SHAPE_GRID) {                
                ofxMtlMapping2DControlsSharedInstance().showGridSettingsCanvas();
            } else {
               ofxMtlMapping2DControlsSharedInstance().hideGridSettingsCanvas();
            }
        }
    
    // ---- INPUT MODE
    } else if (ofxMtlMapping2DGlobal::getEditView() == MAPPING_INPUT_VIEW) {
        if (activeShape != this) {
            previousActiveShape = activeShape;
            activeShape = this;
            
            // Update UI
            if (fromUI) {
                if (!activeShape->isLocked()) {
                    inputPolygon->setAsActive();
                }
            } else {
                ofxMtlMapping2DControlsSharedInstance().setAsActiveShapeWithId(shapeId, shapeType);
            }
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DShape::setLock(bool val)
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
bool ofxMtlMapping2DShape::isLocked()
{
    return _bIsLocked;
}
