#include "ofxMtlMapping2D.h"
#include "ofxMtlMapping2DGlobal.h"
#include "ofxMtlMapping2DControls.h"
#include "ofxMtlMapping2DInput.h"
#include "ofxMtlMapping2DShapeType.h"
#include "ofxMtlMapping2DShapes.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
list<ofxMtlMapping2DShape*>::iterator ofxMtlMapping2D::iteratorForShapeWithId(int shapeId)
{
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        if(shape->shapeId == shapeId) {
            return it;
        }
    }
    
    return ofxMtlMapping2DShapes::pmShapes.end();
}

//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2D::ofxMtlMapping2D() 
{
}

//--------------------------------------------------------------
ofxMtlMapping2D::~ofxMtlMapping2D() 
{
    // ----
    while(!ofxMtlMapping2DShapes::pmShapes.empty()) delete ofxMtlMapping2DShapes::pmShapes.back(), ofxMtlMapping2DShapes::pmShapes.pop_back();
	ofxMtlMapping2DShapes::pmShapes.clear();
}

//--------------------------------------------------------------
void ofxMtlMapping2D::init(int width, int height, int numSample)
{
    _mappingModeState = MAPPING_LOCKED;

    bSaveShapes = false;
    bLoadShapes = false;
    bCreateQuad = false;
    bCreateGrid = false;
    bCreateTriangle = false;
    bCreateMask = false;
    
    bDeleteShape = false;
    
    bSelectedShapeChanged = false;
    selectedShapeId = -1;
    
    // ---
    addListeners();
    
    // ---
    _numSample = numSample;
    _fbo.allocate(width, height, GL_RGBA, _numSample);

    // ---
    // The first time we call ofxMtlMapping2DControls we need to call the init() method
    ofxMtlMapping2DControlsSharedInstance(this).init();
    
    // ---
    ofxMtlMapping2DSettings::infoFont.loadFont("ui/ReplicaBold.ttf", 10);
    
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    setupSyphon();
#endif
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::setModeState(MappingModeState mappingModeState)
{
    _mappingModeState = mappingModeState;
}

//--------------------------------------------------------------
MappingModeState ofxMtlMapping2D::getModeState()
{
    return _mappingModeState;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::update()
{    
    
    if (_mappingModeState == MAPPING_LOCKED) {
        return;
    }
        
    // ---
    // Edit Mode Actions
    
    // Load and Save mapping
    if (bSaveShapes) {
        saveShapesList();
        return;
    }
    else if (bLoadShapes) {
        loadShapesList();
        return;
    }
    
    // Create new shapes
    else if (bCreateQuad) {
        bCreateQuad = false;
        createQuad();
        return;
    }
    else if (bCreateGrid) {
        bCreateGrid = false;
        createGrid();
        return;
    }
    else if (bCreateTriangle) {
        bCreateTriangle = false;
        createTriangle();
        return;
    }
    else if (bCreateMask) {
        bCreateMask = false;
        createMask();
        return;
    }
    
    else if (bDeleteShape) {
        bDeleteShape = false;
        deleteShape();
        return;
    }
    
    // ---
    // Output Mode
    switch (ofxMtlMapping2DGlobal::getEditView()) {
        case MAPPING_CHANGE_TO_INPUT_VIEW:
        {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_INPUT_VIEW);
            
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->setAsIdle();
                shape->inputPolygon->enable();
            }
            break;
        }
            
        case MAPPING_CHANGE_TO_OUTPUT_VIEW:
        {
            ofxMtlMapping2DGlobal::setEditView(MAPPING_OUTPUT_VIEW);
            
            list<ofxMtlMapping2DShape*>::iterator it;
            for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
                ofxMtlMapping2DShape* shape = *it;
                shape->enable();
                
                if(shape->inputPolygon) {
                    // If this Shape is textured and has an 'inputPolygon'
                    shape->inputPolygon->setAsIdle();
                }
            }
            break;
        }
    }
    
    // ---
    // Selected shape with UI
    if (bSelectedShapeChanged) {
        bSelectedShapeChanged = false;

        list<ofxMtlMapping2DShape*>::iterator it = iteratorForShapeWithId(selectedShapeId);
        if(it != ofxMtlMapping2DShapes::pmShapes.end()) {
            ofxMtlMapping2DShape* shape = *it;
            shape->setAsActiveShape(true);
            
            // Put active shape at the top of the list
            ofxMtlMapping2DShapes::pmShapes.push_front(shape);
            ofxMtlMapping2DShapes::pmShapes.erase(it);
        }
    }
    
    // ---
    // Update the Shapes
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        shape->update();
    }
}

