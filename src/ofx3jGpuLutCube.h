/*
	this is based on
	https://github.com/youandhubris/GPU-LUT-OpenFrameworks
	BY HUBRIS
	PORTO · DECEMBER 2014

	cube lut specs:
	https://wwwimages2.adobe.com/content/dam/acom/en/products/speedgrade/cc/pdfs/cube-lut-specification-1.0.pdf

*/

#pragma once

#include "ofMain.h"

#undef NDEBUG
#include <assert.h> 

#define OFX3JGPULUTCUBE_DEBUG // change to your liking

class ofx3jGpuLutCube {

	static bool			isDebug() { 
#ifdef OFX3JGPULUTCUBE_DEBUG
		return true; 
#else
		return false;
#endif
	} 

	struct RGB			{ float r, g, b; };
	vector<RGB>			LUT;

	ofShader			lutShader;
	int					lut3dSize;	// needs to be pow2: 16 32 64 ...
	GLuint				lutTexture3D;

	ofPlanePrimitive	plane;
	ofFbo				fbo;
	ofImage				image;		

	bool				bIsLoaded;
	string				title;

public:

	ofx3jGpuLutCube() : bIsLoaded(false), title("") {}
	~ofx3jGpuLutCube() {}

	void			setup() {}

	bool			load(const string &_cubeLutFilename, const bool &_bNegative=false);

	// this one load an image and applies the lut by factor _mix [0...1]
	static ofImage	load(
		const string	&_imageFilename, 
		const string	&_cubeLutFilename, 
		const bool		&_bNegative = false,
		const float		&_mix = 1.f,
		const int		&_fboNumSamples = 0
	);

	ofImage			&apply(const ofTexture &_tex, const float &_mix = 1.f, const int &_fboNumSamples = 0);

	ofFbo			&getFbo() { return fbo; }

	void			draw(int x, int y) { fbo.draw(x,y); }

	string			getTitle() { return title; }
};