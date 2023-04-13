#include <fstream>
#include <iostream>
#include <mutex>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>
#include <thread>
#include <vector>
#include <cstdlib> // rand() ve srand() fonksiyonları için gerekli
#include <ctime>   // time() fonksiyonu için gerekli

Mat image;
std::mutex imageMutex;
bool exitFlag = false;
std::vector<std::pair<int,int>> line_ ;
std::vector<std::vector<std::pair<int,int>>> base;
int random_number=0;
enum Color {
    BLACK = 0,
    WHITE = 1,
    BLUE = 2,
    GREEN = 3,
    RED = 4,
    YELLOW = 5,
    PINK = 6
};

cv::Scalar get_color(Color color) {
    switch (color) {
    case BLACK:
        return cv::Scalar::all(0);
    case WHITE:
        return cv::Scalar::all(255);
    case BLUE:
        return cv::Scalar(255, 0, 0);
    case GREEN:
        return cv::Scalar(0, 255, 0);
    case RED:
        return cv::Scalar(0, 0, 255);
    case YELLOW:
        return cv::Scalar(0, 255, 255);
    case PINK:
        return cv::Scalar(255, 0, 255);
    default:
        return cv::Scalar::all(0);
    }
}

void createYaml(std::vector<std::vector<std::pair<int,int>>> content) {
    std::string willBeLine= "%YAML:1.0\npointsOnCurves\n";
    for (auto line : content) {
        willBeLine += "   - [ ";
        for (auto point : line) {
            willBeLine+= "["+ std::to_string(point.first)+ ", " + std::to_string(point.second)+"]";
            if( point == line.back())
                willBeLine+="]\n";
            else
                willBeLine+=",";
        }
    }

    std::ofstream file;
    file.open("output.yaml");
    file << willBeLine.c_str();
    file.close();

}

void keyCaptureThread() {
    while (!exitFlag) {
        int key = cv::waitKey(1); // Wait for user input for 1ms
        if (key == 'n'){           // If the 's' key was pressed
            std::cout << "Key 'n' pressed" << std::endl;
            base.push_back(line_);
            line_.clear();
            srand(time(NULL));
            random_number = rand() % 7;
        }

        if (key == 's') {// If the 's' key was pressed
            std::cout << "Key 's' pressed" << std::endl;
            createYaml(base);
        }
    }
}

void onMouse(int event, int x, int y, int flags, void *userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        std::lock_guard<std::mutex> lock(imageMutex); // Lock mutex
        Mat *img = (Mat *)userdata;
        Vec3b pixel = img->at<Vec3b>(y, x); // Get pixel value at clicked point
        std::cout << "Pixel value at (" << x << "," << y << "): ";
        std::cout << "(" << (int)pixel[0] << "," << (int)pixel[1] << ","
                  << (int)pixel[2] << ")" << std::endl;
        line_.push_back(
                    {(int)pixel[0],
                     (int)pixel[0]});
        circle(*img, Point(x, y), 10, get_color(static_cast<Color>(random_number)), 2);
        imshow("Image", *img);
    }
}

int main() {
    image = imread("/home/samet/51.png");
    resize(image, image, Size(1536, 2048));
    if (image.empty()) {
        std::cerr << "Could not read the image" << std::endl;
        return -1;
    }
    namedWindow("Image");
    std::thread capThread(keyCaptureThread);
    capThread.detach();
    setMouseCallback("Image", onMouse, &image);
    while (true) {
        {
            std::lock_guard<std::mutex> lock(imageMutex); // Lock mutex
            imshow("Image", image);
        }
        int key = waitKey(10);
        if (key == 27) // If the 'Esc' key was pressed
        {
            exitFlag = true;
            break;
        }
    }
    return 0;
}
