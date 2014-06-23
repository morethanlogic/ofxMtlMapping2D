#include "ofxMtlMapping2DSettings.h"

// --- available / activated options
const bool  ofxMtlMapping2DSettings::kIsManuallyCreatingShapeEnabled              = true;
const bool  ofxMtlMapping2DSettings::kIsManuallyAddingDeletingVertexEnabled       = true;

// ---
ofTrueTypeFont ofxMtlMapping2DSettings::infoFont;

// ---
int ofxMtlMapping2DSettings::gridDefaultNbCols = 2;
int ofxMtlMapping2DSettings::gridDefaultNbRows = 2;

float ofxMtlMapping2DSettings::zoomFactor = 1.0f;


float ofxMtlMapping2DSettings::xLoNew;
float ofxMtlMapping2DSettings::yLoNew;
float ofxMtlMapping2DSettings::xHiNew;
float ofxMtlMapping2DSettings::yHiNew;

