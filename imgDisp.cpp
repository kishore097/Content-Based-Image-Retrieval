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

using namespace std;
using namespace cv;

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

int main(int argc, char *argv[])
{

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
    std::string path[1107];
    
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
        // cout << buffer << " key" << r << endl;
        }
    }

    int k;
      
    //! [imread] 
    cv::Mat target = cv::imread(path[268], cv::IMREAD_COLOR);
    cv::Mat square9;
    cv::Mat temp;
    vector<cv::Mat> v;

    square9.create(9,9,target.type());
    temp.create(9,9,target.type());


    for(int i = 0 ; i < 9; i++ ){
        for(int j = 0; j < 9; j++ ) {
            for(int c=0; c<3 ; c++ ){
                square9.at<cv::Vec3b>(i,j)[c] = target.at<cv::Vec3b>(target.rows/2 -4 + i,target.cols/2 -4 + j)[c];
            }
        }
    }

    v.push_back(square9.clone());
    // cout << v[0] << endl;

    map<float, int> VM;
    // vector<vector<float>> V;

    for(int k = 0 ; k < 1106; k++ ){
        cv::Mat src = cv::imread(path[k], cv::IMREAD_COLOR);
        vector<cv::Mat> vi;

        for(int i = 0 ; i < 9; i++ ){
            for(int j = 0; j < 9; j++ ) {
                for(int c = 0; c < 3 ; c++ ){
                    temp.at<cv::Vec3b>(i,j)[c] = src.at<cv::Vec3b>(src.rows/2 - 4 + i, src.cols/2 - 4 + j)[c];
                }
            }
        }
   

        int value = 0;

        for(int i = 0; i < 9; i++ ){
            for(int j = 0; j < 9; j++ ) {
                for(int c=0; c < 3 ; c++ ){
                    value += (square9.at<cv::Vec3b>(i,j)[c] - temp.at<cv::Vec3b>(i,j)[c])*(square9.at<cv::Vec3b>(i,j)[c] - temp.at<cv::Vec3b>(i,j)[c]);
                    
                }
            }
        }

 
        VM.insert(pair<float, int>(float(sqrt(value/243)), k));      

    }
  
    map<float, int>::iterator itr;
    // cout << "\nThe distance Values are : \n";
    // cout << "\tDistance\tKEY\n";
    // for (itr = M.begin() ; itr != M.end() ;++itr) {
    //     cout << '\t' << itr->first << '\t' << itr->second
    //         << '\n';
    // }

    float distance[4] ;
    int key[4];

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
        cout << distance[i]<< " for image " << key[i] << endl;
    }



   
    
    //! [empty]
    if(square9.empty())
    {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }
    //! [empty]

    //! [imshow]
    // cv::imshow("Image", square9);

    cv::Mat first = cv::imread(path[key[1]], cv::IMREAD_COLOR);
    cv::Mat second = cv::imread(path[key[2]], cv::IMREAD_COLOR);
    cv::Mat third = cv::imread(path[key[3]], cv::IMREAD_COLOR);


    
    cv::imshow("Baseline Image", target);
    cv::imshow("First Closest", first);
    cv::imshow("Second Closest", second);
    cv::imshow("Third Closest", third);
  
    

    while (true) {
        k = cv::waitKey(0);
        
        if (k == 113) {
            cv::destroyAllWindows();
            break;
        }

    }
    // delete target_array;
    
    
    return 0;
}