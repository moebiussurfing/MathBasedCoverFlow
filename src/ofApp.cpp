#include "ofApp.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

//helper
template <class T>
void imgui_draw_tree_node(const char *name, bool isOpen, T f) {
	if (isOpen) {
		ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	}
	if (ImGui::TreeNode(name)) {
		f();
		ImGui::TreePop();
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	saveGroup(params_Settings, pathSettings);
	ofxSaveCamera(_camera, "ofEasyCamSettings");
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);

	//custom objects
	colors.resize(kSlideN);
	for (auto &c : colors)
	{
		c = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));
	}

	_camera.setDistance(20);
	_camera.setNearClip(0.1);
	_camera.setFarClip(300.0);

	_imgui.setup();

	_to_x.set("selector", 0, 0, kSlideN - 1);
	bCamEnable.set("MOUSE CAM", true);
	bDEBUG.set("DEBUG", true);
	guiVisible.set("GUI", true);

	params_Control.setName("Control");
	params_Control.add(_to_x);
	params_Control.add(bCamEnable);
	params_Control.add(bDEBUG);
	params_Control.add(guiVisible);

	_edgeSlope.set("edgeSlope", 0.3, 0.0f, 1.0f);
	_positionRoughness.set("positionRoughness", 0.7, 0.0f, 4.0f);
	_centerArea.set("centerArea", 1.1, 0.0f, 10.0f);
	_rotationArea.set("rotationArea", 0.7, 0.0f, 4.0f);
	_rotation.set("rotation", 70.0, 0.0f, 90.0f);
	_zoomArea.set("zoomArea", 0.7, 0.0f, 4.0f);
	_zoom.set("zoom", 0.5, 0.0f, 2.0f);

	_kP.set("kP", 5.0f, 0.0f, 10.0f);
	_vMax.set("vMax", 15.0f, 0.0f, 30.0f);
	_aMax.set("aMax", 40.0f, 0.0f, 200.0f);
	_approach.set("approach", 1.0f, 0.0f, 5.0f);
	_approachWide.set("approachWide", 0.5f, 0.0f, 5.0f);

	params_Settings.setName("config");
	params_Settings.add(_edgeSlope);
	params_Settings.add(_positionRoughness);
	params_Settings.add(_centerArea);
	params_Settings.add(_rotationArea);
	params_Settings.add(_rotation);
	params_Settings.add(_zoomArea);
	params_Settings.add(_zoom);
	params_Settings.add(_kP);
	params_Settings.add(_vMax);
	params_Settings.add(_aMax);
	params_Settings.add(_approach);
	loadGroup(params_Settings, pathSettings);

	ofxLoadCamera(_camera, "ofEasyCamSettings");
}

