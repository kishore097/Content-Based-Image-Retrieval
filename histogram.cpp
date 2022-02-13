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
    cv::Mat image = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.0164.jpg", cv::IMREAD_COLOR);


    Mat hist2d;

    const int Hsize = 16;
    int dim[2] = {Hsize, Hsize};

    hist2d = Mat::zeros(2 , dim , CV_32S); 
    
    string ty =  type2str( hist2d.type() );
    printf("Matrix: %s %dx%d \n", ty.c_str(), hist2d.cols, hist2d.rows );

    float *hist2d_v1 = new float[Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize; i++ ){
        hist2d_v1[i] = 0;
        
    }

    int sum = 0, rix = 0, gix = 0;

    for(int i= 0 ; i < image.rows; i++){
      for(int j = 0 ; j< image.cols; j++){
          
        sum = image.at<Vec3b>(i,j)[0] + image.at<Vec3b>(i,j)[1] + image.at<Vec3b>(i,j)[2] +1;
        rix = ( Hsize * image.at<Vec3b>(i,j)[2] )/sum;
        gix = ( Hsize * image.at<Vec3b>(i,j)[1] )/sum;
        hist2d_v1[ rix * Hsize + gix ]++;

      }
    }
    float hist_sum;
    for(int i = 0; i < Hsize * Hsize; i++ ){
        hist_sum += hist2d_v1[i];
        
    }

    float v = 0;

    for(int i = 0; i < Hsize * Hsize; i++ ){
        hist2d_v1[i] =hist2d_v1[i]/ hist_sum;
        v += hist2d_v1[i];

        
    }

    
    map<float, string> VM;

    for(int k = 0 ; k < 1106; k++ ){
        cv::Mat src = cv::imread(path[k], cv::IMREAD_COLOR);
        int sum_temp = 0, rix_temp = 0, gix_temp = 0;

        float *hist2d_v1_temp = new float[Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize; i++ ){
            hist2d_v1_temp[i] = 0;
            
        }
        

        for(int i= 0 ; i < src.rows; i++){
          for(int j = 0 ; j< src.cols; j++){
              
            sum_temp = src.at<Vec3b>(i,j)[0] + src.at<Vec3b>(i,j)[1] + src.at<Vec3b>(i,j)[2] +1;
            rix_temp = ( Hsize * src.at<Vec3b>(i,j)[2] )/sum_temp;
            gix_temp = ( Hsize * src.at<Vec3b>(i,j)[1] )/sum_temp;
            hist2d_v1_temp[ rix_temp * Hsize + gix_temp ]++;

          } 
        }
        float hist_sum_temp = 0;
        for(int i = 0; i < Hsize * Hsize; i++ ){
            hist_sum_temp += hist2d_v1_temp[i];
            
        }
        float v_temp = 0;

        for(int i = 0; i < Hsize * Hsize; i++ ){
            hist2d_v1_temp[i] =float( hist2d_v1_temp[i] )/ hist_sum_temp;
            v_temp += hist2d_v1_temp[i];
            
        }
        float sim = 0;

        for(int i = 0; i < Hsize * Hsize; i++ ){
            
            sim += min (hist2d_v1[i], hist2d_v1_temp[i]);

        }
        float dif = 1 - sim;
 
        VM.insert(pair<float, string>(dif, path[k])); 

        delete hist2d_v1_temp;     

    }

    // // map<float, string>::iterator itr;
    // // cout << "\nThe distance Values are : \n";
    // // cout << "\tDistance\tKEY\n";
    // // for (itr = VM.begin() ; itr != VM.end() ;++itr) {
    // //     cout << '\t' << itr->first << '\t' << itr->second
    // //         << '\n';
    // // }
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
    
    delete hist2d_v1;
    
        
    return 0;

}