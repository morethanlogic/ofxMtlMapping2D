#include "ofxMtlMapping2DPolygon.h"
#include "ofxMtlMapping2DGlobal.h"

ofxMtlMapping2DPolygon*  ofxMtlMapping2DPolygon::activePolygon         = NULL;
ofxMtlMapping2DPolygon*  ofxMtlMapping2DPolygon::previousActivePolygon = NULL;
int ofxMtlMapping2DPolygon::activeVertexId = -1;


//--------------------------------------------------------------
//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::resetActivePolygonVars(){
    ofxMtlMapping2DPolygon::activePolygon = NULL;
    ofxMtlMapping2DPolygon::previousActivePolygon = NULL;
    ofxMtlMapping2DVertex::activeVertex = NULL;
    ofxMtlMapping2DPolygon::activeVertexId = -1;
}



//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DPolygon::ofxMtlMapping2DPolygon()
{    
    _bMouseEventEnabled = false;
    _bMouseGrabbed = false;
    enable();
    
    // ----
    polyline = new ofPolyline();
    shapeId = -1;
}

//--------------------------------------------------------------
ofxMtlMapping2DPolygon::~ofxMtlMapping2DPolygon(){

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
void ofxMtlMapping2DPolygon::init(int sId, bool defaultShape) 
{
    shapeId = sId;
	
    if (defaultShape) {
        createDefaultShape();
    }
    
    updatePolyline();
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::update()
{
	if (activePolygon == this) {        
        list<ofxMtlMapping2DVertex*>::iterator it;
        for (it=vertices.begin(); it!=vertices.end(); it++) {
            ofxMtlMapping2DVertex* vertex = *it;
            
            if(ofxMtlMapping2DVertex::activeVertex == vertex) {
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
void ofxMtlMapping2DPolygon::draw()
{		
    ofEnableAlphaBlending();
        
    // ----
    list<ofxMtlMapping2DVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxMtlMapping2DVertex* vertex = *it;
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
        ofxMtlMapping2DVertex* vertex = *it;
        vertex->drawTop();
    }

	ofDisableAlphaBlending();	
}


//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::drawID() 
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
        ofScale(1/ofxMtlMapping2DGlobal::getCurrZoomFactor(), 1/ofxMtlMapping2DGlobal::getCurrZoomFactor());
        
        
        ofSetHexColor(0x000000);
        ofFill();
        ofRect(-10, -10, 20, 20);
        ofSetHexColor(0xFFFFFF);
        ofxMtlMapping2DSettings::infoFont.drawString(ofToString(shapeId), xOffset, yOffset);
    }
    ofPopMatrix();
}



#pragma mark -
#pragma mark Update Shapes

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::updatePosition(int xInc, int yInc)
{
    list<ofxMtlMapping2DVertex*>::iterator it;
	for (it=vertices.begin(); it!=vertices.end(); it++) {
		ofxMtlMapping2DVertex* vertex = *it;
		vertex->setPosition(vertex->x + xInc, vertex->y + yInc);
        vertex->updateCenter();
	}
    
    updatePolyline();
    
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::updatePolyline()
{
    // ----
    polyline->clear();
    
    list<ofxMtlMapping2DVertex*>::iterator it;
    
    if (shapeType == MAPPING_2D_SHAPE_GRID) {
        for (int i=0; i <= 1; i++) {
            for (it=vertices.begin(); it!=vertices.end(); it++) {
                ofxMtlMapping2DVertex* vertex = *it;
                if (vertex->bIsOnAnEdge && vertex->edgeIndex == i) {
                    // We check this for the grids, so that we don't add the vertices forming the interior of the grid
                    polyline->addVertex(vertex->center.x, vertex->center.y);
                }
            }
        }
        
        for (int i=2; i <= 3; i++) {
            for (it=vertices.end(); it!=vertices.begin(); it--) {
                ofxMtlMapping2DVertex* vertex = *it;
                
                if (vertex->bIsOnAnEdge && vertex->edgeIndex == i) {
                    // We check this for the grids, so that we don't add the vertices forming the interior of the grid
                    polyline->addVertex(vertex->center.x, vertex->center.y);
                }
            }
        }
        
    } else {
        for (it=vertices.begin(); it!=vertices.end(); it++) {
            ofxMtlMapping2DVertex* vertex = *it;
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
bool ofxMtlMapping2DPolygon::hitTest(int tx, int ty) const
{
    if (polyline->inside(tx, ty)) {
        return true;
    } else {
        return false;
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::select(int x, int y)
{
    _grabAnchor.set(x, y);
    
    if(ofxMtlMapping2DVertex::activeVertex)
        return;
    
    setAsActive();
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::setAsActive()
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
void ofxMtlMapping2DPolygon::setAsIdle()
{
    disable();
    disableVertices();
}

#pragma mark -
#pragma mark Mapping Vertices

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::enableVertices()
{
    list<ofxMtlMapping2DVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxMtlMapping2DVertex* vertex = *it;
        vertex->enabled = true;
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::disableVertices()
{
    list<ofxMtlMapping2DVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxMtlMapping2DVertex* vertex = *it;
        vertex->enabled = false;
    }
}

//--------------------------------------------------------------
ofxMtlMapping2DVertex* ofxMtlMapping2DPolygon::getVertex(int index)
{
    list<ofxMtlMapping2DVertex*>::iterator it;
    it = vertices.begin();
    advance(it, index);

    return *it;
}

//--------------------------------------------------------------
int ofxMtlMapping2DPolygon::getNbOfVertices()
{
    return vertices.size();
}

#pragma mark -
#pragma mark Modify Shape at run time
//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::addPoint(int x, int y)
{
	//Check if we are not clicking on an already existing vertex
	list<ofxMtlMapping2DVertex*>::iterator it;
	for (it=vertices.begin(); it!=vertices.end(); it++) {
		ofxMtlMapping2DVertex* vertex = *it;
		
		//CE IF JE DOIS POUVOIR LE REMPLACER PAR UNE VAR STATIC.
		if (vertex->isMouseOver()){
			cout << "EXISTING VERTEX \n";
			return;
		}
	}
	
	//Check if we are not an existing line
	if (vertices.size() >= 2) {
		int nextVertex = 0;
		list<ofxMtlMapping2DVertex*>::iterator it;
		list<ofxMtlMapping2DVertex*>::iterator itNext;
		for (it=vertices.begin(); it!=vertices.end(); it++) {
			it++;
			if (it == vertices.end()) {
				itNext = vertices.begin();
			} else {
				itNext = it;
			}
			it--;
			ofxMtlMapping2DVertex* vertex = *it;
			ofxMtlMapping2DVertex* nextVertex = *itNext;
			
			double dist = distanceToSegment(vertex->center, nextVertex->center, ofVec2f(x, y));
			if ( dist < 10.0) {
				//Create a new vertex
				ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
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
	ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
	newVertex->init(x-newVertex->width/2, y-newVertex->height/2);
	newVertex->enabled = true;
	vertices.push_back(newVertex);
}


#pragma mark -
#pragma mark ofxMSAInteractiveObject related
//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::disable()
{
    if (!_bMouseEventEnabled) {
        return;
    }
    
    _bMouseEventEnabled = false;
    disableAllEvents();
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::enable()
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
void ofxMtlMapping2DPolygon::onPress(int x, int y, int button)
{
    _bMouseGrabbed = true;
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::mouseDragged(int x, int y, int button)
{
    if(ofxMtlMapping2DVertex::activeVertex)
        return;
    
    if (!ofGetWindowRect().inside(x, y))
        return;
    
    if(activePolygon == this && _bMouseGrabbed) {
        updatePosition(x - _grabAnchor.x, y - _grabAnchor.y);
        _grabAnchor.set(x, y);
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::onRelease(int x, int y, int button)
{
    _bMouseGrabbed = false;
}

//--------------------------------------------------------------
void ofxMtlMapping2DPolygon::onReleaseOutside(int x, int y, int button)
{
    _bMouseGrabbed = false;
}
