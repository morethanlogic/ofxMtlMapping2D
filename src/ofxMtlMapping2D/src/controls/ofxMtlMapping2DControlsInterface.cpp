#include "ofxMtlMapping2DControlsInterface.h"

//--------------------------------------------------------------
ofxMtlMapping2DControlsInterface::ofxMtlMapping2DControlsInterface(int width, ofColor color, const string& file)
{
    _canvas = new ofxUIScrollableCanvas(0, 0, width, ofGetHeight());
    //must have modified ofxUI
    //_canvas->setStickyDistance(30.0f);
    _canvas->setScrollableDirections(false, true);
    _canvas->setColorBack(color);
    
    _file = file;
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControlsInterface::getToggleValue(const string& name)
{
    return ((ofxUIToggle *)_canvas->getWidget(name))->getValue();
}

//--------------------------------------------------------------
float ofxMtlMapping2DControlsInterface::getSliderValue(const string& name)
{
    return ((ofxUISlider *)_canvas->getWidget(name))->getScaledValue();
}

//--------------------------------------------------------------
ofPoint ofxMtlMapping2DControlsInterface::get2DPadValue(const string& name)
{
    return ((ofxUI2DPad *)_canvas->getWidget(name))->getScaledValue();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControlsInterface::save()
{
    _canvas->saveSettings(_file);
}

//--------------------------------------------------------------
void ofxMtlMapping2DControlsInterface::load()
{
    _canvas->loadSettings(_file);
}

//--------------------------------------------------------------
void ofxMtlMapping2DControlsInterface::enable()
{
    _canvas->enable();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControlsInterface::disable()
{
    _canvas->disable();
}

//--------------------------------------------------------------
void ofxMtlMapping2DControlsInterface::toggle()
{
    _canvas->toggleVisible();
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControlsInterface::isEnabled()
{
    return _canvas->isEnabled();
}

//--------------------------------------------------------------
bool ofxMtlMapping2DControlsInterface::isHit(int x, int y) {
    return _canvas->isHit(x, y);
}

