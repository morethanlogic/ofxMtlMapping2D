#include "ofxMtlMapping2D.h"
#include "ofxMtlMapping2DSettings.h"
#include "ofxMtlMapping2DControls.h"
#include "ofxMtlMapping2DInput.h"

struct Comparator {
    bool operator()(ofxMtlMapping2DShape* first, ofxMtlMapping2DShape* second) const {
        return first->shapeId > second->shapeId;
    }
};

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2D::ofxMtlMapping2D() 
{
}

//--------------------------------------------------------------
ofxMtlMapping2D::~ofxMtlMapping2D() 
{
    // ----
    while(!_pmShapes.empty()) delete _pmShapes.back(), _pmShapes.pop_back();
	_pmShapes.clear();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::init(int width, int height)
{
    ofxMtlMapping2DControls::mapping2DControls()->disable();
    
    // ----
    _fbo.allocate(width, height, GL_RGBA);

    // ----
    ofxMtlMapping2DSettings::infoFont.loadFont("mapping/controls/ReplicaBold.ttf", 10);
    
    // ----
    loadShapesList("mapping/xml/shapes.xml");
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::update()
{    
    // ---- save mapping to xml
    if(ofxMtlMapping2DControls::mapping2DControls()->saveMapping()) {
        ofxMtlMapping2DControls::mapping2DControls()->resetSaveMapping();
        saveShapesList("mapping/xml/shapes.xml");
    }
    
    
    // ---- load mapping from xml
    if(ofxMtlMapping2DControls::mapping2DControls()->loadMapping()) {
        ofxMtlMapping2DControls::mapping2DControls()->resetLoadMapping();
        loadShapesList("mapping/xml/shapes.xml");
    }
    
    // ----
    // Create a new shape
    if(ofxMtlMapping2DControls::mapping2DControls()->createNewQuad()) {
        ofxMtlMapping2DControls::mapping2DControls()->resetCreateNewShape();
        createQuad(ofGetWidth()/2, ofGetHeight()/2);
        return;
    }
    
    if(ofxMtlMapping2DControls::mapping2DControls()->createNewTriangle()) {
        ofxMtlMapping2DControls::mapping2DControls()->resetCreateNewShape();
        createTriangle(ofGetWidth()/2, ofGetHeight()/2);
        return;
    }
    
    // ----
    // Selected shape with UI
    if(ofxMtlMapping2DControls::mapping2DControls()->selectedShapeId() != -1) {
        list<ofxMtlMapping2DShape*>::iterator it;
        for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            if(shape->shapeId == ofxMtlMapping2DControls::mapping2DControls()->selectedShapeId()) {
                shape->setAsActiveShape(true);
                break;
            }
        }
        ofxMtlMapping2DControls::mapping2DControls()->resetSelectedShapeId();
        //return;
    }
    
    if(ofxMtlMapping2DControls::mapping2DControls()->mappingModeChanged()) {
        ofxMtlMapping2DControls::mapping2DControls()->resetMappingChangedFlag();

        // ---- OUTPUT MODE
        if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->enable();
                shape->inputPolygon->disable();
            }
        // ---- INPUT MODE
        } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->disable();
                shape->inputPolygon->enable();
            }
        }
    
    }
    
    // ----
    if(!ofxMtlMapping2DControls::mapping2DControls()->editShapes())
        return;
    
    // ----
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        shape->update();
    }
}

#pragma mark -
#pragma mark Draw / Edit Mode
//--------------------------------------------------------------
void ofxMtlMapping2D::draw()
{
    
    if(ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
        // ---- OUTPUT MODE
        if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
            render();

          
        // ---- INPUT MODE
        } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
            // ----
            drawFbo(); 
        }
        
        
        // ----
        list<ofxMtlMapping2DShape*>::iterator it;
        for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            
            if(shape != ofxMtlMapping2DShape::activeShape) {
                shape->draw();
            }
            
            
            if(ofxMtlMapping2DControls::mapping2DControls()->showShapesId())  {
                shape->drawID();
            }
        }
        
        if(ofxMtlMapping2DShape::activeShape) {
            //Draw active shape on top
            ofxMtlMapping2DShape::activeShape->draw();
        }
    }
    else {
        render();
    }
}