#pragma mark -
#pragma mark Draw / Edit Mode
//--------------------------------------------------------------
void ofxMtlMapping2D::draw()
{
    
    if (_mappingModeState == MAPPING_EDIT) {
        switch (ofxMtlMapping2DGlobal::getEditView()) {
            case MAPPING_INPUT_VIEW:
                drawFbo();
                break;
                
            case MAPPING_OUTPUT_VIEW:
                render();
                break;
        }
        
        // --- Draw all shapes
        list<ofxMtlMapping2DShape*>::iterator it;
        for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            
            if(shape != ofxMtlMapping2DShape::activeShape) {
                shape->draw();
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
    _fbo.begin();
    ofClear(.0f, .0f, .0f, .0f);
    ofClearAlpha();

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    drawSyphon();
#endif
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::unbind()
{
    _fbo.end();
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
    list<ofxMtlMapping2DShape*>::iterator it;
    
    // Textured shapes
    _fbo.getTextureReference().bind();
    ofSetColor(255, 255, 255, 255);
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
        if (shape->shapeType != MAPPING_2D_SHAPE_MASK) {
            shape->render();
        }
    }
    _fbo.getTextureReference().unbind();
    
    // Masks - non textured shapes
    ofSetColor(0, 0, 0, 255);
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        
        if (shape->shapeType == MAPPING_2D_SHAPE_MASK) {
            shape->render();
        }
    }
}


#pragma mark -
#pragma mark Shapes Related
//--------------------------------------------------------------
void ofxMtlMapping2D::createQuad()
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DQuad();
    newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_QUAD);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createGrid()
{
    ofxMtlMapping2DShape::nextShapeId++;
    
    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DGrid();
    newShape->shapeType = MAPPING_2D_SHAPE_GRID;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_GRID);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createTriangle()
{
    ofxMtlMapping2DShape::nextShapeId++;

    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DTriangle();
    newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_TRIANGLE);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::createMask()
{
    ofxMtlMapping2DShape::nextShapeId++;
    
    ofxMtlMapping2DShape* newShape = new ofxMtlMapping2DMask();
    newShape->shapeType = MAPPING_2D_SHAPE_MASK;
    newShape->init(ofxMtlMapping2DShape::nextShapeId, true);
    ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
    
    ofxMtlMapping2DControlsSharedInstance().addShapeToList(ofxMtlMapping2DShape::nextShapeId, MAPPING_2D_SHAPE_MASK);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::deleteShape()
{
    if (ofxMtlMapping2DShape::activeShape) {
        ofxMtlMapping2DControlsSharedInstance().clearShapesList();
        ofxMtlMapping2DShapes::pmShapes.remove(ofxMtlMapping2DShape::activeShape);
        delete ofxMtlMapping2DShape::activeShape;
        ofxMtlMapping2DShape::resetActiveShapeVars();
    
        // Re populate the UI List
        list<ofxMtlMapping2DShape*>::reverse_iterator it;
        for (it=ofxMtlMapping2DShapes::pmShapes.rbegin(); it!=ofxMtlMapping2DShapes::pmShapes.rend(); it++) {
            ofxMtlMapping2DShape* shape = *it;
            ofxMtlMapping2DControlsSharedInstance().addShapeToList(shape->shapeId, shape->shapeType);

        }
    }
}

#pragma mark -
#pragma mark Add / Remove Listeners

//--------------------------------------------------------------
void ofxMtlMapping2D::addListeners() {
	ofAddListener(ofEvents().mousePressed, this, &ofxMtlMapping2D::mousePressed);
    ofAddListener(ofEvents().keyPressed, this, &ofxMtlMapping2D::keyPressed);
    ofAddListener(ofEvents().windowResized, this, &ofxMtlMapping2D::windowResized);
}

//--------------------------------------------------------------
void ofxMtlMapping2D::removeListeners() {
	ofRemoveListener(ofEvents().mousePressed, this, &ofxMtlMapping2D::mousePressed);   
    ofRemoveListener(ofEvents().keyPressed, this, &ofxMtlMapping2D::keyPressed);
    ofRemoveListener(ofEvents().windowResized, this, &ofxMtlMapping2D::windowResized);

}

#pragma mark -
#pragma mark Events

//--------------------------------------------------------------
void ofxMtlMapping2D::windowResized(ofResizeEventArgs &e)
{
    // resize / re-allocate the source FBO
    _fbo.allocate(e.width , e.height, GL_RGBA, _numSample);

    ofxMtlMapping2DControlsSharedInstance().windowResized();
}


//--------------------------------------------------------------
void ofxMtlMapping2D::mousePressed(ofMouseEventArgs &e)
{
    int eX = e.x;
    int eY = e.y;
    int eButton = e.button;
    
    if (ofxMtlMapping2DControlsSharedInstance().isHit(eX, eY))
        return;
    
    if(_mappingModeState == MAPPING_LOCKED)
        return;
    
    
    // ----
    // A vertex has been selected
    if (ofxMtlMapping2DVertex::activeVertex || eButton == 2) {
      return;
    }
    
    // ----
    // Select an existing shape
    list<ofxMtlMapping2DShape*>::iterator it;
    for (it=ofxMtlMapping2DShapes::pmShapes.begin(); it!=ofxMtlMapping2DShapes::pmShapes.end(); it++) {
        ofxMtlMapping2DShape* shape = *it;
        bool grabbedOne = false;
        
        switch (ofxMtlMapping2DGlobal::getEditView()) {
            case MAPPING_INPUT_VIEW:
                if (shape->inputPolygon || shape->shapeType != MAPPING_2D_SHAPE_MASK) {
                    if(shape->inputPolygon->hitTest(eX, eY)) {
                        grabbedOne = true;
                        shape->inputPolygon->select(eX, eY);
                        shape->inputPolygon->enable();
                    }
                }
                break;
                
            case MAPPING_OUTPUT_VIEW:
                if(shape->hitTest(eX, eY)) {
                    grabbedOne = true;
                    shape->select(eX, eY);
                    shape->enable();
                }
                break;
        }
        
        if(grabbedOne) {
            // Put active shape at the top of the list
            ofxMtlMapping2DShapes::pmShapes.push_front(shape);
            ofxMtlMapping2DShapes::pmShapes.erase(it);
            
            return;
        }
    }
    
    // ----
    if(ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled && ofxMtlMapping2DGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        // Add vertex to the selected shape
        if(ofxMtlMapping2DShape::activeShape) {
            // Only if the shape is a Mask
            if (ofxMtlMapping2DShape::activeShape->shapeType == MAPPING_2D_SHAPE_MASK) {
                ofxMtlMapping2DShape* shape = ofxMtlMapping2DShape::activeShape;
                if (shape) {
                    ofLog(OF_LOG_NOTICE, "Add vertex to shape %i", shape->shapeId);
                    shape->addPoint(eX, eY);
                } else {
                    ofLog(OF_LOG_NOTICE, "No shape has been selected, can not add a vertex");
                }
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::keyPressed(ofKeyEventArgs &e)
{
    // ----
    switch (e.key) {
        case 9:  // TAB
            ofToggleFullscreen();
            break;
            
        case 'l':
            loadShapesList();
            break;
            
        case 'm':
            ofxMtlMapping2DControlsSharedInstance().toggleVisible();
            break;
            
        case 's':
            saveShapesList();
            break;
            
        case 356:
            //left
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->left();
                }
            }
            break;
                
        case 357: //up
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->up();
                }
            }
            break;
        
        case 358: //right
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->right();
                }
            }
            break;
                
        case 359: //down
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape && ofxMtlMapping2DShape::activeVertexId >=0) {
                    ofxMtlMapping2DVertex::activeVertex->down();
                }
            }
            break;
            
        case 127:
            bDeleteShape = true;
            break;
            
        case 8:
            bDeleteShape = true;
            break;
            
        case 'n':
            if (_mappingModeState == MAPPING_EDIT) {
                if(ofxMtlMapping2DShape::activeShape) {
                    ofxMtlMapping2DShape::activeVertexId++;
                    ofxMtlMapping2DShape::activeVertexId %= ofxMtlMapping2DShape::activeShape->vertices.size();
                    
                    //-----
                    if (ofxMtlMapping2DShape::activeShape->vertices.size() > ofxMtlMapping2DShape::activeVertexId)
                    {
                        list<ofxMtlMapping2DVertex*>::iterator it = ofxMtlMapping2DShape::activeShape->vertices.begin();
                        advance(it, ofxMtlMapping2DShape::activeVertexId);
                        ofxMtlMapping2DVertex* vertex = *it;
                        vertex->setAsActive();
                    }
                }
            }
            break;
    }
}


