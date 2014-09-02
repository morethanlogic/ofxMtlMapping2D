#include "ofxGemcutterPolygon.h"
#include "ofxGemcutterGlobal.h"

ofxGemcutterPolygon*  ofxGemcutterPolygon::activePolygon         = NULL;
ofxGemcutterPolygon*  ofxGemcutterPolygon::previousActivePolygon = NULL;
int ofxGemcutterPolygon::activeVertexId = -1;


//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxGemcutterPolygon::resetActivePolygonVars(){
    ofxGemcutterPolygon::activePolygon = NULL;
    ofxGemcutterPolygon::previousActivePolygon = NULL;
    ofxGemcutterVertex::activeVertex = NULL;
    ofxGemcutterPolygon::activeVertexId = -1;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterPolygon::ofxGemcutterPolygon()
{    
    _bMouseEventEnabled = false;
    _bMouseGrabbed = false;
    enable();
    
    // ----
    polyline = new ofPolyline();
    shapeId = -1;
}

//--------------------------------------------------------------
ofxGemcutterPolygon::~ofxGemcutterPolygon(){

    // ----
    while(!vertices.empty()) delete vertices.back(), vertices.pop_back();
	vertices.clear();
    
    // ----
    polyline->clear();
    delete polyline;
    
    // ----
    resetActivePolygonVars();
}


#pragma mark -
#pragma mark Lifecycle

//--------------------------------------------------------------
void ofxGemcutterPolygon::init(int sId, bool defaultShape) 
{
    shapeId = sId;
	
    if (defaultShape) {
        createDefaultShape();
    }
    
    updatePolyline();
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::update()
{
	if (activePolygon == this) {        
        list<ofxGemcutterVertex*>::iterator it;
        for (it=vertices.begin(); it!=vertices.end(); it++) {
            ofxGemcutterVertex* vertex = *it;
            
            if(ofxGemcutterVertex::activeVertex == vertex) {
                activeVertexId = distance(vertices.begin(), it);
            }
            
            if (vertex->toBeRemoved) {
                vertex->kill();
                vertices.remove(vertex);
            }
        }
        
        updatePolyline();
        
	}
    
    //EST CE NECESSAIRE !!!!
    
    else {
        disableVertices();
	}
}


//--------------------------------------------------------------
void ofxGemcutterPolygon::draw()
{		
    ofPushStyle();
    
    ofEnableAlphaBlending();
        
    // ----
    list<ofxGemcutterVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxGemcutterVertex* vertex = *it;
        vertex->drawBack();
    }
    
    // ---
    vector<ofPoint> &polyPoints = polyline->getVertices();
    
    ofFill();
    ofBeginShape();
    {
        if(activePolygon == this) {
            ofSetColor(0, 216, 255, 50);
        } else {
            ofSetColor(0, 216, 255, 20);
        }
        
        for (int i=0; i < polyPoints.size(); i++) {
            ofVertex(polyPoints[i].x, polyPoints[i].y);
        }
    }
    ofEndShape(true);

    ofNoFill();
    ofBeginShape();
    {
        ofSetColor(0, 216, 255, 150);
        for (int i=0; i < polyPoints.size(); i++) {
            ofVertex(polyPoints[i].x, polyPoints[i].y);
        }
    }
    ofEndShape(true);

    // ----
    //Vertices
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxGemcutterVertex* vertex = *it;
        vertex->drawTop();
    }

	ofDisableAlphaBlending();
    ofPopStyle();
}


//--------------------------------------------------------------
void ofxGemcutterPolygon::drawID() 
{
   
    int xOffset;
    int yOffset = 4;
    if (shapeId < 10) {
        xOffset = -3;
    } else if (shapeId < 100) {
        xOffset = -6;
    } else {
        xOffset = -11;
    }
    
    ofPushMatrix();
    {
        ofTranslate(_centroid2D.x, _centroid2D.y);
        ofScale(1/ofxGemcutterGlobal::getCurrZoomFactor(), 1/ofxGemcutterGlobal::getCurrZoomFactor());
        
        
        ofSetHexColor(0x000000);
        ofFill();
        ofRect(-10, -10, 20, 20);
        ofSetHexColor(0xFFFFFF);
        ofxGemcutterSettings::infoFont.drawString(ofToString(shapeId), xOffset, yOffset);
    }
    ofPopMatrix();
}



#pragma mark -
#pragma mark Update Shapes

