#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "ofxCameraSaveLoad.h"

#define NUM_OBJECTS 15
static int kSlideN = NUM_OBJECTS;

class ofApp : public ofBaseApp {

public:

	bool bRotate = false;

	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofEasyCam _camera;
	ofxImGui::Gui _imgui;
	bool drawGui();
	bool mouseOverGui = false;

	ofParameterGroup params_Control;

	ofParameterGroup params_Settings;

	// selection
	ofParameter<int> _to_x = 0;

	ofParameter<bool> bCamEnable = true;
	ofParameter<bool> bDEBUG = true;
	ofParameter<bool> guiVisible = true;

	// coverflow
	ofParameter<float> _edgeSlope;// space between both ends
	ofParameter<float> _positionRoughness;// width of the middle area
	ofParameter<float> _centerArea;// Expand the width of the middle area
	ofParameter<float> _rotationArea;// width of rotating area
	ofParameter<float> _rotation;// amount of rotation
	ofParameter<float> _zoomArea;// zoom width in z direction
	ofParameter<float> _zoom;// zoom amount in z direction

	// move
	ofParameter<float> _kP;// amount proportional to the difference = determine the base speed
	ofParameter<float> _vMax;// Maximum speed limit
	ofParameter<float> _aMax;// Maximum acceleration limit (up)
	ofParameter<float> _approach;// Additional acceleration control when the distance is short-Because deceleration is intense with normal proportional control
	ofParameter<float> _approachWide;// Width for additional acceleration when the distance is short. I feel good with 0.5

	// current coordinates
	double _x = 0.0;
	// Now the speed
	double _v = 0.0;

	//-

	vector <ofColor> colors;

	// my own helper
	template<typename ParameterType>
	bool AddParameter(ofParameter<ParameterType>& parameter);

	//settings
	void loadGroup(ofParameterGroup &g, string path);
	void saveGroup(ofParameterGroup &g, string path);
	string pathSettings = "Settings.xml";

	float rotators[NUM_OBJECTS]{ 0 };
	float currRot = 0;
};
