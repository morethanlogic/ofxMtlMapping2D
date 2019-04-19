#pragma once


#include "ofMain.h"
#include "ofVec2f.h"

//---------------------------------------------------------------------------
static void GLtoOFcoord() {
	ofScale(1, -1, 1);
	ofTranslate(0, -ofGetHeight(), 0);
}

//---------------------------------------------------------------------------
static double distanceToSegment(ofVec2f p1, ofVec2f p2, ofVec2f p3) {
	
	double xDelta = p2.x - p1.x;
	double yDelta = p2.y - p1.y;
	
	if ((xDelta == 0) && (yDelta == 0)) {
	    cout << "WARNING - SfUtils::distanceToSegment - p1 and p2 cannot be the same point \n";
	}
	
	double u = ((p3.x - p1.x) * xDelta + (p3.y - p1.y) * yDelta) / (xDelta * xDelta + yDelta * yDelta);
	
	ofVec2f closestPoint;
	if (u < 0) {
	    closestPoint = p1;
	} else if (u > 1) {
	    closestPoint = p2;
	} else {
	    closestPoint.set(p1.x + u * xDelta, p1.y + u * yDelta);
	}
	
	return closestPoint.distance(p3);
}

