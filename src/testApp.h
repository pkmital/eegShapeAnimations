#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "pkmGaborAtom.h"
#include "ofxMSATimer.h"

#define RECEIVE_PORT 12345
#define MS_PER_FIXATION 750
#define USE_OSC
#define NUM_TRIALS 10

class testApp : public ofBaseApp
{

public:
    void setup                  ();
    void update                 ();
    void draw                   ();
    void exit                   ();
    
    void drawLargeCircle();
    void drawLargeOpenCircle();
    void drawSmallCircle();
    void drawSmallOpenCircle();
    
    void drawSmallSquare();
    void drawSmallOpenSquare();
    void drawLargeSquare();
    void drawLargeOpenSquare();
    
    void drawSmallDiamond();
    void drawSmallOpenDiamond();
    void drawLargeDiamond();
    void drawLargeOpenDiamond();

    void keyPressed             (int key);
    void keyReleased            (int key);
    void mouseMoved             (int x, int y );
    void mouseDragged           (int x, int y, int button);
    void mousePressed           (int x, int y, int button);
    void mouseReleased          (int x, int y, int button);
    void windowResized          (int w, int h);
    void dragEvent              (ofDragInfo dragInfo);
    void gotMessage             (ofMessage msg);
    
private:
    ofShader                    noiseShader;
    int                         screenWidth, 
                                screenHeight;
    
    int                         currentLut, totalStimuli;
    int                         currentTrial;
    
    int                         currentFixation, totalFixations;
    int                         currentXFixation, currentYFixation;
    ofxMSATimer                 fixationTimer;
    std::vector<ofPoint>        fixationPoints;
    int                         MS_PER_STIMULI;
    
    ofxOscReceiver              oscReceiver;
    ofstream                    outfile;
    
    bool                        bNoiseState, bFirstTrial, bReadyForStimuli, bSmallStimuli;
};
