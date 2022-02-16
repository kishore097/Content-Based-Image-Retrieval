#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <map>
#include "filter.h"

//******************* Extension : Detecting Blue Bins *******************//

/*
Convert the image into HSV color space, Mask the image with blue color specturm. 

Compute the histogram of the resulting binay image

Calclulate magnitude of the resulting binary image to detect texture

Calculate the spatial variance of histogram and use it as a one of the features. 

Use squared differences of the histogram as another feature

Computing difference in color histogram and spatial variance

*/


using namespace cv;
using namespace std;
string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

int main (int argc, char *argv[] ){

    char dirname[256];
    char buffer[256];
    FILE *fp;
    DIR *dirp;
    struct dirent *dp;
    int r=0;

    // check for sufficient arguments
    if( argc < 2) {
        printf("usage: %s <directory path>\n", argv[0]);
        exit(-1);
    }

    // get the directory path
    strcpy(dirname, argv[1]);
    // printf("Processing directory %s\n", dirname );

    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    std::string path[1106];
    // std::string path[1107];
    
    // loop over all the files in the image file listing
    while( (dp = readdir(dirp)) != NULL ) {

        
        // check if the file is an image
        if( strstr(dp->d_name, ".jpg") ||
        strstr(dp->d_name, ".png") ||
        strstr(dp->d_name, ".ppm") ||
        strstr(dp->d_name, ".tif") ) {

        strcpy(buffer, dirname);
        strcat(buffer, "/");
        path[r] = strcat(buffer, dp->d_name);
        r++;   
     
        }
        
        
    }
    
    int n = sizeof(path)/sizeof(path[0]);
    sort(path, path + n);

    int k;
      
    //! [imread] 
    cv::Mat image_1 = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.0287.jpg", cv::IMREAD_COLOR);
    // imshow("original", image_1);
    // Rect R=Rect(150,100,100, 300 );
    // //create a Rect with top-left vertex at (10,20), of width 40 and height 60 pixels.

    // rectangle(image_1 ,R,Scalar(0,0,255),1,8,0);


    Mat blur;
    image_1.copyTo(blur); 
    blur5x5(image_1, blur);
    Mat image_3; 
    image_1.copyTo(image_3);
    blurQuantize(blur, image_3, 10);
    imshow("quantize", image_3); 

    
    Mat image_2; 
    image_3.copyTo(image_2);
    
    cvtColor(image_3, image_2, COLOR_BGR2HSV);
    for(int i = 150 ;  i < image_2.rows/2 ; i++){
        for(int j = 150 ; j< image_2.cols/2; j++){  
            Vec3b hsv=image_2.at<Vec3b>(i,j);
            int H=hsv.val[0];
            int S=hsv.val[1];
            int V=hsv.val[2];

            // cout << " H, S , V = " << H << " , "<< S <<" , " << V <<"  (i , j) = "<< i << ","<< j  << " | ";
        }
    }

    Mat image;
    image_2.copyTo(image);
    inRange(image_2, Scalar(110, 200, 100), Scalar(120, 255, 255), image);

    imshow("HSV" , image_2);

    imshow("threshold", image);
 

    string ty =  type2str( image_2.type() );
    printf("Matrix: %s %dx%d \n", ty.c_str(), image_2.cols, image_2.rows );

    vector<float> vx1; 
    vector<float> vy1;
    int h = 0;
    for(int i = 0 ;  i < image.rows ; i++){
        for(int j = 0 ; j< image.cols; j++){  
            if(image.at<uchar>(i,j) == 255){
                vx1.push_back(i);
                vy1.push_back(j);
                h++;
            }
            else{
                continue;
            }
      

        }
    }
    float a1 = 0, b1 = 0, Mi1 = 0, Mj1 = 0, var1 = 0, std1 =0;
    for(int i = 0; i < h ; i++ ){
        Mi1 += vx1[i];
        Mj1 += vy1[i];


    }
 
    // Mean of the i, j values
    a1 = Mi1 / h;
    b1 = Mj1 / h;



    for(int i = 0; i < h ; i++ ){
        var1 += (vx1[i] - a1) * (vx1[i] - a1) + (vy1[i] - b1)* (vy1[i] - b1);

    }
    std1 = sqrt(var1);

/********************************Loop over Database******************************/

    vector<int> vx; 
    vector<int> vy;
    map<float, string> VM;
 

    for(int k =  0; k < 1106; k++ ){
        cv::Mat src_1 = cv::imread(path[k], cv::IMREAD_COLOR);

        Mat src_2; 
        src_1.copyTo(src_2);

        
        cvtColor(src_1, src_2, COLOR_BGR2HSV);

        Mat src;
        src_2.copyTo(src);
        inRange(src_2, Scalar(110, 200, 100), Scalar(120, 255, 255), src);
        float l = 0,  d =0, var = 0 , std = 0;
        // imshow(path[k] , src);
        int v =0;
        vector<float> vx; 
        vector<float> vy;

        for(int i = 0 ;  i < image.rows ; i++){
            for(int j = 0 ; j< image.cols ;j++){  
                if(src.at<uchar>(i,j) == 255){
                    
                    vx.push_back(i);
                    vy.push_back(j);
                    l++;
                    
                }
                else{
                    continue;
                }
        

            }
        }

        float a = 0, b = 0, Mi = 0, Mj = 0;
        for(int i = 0; i < l ; i++ ){
            Mi += vx[i];
            Mj += vy[i];
        }
        a = Mi / l;
        b = Mj / l;

        for(int i = 0; i < l ; i++ ){
            var += (vx[i] - a) * (vx[i] - a) + (vy[i] - b)* (vy[i] -b);

        }
    

        std = sqrt(var);
        // cout << "( " << a << ", " << b << ")"<< " path  :  "<< path[k] << endl;

        cout << "standard Deviation : " << std1-std << endl;
        cout  << " dist   : "  << sqrt((h - l) * (h-l))<< "  ." << "  varinace  :  "<< var<< " path  :  "<< path[k]<< endl;
        

        d  = sqrt((h - l) * (h-l)) + (std1 - std) ;


        VM.insert(pair<float, string>(d, path[k]));      


    }


    float distance[50] ;
    string key[50];

    auto it = VM.begin();
    for(int i = 0; i < 50 && it != VM.end(); ++i){
        ++it;
        if(it != VM.end()){
            distance[i] = it->first;
            key[i] = it->second ;
        }
        else{
            std::cout << "not found";
        }
    }
    for(int i = 0 ; i <50; i++){
        cout << "Distance is : '" << distance[i] << "' for image in the path : " << key[i] << endl;
    }

    
    // // delete t_temp;
    
    cv::Mat first = cv::imread(key[0], cv::IMREAD_COLOR);
    cv::Mat second = cv::imread(key[1], cv::IMREAD_COLOR);
    cv::Mat third = cv::imread(key[2], cv::IMREAD_COLOR);
    cv::Mat fourth = cv::imread(key[3], cv::IMREAD_COLOR);
    cv::Mat fifth = cv::imread(key[4], cv::IMREAD_COLOR);
    cv::Mat six = cv::imread(key[5], cv::IMREAD_COLOR);
    cv::Mat seven = cv::imread(key[6], cv::IMREAD_COLOR);
    cv::Mat eight = cv::imread(key[7], cv::IMREAD_COLOR);
    cv::Mat nine = cv::imread(key[8], cv::IMREAD_COLOR);
    cv::Mat ten = cv::imread(key[9], cv::IMREAD_COLOR);

    cv::imshow("Baseline Image", image);
    cv::imshow("First Closest", first);
    cv::imshow("Second Closest", second);
    cv::imshow("Third Closest", third);
    cv::imshow("4", fourth);
    cv::imshow("5", fifth);
    cv::imshow("6", six);
    cv::imshow("7", seven);
    cv::imshow("8", eight);
    cv::imshow("9", nine);
    cv::imshow("10", ten);
    
  
    waitKey(0);
     
    return 0;

}