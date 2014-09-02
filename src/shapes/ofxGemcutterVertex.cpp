#include "ofxGemcutterVertex.h"
#include "ofxGemcutterGlobal.h"
#include "ofxGemcutterSettings.h"
#include "ofxGemcutterShapes.h"

ofxGemcutterVertex* ofxGemcutterVertex::activeVertex = NULL;

//--------------------------------------------------------------
ofxGemcutterVertex::ofxGemcutterVertex()
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
ofxGemcutterVertex::~ofxGemcutterVertex()
{
}

//--------------------------------------------------------------
void ofxGemcutterVertex::init(float _x, float _y, int _index) 
{
	index = _index;
    setPosition(_x, _y);
	
	//Center
	updateCenter();
}

//--------------------------------------------------------------
void ofxGemcutterVertex::kill()
{
    if(activeVertex == this) {
		activeVertex = NULL;
	}
    
    delete this;
}

//--------------------------------------------------------------
void ofxGemcutterVertex::update()
{
    
}

//--------------------------------------------------------------
void ofxGemcutterVertex::drawBack()
{
    float currWidth = width;
    float currHeight = height;
    
    setSize(30/ofxGemcutterGlobal::getCurrZoomFactor(), 30/ofxGemcutterGlobal::getCurrZoomFactor());
    setPosition(x+(currWidth - width)/2, y+(currHeight - height)/2);
    
    float halfSize = width/2;
    float skinSize = 20/ofxGemcutterGlobal::getCurrZoomFactor();
    
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
void ofxGemcutterVertex::drawTop()
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
    
    float skinOffset = 11/ofxGemcutterGlobal::getCurrZoomFactor();
    float skinSize = 8/ofxGemcutterGlobal::getCurrZoomFactor();

    ofRect(x+skinOffset, y+skinOffset, skinSize, skinSize);
}

//--------------------------------------------------------------
ofxGemcutterVertex* ofxGemcutterVertex::getActiveVertex() 
{
	return activeVertex;
}

//--------------------------------------------------------------
void ofxGemcutterVertex::updateCenter() 
{
    float halfSize = width/2;

    center.set(x+halfSize, y+halfSize);
}

//--------------------------------------------------------------
void ofxGemcutterVertex::snapIt(float _x, float _y) 
{
    setPosition(_x, _y);
    updateCenter();
}


#pragma mark -
#pragma mark up with keyboard
//--------------------------------------------------------------
void ofxGemcutterVertex::left() 
{
    if(activeVertex) {
        snapIt(x-(1/ofxGemcutterGlobal::getCurrZoomFactor()), y);
    }
}

//--------------------------------------------------------------
void ofxGemcutterVertex::up() 
{
    if(activeVertex) {
        snapIt(x, y-(1/ofxGemcutterGlobal::getCurrZoomFactor()));
    }
}

//--------------------------------------------------------------
void ofxGemcutterVertex::right() 
{
    if(activeVertex) {
        snapIt(x+(1/ofxGemcutterGlobal::getCurrZoomFactor()), y);
    }
}

//--------------------------------------------------------------
void ofxGemcutterVertex::down() 
{
    if(activeVertex) {
        snapIt(x, y+(1/ofxGemcutterGlobal::getCurrZoomFactor()));
    }
}


#pragma mark -
#pragma mark Interactive Obj Callback methods
//--------------------------------------------------------------
void ofxGemcutterVertex::mouseDragged(int x, int y, int button)
{
    if(activeVertex == this) {
		this->x = x - width/2;
		this->y = y - height/2;
		updateCenter();
	}
}

//--------------------------------------------------------------
void ofxGemcutterVertex::onPress(int x, int y, int button) 
{
	if (button == 0) {
        _bMouseGrabbed = true;
		activeVertex = this;
	} else if (button == 2 && !isDefiningTectureCoord && ofxGemcutterSettings::kIsManuallyAddingDeletingVertexEnabled
               && ofxGemcutterShape::activeShape->getNbOfVertices() > 3 && ofxGemcutterShape::activeShape->shapeType == MAPPING_2D_SHAPE_MASK) {
		toBeRemoved = true;
		activeVertex = NULL;
	}
}

//--------------------------------------------------------------
void ofxGemcutterVertex::onPressOutside(int x, int y, int button)
{
    if(activeVertex == this) {
		activeVertex = NULL;
	}
}

//--------------------------------------------------------------
void ofxGemcutterVertex::setAsActive() 
{
    activeVertex = this;
}

//--------------------------------------------------------------
void ofxGemcutterVertex::onRelease(int x, int y, int button) 
{
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
void ofxGemcutterVertex::onReleaseOutside(int x, int y, int button) 
{
	if(activeVertex == this) {
		activeVertex = NULL;
	}
    
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
bool ofxGemcutterVertex::isDragged()
{
    return _bMouseGrabbed;
}