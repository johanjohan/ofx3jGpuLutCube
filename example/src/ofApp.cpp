#include "ofApp.h"

#define IMAGE_FILENAME	ofToDataPath("Reflective-color-chart-reference.png")
#define LUT_FILENAME	ofToDataPath("16/HorrorBlue.cube") 
#define LUT_FILENAME	ofToDataPath("16/Lord Kelvin.cube") 


//--------------------------------------------------------------
void ofApp::setup() {

	ofDisableArbTex(); // !!!
	ofSetVerticalSync(false);

	bool b;

	b = chart.load(IMAGE_FILENAME);
	assert(b);

	webCam.initGrabber(1280, 720);

	b = myLut.load(LUT_FILENAME, false); // add param: negative
	assert(b);

	// example with static func
	offDisk = ofx3jGpuLutCube::load(IMAGE_FILENAME, LUT_FILENAME, false, 1.f, 0); 
}

//--------------------------------------------------------------
void ofApp::update() {

	webCam.update();

#ifdef CHECK_FRAME_NEW
	if (webCam.isFrameNew())
#endif
	{
#ifdef COPY_INTO_IMAGE
		image = myLut.apply(webCam.getTexture(), ofGetMouseX() / float(ofGetWidth())); // copy into image takes time....
#else
		myLut.apply(webCam.getTexture(), ofGetMouseX() / float(ofGetWidth()));
#endif
	}

	ofSetWindowTitle(ofToString(ofGetFrameRate(), 1)+ " Hz");
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofBackground(0);


#ifdef COPY_INTO_IMAGE
	if(image.isAllocated()) image.draw(0, 0);
#else
	//myLut.getFbo().draw(0, 0);
	// or
	myLut.draw(0, 0);
#endif

	// charts
	chart.draw(20, 50, offDisk.getWidth() / 8, offDisk.getHeight() / 8);
	offDisk.draw(20 + offDisk.getWidth() / 8, 50, offDisk.getWidth() / 8, offDisk.getHeight() / 8);

	ofDrawBitmapStringHighlight("move mouse x in order to mix the lut in", 20, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}
