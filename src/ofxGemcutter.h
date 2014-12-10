#pragma once

// OF
#include "ofMain.h"

//Mapping
#include "ofxGemcutterDefines.h"
#include "ofxGemcutterSettings.h"
#include "ofxGemcutterVertex.h"
#include "ofxGemcutterQuad.h"
#include "ofxGemcutterGrid.h"
#include "ofxGemcutterTriangle.h"
#include "ofxGemcutterMask.h"
#include "ofxGemcutterShape.h"
#include "ofxGemcutterGlobal.h"

#include "mtlUtils.h"

// Addons
#include "ofxXmlSettings.h"
#include "ofxExtraWindow.h"

#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
#include "ofxSyphon.h"
#endif

#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
    #if defined(TARGET_OSX)
        #include "ofxAVFVideoPlayer.h"
    #elif defined(TARGET_WIN32)
		#include "ofDirectShowPlayer.h"
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
class ofxGemcutter {
    
public:
    ofxGemcutter();
    virtual ~ofxGemcutter();
    
//  void init(int width, int height, string mappingXmlFilePath = "mapping/xml/shapes.xml", string uiXmlFilePath = "mapping/controls/mapping.xml", int numSample = 0);
    void setup(int width, int height, int numSample = 0);
    void exit(ofEventArgs &e);
    
    void update();
    
    void bind();
    void unbind();
    void drawFbo();
    void draw();

    void setModeState(MappingModeState mappingModeState);
    MappingModeState getModeState();

    void mouseMoved(ofMouseEventArgs &e);
    void mousePressed(ofMouseEventArgs &e);
    void mouseDragged(ofMouseEventArgs &e);
    void keyPressed(ofKeyEventArgs &e);
    void windowResized(ofResizeEventArgs &e);
    
    void updateZoomAndOutput(MappingEditView view, bool updateOutputResolution = false);
    void zoomScaleToFit(MappingEditView view, bool updateFBO = false);
    
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
    void outputWindowClosedEvent(int & i);

#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
    void selectSyphonServer(int syphonDirIdx);

    void serverAnnounced(ofxSyphonServerDirectoryEventArgs &arg);
    void serverRetired(ofxSyphonServerDirectoryEventArgs &arg);
#endif
    
    // Video playback related
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
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
    
    float _timeUILastActivation;
    
    void updateUiTimer();

    ofxXmlSettings _shapesListXML;
    list<ofxGemcutterShape*>::iterator iteratorForShapeWithId(int shapeId);
    
    void render();

    void createQuad();
    void createGrid();
    void createTriangle();
    void createMask();
    void deleteShape();
    
    void drawAllShapes(bool bForceOutputMode = false);

    void loadShapesList();
    void saveShapesList();

    void addListeners();
    void removeListeners();
    
    bool _bSelectedShapeChanged;
    int _selectedShapeId;
    bool _bDeleteShape;

    // Syphon related
#if defined(OFX_GEMCUTTER_USE_OFX_SYPHON) && defined(TARGET_OSX)
    ofxSyphonServerDirectory _syphonServerDir;
    ofxSyphonClient _syphonClient;
    int _syphonDirIdx;
    
    void setupSyphon();
    void drawSyphon();
#endif
    
    // Video playback related
#if defined(OFX_GEMCUTTER_USE_VIDEO_PLAYER_OPTION)
    ofRectangle _videoRect;
    
    #if defined(TARGET_OSX)
        ofxAVFVideoPlayer _videoPlayer;
    #elif defined(TARGET_WIN32)
		ofDirectShowPlayer _videoPlayer;    
		ofTexture tex;
    #endif
    
    void setupVideoPlayer();
    void updateVideoPlayer();
    void drawVideoPlayer();
    void exitVideoPlayer();
    void resizeVideo(ofRectangle targetRect);
    
    bool _bIsVideoStopped;
#endif
};