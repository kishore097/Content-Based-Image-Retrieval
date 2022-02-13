#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <cmath>
#include "filter.h"

using namespace cv;
int grayscale(const cv::Mat &src, cv::Mat &dst){    
    for(int i = 0; i < dst.rows; i++){
        for(int j = 0; j < dst.cols; j++){            
                cv::Vec3b value = src.at<cv::Vec3b>(i,j);
                dst.at<uchar>(i,j) = (value.val[0] + value.val[1]+value.val[2])/3;
        }
    }
    return 0;
}


int blur5x5(const cv::Mat &src, cv::Mat &dst){
    cv::Mat tmp;
    tmp.create(src.size(), src.type());
    
    for(int i = 0; i < src.rows; i++){
        for(int j = 2; j < src.cols-2; j++){
            for(int c = 0; c < 3; c++){
                tmp.at<cv::Vec3b>(i,j)[c] = src.at<cv::Vec3b>(i,j-2)[c]*0.1 + src.at<cv::Vec3b>(i,j-1)[c]*0.2 +src.at<cv::Vec3b>(i,j)[c]*0.4 + src.at<cv::Vec3b>(i,j+1)[c]*0.2 + src.at<cv::Vec3b>(i,j+2)[c]*0.1;                
            }           
        }    
    }

    for(int i = 2; i < src.rows-2; i++){
        for(int j = 0; j < src.cols; j++){
            for(int c = 0; c < 3; c++){
                dst.at<cv::Vec3b>(i,j)[c] = tmp.at<cv::Vec3b>(i-2,j)[c]*0.1 + tmp.at<cv::Vec3b>(i-1,j)[c]*0.2 +tmp.at<cv::Vec3b>(i,j)[c]*0.4 + tmp.at<cv::Vec3b>(i+1,j)[c]*0.2 + tmp.at<cv::Vec3b>(i+2,j)[c]*0.1;
            }            
        }   
    }
    return 0;
}

int sobelX3x3(const cv::Mat &src, cv::Mat3s &dst){
    cv::Mat tmp;
    src.copyTo(tmp);

    for(int i = 0; i < src.rows; i++){
        for(int j = 1; j < src.cols-1; j++){
            for(int c = 0; c < 3; c++){
                tmp.at<cv::Vec3b>(i,j)[c] = src.at<cv::Vec3b>(i-1,j)[c]*0.25 + src.at<cv::Vec3b>(i,j)[c]*0.5 + src.at<cv::Vec3b>(i+1,j)[c]*0.25;
            }   
        }
    }

    for(int i = 1; i < src.rows-1; i++){
        for(int j = 0; j < src.cols; j++){
            for(int c = 0; c < 3; c++){
                dst.at<cv::Vec3s>(i,j)[c] = -tmp.at<cv::Vec3b>(i,j-1)[c]*1 +tmp.at<cv::Vec3b>(i,j)[c]*0 + tmp.at<cv::Vec3b>(i,j+1)[c]*1;
            }           
        }   
    }
    return 0;
}

int sobelY3x3(const cv::Mat &src, cv::Mat3s &dst){
    cv::Mat tmp;
    src.copyTo(tmp);

    for(int i = 0; i < src.rows; i++){
        for(int j = 1; j < src.cols-1; j++){
            for(int c = 0; c < 3; c++){
                tmp.at<cv::Vec3b>(i,j)[c] = src.at<cv::Vec3b>(i,j-1)[c]*0.25 +src.at<cv::Vec3b>(i,j)[c]*0.5 + src.at<cv::Vec3b>(i,j+1)[c]*0.25;                
            }            
        }    
    }
    for(int i = 1; i < src.rows-1; i++){
        for(int j = 0; j < src.cols; j++){
            for(int c = 0; c < 3; c++){
                dst.at<cv::Vec3s>(i,j)[c] = tmp.at<cv::Vec3b>(i-1,j)[c]*1 + tmp.at<cv::Vec3b>(i,j)[c]*0 - tmp.at<cv::Vec3b>(i+1,j)[c]*1;                
            }            
        }    
    }
    return 0;
}


int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst ){       
    for(int i = 0; i < sx.rows; i++){
        for(int j = 0; j < sx.cols; j++){
            for(int c = 0; c < 3; c++){
                dst.at<cv::Vec3s>(i,j)[c] = sqrt(sx.at<cv::Vec3s>(i,j)[c]*sx.at<cv::Vec3s>(i,j)[c] + sy.at<cv::Vec3s>(i,j)[c]*sy.at<cv::Vec3s>(i,j)[c]);   
            }           
        }    
    }
    return 0;
}

int blurQuantize( cv::Mat &src, cv::Mat &dst, int levels ){    
    int b = 255 / levels;
    unsigned char x, xt, xf;

    cv::Mat tmp;
    src.copyTo(tmp);

    blur5x5(src, tmp);

    for(int i = 0; i < dst.rows; i++){
        for(int j = 0; j < dst.cols; j++){
            for(int c = 0; c < 3; c++){
                x = tmp.at<cv::Vec3b>(i,j)[c];
                xt = x / b;
                xf = xt * b;
                dst.at<cv::Vec3b>(i,j)[c] = xf;

            }           
        }    
    }  
    return 0;

}

int cartoon(cv::Mat &src, cv::Mat &dst, int levels, int magThreshold ){
    cv::Mat3s sx, sy, temp_mag;
    cv::Mat mag, bq, tmp;

    src.copyTo(sx);
    src.copyTo(sy);
    

    sobelX3x3(src, sx);
    sobelY3x3(src, sy);
    

    src.copyTo(temp_mag);
    magnitude(sx, sy, temp_mag);
    convertScaleAbs(temp_mag, mag);

    src.copyTo(bq);

    blurQuantize(src, bq, levels);

    bq.copyTo(tmp);

    for(int i = 0; i < bq.rows; i++){
        for(int j = 0; j < bq.cols; j++){
            for(int c = 0; c < 3; c++){                
                cv::Vec3b intensity = mag.at<cv::Vec3b>(i,j);
                int px = intensity.val[c];
                (px > magThreshold) ? dst.at<cv::Vec3b>(i,j)[c] = 0 : dst.at<cv::Vec3b>(i,j)[c] = tmp.at<cv::Vec3b>(i,j)[c];              
            }
        }    
    }
    return 0;
}

int negative(const cv::Mat &src, cv::Mat &dst){

    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            for(int c = 0; c < 3; c++){                
                dst.at<cv::Vec3b>(i,j)[c] = 255 - src.at<cv::Vec3b>(i,j)[c];                              
            }
        }    
    }
    return 0;
}

int brightness(cv::Mat &src, cv::Mat &dst, int b) {
    
    src.convertTo(dst, -1, 1, b*10);

    return 0;
   
}