//--------------------------------------------------------------
void ofxGemcutterPolygon::updatePosition(int xInc, int yInc)
{
    list<ofxGemcutterVertex*>::iterator it;
	for (it=vertices.begin(); it!=vertices.end(); it++) {
		ofxGemcutterVertex* vertex = *it;
		vertex->setPosition(vertex->x + xInc, vertex->y + yInc);
        vertex->updateCenter();
	}
    
    updatePolyline();
    
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::updatePolyline()
{
    // ----
    polyline->clear();
    
    list<ofxGemcutterVertex*>::iterator it;
    
    if (shapeType == MAPPING_2D_SHAPE_GRID) {
        for (int i=0; i <= 1; i++) {
            for (it=vertices.begin(); it!=vertices.end(); it++) {
                ofxGemcutterVertex* vertex = *it;
                if (vertex->bIsOnAnEdge && vertex->edgeIndex == i) {
                    // We check this for the grids, so that we don't add the vertices forming the interior of the grid
                    polyline->addVertex(vertex->center.x, vertex->center.y);
                }
            }
        }
        
        for (int i=2; i <= 3; i++) {
            for (it=vertices.end(); it!=vertices.begin(); it--) {
                ofxGemcutterVertex* vertex = *it;
                
                if (vertex->bIsOnAnEdge && vertex->edgeIndex == i) {
                    // We check this for the grids, so that we don't add the vertices forming the interior of the grid
                    polyline->addVertex(vertex->center.x, vertex->center.y);
                }
            }
        }
        
    } else {
        for (it=vertices.begin(); it!=vertices.end(); it++) {
            ofxGemcutterVertex* vertex = *it;
            polyline->addVertex(vertex->center.x, vertex->center.y);
        }
    }
    
    // ----
    _centroid2D = polyline->getCentroid2D();

    
    // ---- Interactive obj
    boundingBox = polyline->getBoundingBox();
    setPosition(boundingBox.x, boundingBox.y);
    setSize(boundingBox.width, boundingBox.height);
}

//--------------------------------------------------------------
bool ofxGemcutterPolygon::hitTest(int tx, int ty) const
{
    if (polyline->inside(tx, ty)) {
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::select(int x, int y)
{
    _grabAnchor.set(x, y);
    
    if(ofxGemcutterVertex::activeVertex)
        return;
    
    setAsActive();
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::setAsActive()
{
    if (activePolygon != this) {
        previousActivePolygon = activePolygon;
        activePolygon = this;
        activeVertexId = -1;
        
        if (previousActivePolygon) {
            previousActivePolygon->setAsIdle();
        }
        
        enableVertices();
    }
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::setAsIdle()
{
    disable();
    disableVertices();
}

#pragma mark -
#pragma mark Mapping Vertices

//--------------------------------------------------------------
void ofxGemcutterPolygon::enableVertices()
{
    list<ofxGemcutterVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxGemcutterVertex* vertex = *it;
        vertex->enabled = true;
    }
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::disableVertices()
{
    list<ofxGemcutterVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxGemcutterVertex* vertex = *it;
        vertex->enabled = false;
    }
}

//--------------------------------------------------------------
ofxGemcutterVertex* ofxGemcutterPolygon::getVertex(int index)
{
    list<ofxGemcutterVertex*>::iterator it;
    it = vertices.begin();
    advance(it, index);

    return *it;
}

//--------------------------------------------------------------
int ofxGemcutterPolygon::getNbOfVertices()
{
    return vertices.size();
}

#pragma mark -
#pragma mark Modify Shape at run time
//--------------------------------------------------------------
void ofxGemcutterPolygon::addPoint(int x, int y)
{
	//Check if we are not clicking on an already existing vertex
	list<ofxGemcutterVertex*>::iterator it;
	for (it=vertices.begin(); it!=vertices.end(); it++) {
		ofxGemcutterVertex* vertex = *it;
		
		//CE IF JE DOIS POUVOIR LE REMPLACER PAR UNE VAR STATIC.
		if (vertex->isMouseOver()){
			cout << "EXISTING VERTEX \n";
			return;
		}
	}
	
	//Check if we are not an existing line
	if (vertices.size() >= 2) {
		int nextVertex = 0;
		list<ofxGemcutterVertex*>::iterator it;
		list<ofxGemcutterVertex*>::iterator itNext;
		for (it=vertices.begin(); it!=vertices.end(); it++) {
			it++;
			if (it == vertices.end()) {
				itNext = vertices.begin();
			} else {
				itNext = it;
			}
			it--;
			ofxGemcutterVertex* vertex = *it;
			ofxGemcutterVertex* nextVertex = *itNext;
			
			double dist = distanceToSegment(vertex->center, nextVertex->center, ofVec2f(x, y));
			if ( dist < 10.0) {
				//Create a new vertex
				ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
				newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
				newVertex->enabled = true;
				it++;
				vertices.insert(it, newVertex);
				it--;
				return;
			}
		}
        return;
	}
	
	//Create a new vertex
	ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
	newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
	newVertex->enabled = true;
	vertices.push_back(newVertex);
}


#pragma mark -
#pragma mark ofxMSAInteractiveObject related
//--------------------------------------------------------------
void ofxGemcutterPolygon::disable()
{
    if (!_bMouseEventEnabled) {
        return;
    }
    
    _bMouseEventEnabled = false;
    disableAllEvents();
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::enable()
{
    if (_bMouseEventEnabled) {
        return;
    }
    
    _bMouseEventEnabled = true;
    
    disableAllEvents();
	enableMouseEvents();
    
    _bMouseGrabbed = true;
}

#pragma mark -
#pragma mark Interactive Obj Callback methods
//--------------------------------------------------------------
void ofxGemcutterPolygon::onPress(int x, int y, int button)
{
    _bMouseGrabbed = true;
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::mouseDragged(int x, int y, int button)
{
    if(ofxGemcutterVertex::activeVertex)
        return;
    
    if (!ofGetWindowRect().inside(x, y))
        return;
    
    if(activePolygon == this && _bMouseGrabbed) {
        updatePosition(x - _grabAnchor.x, y - _grabAnchor.y);
        _grabAnchor.set(x, y);
	}
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::onRelease(int x, int y, int button)
{
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
void ofxGemcutterPolygon::onReleaseOutside(int x, int y, int button)
{
    _bMouseGrabbed = false;
}
