#include "ofxMtlMapping2DVertex.h"
#include "ofxMtlMapping2DSettings.h"

ofxMtlMapping2DVertex* ofxMtlMapping2DVertex::activeVertex = NULL;

//--------------------------------------------------------------
ofxMtlMapping2DVertex::ofxMtlMapping2DVertex()
{
	disableAllEvents();
	enableMouseEvents();
	enabled = false;
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
    ofFill();
    if(activeVertex == this) {
        ofSetColor(255, 255, 255, 150);
        ofCircle(x+15, y+15, 20);
    } else if(isMouseOver()) {
        ofSetColor(255, 255, 255, 125);
        ofCircle(x+15, y+15, 20);
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
    ofRect(x+11, y+11, 8, 8);		
}

//--------------------------------------------------------------
ofxMtlMapping2DVertex* ofxMtlMapping2DVertex::getActiveVertex() 
{
	return activeVertex;
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::updateCenter() 
{
	center.set(x+15, y+15);
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
    if(activeVertex)
        snapIt(x-1, y);
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::up() 
{
    if(activeVertex)
        snapIt(x, y-1);
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::right() 
{
    if(activeVertex)
        snapIt(x+1, y);
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::down() 
{
    if(activeVertex)
        snapIt(x, y+1);
}


#pragma mark -
#pragma mark Interactive Obj Callback methods
//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onRollOver(int x, int y) 
{
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onRollOut() 
{
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onMouseMove(int x, int y)
{
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onDragOver(int x, int y, int button) 
{
	if(activeVertex == this) {
		this->x = x - width/2;
		this->y = y - height/2;
		updateCenter();
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onDragOutside(int x, int y, int button) 
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
		activeVertex = this;
	} else if (button == 2 && !isDefiningTectureCoord && ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled) {
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
}

//--------------------------------------------------------------
void ofxMtlMapping2DVertex::onReleaseOutside(int x, int y, int button) 
{
	if(activeVertex == this) {
		activeVertex = NULL;
	}
}