#pragma mark -
#pragma mark FBO
//--------------------------------------------------------------
void ofxMtlMapping2D::bind()
{
    _fbo.bind();
    ofClear(.0f, .0f, .0f, .0f);
    ofClearAlpha();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::unbind()
{
    _fbo.unbind();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::drawFbo()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    _fbo.draw(0, 0);
}

#pragma mark -
#pragma mark Render - Mapping Mode
//--------------------------------------------------------------
void ofxMtlMapping2D::render()
{
    _fbo.getTextureReference().bind();
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        shape->render();
    }
    _fbo.getTextureReference().unbind();
}


#pragma mark -
#pragma mark Shapes Related
//--------------------------------------------------------------
void ofxMtlMapping2D::createQuad(float _x, float _y)
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DQuad();
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    _pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControls::mapping2DControls()->addShapeToList(ofxMtlMapping2DShape::nextShapeId);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createTriangle(float _x, float _y)
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DTriangle();
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    _pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControls::mapping2DControls()->addShapeToList(ofxMtlMapping2DShape::nextShapeId);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::deleteShape()
{
    if (ofxMtlMapping2DShape::activeShape) {
        ofxMtlMapping2DControls::mapping2DControls()->clearShapesList();
        _pmShapes.remove(ofxMtlMapping2DShape::activeShape);
        ofxMtlMapping2DShape::resetActiveShapeVars();
    }
    
    // Re populate the UI List
    list<ofxMtlMapping2DShape*>::reverse_iterator it;
    for (it=_pmShapes.rbegin(); it!=_pmShapes.rend(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        ofxMtlMapping2DControls::mapping2DControls()->addShapeToList(shape->shapeId);

    }
}

//--------------------------------------------------------------
//void ofxMtlMapping2D::disableAllShapes()
//{
//    // Disable all the shapes.
//    list<ofxMtlMapping2DShape*>::iterator it;
//    for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
//        ofxMtlMapping2DShape* shape = *it;
//        shape->disable();
//    }
//}

#pragma mark -
#pragma mark Mouse event
//--------------------------------------------------------------
void ofxMtlMapping2D::mousePressed(int x, int y, int button)
{
    if (ofxMtlMapping2DControls::mapping2DControls()->isHit(x, y))
        return;
    
    if(!ofxMtlMapping2DControls::mapping2DControls()->editShapes())
        return;
    
    
    // ----
    // A vertex has been selected
    if (ofxMtlMapping2DVertex::activeVertex || button == 2) {
      return;
    }
    
    // ----
    // Select an existing shape
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=_pmShapes.begin(); it!=_pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        bool grabbedOne = false;
        if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
            if(shape->hitTest(x, y)) {
                grabbedOne = true;
                shape->enable();
            }
        } else if (ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_INPUT) {
            if(shape->inputPolygon->hitTest(x, y)) {
                grabbedOne = true;
                shape->inputPolygon->enable();
            }
        }
        
        if(grabbedOne) {
            // Put active shape at the top of the list
            _pmShapes.push_front(shape);
            _pmShapes.erase(it);
            
            return;
        }
    }
    
    // ----
    if(ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled && ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
        // Add vertex to the selected shape
        if(ofxMtlMapping2DShape::activeShape) {
            ofxMtlMapping2DShape* shape = ofxMtlMapping2DShape::activeShape;
            if (shape) {
                ofLog(OF_LOG_NOTICE, "Add vertex to shape %i", shape->shapeId);
                shape->addPoint(x, y);
            } else {
                ofLog(OF_LOG_NOTICE, "No shape has been selected, can not add a vertex");
            }
        }
    }
    
}

