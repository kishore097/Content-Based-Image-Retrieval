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


//******************* Task 3 : Multi Histogram *******************//

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
   

    // open the directory
    dirp = opendir( dirname );
    if( dirp == NULL) {
        printf("Cannot open directory %s\n", dirname);
        exit(-1);
    }
    std::string path[1106];
   
    
    // loop over all the files in the image file listing
    while( (dp = readdir(dirp)) != NULL ) {

        
        // check if the file is an image
        if( strstr(dp->d_name, ".jpg") ||
        strstr(dp->d_name, ".png") ||
        strstr(dp->d_name, ".ppm") ||
        strstr(dp->d_name, ".tif") ) {

      

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
    cv::Mat image = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.0274.jpg", cv::IMREAD_COLOR);


    const int Hsize = 8;


    float *hist3d_1 = new float[Hsize * Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
        hist3d_1[i] = 0;
        
    }
    float *hist3d_2 = new float[Hsize * Hsize * Hsize];

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
        hist3d_2[i] = 0;
        
    }
    


  
    int divisor = 256/Hsize, rix = 0, gix = 0, bix =0;


    for(int i= 0 ; i < image.rows/2; i++){
        for(int j = 0 ; j< image.cols; j++){  
            for(int c = 0 ; c < 3 ; c++){ 
                
                rix = (  image.at<Vec3b>(i,j)[2] )/divisor;
                gix = ( image.at<Vec3b>(i,j)[1] )/divisor;
                bix = (  image.at<Vec3b>(i,j)[0] )/divisor;
                hist3d_1[ rix * Hsize * Hsize + gix * Hsize + bix ]++;

            }  
        }
    }



    for(int i= image.rows/2 ; i < image.rows; i++){
        for(int j = 0 ; j< image.cols; j++){ 
            for(int c = 0 ; c < 3 ; c++){ 
    
                rix = (  image.at<Vec3b>(i,j)[2] )/divisor;
                gix = ( image.at<Vec3b>(i,j)[1] )/divisor;
                bix = (  image.at<Vec3b>(i,j)[0] )/divisor;
                hist3d_2[ rix * Hsize * Hsize + gix * Hsize + bix ]++;

            }  
        }
    }


    float hist_sum = image.rows * image.cols * 3;

    float v;

    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){

        hist3d_1[i] =hist3d_1[i] / hist_sum;

    }
    for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){

        hist3d_2[i] =hist3d_2[i] / hist_sum;

    }


    cout << " Histogram for target image computed. \n Now computing for images in the database " << endl; 

/************************** Loop over Database Images ******************************/
    
    map<float, string> VM;

    for(int k = 0 ; k < 1106; k++ ){
        cv::Mat src = cv::imread(path[k], cv::IMREAD_COLOR);
        int rix_temp = 0, gix_temp = 0, bix_temp = 0;

        float *hist3d_1_temp = new float[Hsize * Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            hist3d_1_temp[i] = 0;
            
        }
        float *hist3d_2_temp = new float[Hsize * Hsize * Hsize];
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            hist3d_2_temp[i] = 0;
            
        }

        for(int i= 0 ; i < src.rows/2; i++){
          for(int j = 0 ; j< src.cols; j++){
            for(int c = 0 ; c < 3 ; c++){    
                
                rix_temp = ( src.at<Vec3b>(i,j)[2] )/divisor;
                gix_temp = (  src.at<Vec3b>(i,j)[1] )/divisor;
                bix_temp = (  src.at<Vec3b>(i,j)[0] )/divisor;

                hist3d_1_temp[  rix_temp * Hsize * Hsize + gix_temp * Hsize + bix_temp  ]++;
            }
          } 
        }
   
        for(int i= src.rows/2 ; i < src.rows; i++){
          for(int j = 0 ; j< src.cols; j++){
            for(int c = 0 ; c < 3 ; c++){    
    
                rix_temp = ( src.at<Vec3b>(i,j)[2] )/divisor;
                gix_temp = (  src.at<Vec3b>(i,j)[1] )/divisor;
                bix_temp = (  src.at<Vec3b>(i,j)[0] )/divisor;

                hist3d_2_temp[  rix_temp * Hsize * Hsize + gix_temp * Hsize + bix_temp  ]++;
            }
          } 
        }
        float hist_sum_temp = src.rows * src.cols * 3;
    
        float v_temp = 0;

        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            hist3d_1_temp[i] = hist3d_1_temp[i] / hist_sum_temp;
            
            
        }
        for(int i = 0; i < Hsize * Hsize * Hsize; i++ ){
            hist3d_2_temp[i] = hist3d_2_temp[i] / hist_sum_temp;
            
            
        }
        float sim_1 = 0, sim_2 = 0;


        for(int i = 0; i <Hsize * Hsize * Hsize; i++ ){
            
            sim_1 += min (hist3d_1[i], hist3d_1_temp[i]);

        }
        for(int i = 0; i <Hsize * Hsize * Hsize; i++ ){
            
            sim_2 += min (hist3d_2[i], hist3d_2_temp[i]);

        }

        float dif = (1- sim_1) + (1- sim_2)  ;
 

  
 
        VM.insert(pair<float, string>(dif, path[k]));      
        delete hist3d_1_temp;
        delete hist3d_2_temp;

    }

    map<float, string>::iterator itr;
    cout << "\nThe distance Values are : \n";
    cout << "\tDistance\tKEY\n";
    for (itr = VM.begin() ; itr != VM.end() ;++itr) {
        cout << '\t' << itr->first << '\t' << itr->second
            << '\n';
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
    delete hist3d_2;
     
    return 0;

}