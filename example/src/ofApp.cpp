#include "ofApp.h"

#define IMAGE_FILENAME ofToDataPath("Reflective-color-chart-reference.png")

#define LUT_FILENAME ofToDataPath("lut21_FujiF125Adobe.cube") // bad
#define LUT_FILENAME ofToDataPath("lut33 Rec709 Fujifilm 3513DI D65.cube") 
#define LUT_FILENAME ofToDataPath("lut64_FJNeo.cube") 
#define LUT_FILENAME ofToDataPath("2-test.cube") 
#define LUT_FILENAME ofToDataPath("2-domained.cube") 
#define LUT_FILENAME ofToDataPath("lut32_Gotham.cube")
#define LUT_FILENAME ofToDataPath("16-identity16.cube") 
#define LUT_FILENAME ofToDataPath("64-FJ 8563 RL.cube") 
#define LUT_FILENAME ofToDataPath("32/TealOrangePlusContrast.cube") 
#define LUT_FILENAME ofToDataPath("32/Lomo-fi.cube") 
#define LUT_FILENAME ofToDataPath("32/HorrorBlue.cube") 


/*
	arturo said:

	ofDisableArbTex has a confusing name, it should be called ofDisableRectTex or something like that.
	By default OF uses rectangular textures instead of 2D textures (the default in opengl).
	That’s like that for legacy reasons:
	rectangular textures used to be the only way to support non power of 2 sizes in textures,
	in old hardware you couldn’t create textures unless their size was something like 256 or 512…
	but you could do so with rectangular textures. as a side effect rectangular textures have
	pixel coordinates instead of normalized ones.

	in nowadays graphics cards using 2d textures doesn’t limit the size so it’s totally ok
	to always disable rectangular textures by default and if you are going to use textures to map 3d objects it’s recomended to do so

	we can’t disable rectangular textures by default in OF cause that would break old projects in weird ways that would be hard to detect.

*/

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