#pragma mark -
#pragma mark Keyboard event
//--------------------------------------------------------------
void ofxMtlMapping2D::keyPressed(int key)
{    
    // ----
    switch (key) {
        case 9:  // TAB
            ofToggleFullscreen();
            break;
            
        case 'l':
            ofxMtlMapping2DControls::mapping2DControls()->disable();
            
            break;
            
        case 'm':
            ofxMtlMapping2DControls::mapping2DControls()->toggle();
            
            break;
            
        case 's':
            ofxMtlMapping2DControls::mapping2DControls()->save();
            saveShapesList("mapping/xml/shapes.xml");

            break;
            
        case 356:
            //left
            if (ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeShapeCurrVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->left();
                }
            }
            break;
                
        case 357: //up
            if (ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeShapeCurrVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->up();
                }
            }
            break;
        
        case 358: //right
            if (ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeShapeCurrVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->right();
                }
            }
            break;
                
        case 359: //down
            if (ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeShapeCurrVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->down();
                }
            }
            break;
            
        case 127:
            deleteShape();
            break;
            
        case 8:
            deleteShape();
            break;
            
        case 'n':
            if (ofxMtlMapping2DControls::mapping2DControls()->editShapes()) {
                if(ofxMtlMapping2DShape::activeShape) {
                    ofxMtlMapping2DShape::activeShapeCurrVertexId++;
                    if(ofxMtlMapping2DShape::activeShapeCurrVertexId >=  ofxMtlMapping2DShape::activeShape->vertices.size()) {
                        ofxMtlMapping2DShape::activeShapeCurrVertexId = 0;
                    }
                    
                    //-----
                    list<ofxMtlMapping2DVertex*>::iterator it;
                    
                    int vertexCounter = 0;
                    for (it=ofxMtlMapping2DShape::activeShape->vertices.begin(); it!=ofxMtlMapping2DShape::activeShape->vertices.end(); it++) {
                        ofxMtlMapping2DVertex* vertex = *it;
                        
                        if(vertexCounter == ofxMtlMapping2DShape::activeShapeCurrVertexId) {
                            vertex->setAsActive();
                            break;
                        }
                        
                        vertexCounter++;
                    }
                }
            }
            break;
    }
}


#pragma mark -
#pragma mark Load and Save Shapes List
//--------------------------------------------------------------
void ofxMtlMapping2D::loadShapesList(string _xmlPath)
{
    // Delete everything
    while(!_pmShapes.empty()) delete _pmShapes.back(), _pmShapes.pop_back();
    _pmShapes.clear();
    ofxMtlMapping2DShape::resetActiveShapeVars();
    
    
    //LOAD XML
    // ----
	//the string is printed at the top of the app
	//to give the user some feedback
	string feedBackMessage = "loading shapes.xml";
	ofLog(OF_LOG_NOTICE, "Status > " + feedBackMessage);
    
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( _shapesListXML.loadFile(_xmlPath) ){
		feedBackMessage = "shapes.xml loaded!";
	}else{
		feedBackMessage = "unable to load shapes.xml check data/ folder";
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
                
                string shapeType = _shapesListXML.getValue("setting", "nan", 0);

                if (shapeType == "quad") {
                     newShape = new ofxMtlMapping2DQuad();
                } else if (shapeType == "triangle") {
                    newShape = new ofxMtlMapping2DTriangle();
                } else {
                    newShape = new ofxMtlMapping2DQuad();
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
                _shapesListXML.popTag();
                
                
                newShape->init(shapeId);
                _pmShapes.push_front(newShape);
                
                ofxMtlMapping2DControls::mapping2DControls()->addShapeToList(shapeId);
				
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
void ofxMtlMapping2D::saveShapesList(string _xmlPath)
{
    
    list<ofxMtlMapping2DShape*> pmShapesCopy;
    pmShapesCopy.resize (_pmShapes.size());
    copy (_pmShapes.begin(), _pmShapes.end(), pmShapesCopy.begin());
    pmShapesCopy.sort(Comparator());

    ofxXmlSettings newShapesListXML;
	int shapeCounter = 0;
	
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
            cout << (*itShape).first << " => " << (*itShape).second << endl;
            
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
		
		newShapesListXML.popTag();
		
		shapeCounter++;
	}
	
	//Save to file
	newShapesListXML.saveFile(_xmlPath);
    ofLog(OF_LOG_NOTICE, "Status > settings saved to xml!");

}

//--------------------------------------------------------------
void ofxMtlMapping2D::chessBoard(int nbOfCol)
{
    ofSetColor(ofColor::white);
    ofFill();
    
    int boardWidth = ofGetWidth();
    int boardHeight = ofGetHeight();
    
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


