#include "ofxMtlMapping2DGrid.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
ofxMtlMapping2DGrid::ofxMtlMapping2DGrid()
{
    gridWidth = 600;
    gridHeight = 500;
    
    // --- Mesh
	internalMesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

//--------------------------------------------------------------
ofxMtlMapping2DGrid::~ofxMtlMapping2DGrid(){

}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::update()
{
    ofxMtlMapping2DShape::update();
    
    if (activePolygon == this) {        
        updateVertices();
	}
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::updateVertices(){
	//upadate the control quads
	for (int c_i = 0; c_i < controlQuads.size(); c_i++) {
		ControlQuad * c_quad = controlQuads[c_i];

        // update the internal quads
        for (int i_i = 0; i_i < c_quad->internalQuads.size(); i_i++) {
            InternalQuad * i_quad = c_quad->internalQuads[i_i];
            
            // interpolate the edges
            vector<ofPoint> left, right;
            left.resize(resolution.y+1);
            right.resize(resolution.y+1);
            
            // ---
            list<ofxMtlMapping2DVertex*>::iterator it = vertices.begin();
            advance(it, c_quad->TL_Index);
            ofxMtlMapping2DVertex* TL_vertex = *it;
            
            it = vertices.begin();
            advance(it, c_quad->BL_Index);
            ofxMtlMapping2DVertex* BL_vertex = *it;
            
            it = vertices.begin();
            advance(it, c_quad->TR_Index);
            ofxMtlMapping2DVertex* TR_vertex = *it;
            
            it = vertices.begin();
            advance(it, c_quad->BR_Index);
            ofxMtlMapping2DVertex* BR_vertex = *it;            
            
            // ---
            for (int y = 0; y <= resolution.y; y++) {
                left[y] =	TL_vertex->center + ((BL_vertex->center - TL_vertex->center) / resolution.y) * (float)y;
                right[y] =	TR_vertex->center + ((BR_vertex->center - TR_vertex->center) / resolution.y) * (float)y;
            }
            
            // interpolate all internal points
            vector<vector<SelectablePoint> >grid;
            grid.resize(resolution.x+1);
            for (int x = 0; x <= resolution.x; x++) {
                grid[x].resize(resolution.x+1);
                for (int y = 0; y <= resolution.y; y++) {
                    ofPoint l = left[y];
                    ofPoint r = right[y];
                    
                    grid[x][y].set(l + ((r - l) / resolution.x) * (float)x);
                }
            }
            
            // update the internal quads
            i_quad->TL = grid[i_quad->x][i_quad->y];
            i_quad->BL = grid[i_quad->x][i_quad->y+1];
            i_quad->TR = grid[i_quad->x+1][i_quad->y];
            i_quad->BR = grid[i_quad->x+1][i_quad->y+1];
            
            // update the mesh
            internalMesh.setVertex(i_quad->index + 0, i_quad->TL);
            internalMesh.setVertex(i_quad->index + 1, i_quad->BL);
            internalMesh.setVertex(i_quad->index + 2, i_quad->TR);
            
            internalMesh.setVertex(i_quad->index + 3, i_quad->TR);
            internalMesh.setVertex(i_quad->index + 4, i_quad->BL);
            internalMesh.setVertex(i_quad->index + 5, i_quad->BR);
        }
    }
	
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::draw()
{
    ofxMtlMapping2DShape::draw();
    drawUI();
}

//--------------------------------------------------------------
void ofxMtlMapping2DGrid::drawUI()
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
    int dummy=0;
	onGridChange(dummy);
    
    // ---
    ofVec2f c_Size(gridWidth / gridSize.x, gridHeight / gridSize.y);
    ofVec2f i_Size = c_Size / resolution;
    int c_index = 0;
    int i_index = 0;
    
    // --- MTL Controls
    ofxMtlMapping2DVertex* newVertex;
    
    for (int y = 0; y <= (int)gridSize.y; y++) {
		ofVec2f c_RowStart =  y * ofVec2f(0, c_Size.y);
        
        for (int x = 0; x <= (int)gridSize.x; x++) {
            ofVec2f c_Start = c_RowStart + x * ofVec2f(c_Size.x, 0);
            
            // ---
            newVertex = new ofxMtlMapping2DVertex();
            newVertex->init(c_Start.x - newVertex->width/2, c_Start.y - newVertex->height/2);
            vertices.push_back(newVertex);
        }
    }
    
    // ---
    //Create a new vertex
    int xOffset = ofGetWidth()/2.0;
    int yOffset = ofGetHeight()/2.0;
    
    int x = 0;
    int y = 0;
    
    // ---
    shapeSettings["type"] = "grid";

    inputPolygon = new ofxMtlMapping2DInput();
    
    // --- Input
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 100;
    y = 0;
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 100;
    y = 100;
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    x = 0;
    y = 100;
    newVertex = new ofxMtlMapping2DVertex();
    newVertex->init(xOffset + x - newVertex->width/2, yOffset + y - newVertex->height/2);
    newVertex->isDefiningTectureCoord = true;
    inputPolygon->vertices.push_back(newVertex);
    
    // ----
    inputPolygon->init(shapeId);
}

//--------------------------------------------------------------
// This is called when we do change the specifics of the grid. 
void ofxMtlMapping2DGrid::onGridChange(int & value)
{
    resolution.x = 6;
	resolution.y = 6;
    gridSize.x = 2;
    gridSize.y = 2;
    
    // ---
    internalMesh.clearVertices();
    
    ofVec2f c_Size(gridWidth / gridSize.x, gridHeight / gridSize.y);
    ofVec2f i_Size = c_Size / resolution;
    int c_index = 0;
    int i_index = 0;
    
    int mtlVertexIndex;
	
    // ---
    for (int y = 0; y < (int)gridSize.y; y++) {
		ofVec2f c_RowStart =  y * ofVec2f(0, c_Size.y);
        
        for (int x = 0; x < (int)gridSize.x; x++) {
            ofVec2f c_Start = c_RowStart + x * ofVec2f(c_Size.x, 0);
            
            mtlVertexIndex = y * (gridSize.x + 1) + x;
            //cout << "mtlVertexIndex ::" << mtlVertexIndex << endl;
            
            ControlQuad* c_quad = new ControlQuad();
            c_quad->index = c_index;
            
            c_quad->TL_Index = mtlVertexIndex;
            c_quad->BL_Index = mtlVertexIndex + (gridSize.x + 1);
            c_quad->TR_Index = mtlVertexIndex + 1;
            c_quad->BR_Index = (mtlVertexIndex + 1) + (gridSize.x + 1);
            
            cout << "--------------" << endl;
            cout << "c_quad->TL_Index ::" << c_quad->TL_Index << endl;
            cout << "c_quad->BL_Index ::" << c_quad->BL_Index << endl;
            cout << "c_quad->TR_Index ::" << c_quad->TR_Index << endl;
            cout << "c_quad->BR_Index ::" << c_quad->BR_Index << endl;
            cout << "--------------" << endl;
            
            controlQuads.push_back(c_quad);
            
            c_index+=4;
            
			
            for (int i_y = 0; i_y < resolution.y; i_y++) {
                ofVec2f i_RowStart = c_Start + i_y * ofVec2f(0, i_Size.y);
                
                for (int i_x = 0; i_x < resolution.x; i_x++) {
                    ofVec2f i_Start = i_RowStart + i_x * ofVec2f(i_Size.x, 0);
                    
                    InternalQuad* i_quad = new InternalQuad();
                    i_quad->index = i_index;
					i_quad->x = i_x;
					i_quad->y = i_y;
                    i_quad->TL.set(i_Start.x, i_Start.y);
                    i_quad->BL.set(i_Start.x, i_Start.y + i_Size.y);
                    i_quad->TR.set(i_Start.x + i_Size.x, i_Start.y);
                    i_quad->BR.set(i_Start.x + i_Size.x, i_Start.y + i_Size.y);
					
					c_quad->internalQuads.push_back(i_quad);
                    
                    internalMesh.addVertex(i_quad->TL);
                    internalMesh.addVertex(i_quad->BL);
                    internalMesh.addVertex(i_quad->TR);
                    
					internalMesh.addVertex(i_quad->TR);
					internalMesh.addVertex(i_quad->BL);
					internalMesh.addVertex(i_quad->BR);
					
                    internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
                    internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
                    internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
                    internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
					internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
                    internalMesh.addColor(ofFloatColor(1.0,1.0,1.0,1.0));
                    
                    i_index+=6;
                }
            }
        }
    }
    
    // Generate the coordinates;
    float dummy = 0;
	onCoordinatesChange(dummy);
}

//--------------------------------------------------------------
// This is called when we change the texture coordinates (inpot polygon)
void ofxMtlMapping2DGrid::onCoordinatesChange(float & value)
{    
	coordinatesStart.x =  0;
	coordinatesStart.y =  0;
	coordinatesEnd.x =  ofGetWidth();
	coordinatesEnd.y =  ofGetHeight();
	
//	if(ofGetUsingNormalizedTexCoords()){
//		// openframeworks is mental with normalized coordinates inside a shader...
//		// a shame.. but here is the dirty hack
//		float capX = texture->getWidth() / ofNextPow2(texture->getWidth());
//		float capY = texture->getHeight() / ofNextPow2(texture->getHeight());
//		
//		coordinatesStart.x =  ofMap(coordinatesStart.x, 0, 1, 0, capX);
//		coordinatesStart.y =  ofMap(coordinatesStart.y, 0, 1, 0, capY);
//		coordinatesEnd.x =  ofMap(coordinatesEnd.x, 0, 1, 0, capX);
//		coordinatesEnd.y =  ofMap(coordinatesEnd.y, 0, 1, 0, capY);
//	}
    
    
	internalMesh.clearTexCoords();
	
    ofVec2f uvSize = coordinatesEnd - coordinatesStart;
    ofVec2f uvCellSize = uvSize / gridSize;
    ofVec2f i_Size = uvCellSize / resolution;
    for (int y = 0; y < gridSize.y; y++) {
        ofVec2f uvRowStart = coordinatesStart + y * ofVec2f(0, uvCellSize.y);
        
        for (int x = 0; x < gridSize.x; x++) {
            ofVec2f uvCellStart = uvRowStart + x * ofVec2f(uvCellSize.x, 0);
            
            
            for (int i_y = 0; i_y < resolution.y; i_y++) {
                ofVec2f i_RowStart = uvCellStart + i_y * ofVec2f(0, i_Size.y);
                
                for (int i_x = 0; i_x < resolution.x; i_x++) {
                    ofVec2f i_Start = i_RowStart + i_x * ofVec2f(i_Size.x, 0);
                    
					ofVec2f TL = ofVec2f(i_Start.x, i_Start.y);
					ofVec2f BL = ofVec2f(i_Start.x, i_Start.y + i_Size.y);
					ofVec2f TR = ofVec2f(i_Start.x + i_Size.x, i_Start.y);
					ofVec2f BR = ofVec2f(i_Start.x + i_Size.x, i_Start.y + i_Size.y);
					
					internalMesh.addTexCoord(TL);
					internalMesh.addTexCoord(BL);
					internalMesh.addTexCoord(TR);
					
					internalMesh.addTexCoord(TR);
					internalMesh.addTexCoord(BL);
					internalMesh.addTexCoord(BR);
                }
            }
        }
    }
}