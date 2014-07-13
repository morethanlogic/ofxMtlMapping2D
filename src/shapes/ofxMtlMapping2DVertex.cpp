#include "ofxMtlMapping2DVertex.h"
#include "ofxMtlMapping2DGlobal.h"
#include "ofxMtlMapping2DSettings.h"
#include "ofxMtlMapping2DShapes.h"

ofxMtlMapping2DVertex* ofxMtlMapping2DVertex::activeVertex = NULL;

//--------------------------------------------------------------
ofxMtlMapping2DVertex::ofxMtlMapping2DVertex()
{
	disableAllEvents();
	enableMouseEvents();
	enabled = false;
    _bMouseGrabbed = false;
    bIsOnAnEdge = true;
    edgeIndex = -1;
	
	//Vertex
	setSize(30, 30);
	toBeRemoved = false;
    isDefiningTectureCoord = false;
	updateCenter();
}

//--------------------------------------------------------------
ofxMtlMapping2DVertex::~ofxMtlMapping2DVertex()
{
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::init(float _x, float _y, int _index) 
{
	index = _index;
    setPosition(_x, _y);
	
	//Center
	updateCenter();
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::kill()
{
    if(activeVertex == this) {
		activeVertex = NULL;
	}
    
    delete this;
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::update()
{
    
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::drawBack()
{
    float currWidth = width;
    float currHeight = height;
    
    setSize(30/ofxMtlMapping2DGlobal::getCurrZoomFactor(), 30/ofxMtlMapping2DGlobal::getCurrZoomFactor());
    setPosition(x+(currWidth - width)/2, y+(currHeight - height)/2);
    
    float halfSize = width/2;
    float skinSize = 20/ofxMtlMapping2DGlobal::getCurrZoomFactor();
    
    ofFill();
    if(activeVertex == this) {
        ofSetColor(255, 255, 255, 150);
        ofCircle(x+halfSize, y+halfSize, skinSize);
    } else if(isMouseOver()) {
        ofSetColor(255, 255, 255, 125);
        ofCircle(x+halfSize, y+halfSize, skinSize);
    } else {
        //ofSetColor(255, 255, 255, 80);
    }		
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::drawTop()
{
    ofNoFill();
    if(activeVertex == this) {
        ofSetColor(255, 255, 0, 150);
    } else if(isMouseOver()) {
        ofSetColor(255, 0, 0, 120);
    } else {
        ofSetColor(255, 255, 255, 80);
    }
    
    //ofCircle(x+11, y+11, 8);
    
    float skinOffset = 11/ofxMtlMapping2DGlobal::getCurrZoomFactor();
    float skinSize = 8/ofxMtlMapping2DGlobal::getCurrZoomFactor();

    ofRect(x+skinOffset, y+skinOffset, skinSize, skinSize);
}

//--------------------------------------------------------------
ofxMtlMapping2DVertex* ofxMtlMapping2DVertex::getActiveVertex() 
{
	return activeVertex;
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::updateCenter() 
{
    float halfSize = width/2;

    center.set(x+halfSize, y+halfSize);
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::snapIt(float _x, float _y) 
{
    setPosition(_x, _y);
    updateCenter();
}


#pragma mark -
#pragma mark up with keyboard
//--------------------------------------------------------------
void ofxMtlMapping2DVertex::left() 
{
    if(activeVertex) {
        snapIt(x-(1/ofxMtlMapping2DGlobal::getCurrZoomFactor()), y);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::up() 
{
    if(activeVertex) {
        snapIt(x, y-(1/ofxMtlMapping2DGlobal::getCurrZoomFactor()));
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::right() 
{
    if(activeVertex) {
        snapIt(x+(1/ofxMtlMapping2DGlobal::getCurrZoomFactor()), y);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::down() 
{
    if(activeVertex) {
        snapIt(x, y+(1/ofxMtlMapping2DGlobal::getCurrZoomFactor()));
    }
}


#pragma mark -
#pragma mark Interactive Obj Callback methods
//--------------------------------------------------------------
void ofxMtlMapping2DVertex::mouseDragged(int x, int y, int button)
{
    if(activeVertex == this) {
		this->x = x - width/2;
		this->y = y - height/2;
		updateCenter();
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onPress(int x, int y, int button) 
{
	if (button == 0) {
        _bMouseGrabbed = true;
		activeVertex = this;
	} else if (button == 2 && !isDefiningTectureCoord && ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled
               && ofxMtlMapping2DShape::activeShape->getNbOfVertices() > 3 && ofxMtlMapping2DShape::activeShape->shapeType == MAPPING_2D_SHAPE_MASK) {
		toBeRemoved = true;
		activeVertex = NULL;
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onPressOutside(int x, int y, int button)
{
    if(activeVertex == this) {
		activeVertex = NULL;
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::setAsActive() 
{
    activeVertex = this;
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onRelease(int x, int y, int button) 
{
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onReleaseOutside(int x, int y, int button) 
{
	if(activeVertex == this) {
		activeVertex = NULL;
	}
    
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
bool ofxMtlMapping2DVertex::isDragged()
{
    return _bMouseGrabbed;
}