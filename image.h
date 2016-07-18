#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QLabel>
#include <QMainWindow>
#include <QtCore>
#include <math.h>

using namespace cv;

class Image
{
public:
    Image();
    void setCameraNum(int i);
    void takePicture();
    QImage convertToQImage();
    void displayImage(QLabel& location);
    void copy(Image *imIn);
    void copyMat(Mat toCopy);
    void sobelCV();
    void sobelMe();
    void gaussian(int mult);
    void invert();
    void difference(Image *im1,Image *im2);
    void differenceBinary(Image *im1,Image *im2, int threshold);
    void highlightMask(Image *mask);
    void identifyWhite();
    void repaintMast(Image *cam,Image *mask);

    void createHist(int bucketsIn);
    void clearHist();
    void addHSV(Image *cam, int xl, int xr, int yl, int yr);
    void addHSVfromFile(Mat histIn);
    void histToSrc();
    void colorHistHSV(Image *histIm, int thresh, Image *history);
    //Alex adds
    void paintScreen(Image *histIm, int thresh, Image *history);
    bool collisionCheck(int x, int y, Image *history);
    void rectList();
    void onHit(Image *history);
    void drawRect(int xS, int xE, int yS, int yE);
    void drawRect2(int xS, int xE, int yS, int yE);
    void hitCheck(int xS, int xE, int yS, int yE, Image *history);
    //end alex adds
    void savePath(Mat path2Save);   ///******** new as of Thursday
    bool hasHistory();
    void showPath(Image *history);
    void clearHistory();
    Mat prepPath();
    void checkPath();
    bool pixelUsed = false;
    Mat src;
    Mat path2Show;
    void saveHist();

    //Mat src;
    Mat srcClean;
    Mat display;
    VideoCapture cam;
    long **hist;
    int buckets;

    //alex adds more
    int xStart;
    int xEnd;
    int yStart;
    int yEnd;
    bool collision = false;
    int numSq = 8;
    int xStartLs[8];
    int yStartLs[8];
    int xEndLs[8];
    int yEndLs[8];
    int rPath;
    int gPath;
    int bPath;
    bool canClear = true;
    //end alex adds more
};

#endif // IMAGE_H
