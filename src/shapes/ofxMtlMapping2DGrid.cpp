#include "ofxMtlMapping2DGrid.h"
#include "ofxMtlMapping2DControls.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DGrid::ofxMtlMapping2DGrid()
{
	internalMesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

//--------------------------------------------------------------
ofxMtlMapping2DGrid::~ofxMtlMapping2DGrid(){

}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::update()
{
    ofxMtlMapping2DShape::update();
    
    if (activeShape == this) {
        updateVertices();
        updateUVMap();
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::updateVertices(){
    // ---
    for (int y = 0; y < gridNbRows; y++) {
		int rowControlPointIndex =  y * (gridNbCols + 1);
        
        for (int x = 0; x < gridNbCols; x++) {
            int controlPointIndex = rowControlPointIndex + x;
            
            // Quad
            int topLeft = controlPointIndex;
            int topRight = controlPointIndex + 1;
            int bottomRight = (controlPointIndex + 1) + (gridNbCols + 1);
            int bottomLeft = controlPointIndex + (gridNbCols + 1);
            
            // ---
            list<ofxMtlMapping2DVertex*>::iterator it = vertices.begin();
            advance(it, topLeft);
            ofxMtlMapping2DVertex* topLeftVertex = *it;
            
            it = vertices.begin();
            advance(it, topRight);
            ofxMtlMapping2DVertex* topRightVertex = *it;
            
            it = vertices.begin();
            advance(it, bottomRight);
            ofxMtlMapping2DVertex* bottonRightVertex = *it;
            
            it = vertices.begin();
            advance(it, bottomLeft);
            ofxMtlMapping2DVertex* bottomLeftVertex = *it;
            
            // --- Interpolate the quad's vertical edges
            vector<ofVec2f> left, right;
            left.resize(gridVerticalResolution+1);
            right.resize(gridVerticalResolution+1);
            
            for (int i_y = 0; i_y <= gridVerticalResolution; i_y++) {
                left[i_y] =	topLeftVertex->center + ((bottomLeftVertex->center - topLeftVertex->center) / gridVerticalResolution) * (float)i_y;
                right[i_y] = topRightVertex->center + ((bottonRightVertex->center - topRightVertex->center) / gridVerticalResolution) * (float)i_y;
            }
            
            // --- Interpolate all internal points
            vector<vector<ofVec2f> >grid;
            grid.resize(gridHorizontalResolution+1);
            for (int i_x = 0; i_x <= gridHorizontalResolution; i_x++) {
                grid[i_x].resize(gridVerticalResolution+1);
                for (int i_y = 0; i_y <= gridVerticalResolution; i_y++) {
                    ofVec2f l = left[i_y];
                    ofVec2f r = right[i_y];
                    
                    grid[i_x][i_y].set(l + ((r - l) / gridHorizontalResolution) * (float)i_x);
                }
            }
            
            // --- Update Control Mesh
            controlMesh.setVertex(topLeft, topLeftVertex->center);
            controlMesh.setVertex(topRight, topRightVertex->center);
            controlMesh.setVertex(bottomRight, bottonRightVertex->center);
            controlMesh.setVertex(bottomLeft, bottomLeftVertex->center);
            
            // --- Update internal mesh
            int quadStartIndex = (y * ((gridNbCols * gridHorizontalResolution) + 1) * gridVerticalResolution) + (x * gridHorizontalResolution);

            for (int i_y = 0; i_y < gridVerticalResolution; i_y++) {
                for (int i_x = 0; i_x < gridHorizontalResolution; i_x++) {
                    
                    int vertexIndex = quadStartIndex + ( i_y * ((gridNbCols * gridHorizontalResolution) + 1) + i_x );

                    int i_topLeft = vertexIndex;
                    int i_topRight = vertexIndex + 1;
                    int i_bottomRight = (vertexIndex + 1) + ((gridNbCols * gridHorizontalResolution) + 1);
                    int i_bottomLeft = vertexIndex + ((gridNbCols * gridHorizontalResolution) + 1);
                    
                    ofVec2f i_topLeftVertex = grid[i_x][i_y];
                    ofVec2f i_topRightVertex = grid[i_x+1][i_y];
                    ofVec2f i_bottomRightVertex = grid[i_x+1][i_y+1];
                    ofVec2f i_bottomLeftVertex = grid[i_x][i_y+1];
                    
                    internalMesh.setVertex(i_topLeft, i_topLeftVertex);
                    internalMesh.setVertex(i_topRight, i_topRightVertex);
                    internalMesh.setVertex(i_bottomRight, i_bottomRightVertex);
                    internalMesh.setVertex(i_bottomLeft, i_bottomLeftVertex);
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::draw()
{
    // ---- OUTPUT MODE
    if(ofxMtlMapping2DControls::mapping2DControls()->mappingMode() == MAPPING_MODE_OUTPUT) {
        ofSetColor(ofColor::white);
        controlMesh.drawWireframe();
    }
    
    //drawInternalMesh();
    
    ofxMtlMapping2DShape::draw();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::drawInternalMesh()
{
    ofSetColor(ofColor::red);
    internalMesh.drawWireframe();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::render()
{    
    internalMesh.draw();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::createDefaultShape()
{	           
    shapeSettings["type"] = "grid";
    shapeSettings["cols"] = ofToString(ofxMtlMapping2DSettings::gridDefaultNbCols);
    shapeSettings["rows"] = ofToString(ofxMtlMapping2DSettings::gridDefaultNbRows);

    // ---
    updateGridAndMesh(true);
    
    // ---
    //Create a new vertex
    int xOffset = .0f; //ofGetWidth()/2.0;
    int yOffset = .0f; //ofGetHeight()/2.0;
    
    int x = 0;
    int y = 0;
    
    // --- Input
    inputPolygon = new ofxMtlMapping2DInput();
    
    ofxMtlMapping2DVertex* newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofGetWidth();
    y = 0;
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofGetWidth();
    y = ofGetHeight();
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = ofGetHeight();
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    // ---
    inputPolygon->init(shapeId);
    
    // --- Generate the texture coordinates;
	updateUVMap();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::initShape()
{            
    updateGridAndMesh();
    updateUVMap();
    updateVertices();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::updateGrid()
{
    shapeSettings["cols"] = ofToString(gridNbCols);
    shapeSettings["rows"] = ofToString(gridNbRows);
    
    updateGridAndMesh(true);
    updateUVMap();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::updateGridAndMesh(bool startFresh)
{
    // ---
    gridWidth = ofGetWidth();
    gridHeight = ofGetHeight();
    gridNbCols = ofToInt(shapeSettings["cols"]);
    gridNbRows = ofToInt(shapeSettings["rows"]);
    gridHorizontalResolution = 12;
    gridVerticalResolution = 12;
    
    gridQuadWidth = gridWidth / gridNbCols;
    gridQuadHeight = gridHeight / gridNbRows;
    gridCellWidth = gridQuadWidth / gridHorizontalResolution;
    gridCellHeight = gridQuadHeight / gridVerticalResolution;
    
    // --- Controls
    if (startFresh) {
        ofxMtlMapping2DVertex* newVertex;
        
        if (vertices.size() != 0) {        
            while(!vertices.empty()) delete vertices.back(), vertices.pop_back();
            vertices.clear();
            
            polyline->clear();
            
            // ----
            //ofxMtlMapping2DShape::resetActiveShapeVars();
            //ofxMtlMapping2DPolygon::resetActivePolygonVars();
            
            //ofxMtlMapping2DControls::mapping2DControls()->unselectShapesToggles();
        }
            
        // --- new shape
        for (int y = 0; y <= gridNbRows; y++) {
            float controlPointY = y * gridQuadHeight;
            
            for (int x = 0; x <= gridNbCols; x++) {
                float controlPointX = x * gridQuadWidth;
                
                // ---
                newVertex = new ofxMtlMapping2DVertex();
                if (y == 0) {
                    newVertex->edgeIndex = 0;
                } else if(y == gridNbRows) {
                    newVertex->edgeIndex = 2;
                } else if (x == 0) {
                    newVertex->edgeIndex = 3;
                } else if (x == gridNbCols) {
                    newVertex->edgeIndex = 1;
                } else {
                    newVertex->edgeIndex = -1;
                    newVertex->bIsOnAnEdge = false;
                }
                newVertex->init(controlPointX - newVertex->width/2, controlPointY - newVertex->height/2);
                vertices.push_back(newVertex);
            }
        }
        
        enableVertices();
    } else {
        cout << "1." << endl;
        for (int y = 0; y <= gridNbRows; y++) {
            int rowControlPointIndex =  y * (gridNbCols + 1);
            cout << "2." << endl;
            cout << "rowControlPointIndex :: " << rowControlPointIndex << endl;
            for (int x = 0; x <= gridNbCols; x++) {
                int controlPointIndex = rowControlPointIndex + x;
                cout << "3." << endl;
                cout << "controlPointIndex :: " << controlPointIndex << endl;
                
                list<ofxMtlMapping2DVertex*>::iterator it = vertices.begin();
                advance(it, controlPointIndex);
                ofxMtlMapping2DVertex* controlVertex = *it;
                
                // ---
                if (y == 0) {
                    controlVertex->edgeIndex = 0;
                } else if(y == gridNbRows) {
                    controlVertex->edgeIndex = 2;
                } else if (x == 0) {
                    controlVertex->edgeIndex = 3;
                } else if (x == gridNbCols) {
                    controlVertex->edgeIndex = 1;
                } else {
                    controlVertex->edgeIndex = -1;
                    controlVertex->bIsOnAnEdge = false;
                }
            }
        }
    }
    
    // ---
    controlMesh.clear();

    list<ofxMtlMapping2DVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxMtlMapping2DVertex* vertex = *it;
        
        controlMesh.addVertex(ofVec3f(vertex->center.x, vertex->center.y, .0f));

    }
    
    for (int y = 0; y < gridNbRows; y++) {
        for (int x = 0; x < gridNbCols; x++) {
            
            int vertexIndex = y * (gridNbCols + 1) + x;
            
            int topLeft = vertexIndex;
            int topRight = vertexIndex + 1;
            int bottomRight = (vertexIndex + 1) + (gridNbCols + 1);
            int bottomLeft = vertexIndex + (gridNbCols + 1);
            
            controlMesh.addIndex(topLeft);
            controlMesh.addIndex(topRight);
            controlMesh.addIndex(bottomLeft);
            
            controlMesh.addIndex(topRight);
            controlMesh.addIndex(bottomRight);
            controlMesh.addIndex(bottomLeft);
        }
    }
    
    // --- Internal mesh vertices
    internalMesh.clearVertices();
    
    for (int y = 0; y <= (gridNbRows * gridVerticalResolution); y++) {
		float vertexY =  y * gridCellHeight;
        
        for (int x = 0; x <= (gridNbCols * gridHorizontalResolution); x++) {
            float vertexX = x * gridCellWidth;
            
            internalMesh.addVertex(ofVec3f(vertexX, vertexY, .0f));
        }
    }
    
    // --- Internal mesh indices
    internalMesh.clearIndices();
    
    int nbOfVertices = (gridNbCols * gridHorizontalResolution) * (gridNbRows * gridVerticalResolution);
    
    for (int y = 0; y < (gridNbRows * gridVerticalResolution); y++) {
        for (int x = 0; x < (gridNbCols * gridHorizontalResolution); x++) {
            
            int vertexIndex = y * ((gridNbCols * gridHorizontalResolution) + 1) + x;
            
            int topLeft = vertexIndex;
            int topRight = vertexIndex + 1;
            int bottomRight = (vertexIndex + 1) + ((gridNbCols * gridHorizontalResolution) + 1);
            int bottomLeft = vertexIndex + ((gridNbCols * gridHorizontalResolution) + 1);
            
            internalMesh.addIndex(topLeft);
            internalMesh.addIndex(topRight);
            internalMesh.addIndex(bottomLeft);
            
            internalMesh.addIndex(topRight);
            internalMesh.addIndex(bottomRight);
            internalMesh.addIndex(bottomLeft);
        }
    }
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::updateUVMap()
{    
	internalMesh.clearTexCoords();
    
    ofVec2f coordinatesStart;
    ofVec2f coordinatesEnd;
    
    coordinatesStart.x =  inputPolygon->polyline->getVertices()[0].x;
    coordinatesStart.y =  inputPolygon->polyline->getVertices()[0].y;
    coordinatesEnd.x =  inputPolygon->polyline->getVertices()[2].x;
    coordinatesEnd.y =  inputPolygon->polyline->getVertices()[2].y;
        
    // ---
    ofVec2f uvSize = coordinatesEnd - coordinatesStart;
    float uvCellWidth = uvSize.x / (gridNbCols * gridHorizontalResolution);
    float uvCellHeight = uvSize.y / (gridNbRows * gridVerticalResolution);
    
    for (int y = 0; y <= (gridNbRows * gridVerticalResolution); y++) {
		float uvCellY =  coordinatesStart.y + y * uvCellHeight;
        
        for (int x = 0; x <= (gridNbCols * gridHorizontalResolution); x++) {
            float uvCellX = coordinatesStart.x + x * uvCellWidth;
            
            internalMesh.addTexCoord(ofVec2f(uvCellX, uvCellY));
        }
    }
}