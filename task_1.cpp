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

/*******************Task 1 : Baseline Matching comparing a 9 x 9 patch in the image.*******************/


using namespace std;
using namespace cv;

bool sortcol(const vector<float>& v1, const vector<float>& v2)
{
    return v1[0] < v2[0];
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
    cv::Mat target = cv::imread("/home/kishore/PRCV/Project_2/olympus/pic.1016.jpg", cv::IMREAD_COLOR);
    cv::Mat square9;
    cv::Mat temp;
    vector<vector<int>> v( 1 , vector<int> (243, 0));

    square9.create(9,9,target.type());
    temp.create(9,9,target.type());


    for(int i = 0 ; i < 9; i++ ){
        for(int j = 0; j < 9; j++ ) {
            for(int c=0; c<3 ; c++ ){
               
                v[k][i * 9  + j + c * 9 * 9]  = target.at<cv::Vec3b>(target.rows/2 -4 + i,target.cols/2 -4 + j)[c];
            }
        }
    }

    cout << " Histogram for target image computed succesfully \n Now computing histogram of images in the database... " << endl; 


    vector<vector<float>> val( 1106 , vector<float> (2));
    vector<string> p;

    for(int k = 0 ; k < 1106; k++ ){
        cv::Mat src = cv::imread(path[k], cv::IMREAD_COLOR);
        vector<vector<int>> vi( 1106 , vector<int> (243, 0));

        for(int i = 0 ; i < 9; i++ ){
            for(int j = 0; j < 9; j++ ) {
                for(int c = 0; c < 3 ; c++ ){
                    vi[k][i * 9  + j  + c * 9 *9]  = src.at<cv::Vec3b>(src.rows/2 - 4 + i, src.cols/2 - 4 + j)[c];
                } 
            }
        }


        for(int i = 0 ; i < 243; i++ ){
            val[k][0] += (v[0][i] - vi[k][i]) * (v[0][i] - vi[k][i]);
    
        }

        val[k][1] = k;
      
    }

    sort(val.begin(), val.end(), sortcol);

    cout<< " Displayng the top four closest images : " << endl;


    for  ( int k = 0 ; k < 1106 ; k++){
        cout << "Distance : " << val[k][0] << "  path :  " << val[k][1] << endl;
    }  
    

    cv::Mat first = cv::imread(path[int(val[1][1])], cv::IMREAD_COLOR);
    cv::Mat second = cv::imread(path[int(val[2][1])], cv::IMREAD_COLOR);
    cv::Mat third = cv::imread(path[int(val[3][1])], cv::IMREAD_COLOR);


    
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
    
    
    return 0;
}