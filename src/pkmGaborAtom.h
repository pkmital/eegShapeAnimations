//
//  pkmGaborAtom.h
//  gaborAnimations
//
//  Created by Parag Mital on 10/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#define USE_GPU

#include "ofMain.h"
#include "ofxOpenCv.h"


class pkmGaborAtom {
public:

    pkmGaborAtom()
    {
        
#ifdef USE_GPU
        bMultiple = true;
        gaborShader.load(ofToDataPath("gabor"));
#endif
        
    }
    
    void toggleMultiple()
    {
        bMultiple = !bMultiple;
        if(bMultiple)
        {
            gaborShader.load(ofToDataPath("gabormultiple"));
        }
        else
        {
            gaborShader.load(ofToDataPath("gabor"));
        }
    }
    
    void allocate(int kernelSize = 127)
    {
        // must be an odd kernel size
        this->kernelSize = ((kernelSize + 1) % 2) * (kernelSize + 1) + ((kernelSize) % 2) * (kernelSize);

#ifdef USE_GPU
        fboScreen.allocate(kernelSize, kernelSize, GL_RGB, 4);
        fboAtom.allocate(kernelSize, kernelSize, GL_RGB, 0);        
#else   
        atom.allocate(kernelSize, kernelSize);
#endif
    }
    
    void update(float sigma = 32, float theta = 0, float lm = 1, float psi = 90)
    {
#ifdef USE_GPU
        fboScreen.begin();
        gaborShader.begin();
        gaborShader.setUniform1f("th", theta);
        gaborShader.setUniform1f("sig", sigma);
        gaborShader.setUniform1f("lm", lm);
        gaborShader.setUniform1f("ps", psi);
        gaborShader.setUniform1f("ks", kernelSize);
        gaborShader.setUniformTexture("image", fboAtom.getTextureReference(), 1);
        fboAtom.draw(0, 0);
        gaborShader.end();
        fboScreen.end();
#else
        cv::Mat atomImg = atom.getCvImage();
        mkKernel(atomImg, kernelSize, sigma, theta, lm, psi);
        atomImg /= 2.;
        atomImg += 0.5;
        atom.flagImageChanged();
#endif
    }
    
    void draw(int px = 0, int py = 0)
    {
#ifdef USE_GPU
        fboScreen.draw(px, py);
#else
        atom.draw(px, py);
#endif
    }
    
    void draw(int px, int py, int w, int h)
    {
#ifdef USE_GPU
        fboScreen.draw(px, py, w, h);
#else
        atom.draw(px, py, w, h);
#endif
    }
    
protected:
    
    // taken from https://github.com/dominiklessel/opencv-gabor-filter/blob/master/gaborFilter.cpp
    void mkKernel(cv::Mat &kernel, int ks, double sig, double th, double lm, double ps)
    {
        int hks = (ks-1)/2;
        double theta = th*CV_PI/180;
        double psi = ps*CV_PI/180;
        double del = 2.0/(ks-1);
        double lmbd = lm;
        double sigma = sig/ks;
        double x_theta;
        double y_theta;
        //cv::Mat kernel(ks,ks, CV_32F);
        for (int y=-hks; y<=hks; y++)
        {
            for (int x=-hks; x<=hks; x++)
            {
                x_theta = x*del*cos(theta)+y*del*sin(theta);
                y_theta = -x*del*sin(theta)+y*del*cos(theta);
                kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos(2*CV_PI*x_theta/lmbd + psi);
            }
        }
    }
    
private:
    
    ofShader                gaborShader;
    ofxCvFloatImage         atom;
    ofFbo                   fboAtom, fboScreen;
    int                     kernelSize;
    bool                    bMultiple;
    
};