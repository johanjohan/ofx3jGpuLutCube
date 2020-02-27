
#pragma once

#include "ofMain.h"
#include "ofx3jGpuLutCube.h"

#undef NDEBUG
#include <assert.h> 

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);

	// Video
	ofVideoGrabber  webCam;
	ofImage			image;
	ofx3jGpuLutCube myLut;

	ofImage			offDisk;
	ofImage			chart;

#define COPY_INTO_IMAGE
#define CHECK_FRAME_NEW

};
