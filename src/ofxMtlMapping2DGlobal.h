#pragma once

#include "ofMain.h"

#define kSyphonOutputServerName "MTLMapperOutput"
//--------------------------------------------------------------
//--------------------------------------------------------------
enum MappingEditView
{
    MAPPING_CHANGE_TO_INPUT_VIEW,
    MAPPING_INPUT_VIEW,
    MAPPING_CHANGE_TO_OUTPUT_VIEW,
    MAPPING_OUTPUT_VIEW
};

//--------------------------------------------------------------
class ofxMtlMapping2DGlobal
{
public:
    static void setEditView(MappingEditView mappingEditView);
    static MappingEditView getEditView();

private:

    static MappingEditView _mappingEditView;
};