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

//*********************** Task 5 : Custom Design and Extension (create additional features and matching method)***********************//

/*

Convert the image into HSV color space, Mask the image with defined color specturm. 
Compute the histogram of the resulting binay image
calclulate magnitude of the resulting binary image to detect texture
calculate the spatial variance of the both histograms and use it as a features. 

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
    cv::Mat image_1 = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.0013.jpg", cv::IMREAD_COLOR);
    imshow("original", image_1);


    Mat blur;
    image_1.copyTo(blur); 
    blur5x5(image_1, blur);
    Mat image_3; 
    image_1.copyTo(image_3);
    blurQuantize(blur, image_3, 10);
    
    
    Mat image_2; 
    image_3.copyTo(image_2);
    
    cvtColor(image_3, image_2, COLOR_BGR2HSV);

    Mat image;
    image_2.copyTo(image);
    inRange(image_2, Scalar(0, 100, 100), Scalar(10, 255, 255), image);

    imshow("threshold", image);

    // Compute Magnitude
    Mat sx  ; 
    image.copyTo(sx  );                      

    sobelX3x3_1d( image, sx  );
    
    Mat sy  ;
    image.copyTo(sy  ); 
    sobelY3x3_1d(image, sy  );
    
    Mat temp_mag_2;
    image.copyTo(temp_mag_2);
    
    Mat mag  ;
    magnitude_1d(sx  , sy  , temp_mag_2);
    convertScaleAbs(temp_mag_2, mag  ); 
    imshow("Magnitude" ,mag  );
    
    // Compute Spatial variance of the binary Image 
    vector<float> vx1; 
    vector<float> vy1;
    float h = 0;
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


    //Computer spatial variance of the magnitude image
    vector<float> mx1; 
    vector<float> my1;
    int m1 = 0;
    for(int i = 0 ;  i < image.rows ; i++){
        for(int j = 0 ; j< image.cols; j++){  
            if(mag.at<uchar>(i,j) == 255){
                mx1.push_back(i);
                my1.push_back(j);
                m1++;
            }
            else{
                continue;
            }
      

        }
    }
    float c1 = 0, d1 = 0, Mmi1 = 0, Mmj1 = 0, mvar1 = 0, mstd1 =0;
    for(int i = 0; i < m1 ; i++ ){
        Mmi1 += mx1[i];
        Mmj1 += my1[i];


    }
 
    // Mean of the i, j values
    c1 = Mmi1 / m1;
    d1 = Mmj1 / m1;



    for(int i = 0; i < m1 ; i++ ){
        mvar1 += (mx1[i] - c1) * (mx1[i] - c1) + (my1[i] - d1)* (my1[i] - d1);

    }
    mstd1 = sqrt(mvar1);

    cout << mstd1 << endl;


    
/********************************Loop over Database******************************/

    
    map<float, string> VM;
 

    for(int k =  0; k < 283; k++ ){
        cv::Mat src_1 = cv::imread(path[k] , cv::IMREAD_COLOR);
        vector<float> vx; 
        vector<float> vy;
        vector<float> mx; 
        vector<float> my;

       
        Mat blur_temp;        
        src_1.copyTo(blur_temp); 
        blur5x5(src_1, blur_temp);
        Mat src_3; 
        src_1.copyTo(src_3);
        blurQuantize(blur_temp, src_3, 10);
       
        Mat src_2; 
        src_3.copyTo(src_2);

        
        cvtColor(src_3, src_2, COLOR_BGR2HSV);

        Mat src;
        src_2.copyTo(src);
        inRange(src_2, Scalar(0, 100, 100), Scalar(10, 255, 255), src);

        Mat sx_temp  ; 
        src.copyTo(sx_temp  );                      

        sobelX3x3_1d( src, sx_temp  );
        
        Mat sy_temp  ;
        src.copyTo(sy_temp  ); 
        sobelY3x3_1d(src, sy_temp  );
        
        Mat temp_mag_1;
        src.copyTo(temp_mag_1);
        
        Mat mag_temp  ;
        magnitude_1d(sx_temp  , sy_temp  , temp_mag_1);
        convertScaleAbs(temp_mag_1, mag_temp  ); 
       

        float l = 0; 
       

        for(int i = 0 ;  i < src.rows ; i++){
            for(int j = 0 ; j< src.cols ;j++){  
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

        float a = 0, b = 0, Mi = 0, Mj = 0,  dif = 0, var = 0 , std = 0;
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


        
        int m = 0;
        for(int i = 0 ;  i < mag_temp.rows ; i++){
            for(int j = 0 ; j< mag_temp.cols; j++){  
                if(mag_temp.at<uchar>(i,j) == 255){
                    mx.push_back(i);
                    my.push_back(j);
                    m++;
                }
                else{
                    continue;
                }
        

            }
        }
        float c = 0, d = 0, Mmi = 0, Mmj = 0, mvar = 0, mstd=0;
        for(int i = 0; i < m ; i++ ){
            Mmi += mx[i];
            Mmj += my[i];


        }
    
        // Mean of the i, j values
        c = Mmi/ m;
        d = Mmj / m;



        for(int i = 0; i < m ; i++ ){
            mvar += (mx[i] - c) * (mx[i] - c) + (my[i] - d)* (my[i] - d);

        }
        mstd = sqrt(mvar);

        // New Distance Metric - differences in the spatial variance assign Weights

        dif  = 0.7*(std - std1)*(std - std1) + 0.3*(mstd1 - mstd) * (mstd1 - mstd);

        

        VM.insert(pair<float, string>(dif, path[k]));      

    }


    float distance[20] ;
    string key[20];

    auto it = VM.begin();
    for(int i = 0; i < 20 && it != VM.end(); ++i){
        ++it;
        if(it != VM.end()){
            distance[i] = it->first;
            key[i] = it->second ;
        }
        else{
            std::cout << "not found";
        }
    }
    for(int i = 0 ; i <20; i++){
        cout << "Distance is : '" << distance[i] << "' for image in the path : " << key[i] << endl;
    }
    
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

    // Display top 10 best results
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