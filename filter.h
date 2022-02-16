
int grayscale(const cv::Mat &src, cv::Mat &dst);

int blur5x5(const cv::Mat &src, cv::Mat &dst);

int sobelX3x3(const cv::Mat &src, cv::Mat3s &dst);

int sobelY3x3(const cv::Mat &src, cv::Mat3s &dst);

int magnitude( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );

int blurQuantize( cv::Mat &src, cv::Mat &dst, int levels );

int cartoon( cv::Mat &src, cv::Mat&dst, int levels, int magThreshold );

int negative(const cv::Mat &src, cv::Mat &dst);

int brightness(cv::Mat &src, cv::Mat &dst, int b);

int orientation( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );

int lawsl5e5(const cv::Mat &src, cv::Mat3s &dst);

int histogram2d(cv::Mat &src, float histogram[], int Hsize);

int histogram3d(cv::Mat &src, float histogram[], int Hsize);

int readDir(char argv[1] , std::string path[] );

int sobelX3x3_1d(const cv::Mat &src, cv::Mat &dst);

int sobelY3x3_1d(const cv::Mat &src, cv::Mat &dst);

int magnitude_1d( cv::Mat &sx, cv::Mat &sy, cv::Mat &dst );
