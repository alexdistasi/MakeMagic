#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cam=new Image();
    cam->setCameraNum(0);
    cam->takePicture();
    cam2=new Image();
    cam2->copy(cam);
    old = new Image();
    history = new Image();
    history->copy(cam);
    history->clearHistory();
    hsvHist=new Image();
    int numBuckets=50;
    hsvHist->createHist(numBuckets);

    hsvOut=new Image();

    Mat histReadIn;

    FileStorage fsHistIn("histogram.xml", FileStorage::READ);
    if(fsHistIn.isOpened() == false){
        std::cout << "Error reading in histogram." << std::endl;
        //cry
    }
    fsHistIn["histogram"] >> histReadIn;
    fsHistIn.release();

    hsvHist->addHSVfromFile(histReadIn);
    hsvHist->histToSrc();
    histExist = true;

    QTimer *qTimer= new QTimer(this);
    connect(qTimer, SIGNAL(timeout()),this,SLOT(displayFrame()));
    qTimer->start(5);

    ui->imDisplay5->setScaledContents(true);
    ui->imDisplay6->setScaledContents(true);

    //histExist=false;
    capPause=false;
    hsvThreshold=235;


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::displayFrame(){
    cam->takePicture();

    if(histExist){
        old->copy(cam);
        //old->colorHistHSV(hsvHist,hsvThreshold, history);
        old->paintScreen(hsvHist,hsvThreshold, history);
        old->showPath(history);
        old->rectList();
        history->displayImage(*ui->imDisplay5);
        old->displayImage(*ui->imDisplay6);
    }

}


void MainWindow::on_hsvSlider_valueChanged(int value)
{
    hsvThreshold=255-value;
    ui->hsvThresh->setText(QString("Threshold Value: %1").arg(value));
}


void MainWindow::on_sHisto_clicked()
{
    hsvHist->saveHist();
}
