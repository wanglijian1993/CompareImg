#include <opencv2/opencv.hpp>
#include<opencv2/imgproc/types_c.h>
#include <string.h>
#include<iostream>  
#include<vector>  
#include <bitset>
using namespace cv;
using namespace std;
string img1 = "C:/Users/wanglj/Desktop/imgs/kele1.jpg";
string img2 = "C:/Users/wanglj/Desktop/imgs/kele2.jpg";
std::string binaryToHex(string& binaryStr) {
    std::bitset<8> bits;
    std::stringstream hexStream;

    // 处理二进制字符串，每8位转为一个十六进制字符
    for (size_t i = 0; i < binaryStr.size(); i += 8) {
        bits = std::bitset<8>(binaryStr.substr(i, 8));
        hexStream << std::hex << bits.to_ulong();
    }

    return hexStream.str();
}

Mat readAndResizeImgage(const string& imagePath, int w, int h) {

    Mat srcImg = imread(imagePath);

    if (srcImg.empty()) {
        printf("无法获取图像");
        return Mat(0, 0, CV_8UC1);
    }
    Mat resizedImage;
    resize(srcImg, resizedImage, Size(w, h));
    return resizedImage;
}
string calculateBinaryHash(const Mat* image, double threshold, int rows, int cols) {
    string binaryHash;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int pixelValue =(*image).at<int>(i, j);
            binaryHash += (pixelValue >= threshold) ? '1' : '0';
        }
    }

    return binaryHash;
}
string calculateBinaryHashNew(const Mat* image, double threshold, int rows, int cols) {
    string binaryHash;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            uchar pixelValue = (*image).at<uchar>(i, j);
            int result = static_cast<int>(pixelValue - threshold);
            binaryHash += (result >=0) ? '1' : '0';
        }
    }

    return binaryHash;
}
double calculateSimilarity(const string& hash1, const string& hash2) {
    int equalBits = 0;

    for (size_t i = 0; i < hash1.size(); i++) {
        if (hash1[i] == hash2[i]) {
            equalBits++;
        }
    }

    return (static_cast<double>(equalBits) / hash1.size()) * 100;
}
/*
 均值哈希算法
*/
void aHash() {
    Mat srcOpImg = imread(img1);
    Mat destOptImg = imread(img2);
    resize(srcOpImg, srcOpImg, Size(8, 8));
    resize(destOptImg, destOptImg, Size(8, 8));

    cvtColor(srcOpImg, srcOpImg, CV_RGBA2GRAY);
    cvtColor(destOptImg, destOptImg, CV_RGBA2GRAY);

    double threshold1 = mean(srcOpImg)[0];
    double threshold2 = mean(destOptImg)[0];
    cout << "平均值1:" << threshold1 << endl;
    cout << "平均值2:" << threshold2 << endl;
    string hash1 = calculateBinaryHashNew(&srcOpImg, threshold1, srcOpImg.rows, srcOpImg.cols);
    string hash2 = calculateBinaryHashNew(&destOptImg, threshold2, destOptImg.rows, destOptImg.cols);
    string hexString1 = binaryToHex(hash1);
    string hexString2 = binaryToHex(hash2);
    cout << "2进制1:" << hash1 << endl;
    cout << "2进制2:" << hash2 << endl;
    cout << "16进制1:" << hexString1 << endl;
    cout << "16进制2:" << hexString2 << endl;

    double similarity = calculateSimilarity(hash1, hash2);

    cout << "相似度数量:" << similarity << "%" << endl;
}

double meanPHash(Mat& mat) {
    double count = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int pixelValue = mat.at<int>(i, j);
            count += pixelValue;
        }
    }
    return count;
}
/*
  感知算法
 */
void pHash() {
    Mat srcOpImg = readAndResizeImgage(img1, 64, 64);
    Mat destOptImg = readAndResizeImgage(img2, 64, 64);

    cvtColor(srcOpImg, srcOpImg, CV_RGBA2GRAY);
    cvtColor(destOptImg, destOptImg, CV_RGBA2GRAY);
    srcOpImg.convertTo(srcOpImg, CV_32F);
    destOptImg.convertTo(destOptImg, CV_32F);
    dct(srcOpImg, srcOpImg);
    dct(destOptImg, destOptImg);
    double count1 = 0;
    double count2 = 0;
    count1 = meanPHash(srcOpImg);
    count2 = meanPHash(destOptImg);
    double threshold1 = count1 / 64;
    double threshold2 = count2 /64;
    string hash1;
    string hash2;
    hash1 = calculateBinaryHash(&srcOpImg, threshold1, 8, 8);
    hash2 = calculateBinaryHash(&destOptImg, threshold2, 8, 8);
    string hexString1 = binaryToHex(hash1);
    string hexString2 = binaryToHex(hash2);
    double similarity = calculateSimilarity(hash1, hash2);

    cout << "2进制1:" << hash1 << endl;
    cout << "2进制2:" << hash2 << endl;
    cout << "16进制1:" << hexString1 << endl;
    cout << "16进制2:" << hexString2 << endl;
    cout << "相似度数量:" << similarity << "%" << endl;
}

int PixelToBinart(Mat& mat,int values[8][8]) {
    int rows = mat.rows;
    int cols = mat.cols;
    int count=0;
    for (int i = 0; i < (rows - 1); i++) {
        for (int j = 0; j < cols; j++) {
            int8_t value = static_cast<int8_t>(mat.at<uchar>(i, j));
            int8_t nextValue = static_cast<int8_t>(mat.at<uchar>((i + 1), j));
            value = nextValue - value;
            count += value;
            values[i][j] = value;
        }
    }

    return count;

}
string calculateBinaryHash_array(int array[8][8],int threshold1) {
   string hash1;
   for (int i = 0; i < 8; i++) {
       for (int j = 0; j < 8; j++) {
       
           hash1 += (array[i][j] - threshold1 >= 0) ? '1' : '0';

       }
   }
   return hash1;
}
/*
  差异算法
*/
void dHash() {

    Mat srcOpImg = readAndResizeImgage(img1, 8, 9);
    Mat destOptImg = readAndResizeImgage(img2,8, 9);
    cvtColor(srcOpImg, srcOpImg, CV_RGBA2GRAY);
    cvtColor(destOptImg, destOptImg, CV_RGBA2GRAY);
    int count1=0;
    int count2=0;
    int pixels1[8][8];
    int pixels2[8][8];
    count1 = PixelToBinart(srcOpImg,pixels1);
    count2 = PixelToBinart(destOptImg,pixels2);
    double threshold1=0;
    double threshold2=0;
    threshold1 = count1 / 64;
    threshold2 = count2 / 64;
    string hash1;
    string hash2;


    hash1 = calculateBinaryHash_array(pixels1, threshold1);
    hash2 = calculateBinaryHash_array(pixels2, threshold2);
    string hexString1 = binaryToHex(hash1);
    string hexString2 = binaryToHex(hash2);
   double similarity = calculateSimilarity(hash1, hash2);
    cout << "2进制1:" << hash1 << endl;
    cout << "2进制2:" << hash2 << endl;
   cout << "16进制1:" << hexString1 << endl;
    cout << "16进制2:" << hexString2 << endl;
    cout << "相似度数量:" << similarity << "%" << endl;
}

int main() {

    //均值哈希算法
    aHash();
    //感知哈希算法
   
     pHash();
    //差值哈希算法
    dHash();
    waitKey();
}

