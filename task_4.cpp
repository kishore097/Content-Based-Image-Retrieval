#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <iostream>
#include <map>
#include "filter.h"


//******************* Task 4 : Texture and Color Analysis *******************//

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

        // printf("processing image file: %s\n", dp->d_name);

        // build the overall filename
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
    cv::Mat image = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.0844.jpg", cv::IMREAD_COLOR);
    Mat mag;
    Mat orient;

    string ty =  type2str( image.type() );
    printf("Matrix: %s %dx%d \n", ty.c_str(), image.cols, image.rows );

    // image.copyTo(mag); 
    cv::Mat3s sx, temp_orient ; 

    image.copyTo(sx);                      

    sobelX3x3(image, sx);
    

    cv::Mat3s sy;
    image.copyTo(sy); 
    sobelY3x3(image, sy);
    
    cv::Mat3s temp_mag;
    image.copyTo(temp_mag);

    magnitude(sx, sy, temp_mag);
    convertScaleAbs(temp_mag, mag);
    imshow("magnitude", mag);

    image.copyTo(temp_orient);
    orientation(sx,sy,temp_orient);
    convertScaleAbs(temp_orient, orient);

    imshow("orientaion", orient);

    string ty1 =  type2str( mag.type() );
    printf("Matrix mag: %s %dx%d \n", ty1.c_str(), mag.cols, mag.rows );

    string ty2 =  type2str( temp_orient.type() );
    printf("Matrix orient: %s %dx%d \n", ty2.c_str(), temp_orient.cols, temp_orient.rows );

    const int Hsize = 8;

    float *hist3d_1 = new float[Hsize * Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
        hist3d_1[i] = 0;
        
    }
    float *texture = new float[Hsize * Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
        texture[i] = 0;
        
    }

    float *gradient = new float[Hsize * Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
        gradient[i] = 0;
        
    }
  
    int divisor = 256/Hsize, rix = 0, gix = 0, bix =0;

    for(int i= 0 ; i < image.rows; i++){
        for(int j = 0 ; j< image.cols; j++){  
                         
            rix = (  image.at<Vec3b>(i,j)[2] )/divisor;
            gix = ( image.at<Vec3b>(i,j)[1] )/divisor;
            bix = (  image.at<Vec3b>(i,j)[0] )/divisor;
            hist3d_1[ rix * Hsize * Hsize + gix * Hsize + bix ]++;
         
        }
    }


    for(int i= 0 ; i < mag.rows; i++){
        for(int j = 0 ; j< mag.cols; j++){ 
               
            rix = (  mag.at<Vec3b>(i,j)[2] )/divisor;
            gix = ( mag.at<Vec3b>(i,j)[1] )/divisor;
            bix = (  mag.at<Vec3b>(i,j)[0] )/divisor;
            texture[ rix * Hsize * Hsize + gix * Hsize + bix ]++;
            
        }
    }



    float hist_sum = image.rows * image.cols * 3;

    float v;

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){

        hist3d_1[i] =hist3d_1[i] / hist_sum;

    }
    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){

        texture[i] = texture[i] / hist_sum;
        gradient[i] = gradient[i] / hist_sum;

    }
    int hist = 1, text = 2;
    
    cout << " Histogram for target image computed. \n Now computing for images in the database " << endl; 

 /******************************* Loop Over Database ***************************/

    map<float, string> VM;
    Mat mag_temp, orient_temp;

    for(int k = 0 ; k < 1106; k++ ){
        cv::Mat src = cv::imread(path[k], cv::IMREAD_COLOR);
        int rix_temp = 0, gix_temp = 0, bix_temp = 0;
        
   
         cv::Mat3s sx_temp; 
        src.copyTo(sx_temp);                      

        sobelX3x3(src, sx_temp);
        
        cv::Mat3s sy_temp;
        src.copyTo(sy_temp); 
        sobelY3x3(src, sy_temp);
        
        cv::Mat3s temp_mag_2;
        src.copyTo(temp_mag_2);

        magnitude(sx_temp, sy_temp, temp_mag_2);
        convertScaleAbs(temp_mag_2, mag_temp);          

        Mat3s temp_orient;
        Mat orient_temp;
        image.copyTo(temp_orient);
        orientation(sx_temp , sy_temp , temp_orient);
        convertScaleAbs(temp_orient, orient_temp);
            
    
        float *hist3d_1_temp = new float[Hsize * Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            hist3d_1_temp[i] = 0;       
        }

        float *texture_temp = new float[Hsize * Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            texture_temp[i] = 0;        
        }

        float *gradient_temp = new float[Hsize * Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            gradient_temp[i] = 0;
        }

        for(int i= 0 ; i < src.rows; i++){
          for(int j = 0 ; j< src.cols; j++){
           
            rix_temp = ( src.at<Vec3b>(i,j)[2] )/divisor;
            gix_temp = (  src.at<Vec3b>(i,j)[1] )/divisor;
            bix_temp = (  src.at<Vec3b>(i,j)[0] )/divisor;
            hist3d_1_temp[  rix_temp * Hsize * Hsize + gix_temp * Hsize + bix_temp  ]++;
        
          } 
        }
       
        for(int i= 0 ; i < mag_temp.rows; i++){
          for(int j = 0 ; j< mag_temp.cols; j++){
           
            rix_temp = ( mag_temp.at<Vec3b>(i,j)[2] )/divisor;
            gix_temp = (  mag_temp.at<Vec3b>(i,j)[1] )/divisor;
            bix_temp = (  mag_temp.at<Vec3b>(i,j)[0] )/divisor;
            texture_temp[  rix_temp * Hsize * Hsize + gix_temp * Hsize + bix_temp  ]++;
            
          } 
        }
 

        float hist_sum_temp = src.rows * src.cols * 3;
    
        float v_temp = 0;

        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){

            hist3d_1_temp[i] = hist3d_1_temp[i] / hist_sum_temp;
            texture_temp[i] = texture_temp[i] / hist_sum_temp;
            gradient_temp[i] = gradient_temp[i] /  hist_sum_temp;

                       
        }

        
        float sim_1 = 0, sim_2 = 0 , sim_3 = 0;


        for(int i = 0; i <Hsize * Hsize * Hsize; i++ ){
            
            sim_1 += min (hist3d_1[i], hist3d_1_temp[i]);
            sim_2 += min (texture[i], texture_temp[i]);
            sim_3 += min(gradient[i], gradient_temp[i]);

        }

        float dif = hist*(1- sim_1) + text*(1- sim_2)  ;
  
        VM.insert(pair<float, string>(dif, path[k]));   

        delete hist3d_1_temp;
        delete texture_temp;
        delete gradient_temp;

    }


    float distance[4] ;
    string key[4];

    auto it = VM.begin();
    for(int i = 0; i < 4 && it != VM.end(); ++i){
        ++it;
        if(it != VM.end()){
            distance[i] = it->first;
            key[i] = it->second ;
        }
        else{
            std::cout << "not found";
        }
    }

    cout<< " Displayng the top four closest images : " << endl;

    for(int i = 0 ; i <4; i++){
        cout << "Distance is : '" << distance[i] << "' for image in the path : " << key[i] << endl;
    }

    
    cv::Mat first = cv::imread(key[0], cv::IMREAD_COLOR);
    cv::Mat second = cv::imread(key[1], cv::IMREAD_COLOR);
    cv::Mat third = cv::imread(key[2], cv::IMREAD_COLOR);

    cv::imshow("Baseline Image", image);
    cv::imshow("First Closest", first);
    cv::imshow("Second Closest", second);
    cv::imshow("Third Closest", third);
  
    waitKey(0);

    
    delete hist3d_1;
    delete texture;
    delete gradient;
     
    return 0;

}