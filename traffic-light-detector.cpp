//
//  traffic-light-detector.cpp
//  ngi
//
//  Created by Owen Nuckolls on 6/3/22.
//  Copyright © 2022 Owen Nuckolls. All rights reserved.
//

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

//method to read a string in from user input
std::string read_string() {
  std::string s; unsigned int uc; int c;
  while ((uc = (unsigned int)getchar()) <= 32u);
  if (uc < 256u) s.push_back((char)uc);
  while ((c = getchar()) > 32) s.push_back((char)c);
  return s;
}

int main(int argc, const char * argv[]) {
    // insert code here..
    std::string originFile;
    std::string destFile;
    int topLeftXCoor;
    int topLeftYCoor;
    int radius;
    int gapSize;
    printf("Please enter the file path of the video to be analyzed:\n");
    originFile = read_string();
    printf("Please enter the desired file path for the final video:\n");
    destFile = read_string();
    printf("Please enter the x coordinate of the top left of the traffic light, in pixels:\n");
    scanf("%d", &topLeftXCoor);
    printf("Please enter the y coordinate of the top left of the traffic light, in pixels:\n");
    scanf("%d", &topLeftYCoor);
    printf("Please enter the radius of one light, in pixels:\n");
    scanf("%d", &radius);
    printf("Please enter the size of the gap between lights, in pixels:\n");
    scanf("%d", &gapSize);

    cv::VideoCapture cap(originFile);

    int codec = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));
    cv::Size srcSize = cv::Size((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    cv::VideoWriter dest_cap;
    dest_cap.open(destFile, codec, fps, srcSize, true);
    
    if (!cap.isOpened()){
        std::cout << "Error opening video stream or file" << std::endl;
    }
    //else{
        //cap.set(cv::CAP_PROP_FPS, 30);
    //}
    
    while(1){
        
        cv::Mat_<cv::Vec3b> frame;
        cap>>frame;
        if(frame.empty()){
            break;
        }
        
        int greenTot = 0;
        int yellowTot = 0;
        int redTot = 0;
        
        for(int i=topLeftXCoor; i<topLeftXCoor+2*radius+1; i++){
            for(int j=topLeftYCoor+4*radius+2*gapSize; j<topLeftYCoor+6*radius+2*gapSize+1; j++){
                cv::Vec3d pixel = frame(j, i); //green
                greenTot = greenTot + pixel[0]/4 + 2*pixel[1] + pixel[2]/4;
            }
        }
        
        for(int i=topLeftXCoor; i<topLeftXCoor+2*radius+1; i++){
            for(int j=topLeftYCoor+2*radius+gapSize; j<topLeftYCoor+4*radius+gapSize+1; j++){
                cv::Vec3d pixel = frame(j, i); //yellow
                yellowTot = yellowTot + pixel[0]/2 + pixel[1] + pixel[2];
            }
        }
        
        for(int i=topLeftXCoor; i<topLeftXCoor+2*radius+1; i++){
            for(int j=topLeftYCoor; j<topLeftYCoor+2*radius+1; j++){
                cv::Vec3d pixel = frame(j, i); //red
                redTot = redTot + pixel[0]/4 + pixel[1]/4 + 2*pixel[2];
            }
        }
        
        //prints to stdout the proportionate BGR totals for each light, for testing
        std::cout << greenTot << " " << yellowTot << " " << redTot << std::endl;
        
        std::string str;
        if(greenTot > yellowTot && greenTot > redTot){
            str = "green";
            cv::putText(frame,str,cv::Point(topLeftXCoor,topLeftYCoor),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,0),2,false);
        }
        else if(yellowTot > redTot && yellowTot >= greenTot){
            str = "yellow";
            cv::putText(frame,str,cv::Point(topLeftXCoor,topLeftYCoor),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,255,255),2,false);
        }
        else if(redTot > yellowTot && redTot >= greenTot){
            str = "red";
            cv::putText(frame,str,cv::Point(topLeftXCoor,topLeftYCoor),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(0,0,255),2,false);
        }
        else{
            str = "error";
            cv::putText(frame,str,cv::Point(topLeftXCoor,topLeftYCoor),cv::FONT_HERSHEY_DUPLEX,1,cv::Scalar(255,0,0),2,false);
        }
        
        cv::imshow("frame ", frame);
        char c = (char)  cv::waitKey(25);
        if(c==32)
            break;

        dest_cap << frame;
        
        
    }

    cap.release();
    dest_cap.release();
    
    return 0;
}
