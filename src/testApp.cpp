#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    // カメラの設定
    // format7 mode 1
    // size = 320 x 240
    //
    camera.setFormat7(true, 1);
    camera.setSize(CAMERA_WIDTH,CAMERA_HEIGHT);
    camera.setExposure(50.0);
    camera.setFrameRate(60);
	camera.setup();
    
    // ネットワークの設定
	udpConnection.Create();
//	udpConnection.Connect("192.168.11.41",PORT);
	udpConnection.Connect("localhost",PORT);
	udpConnection.SetNonBlocking(true);
    
    // GUI
    params.setName("camera settings");
	params.add(binary_thresh.set("binary threshold",128,0,255));
	params.add(maxSizeThreshold.set("max size threshold",3000,0,10000));
	params.add(minSizeThreshold.set("min size threshold",20,0,200));
	params.add(targetDisplayWidth.set("target display w",1920,0,1920));
	params.add(targetDisplayHeight.set("target display h",1080,0,1080));
	params.add(trackingDistanceThreshold.set("tracking distance",20.0,0.0,100.0));
    
    gui.setup(params);
    gui.loadFromFile("settings.xml");
    
    showGUI = true;
    
    // マーカ検出用
	displayDetector.initialize(10, trackingDistanceThreshold);
	transformer.initialize(targetDisplayWidth, targetDisplayHeight);				// 座標変換用
	cornerRegions = new MarkerRegion[10];
	memset(cornerRegions, 0, sizeof(MarkerRegion) * 10);
    // 狙う場所、今回は画面の中心を狙っていることとする
	aimingPoint.x = CAMERA_WIDTH / 2.0f;
	aimingPoint.y = CAMERA_HEIGHT / 2.0f;
    
}

//--------------------------------------------------------------
void testApp::update(){
    
	if(camera.grabVideo(curFrame)) {
		curFrame.update();
        
        // ofxCvGrayscaleImage型に変換
        srcGray.setFromPixels(curFrame.getPixels(), curFrame.getWidth(), curFrame.getHeight());
        
        // 二値化
        srcGray.threshold(binary_thresh);
        
        // マーカ検出と並び替え
        numRegionsDetected = displayDetector.detectDisplayCorners(srcGray.getCvImage(), cornerRegions, maxSizeThreshold, minSizeThreshold);
        
        cout << numRegionsDetected << endl;
#if 1
        /* 四隅のマーカがちゃんと検出できている */
        if ( numRegionsDetected == 4 ){
            impactPoint = transformer.getImpactPoint(cornerRegions, aimingPoint);				// 当った位置を調べる
            
            /* 当っているか判断する */
            if ( impactPoint.x >= 0 && impactPoint.x < targetDisplayWidth && impactPoint.y >= 0 && impactPoint.y < targetDisplayHeight ){
                printf("hit: %03.02f %03.02f -> %03.02f %03.02f\n", aimingPoint.x, aimingPoint.y, impactPoint.x, impactPoint.y);
                
                /* 着弾位置をアプリに送信 */
                float x = impactPoint.x / (float)targetDisplayWidth;
                float y = impactPoint.y / (float)targetDisplayHeight;
//                bulletCounter++;
//                udp->sendMessage(&sendBuffer, sizeof(DISPLAYSHOOTER_NETWORK_DATA));
                
                cout << "x:" << x << " y:" << y << endl;
                
                string message="";
                message+=ofToString(x)+"|"+ofToString(y)+"|0[/p]";
                udpConnection.Send(message.c_str(),message.length());
                
            }else{
//                string message="";
//                message+=ofToString(0.5)+"|"+ofToString(0.5)+"|0[/p]";
//                udpConnection.Send(message.c_str(),message.length());
                printf("Out of Display: %03.02f %03.02f -> %03.02f %03.02f\n", aimingPoint.x, aimingPoint.y, impactPoint.x, impactPoint.y);
            }
        }
#endif
        
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255);
    
	if(camera.isReady()) {
		// Camera doesn't draw itself, curFrame does.
		curFrame.draw(0, 0, 640, 480);
//        srcGray.draw(curFrame.getWidth()+1, 0);
	}
    
    showResult(640,480);
    
    if(showGUI) gui.draw();
}

/* 結果描画 */
void testApp::showResult(int w, int h){
    
    CvPoint2D32f tmpPoints[4];
    
	/* マーカ描画 */
	for (int i = 0; i < 4; i++){
        tmpPoints[i].x = cornerRegions[i].centroid.x * w / CAMERA_WIDTH;
        tmpPoints[i].y = cornerRegions[i].centroid.y * h / CAMERA_HEIGHT;
        ofSetColor(255,0,0);
        ofNoFill();
        ofEllipse(tmpPoints[i].x, tmpPoints[i].y, 10, 10);
	}
    
	/* ディスプレイが検出されたら囲む */
	if ( numRegionsDetected == 4 ){
        ofLine(tmpPoints[0].x, tmpPoints[0].y, tmpPoints[1].x, tmpPoints[1].y);
        ofLine(tmpPoints[1].x, tmpPoints[1].y, tmpPoints[2].x, tmpPoints[2].y);
        ofLine(tmpPoints[2].x, tmpPoints[2].y, tmpPoints[3].x, tmpPoints[3].y);
        ofLine(tmpPoints[3].x, tmpPoints[3].y, tmpPoints[0].x, tmpPoints[0].y);
	}
    
} // showResult

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
	if(key == 'g') {
        showGUI = !showGUI;
	}
    
	if(key == 'S') {
		gui.saveToFile("settings.xml");
	}
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
