#include "ofxGemcutterGrid.h"
#include "ofxGemcutterGlobal.h"
#include "ofxGemcutterControls.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
ofxGemcutterGrid::ofxGemcutterGrid()
{
	internalMesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

//--------------------------------------------------------------
ofxGemcutterGrid::~ofxGemcutterGrid(){

}

//--------------------------------------------------------------
void ofxGemcutterGrid::update()
{
    ofxGemcutterShape::update();
    
    if (activeShape == this) {
        updateVertices();
        updateUVMap();
	}
}

//--------------------------------------------------------------
void ofxGemcutterGrid::updateVertices(){
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
            list<ofxGemcutterVertex*>::iterator it = vertices.begin();
            advance(it, topLeft);
            ofxGemcutterVertex* topLeftVertex = *it;
            
            it = vertices.begin();
            advance(it, topRight);
            ofxGemcutterVertex* topRightVertex = *it;
            
            it = vertices.begin();
            advance(it, bottomRight);
            ofxGemcutterVertex* bottonRightVertex = *it;
            
            it = vertices.begin();
            advance(it, bottomLeft);
            ofxGemcutterVertex* bottomLeftVertex = *it;
            
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
void ofxGemcutterGrid::draw()
{
    // ---- OUTPUT MODE
    if(ofxGemcutterGlobal::getEditView() == MAPPING_OUTPUT_VIEW) {
        ofSetColor(ofColor::white);
        controlMesh.drawWireframe();
    }
    
    //drawInternalMesh();
    
    ofxGemcutterShape::draw();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::drawInternalMesh()
{
    ofSetColor(ofColor::red);
    internalMesh.drawWireframe();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::render()
{    
    internalMesh.draw();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::createDefaultShape()
{	           
    shapeSettings["type"] = "grid";
    shapeSettings["cols"] = ofToString(ofxGemcutterSettings::gridDefaultNbCols);
    shapeSettings["rows"] = ofToString(ofxGemcutterSettings::gridDefaultNbRows);

    // ---
    updateGridAndMesh(true);
    
    // ---
    //Create a new vertex
    int xOffset = .0f;
    int yOffset = .0f;
    
    int x = 0;
    int y = 0;
    
    // --- Input
    inputPolygon = new ofxGemcutterInput();
    
    ofxGemcutterVertex* newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofxGemcutterGlobal::outputWidth;
    y = 0;
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = ofxGemcutterGlobal::outputWidth;
    y = ofxGemcutterGlobal::outputHeight;
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = ofxGemcutterGlobal::outputHeight;
    newVertex = new ofxGemcutterVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    // ---
    inputPolygon->init(shapeId);
    
    // --- Generate the texture coordinates;
	updateUVMap();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::initShape()
{            
    updateGridAndMesh();
    updateUVMap();
    updateVertices();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::updateGrid()
{
    shapeSettings["cols"] = ofToString(gridNbCols);
    shapeSettings["rows"] = ofToString(gridNbRows);
    
    updateGridAndMesh(true);
    updateUVMap();
}

//--------------------------------------------------------------
void ofxGemcutterGrid::updateGridAndMesh(bool startFresh)
{
    // ---
    gridWidth = ofxGemcutterGlobal::outputWidth;
    gridHeight = ofxGemcutterGlobal::outputHeight;
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
        ofxGemcutterVertex* newVertex;
        
        if (vertices.size() != 0) {        
            while(!vertices.empty()) delete vertices.back(), vertices.pop_back();
            vertices.clear();
            
            polyline->clear();
            
            // ----
            //ofxGemcutterShape::resetActiveShapeVars();
            //ofxGemcutterPolygon::resetActivePolygonVars();
            
            //ofxGemcutterControls::mapping2DControls()->unselectShapesToggles();
        }
            
        // --- new shape
        for (int y = 0; y <= gridNbRows; y++) {
            float controlPointY = y * gridQuadHeight;
            
            for (int x = 0; x <= gridNbCols; x++) {
                float controlPointX = x * gridQuadWidth;
                
                // ---
                newVertex = new ofxGemcutterVertex();
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
        for (int y = 0; y <= gridNbRows; y++) {
            int rowControlPointIndex =  y * (gridNbCols + 1);
            for (int x = 0; x <= gridNbCols; x++) {
                int controlPointIndex = rowControlPointIndex + x;

                list<ofxGemcutterVertex*>::iterator it = vertices.begin();
                advance(it, controlPointIndex);
                ofxGemcutterVertex* controlVertex = *it;
                
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

    list<ofxGemcutterVertex*>::iterator it;
    for (it=vertices.begin(); it!=vertices.end(); it++) {
        ofxGemcutterVertex* vertex = *it;
        
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
void ofxGemcutterGrid::updateUVMap()
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