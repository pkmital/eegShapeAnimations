#include "testApp.h"

string getNextFilename(string basename, string extension)
{
	string filename;
	int fileno = 1;
	ifstream in;
	do {
		in.close();
		stringstream str;
		// format the filename here
		if(fileno == 1)
            str << basename << "." << extension;
		else
            str << basename << "(" << (fileno-1) << ")." << extension;
		filename = str.str();
		++fileno; 
		// attempt to open for read
		in.open( ofToDataPath(filename).c_str() );
	} while( in.is_open() );
	in.close();	
    return filename;
}


void testApp::exit(){
    outfile.close();
}

//--------------------------------------------------------------
void testApp::setup(){
    screenWidth         = 1920;
    screenHeight        = 1080;
    currentTrial        = 0;
    currentLut          = 0;
    bNoiseState         = true;
    bFirstTrial         = true;
    bReadyForStimuli    = false;
    bSmallStimuli       = false;
    
    ofSetCircleResolution(100);
    ofEnableSmoothing();
    ofSetWindowPosition(1440, 0);
    ofSetFullscreen(true);
    
    totalStimuli        = 3;
    
    // Generate a vector with all possible numbers and shuffle it.
    for (float x = 0.2; x <= 0.8; x+=0.05)
    {
        for (float y = 0.2; y <= 0.8; y+=0.05)
        {
            fixationPoints.push_back(ofPoint(x * screenWidth,y * screenHeight));
        }
    }
    std::random_shuffle(fixationPoints.begin(), fixationPoints.end());
    MS_PER_STIMULI = MS_PER_FIXATION*fixationPoints.size();
    printf("Fixations per stimuli: %d\nTime per stimuli: %f (s)\nTime per Trial: %f (s)\nTotal Time: %f (s)",
           fixationPoints.size(), 
           MS_PER_STIMULI / 1000.0,
           MS_PER_STIMULI / 1000.0 * 3.0,
           MS_PER_STIMULI / 1000.0 * 3.0 * (float)NUM_TRIALS);
    
    //////////////////////////////////////////////////////
	// now create the file so that we can start adding frames to it:
    string filename = getNextFilename("eeg-record-trial-" + ofToString(currentTrial), "txt");
	outfile.open( ofToDataPath(filename).c_str() );
    
    oscReceiver.setup(RECEIVE_PORT);
    noiseShader.load(ofToDataPath("noise"));
    ofSetVerticalSync(true);
    ofSetWindowShape(screenWidth, screenHeight);
    ofResetElapsedTimeCounter();
}

//--------------------------------------------------------------
void testApp::update(){
    /*
    sigma = 2.0 * kernelSize * sinf(ofGetElapsedTimef() / 12.0) / 5.0;
    theta = 180.0 * cosf(ofGetElapsedTimef() / 6.0);
    freq = sinf(ofGetElapsedTimef() / 2.0);
     */

    if (bReadyForStimuli)
    {
        bReadyForStimuli = false;
        bNoiseState = false;
        ofResetElapsedTimeCounter();
        fixationTimer.setStartTime();
    }
    else if (bNoiseState) {
        
    }
    else {
        if(ofGetElapsedTimeMillis() >= MS_PER_STIMULI) {
            ofResetElapsedTimeCounter();
            bNoiseState = true;
            currentLut++;
            currentFixation = 0;
            std::random_shuffle(fixationPoints.begin(), fixationPoints.end());  
            currentXFixation = fixationPoints[currentFixation].x;
            currentYFixation = fixationPoints[currentFixation].y;      
            if (currentLut == totalStimuli) {
                currentLut = 0;
                currentTrial++;
                if (currentTrial == NUM_TRIALS) {
                    cout << "[OK] Finished!" << endl;
                    outfile.close();
                    OF_EXIT_APP(0);
                }
                outfile.close();
                string filename = getNextFilename("eeg-record-trial-" + ofToString(currentTrial), "txt");
                outfile.open( ofToDataPath(filename).c_str() );
            }
            
        }
        else if(fixationTimer.getElapsedMillis() >= MS_PER_FIXATION)
        {
            fixationTimer.setStartTime();
            currentFixation++;
            if (currentFixation < fixationPoints.size()) {
                currentXFixation = fixationPoints[currentFixation].x;
                currentYFixation = fixationPoints[currentFixation].y;
            }
        }
    }
    
    string delimiter = ",";
#ifdef USE_OSC
    while (oscReceiver.hasWaitingMessages()) {
        ofxOscMessage m;
        if(oscReceiver.getNextMessage(&m))
        {
            if (m.getAddress() == "/eeg") {
#endif
                outfile << ofGetTimestampString() 
                << delimiter << bNoiseState 
                << delimiter << currentTrial 
                << delimiter << currentLut
                << delimiter << currentFixation
                << delimiter << fixationTimer.getElapsedMillis()
                << delimiter << currentXFixation
                << delimiter << currentYFixation;
#ifdef USE_OSC
                for (int ch = 0; ch < 22; ch++) {
                    outfile << delimiter << m.getArgAsFloat(ch);
                }
#endif
                outfile << delimiter;
                outfile << endl;
                
#ifdef USE_OSC
            }
        }
    }
#endif
    
    
}