#pragma mark -
#pragma mark Load and Save Shapes List
//--------------------------------------------------------------
void ofxMtlMapping2D::loadShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemLoadDialog();
    
    if (!fileDialogResult.bSuccess) {
        return;
    }
    
    loadXml(fileDialogResult.getPath());
}

//--------------------------------------------------------------
void ofxMtlMapping2D::loadXml(string xmlFile)
{
    
    // UI
    ofxMtlMapping2DControlsSharedInstance().clearShapesList();
    
    // Delete everything
    while(!ofxMtlMapping2DShapes::pmShapes.empty()) delete ofxMtlMapping2DShapes::pmShapes.back(), ofxMtlMapping2DShapes::pmShapes.pop_back();
    ofxMtlMapping2DShapes::pmShapes.clear();
    ofxMtlMapping2DShape::resetActiveShapeVars();
    
    
    //LOAD XML
    // ----
	//the string is printed at the top of the app
	//to give the user some feedback
	string feedBackMessage = "loading " + xmlFile;
	ofLog(OF_LOG_NOTICE, "Status > " + feedBackMessage);
    
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( _shapesListXML.loadFile(xmlFile) ){
		feedBackMessage = xmlFile + " loaded!";
	}else{
		feedBackMessage = "unable to load " + xmlFile + " check data/ folder";
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
                
                for(int j = 0; j < numShapeSettingTags; j++){
                    string key = _shapesListXML.getAttribute("setting", "key", "nc", j);
                    
                    if (key == "type") {
                        string shapeType = _shapesListXML.getValue("setting", "nan", j);

                        if (shapeType == "quad") {
                            newShape = new ofxMtlMapping2DQuad();
                            newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
                        } else if (shapeType == "grid") {
                            newShape = new ofxMtlMapping2DGrid();
                            newShape->shapeType = MAPPING_2D_SHAPE_GRID;
                        } else if (shapeType == "triangle") {
                            newShape = new ofxMtlMapping2DTriangle();
                            newShape->shapeType = MAPPING_2D_SHAPE_TRIANGLE;
                        } else if (shapeType == "mask") {
                            newShape = new ofxMtlMapping2DMask();
                            newShape->shapeType = MAPPING_2D_SHAPE_MASK;
                        } else {
                            newShape = new ofxMtlMapping2DQuad();
                            newShape->shapeType = MAPPING_2D_SHAPE_QUAD;
                        }
                    }
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
                
                
                if(newShape->shapeType != MAPPING_2D_SHAPE_MASK) {
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
                    newShape->inputPolygon->disable();
                    
                    _shapesListXML.popTag();
                }
                
                newShape->init(shapeId);
                newShape->disable();
                ofxMtlMapping2DShapes::pmShapes.push_front(newShape);
                
                ofxMtlMapping2DControlsSharedInstance().addShapeToList(shapeId, newShape->shapeType);
				
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
void ofxMtlMapping2D::saveShapesList()
{
    ofFileDialogResult fileDialogResult = ofSystemSaveDialog("", "Save Shapes List");
    string mappingXmlFilePath = fileDialogResult.getPath();

    
    list<ofxMtlMapping2DShape*> pmShapesCopy;
    pmShapesCopy.resize (ofxMtlMapping2DShapes::pmShapes.size());
    copy (ofxMtlMapping2DShapes::pmShapes.begin(), ofxMtlMapping2DShapes::pmShapes.end(), pmShapesCopy.begin());
    pmShapesCopy.sort(Comparator());

    ofxXmlSettings newShapesListXML;
	
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
        
        if(shape->shapeType != MAPPING_2D_SHAPE_MASK) {
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
		}
		newShapesListXML.popTag();
    }
	
	//Save to file
	newShapesListXML.saveFile(mappingXmlFilePath);
    ofLog(OF_LOG_NOTICE, "Status > settings saved to xml!");

}

#pragma mark -
#pragma mark Getters
//--------------------------------------------------------------
vector<ofPolyline*> ofxMtlMapping2D::getMaskShapes()
{
    return ofxMtlMapping2DShapes::getShapesOutputPolylineWithType(MAPPING_2D_SHAPE_MASK);
}

#pragma mark -
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

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
#pragma mark -
#pragma mark Syphon

//--------------------------------------------------------------
void ofxMtlMapping2D::setupSyphon()
{    
    //setup our directory
	_syphonServerDir.setup();
    //setup our client
    _syphonClient.setup();
    
    //register for our directory's callbacks
    ofAddListener(_syphonServerDir.events.serverAnnounced, this, &ofxMtlMapping2D::serverAnnounced);
    ofAddListener(_syphonServerDir.events.serverRetired, this, &ofxMtlMapping2D::serverRetired);
    
    _syphonDirIdx = -1;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::drawSyphon()
{
    if(_syphonServerDir.isValidIndex(_syphonDirIdx)) {
        ofSetColor(255, 255, 255, 255);
        _syphonClient.draw(0, 0);
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2D::selectSyphonServer()
{
    _syphonDirIdx++;
    if(_syphonDirIdx > _syphonServerDir.size() - 1)
        _syphonDirIdx = 0;
    
    _syphonClient.set(_syphonServerDir.getDescription(_syphonDirIdx));
    string serverName = _syphonClient.getServerName();
    string appName = _syphonClient.getApplicationName();
    
    if(serverName == ""){
        serverName = "null";
    }
    if(appName == ""){
        appName = "null";
    }
    ofLog() << "Syphon" + serverName + ":" + appName;
    
}

//--------------------------------------------------------------
void ofxMtlMapping2D::serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& _syphonServerDir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Announced") << " Server Name: " << _syphonServerDir.serverName << " | App Name: " << _syphonServerDir.appName;
    }
    _syphonDirIdx = 0;
}

//--------------------------------------------------------------
void ofxMtlMapping2D::serverRetired(ofxSyphonServerDirectoryEventArgs &arg)
{
    for( auto& _syphonServerDir : arg.servers ){
        ofLogNotice("ofxSyphonServerDirectory Server Retired") << " Server Name: " <<_syphonServerDir.serverName << " | App Name: " << _syphonServerDir.appName;
    }
    _syphonDirIdx = 0;
}


#endif


