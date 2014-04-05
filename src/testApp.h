#pragma once

#include "ofMain.h"
#include "ofxLibdc.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxNetwork.h"

#include "DisplayDetector.h"
#include "Transformer.h"

static const string ADDRESS = "127.0.0.1";
static const int PORT = 11999;

static const int CAMERA_WIDTH = 320;
static const int CAMERA_HEIGHT = 240;

static const int NUM_CAMERA = 2;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void showResult(int x, int y, int w, int h);
    
	// ofxLibdc is a namespace, and Camera is the object
	ofxLibdc::Camera camera[NUM_CAMERA];
	
	// ofImage is where we store the current frame we grabbed from the Camera
	ofImage curFrame[NUM_CAMERA];
	
    // GUI
	ofxPanel gui;
    ofParameterGroup params;
	ofParameter<int> binary_thresh;
	ofParameter<float> maxSizeThreshold;    // マーカとみなす最大の面積
	ofParameter<float> minSizeThreshold;    // マーカとみなす最小の面積
	ofParameter<int> targetDisplayWidth, targetDisplayHeight; // ターゲットディスプレイの設定
	ofParameter<float> trackingDistanceThreshold;    // マーカ追跡用
    
    bool showGUI;
    
	/* カメラ画像中の照準位置 */
	CvPoint2D32f aimingPoint;
	CvPoint2D32f impactPoint;
    
    // 画像処理用変数
    ofxCvColorImage rgb;
    ofxCvGrayscaleImage srcGray;
    
	/* 画像処理クラス */
//	ImageBinarizer imageBinarizer;
	Transformer transformer;
	DisplayDetector displayDetector;
    
	/* 検出されたコーナー */
	MarkerRegion *cornerRegions;
	int numRegionsDetected;
    
	/* データの送信 */
    ofxUDPManager udpConnection;

};