//-----------------------_approachWide;//---------------------------------------
void ofApp::update() {
	double d = std::min(ofGetLastFrameTime(), 1.0 / 30.0);
	auto impluse = [](double x, double a) {
		double over_a = 1.0 / a;
		return over_a * x * glm::exp(1.0 - over_a * x);
	};
	auto near_more = [](double d, double s) {
		return glm::exp(-(d * d) / (s * s));
	};

	int N = 10;
	double delta = d / N;
	for (int i = 0; i < N; ++i) {
		// Poor
		double d = _to_x - _x;

		// Proportional control (P)
		auto pv = d * _kP;

		// + speed when the distance is short
		auto nv = impluse(glm::abs(d), 0.5) * _approach;
		if (pv < 0.0) {
			nv = -nv;
		}

		auto v = pv + nv;

		// Speed ​​limit
		v = std::min(v, (double)_vMax);
		v = std::max(v, (double)-_vMax);

		// want to limit acceleration only
		if (glm::abs(_v) < glm::abs(v)) {
			auto amax = _aMax * delta;
			double a = v - _v;
			a = std::min(a, amax);
			a = std::max(a, -amax);
			v = _v + a;
		}

		_v = v;
		_x += _v * delta;
	}

}
//--------------------------------------------------------------
bool ofApp::drawGui() {
	auto mainSettings = ofxImGui::Settings();

	_imgui.begin();

	if (ofxImGui::BeginWindow("CoverFlow", mainSettings, false))
	{
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ofVec4f(0.0f, 0.2f, 0.2f, 0.8f));
		//ImGui::SetNextWindowPos(ofVec2f(10, 30), ImGuiCond_Once);
		//ImGui::SetNextWindowSize(ofVec2f(500, ofGetHeight() * 0.8), ImGuiCond_Once);

		//ImGui::Begin("Config");

		ofxImGui::AddGroup(params_Control, mainSettings);

		imgui_draw_tree_node("Cover Flow", true, [=]() {
			AddParameter(_edgeSlope);
			AddParameter(_positionRoughness);
			AddParameter(_centerArea);
			AddParameter(_rotationArea);
			AddParameter(_rotation);
			AddParameter(_zoomArea);
			AddParameter(_zoom);
			});

		imgui_draw_tree_node("Movement", true, [=]() {
			AddParameter(_kP);
			AddParameter(_vMax);
			AddParameter(_aMax);
			AddParameter(_approach);
			AddParameter(_approachWide);
			});


		//ImGui::End();
		//ImGui::PopStyleColor();
	}
	ofxImGui::EndWindow(mainSettings);

	_imgui.end();

	return mainSettings.mouseOverGui;
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (bDEBUG)
		ofBackgroundGradient(ofColor(40, 40, 40), ofColor(0, 0, 0), OF_GRADIENT_CIRCULAR);
	else
	{
		ofBackground(40);
	}

	_camera.begin();

	if (bDEBUG)
	{
		// + draw a grid on the floor
		ofPushStyle();
		ofSetLineWidth(1.0f);
		ofSetColor(ofColor(60));
		ofPushMatrix();
		ofRotate(90, 0, 0, -1);
		ofDrawGridPlane(10, 10, false);
		ofPopMatrix();
		ofPopStyle();
	}

	if (bDEBUG)
	{
		ofPushStyle();
		ofSetLineWidth(4.0f);
		ofDrawAxis(10);
		ofPopStyle();
	}

	//-

	ofEnableDepthTest();

	//-

	// Location field
	// Integrate[t + w*exp(-(x^2)/s^2), {x, 0, z}]
	auto position_field = [](double z, double s, double t, double w) {
		return 0.5 * glm::sqrt(glm::pi<double>()) * s * w * std::erf(z / s) + t * z;
	};

	// rotating field
	auto rotation_field = [](double z, double s) {
		auto v = glm::exp(-(z * z) / (s * s)) - 1.0;
		return 0.0 < z ? v : -v;
	};

	// Zoom field
	auto zoom_field = [](double z, double s) {
		return glm::exp(-(z * z) / (s * s));
	};

	// Those who line up in the back
	// Integrate[erf(u*sqrt(pi) x), {x, 0, z}]
	//auto zf = [](double z, double u) {
	//	double sqPI = glm::sqrt(glm::pi<double>());
	//	double a = -glm::pi<double>() * u * z * std::erf(sqPI * u * z) - glm::exp(-glm::pi<double>() * u * u * z * z) + 1.0;
	//	double b = glm::pi<double>() * u;
	//	return -a / b;
	//};

	double step = 1.0;

	// double ofs = -(glm::sin(ofGetElapsedTimef() * 0.5) * 0.5 + 0.5) * 5.0;
	for (int i = 0; i < kSlideN; ++i) {
		double x = i * step - _x;
		double position = position_field(x, _positionRoughness, _edgeSlope, _centerArea);
		double rot = rotation_field(x, _rotationArea);
		double zoomValue = zoom_field(x, _zoomArea);
		// double ofz = - 1.0 * zf(x, 2.0);

		ofSetColor(i % 2 == 0 ? 128 : 200);
		ofPushMatrix();
		ofTranslate(position, 0, zoomValue * _zoom);
		ofRotateY(rot * _rotation);

		//-

		//container or clickable object
		if (bDEBUG) ofDrawRectangle(-0.5, -0.5, 1.0f, 1.0f);

		//-

		//objects
		ofSetColor(colors[i]);

		//highlight rotate selected
		bRotate = true;
		if (_to_x == i && bRotate)
		{
			//ofPushMatrix();

			//float scale = 1.25f;
			//ofScale(scale, scale);

			//float dur = 25 * 20.f;
			//float _deg = ((ofGetFrameNum() % (int)dur) / dur) * 360.0f;
			//ofRotateYDeg(-_deg);

			currRot++;
			ofRotateY(rot * _rotation - currRot);
		}

		//alternate prim type
		if (i % 2 == 0)
		{
			ofFill();
			ofDrawBox(.5);
			ofSetColor(255);
			ofNoFill();
			ofDrawBox(.5);
			//ofDrawCircle(-0.5, -0.5, .5f);
		}
		else
		{
			ofFill();
			ofDrawCone(.25, .5);
			ofSetColor(255);
			ofNoFill();
			ofDrawCone(.25, .5);
			//ofDrawRectangle(-0.5, -0.5, 1.0f, 1.0f);
		}

		//if (_to_x == i)
		//{
		//	ofPopMatrix();
		//}

		ofPopMatrix();
	}

	//// test drawing
	//if (bDEBUG)
	//{
	//	ofSetColor(ofColor::red);
	//	ofDrawSphere(_x, 1.0f, 0.5f);
	//	ofSetColor(ofColor::green);
	//	ofDrawSphere(_to_x, 1.0f, 0.4f);
	//}

	_camera.end();

	//-

	// Gui
	mouseOverGui = false;
	if (guiVisible)
	{
		mouseOverGui = drawGui();
	}

	//cam
	if (mouseOverGui)
	{
		_camera.disableMouseInput();
	}
	else
	{
		if (bCamEnable)
			_camera.enableMouseInput();
	}
	//if (bDEBUG)
	//	cout << "mouseOverGui: " << (mouseOverGui ? "IN" : "OUT") << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	const int _to_x_PRE = _to_x;

	if (key == OF_KEY_RIGHT) {
		_to_x += 1;
	}
	if (key == OF_KEY_LEFT) {
		_to_x -= 1;
	}

	//clamp
	_to_x = std::min(_to_x.get(), kSlideN - 1);
	_to_x = std::max(_to_x.get(), 0);

	//rot
	rotators[_to_x_PRE] = currRot;
	currRot = rotators[_to_x];


	switch (key) {
	case 'd':
		bDEBUG = !bDEBUG;
		break;
	case 'g':
		guiVisible = !guiVisible;
		break;
	case 's':
		ofxSaveCamera(_camera, "ofEasyCamSettings");
		break;
	case 'r':
		_camera.reset();
		_camera.setLensOffset(ofVec2f());
		_camera.setForceAspectRatio(false);
		break;
	case 'l':
		ofxLoadCamera(_camera, "ofEasyCamSettings");
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


// my own helper
//--------------------------------------------------------------
template<typename ParameterType>
bool ofApp::AddParameter(ofParameter<ParameterType>& parameter)
{
	auto tmpRef = parameter.get();
	const auto& info = typeid(ParameterType);

	//convert string to char pointer
	string _str = parameter.getName();
	const char * _name(_str.c_str());

	if (info == typeid(float))
	{
		if (ImGui::SliderFloat(_name, (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(int))
	{
		if (ImGui::SliderInt(_name, (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(bool))
	{
		if (ImGui::Checkbox(_name, (bool *)&tmpRef))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	ofLogWarning(__FUNCTION__) << "Could not create GUI element for type " << info.name();
	return false;
}


//--------------------------------------------------------------
void ofApp::loadGroup(ofParameterGroup &g, string path)
{
	ofLogNotice("ofApp") << "loadGroup: " << g.getName() << " to " << path;
	ofLogVerbose("ofApp") << "parameters: " << g.toString();
	ofXml settings;
	settings.load(path);
	ofDeserialize(settings, g);
}

//--------------------------------------------------------------
void ofApp::saveGroup(ofParameterGroup &g, string path)
{
	ofLogNotice("ofApp") << "saveGroup: " << g.getName() << " to " << path;
	ofLogVerbose("ofApp") << "parameters: " << g.toString();
	ofXml settings;
	ofSerialize(settings, g);
	settings.save(path);
}