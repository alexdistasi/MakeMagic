#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QRubberBand>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QTimer>
#include "image.h"

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void displayFrame();


    void on_hsvSlider_valueChanged(int value);

    void on_sHisto_clicked();

private:
    Ui::MainWindow *ui;
    Image *cam,*cam2,*green;
    Image *sobel,*old,*dif,*mask,*highlight;
    Image *rgbHist,*hsvHist,*rgbOut,*hsvOut;
    bool histExist,capPause;
    QPoint origin;
    QRubberBand *rubberBand;
    int xl,yl,xr,yr;
    int xOrigin,yOrigin,xTemp,yTemp;
    int rgbThreshold,hsvThreshold;
    Image* history;
};








#endif // MAINWINDOW_H
