#include "ofxMtlMapping2DShape.h"
#include "ofxMtlMapping2DControls.h"

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
    _super::init(sId, defaultShape);
    
    calcHomography();
}

//--------------------------------------------------------------
void ofxMtlMapping2DShape::update()
{
    _super::update();
    
    
    if(activePolygon != this && activePolygon != inputPolygon)
        return;
    
    if(activePolygon == this || activePolygon == inputPolygon) {
        setAsActiveShape();
        
        // ---- recalculate the homography transformation matrix (for textured Shapes - for now only Quads) .
        calcHomography();
    }
    
    if (activeShape == this) {
        if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
            if (shapeType == MAPPING_2D_SHAPE_QUAD) {
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
    if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
        _super::draw();

    // ---- INPUT MODE
    } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
        inputPolygon->draw();
    }
    
    if(ofxMtlMapping2DControls::mapping2DControls()->showShapesId())  {
        drawID();
    }
    
	ofDisableAlphaBlending();
}


//--------------------------------------------------------------
void ofxMtlMapping2DShape::drawID() 
{
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {        
        _super::drawID();
    
    // ---- INPUT MODE
    } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
        inputPolygon->drawID();
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DShape::setAsActiveShape(bool fromUI)
{
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
        if (activeShape != this) {
            // ----
            if (shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxMtlMapping2DControls::mapping2DControls()->hideInputModeToggle();
            } else {
                ofxMtlMapping2DControls::mapping2DControls()->showInputModeToggle();
            }
            
            // ----
            previousActiveShape = activeShape;
            activeShape = this;
            
            // Update UI
            if (fromUI) {
                setAsActive();
            } else {
                ofxMtlMapping2DControls::mapping2DControls()->setAsActiveShapeWithId(shapeId, shapeType);
            }
        }
    
    // ---- INPUT MODE
    } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
        if (activeShape != this) {
            previousActiveShape = activeShape;
            activeShape = this;
            
            // Update UI
            if (fromUI) {
                inputPolygon->setAsActive();
            } else {
                ofxMtlMapping2DControls::mapping2DControls()->setAsActiveShapeWithId(shapeId, shapeType);
            }
        }
    }
}