void testApp::drawLargeCircle(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 3.0);
}

void testApp::drawLargeOpenCircle(){    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 2.5);
    ofSetColor(0);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 5.0);
}

void testApp::drawSmallSquare(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 3.0, min(screenWidth,screenHeight) / 3.0);
}

void testApp::drawSmallOpenSquare(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, screenWidth * 0.4, screenHeight * 0.4);
    ofSetColor(0);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, screenWidth * 0.2, screenHeight * 0.2);
}

void testApp::drawSmallCircle(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 5.0);
}

void testApp::drawSmallOpenCircle(){    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 4.0);
    ofSetColor(0);
    ofCircle(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 8.0);
}

void testApp::drawLargeSquare(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 2.0, min(screenWidth,screenHeight) / 2.0);
}

void testApp::drawLargeOpenSquare(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 1.5, min(screenWidth,screenHeight) / 1.5);
    ofSetColor(0);
    ofRect(screenWidth / 2.0, screenHeight / 2.0, min(screenWidth,screenHeight) / 3.0, min(screenWidth,screenHeight) / 3.0);
}


void testApp::drawSmallDiamond(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofPushMatrix();
    ofTranslate(screenWidth / 2.0, screenHeight / 2.0);
    ofRotate(45, 0, 0, 1);
    ofRect(0, 0, min(screenWidth,screenHeight) / 3.0, min(screenWidth,screenHeight) / 3.0);
    ofPopMatrix();
}

void testApp::drawSmallOpenDiamond(){    
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofPushMatrix();
    ofTranslate(screenWidth / 2.0, screenHeight / 2.0);
    ofRotate(45, 0, 0, 1);
    ofRect(0, 0, min(screenWidth,screenHeight) / 2.5, min(screenWidth,screenHeight) / 2.5);
    ofSetColor(0);
    ofRect(0, 0, min(screenWidth,screenHeight) / 5.0, min(screenWidth,screenHeight) / 5.0);
    ofPopMatrix();
}

void testApp::drawLargeDiamond(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofPushMatrix();
    ofTranslate(screenWidth / 2.0, screenHeight / 2.0);
    ofRotate(45, 0, 0, 1);
    ofRect(0, 0, min(screenWidth,screenHeight) / 2.0, min(screenWidth,screenHeight) / 2.0);
    ofPopMatrix();
}

void testApp::drawLargeOpenDiamond(){
    ofSetRectMode(OF_RECTMODE_CENTER);
    ofSetColor(128);
    ofPushMatrix();
    ofTranslate(screenWidth / 2.0, screenHeight / 2.0);
    ofRotate(45, 0, 0, 1);
    ofRect(0, 0, min(screenWidth,screenHeight) / 1.5, min(screenWidth,screenHeight) / 1.5);
    ofSetColor(0);
    ofRect(0, 0, min(screenWidth,screenHeight) / 3.0, min(screenWidth,screenHeight) / 3.0);
    ofPopMatrix();
}



//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    
    if (bNoiseState) {
        ofSetRectMode(OF_RECTMODE_CORNER);
        noiseShader.begin();
        noiseShader.setUniform1f("time", rand() % 100000 / 100000.0);
        ofRect(0, 0, screenWidth, screenHeight);
        noiseShader.end();
        
        ofSetColor(255);
        float halfScreenWidth = screenWidth / 2.0;
        float halfScreenHeight = screenHeight / 2.0;
        ofLine(halfScreenWidth, halfScreenHeight - 5, halfScreenWidth, halfScreenHeight + 5);
        ofLine(halfScreenWidth - 5, halfScreenHeight, halfScreenWidth + 5, halfScreenHeight);
    }
    else {
        
        /*
        if (currentLut == 0) {
            bSmallStimuli = false;
            drawLargeOpenCircle();
        }
        else if(currentLut == 1) {
            bSmallStimuli = true;
            drawSmallOpenSquare();
        }
        else if(currentLut == 2) {
            bSmallStimuli = true;
            drawSmallOpenCircle();
        }
        else if(currentLut == 3) {
            bSmallStimuli = false;
            drawLargeOpenSquare();
        }
        else if(currentLut == 4) {
            bSmallStimuli = true;
            drawSmallOpenDiamond();
        }
        else if(currentLut == 5) {
            bSmallStimuli = false;
            drawLargeOpenDiamond();
        }
        */
        
        if (currentLut == 0) {
            drawSmallOpenSquare();
        }
        else if(currentLut == 1) {
            drawSmallOpenCircle();
        }
        else if(currentLut == 2) {
            drawSmallOpenDiamond();
        }
        
        ofDisableAlphaBlending();
        int fixationCrossRadius = 8;
        ofSetColor(255);
        ofLine(currentXFixation, currentYFixation - fixationCrossRadius, 
               currentXFixation, currentYFixation + fixationCrossRadius);
        ofLine(currentXFixation - fixationCrossRadius, currentYFixation, 
               currentXFixation + fixationCrossRadius, currentYFixation);
    }
    //ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == 'f')
        ofToggleFullscreen();
    else if(key == ' ')
    {
        bReadyForStimuli = true;
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