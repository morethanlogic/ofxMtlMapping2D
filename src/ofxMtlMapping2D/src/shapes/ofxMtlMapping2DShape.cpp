#include "ofxMtlMapping2DShape.h"
#include "ofxMtlMapping2DControls.h"

int ofxMtlMapping2DShape::nextShapeId = 0;
ofxMtlMapping2DShape*  ofxMtlMapping2DShape::activeShape = NULL;
ofxMtlMapping2DShape*  ofxMtlMapping2DShape::previousActiveShape = NULL;
int                 ofxMtlMapping2DShape::activeShapeCurrVertexId = -1;

//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxMtlMapping2DShape::resetActiveShapeVars(){
    delete ofxMtlMapping2DShape::activeShape;
    ofxMtlMapping2DShape::activeShape = NULL;
    ofxMtlMapping2DShape::previousActiveShape = NULL;
    ofxMtlMapping2DShape::activeShapeCurrVertexId = -1;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DShape::ofxMtlMapping2DShape()
{
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
    }
    
    if (activeShape == this) {
        if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
            inputPolygon->update();
        }
        
        // ---- recalculate the homography transformation matrix.
        calcHomography();
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
	if (activeShape != this) {
        previousActiveShape = activeShape;
        activeShape = this;
        activeShapeCurrVertexId = -1;
        
        // Update UI
        if (fromUI) {
            setAsActive();
        } else {
            ofxMtlMapping2DControls::mapping2DControls()->setAsActiveShapeWithId(shapeId);
        }
    }
}
