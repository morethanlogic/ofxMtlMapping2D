#pragma once

// OF
#include "ofMain.h"

//Mapping
#include "ofxMtlMapping2DSettings.h"
#include "ofxMtlMapping2DVertex.h"
#include "ofxMtlMapping2DQuad.h"
#include "ofxMtlMapping2DGrid.h"
#include "ofxMtlMapping2DTriangle.h"
#include "ofxMtlMapping2DMask.h"
#include "ofxMtlMapping2DShape.h"

#include "mtlUtils.h"

// Addons
#include "ofxXmlSettings.h"
#include "ofxExtraWindow.h"

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
#include "ofxSyphon.h"
#endif

#if defined(USE_VIDEO_PLAYER_OPTION)
    #if defined(TARGET_OSX)
        #include "ofxAVFVideoPlayer.h"
    #elif definfed(TARGET_WIN32)

    #endif
#endif


//--------------------------------------------------------------
//--------------------------------------------------------------
enum MappingModeState
{
    MAPPING_LOCKED,
    MAPPING_EDIT
};

//--------------------------------------------------------------
//--------------------------------------------------------------
class ofxMtlMapping2D {
    
public:
    ofxMtlMapping2D();
    virtual ~ofxMtlMapping2D();
    
//  void init(int width, int height, string mappingXmlFilePath = "mapping/xml/shapes.xml", string uiXmlFilePath = "mapping/controls/mapping.xml", int numSample = 0);
    void init(int width, int height, int numSample = 0);
    void update();
    
    void bind();
    void unbind();
    void drawFbo();
    void draw();

    void setModeState(MappingModeState mappingModeState);
    MappingModeState getModeState();

    void mousePressed(ofMouseEventArgs &e);
    void mouseDragged(ofMouseEventArgs &e);
    void keyPressed(ofKeyEventArgs &e);
    void windowResized(ofResizeEventArgs &e);
    
    void updateZoomAndOutput(bool updateFBO = false);

    vector<ofPolyline*> getMaskShapes();
    void chessBoard(int nbOfCol = 10);

    void loadXml(string xmlFile);
        
    bool bSaveShapes;
    bool bLoadShapes;
    bool bCreateQuad;
    bool bCreateGrid;
    bool bCreateTriangle;
    bool bCreateMask;

    void selectShapeId(int shapeId);
    void setLockForShapeId(int shapeId, bool bLocked);

    void openOuputWindow(ofRectangle rect);
    void closeOutputWindow();

#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    void selectSyphonServer(int syphonDirIdx);

    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
#endif
    
    // Video playback related
#if defined(USE_VIDEO_PLAYER_OPTION)
    void loadVideo(string filePath);
    void playVideo();
    void pauseVideo();
    void stopVideo();
    void setVideoPostion(float position);
    
    float getVideoDurationInSecond();
    float videoPositionInSeconds;
#endif


private:
    //string _mappingXmlFilePath;
    MappingModeState _mappingModeState;

    ofFbo _fbo;
    int _numSample;
    
    ofxExtraWindow _outputWindow;
    
    float _sensitivityXY;
    ofVec2f _mouse;
	ofVec2f _lastMouse;
	ofVec2f _mouseVel;
    
    float _moveX;
	float _moveY;

    ofxXmlSettings _shapesListXML;
    list<ofxMtlMapping2DShape*>::iterator iteratorForShapeWithId(int shapeId);
    
    void render(bool bIsOutput);

    void createQuad();
    void createGrid();
    void createTriangle();
    void createMask();
    void deleteShape();
    
    void drawAllShapes();

    void loadShapesList();
    void saveShapesList();

    void addListeners();
    void removeListeners();
    
    bool _bSelectedShapeChanged;
    int _selectedShapeId;
    bool _bDeleteShape;

    // Syphon related
#if defined(USE_OFX_SYPHON) && defined(TARGET_OSX)
    ofxSyphonServerDirectory _syphonServerDir;
    ofxSyphonClient _syphonClient;
    int _syphonDirIdx;
    
    void setupSyphon();
    void drawSyphon();
#endif
    
    // Video playback related
#if defined(USE_VIDEO_PLAYER_OPTION)
    int _videoXOffset;
    int _videoYOffset;
    int _videoWidth;
    int _videoHeight;
    float _videoRatio;
    
    #if defined(TARGET_OSX)
        ofxAVFVideoPlayer _videoPlayer;
    #elif definfed(TARGET_WIN32)
    
    #endif
    
    void setupVideoPlayer();
    void updateVideoPlayer();
    void drawVideoPlayer();
    void exitVideoPlayer();
    void resizeVideo(ofRectangle outputRect);
    
    bool _bIsVideoStopped;
#endif
};