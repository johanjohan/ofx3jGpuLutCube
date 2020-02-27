#include "ofx3jGpuLutCube.h"


bool ofx3jGpuLutCube::load(const string & _cubeLutFilename, const bool &_bNegative) {

	bIsLoaded = false;

	if (isDebug()) ofLogNotice(__FUNCTION__) << _cubeLutFilename;

	ifstream file(_cubeLutFilename.c_str());
	if (!file.is_open()) {
		ofLogWarning(__FUNCTION__) << "issue opening _cubeLutFilename: " << _cubeLutFilename;
		std::exit(1);
		return false;
	}

	// if DOMAIN_MIN is omitted from the file, the lower bounds shall be 0 0 0.
	RGB lower;
	lower.r = lower.g = lower.b = 0;
	// if DOMAIN_MAX is omitted from the file, the upper bound shall be 111
	RGB upper;
	upper.r = upper.g = upper.b = 1;

	LUT.clear();
	lut3dSize = 32; // default to most common size, shall be an integer in the range [2, 256].
	while (!file.eof()) {

		string row;
		getline(file, row);

		if (row.empty()) continue;

		//row = ofTrim(row); // slow

		RGB line;
		if (sscanf(row.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) {
			line.r = ofNormalize(line.r, lower.r, upper.r);
			line.g = ofNormalize(line.g, lower.g, upper.g);
			line.b = ofNormalize(line.b, lower.b, upper.b);
			LUT.push_back(line);
		}
		else if (ofIsStringInString(row, "TITLE")) {
			vector<string> subs = ofSplitString(ofTrim(row), "\""); // split at "
			if (subs.size() >= 2) {
				title = subs[1];
				ofStringReplace(title, "\"", ""); // erase trailing "
				if (isDebug()) ofLogNotice(__FUNCTION__) << "found: " << subs[0] << " " << title;
			}
		}
		else if (ofIsStringInString(row, "LUT_1D_SIZE")) {
			ofLogError(__FUNCTION__) << "LUT_1D_SIZE is unsupported!";
			std::exit(1);
		}
		else if (ofIsStringInString(row, "LUT_3D_SIZE")) {
			vector<string> subs = ofSplitString(ofTrim(row), " ");
			if (subs.size() >= 2) {
				lut3dSize = ofToInt(subs[1]);
				if (isDebug()) ofLogNotice(__FUNCTION__) << "found: " << subs[0] << " " << subs[1];

				// leave immediately if not pow2
				if (ofNextPow2(lut3dSize) != lut3dSize) {
					ofLogError() << "LUT needs to be pow2, i.e. 16, 32, 64...";
					return false;
				}
			}
		}
		else if (ofIsStringInString(row, "DOMAIN_MIN")) {
			vector<string> subs = ofSplitString(ofTrim(row), " ");
			if (subs.size() >= 4) {
				lower.r = ofToInt(subs[1]);
				lower.g = ofToInt(subs[2]);
				lower.b = ofToInt(subs[3]);
				if (isDebug()) {
					ofLogNotice(__FUNCTION__) << "found: " << subs[0] << " " << lower.r << " " << lower.g << " " << lower.b;
				}
			}
		}
		else if (ofIsStringInString(row, "DOMAIN_MAX")) {
			vector<string> subs = ofSplitString(ofTrim(row), " ");
			if (subs.size() >= 4) {
				upper.r = ofToInt(subs[1]);
				upper.g = ofToInt(subs[2]);
				upper.b = ofToInt(subs[3]);
				if (isDebug()) {
					ofLogNotice(__FUNCTION__) << "found: " << subs[0] << " " << upper.r << " " << upper.g << " " << upper.b;
				}
			}
		}
		else if (ofTrim(row)[0] == '#') { // can access [0] because empty lines are skipped above
			if (isDebug()) ofLogNotice(__FUNCTION__) << "found comment: " << row;
		}
		else {
			ofLogWarning(__FUNCTION__) << "unknow key in row: " << row;
		}
	} // while
	if (isDebug()) cout << endl;

	if (LUT.size() != (pow(lut3dSize, 3.0))) {
		ofLogError() << "LUT size is incorrect.";
		return false;
	}
	else if (ofNextPow2(lut3dSize) != lut3dSize) {
		ofLogError() << "LUT needs to be pow2, i.e. 16, 32, 64...";
		return false;
	}

#if 0
	ofLogNotice(__FUNCTION__) << "lut3dSize : " << lut3dSize;
	ofLogNotice(__FUNCTION__) << "LUT.size(): " << LUT.size() << " --> " << int(ceil(pow(LUT.size(), 1.0 / 3.0)));
#endif

	if(_bNegative) std::reverse(LUT.begin(), LUT.end());

	// Create a 3D texture
	// Reference from http://content.gpwiki.org/index.php/OpenGL:Tutorials:3D_Textures
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &lutTexture3D);
	glBindTexture(GL_TEXTURE_3D, lutTexture3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexImage3D(
		GL_TEXTURE_3D, 0, GL_RGB,
		lut3dSize, lut3dSize, lut3dSize,   // lut3dSize
		0, GL_RGB,
		GL_FLOAT, &LUT[0]
	);
	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	if (!lutShader.isLoaded()) {
		string path = ofFilePath::getCurrentExeDir() + "../../../../addons/ofx3jGpuLutCube/shader/lut_filter";
		if (isDebug()) ofLogNotice(__FUNCTION__) << "loading shader: " << path;
		bool b = lutShader.load(path);
		if (!b || !lutShader.isLoaded()) {
			ofLogError() << "issue loading shader!!!";
			return false;
		}
	}

	// finally...
	bIsLoaded = true;
	return bIsLoaded;
}

ofImage ofx3jGpuLutCube::load(
	const string	&_imageFilename, 
	const string	&_cubeLutFilename, 
	const bool		&_bNegative,
	const float		&_mix,
	const int		&_fboNumSamples
) {
	if (isDebug()) ofLogNotice(__FUNCTION__) << "_imageFilename: " << _imageFilename;
	ofImage image;
	if (image.load(_imageFilename)) {
		ofx3jGpuLutCube lut;
		if (lut.load(_cubeLutFilename, _bNegative)) {
			return lut.apply(image.getTexture(), _mix, _fboNumSamples);
		}
	}
	ofLogWarning(__FUNCTION__) << "issue loading...";
	return ofImage(); // empty, there was an issue before...
}

// _image needs to be GL_TEXTURE_2D, use ofDisableArbTex() before allocating image
// may assign to itself i.e.: image = lut.apply(image)
ofImage  & ofx3jGpuLutCube::apply(const ofTexture &_tex, const float & _mix, const int & _fboNumSamples) {

	if (!bIsLoaded) {
		ofLogError(__FUNCTION__) << "lut is not loaded: call load() before!";
		std::exit(1);
	}

	if (!fbo.isAllocated() || _tex.getWidth() != fbo.getWidth() || _tex.getHeight() != fbo.getHeight()) {

		// check image GL_TEXTURE_2D
		if (_tex.getTextureData().textureTarget != GL_TEXTURE_2D) {
			ofLogError(__FUNCTION__) << "_tex: bad textureTarget: call ofDisableArbTex() before!";
			std::exit(1);
		}

		const bool bArbTex = ofGetUsingArbTex();
		ofDisableArbTex();
		{
			if (isDebug()) ofLogNotice(__FUNCTION__) << "allocating fbo: " << _tex.getWidth() << "x" << _tex.getHeight();
			fbo.allocate(_tex.getWidth(), _tex.getHeight(), GL_RGBA, _fboNumSamples);

			// check fbo GL_TEXTURE_2D
			if (fbo.getTexture().getTextureData().textureTarget != GL_TEXTURE_2D) {
				ofLogError(__FUNCTION__) << "fbo: bad textureTarget: call ofDisableArbTex() before!";
				std::exit(1);
			}
		}
		if (bArbTex) { ofEnableArbTex(); }

		plane.set(_tex.getWidth(), _tex.getHeight(), 2, 2);
		plane.setPosition(_tex.getWidth() / 2, _tex.getHeight() / 2, 0);
	}

	fbo.begin();
	{
		ofClear(0, 0);

		lutShader.begin();
		{
			lutShader.setUniformTexture("tex", _tex, 0);
			lutShader.setUniformTexture("lutTexture", GL_TEXTURE_3D, lutTexture3D, 1);
			lutShader.setUniform1f("lutSize", lut3dSize);
			lutShader.setUniform2f("mouse", _mix, _mix); // mix%

			ofPushStyle();
			{
				ofSetColor(ofColor::white);
				plane.draw();
			}
			ofPopStyle();
		}
		lutShader.end();

#if 0
		if (isDebug())
		{
			// debug
			ofSetColor(ofColor::green);
			ofDrawCircle(fbo.getWidth() / 2, fbo.getHeight() / 2, fbo.getHeight() / 12);
		}
#endif

			}
	fbo.end();
	fbo.readToPixels(image.getPixels()); // had an issue before directly passing back into an image passed by reference...
	return image;
		}
