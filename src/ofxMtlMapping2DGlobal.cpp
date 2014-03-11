#include "ofxMtlMapping2DGlobal.h"

//--------------------------------------------------------------
MappingEditView ofxMtlMapping2DGlobal::_mappingEditView = MAPPING_OUTPUT_VIEW;

//--------------------------------------------------------------
void ofxMtlMapping2DGlobal::setEditView(MappingEditView mappingEditView)
{
    _mappingEditView = mappingEditView;
}

//--------------------------------------------------------------
MappingEditView ofxMtlMapping2DGlobal::getEditView()
{
    return _mappingEditView;
}