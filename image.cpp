#include "image.h"
#include <iostream>

Image::Image()
{
    rPath = 0;
    gPath = 0;
    bPath = 0;

}

void Image::setCameraNum(int i){
    cam=VideoCapture(i);
}

void Image::copyMat(Mat toCopy){
    src = toCopy.clone();
}

void Image::takePicture(){
    //for(int i=0;i<5;i++)
    cam>>src;
    flip(src, src, 1);
    srcClean=src.clone();
}

QImage Image::convertToQImage(){
    cvtColor(src,display,CV_BGR2RGB);
    QImage img=QImage((const unsigned char*)(display.data), display.cols,display.rows,
                      display.step,QImage::Format_RGB888);
    return img;
}
void Image::displayImage(QLabel &location){
    QImage img=convertToQImage();
    location.setPixmap(QPixmap::fromImage(img));
}
void Image::copy(Image *imIn){
    src=imIn->src.clone();
    srcClean=src.clone();
}
void Image::sobelCV(){
    Mat src_gray;
    Mat grad;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;


    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

    /// Convert it to gray
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Generate grad_x and grad_y
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    /// Gradient X
    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    src.~Mat();
    cvtColor(grad,src,CV_GRAY2RGB);

}
void Image::invert(){
    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++){
            for(int c=0;c<3;c++)
                src.at<Vec3b>(y,x)[c]=255-src.at<Vec3b>(y,x)[c];
        }
}
void Image::difference(Image *im1, Image *im2){
    if(!src.data)
        src=Mat::zeros(im1->src.size(),im1->src.type());

    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++)
            for(int c=0;c<3;c++)
                src.at<Vec3b>(y,x)[c]=abs(im1->src.at<Vec3b>(y,x)[c]-im2->src.at<Vec3b>(y,x)[c]);

}
void Image::sobelMe(){
    int temp;
    Mat out;
    out=Mat::zeros(src.size(),src.type());
    int total;

    for(int y=1;y<src.rows-1;y++)
        for(int x=1;x<src.cols-1;x++){
            total=0;
            for(int c=0;c<3;c++){
                temp=0;
                temp+=-1*src.at<Vec3b>(y-1,x)[c];
                temp+=-1*src.at<Vec3b>(y,x-1)[c];
                temp+=-2*src.at<Vec3b>(y-1,x-1)[c];
                temp+=1*src.at<Vec3b>(y+1,x)[c];
                temp+=1*src.at<Vec3b>(y,x+1)[c];
                temp+=2*src.at<Vec3b>(y+1,x+1)[c];
                temp=abs(temp);
                temp*=8;
                if(temp>255)
                    temp=255;
                total+=temp;
                out.at<Vec3b>(y,x)[c]=temp;
            }
            /*if(total>500){
                out.at<Vec3b>(y,x)[0]=255;
                out.at<Vec3b>(y,x)[1]=255;
                out.at<Vec3b>(y,x)[2]=255;

            } else if(total>300){
                out.at<Vec3b>(y,x)[1]=255;
            } else if(total>200){
                out.at<Vec3b>(y,x)[0]=255;
                out.at<Vec3b>(y,x)[1]=255;
            }  else if(total>50){
                out.at<Vec3b>(y,x)[2]=255;
            }*/
        }
    src=out.clone();
}

void Image::gaussian(int mult){
    int temp;
    Mat out;
    out=Mat::zeros(src.size(),src.type());
    int total;

    for(int y=1;y<src.rows-1;y++)
        for(int x=1;x<src.cols-1;x++){
            total=0;
            for(int c=0;c<3;c++){
                temp=0;
                temp+=1*src.at<Vec3b>(y-1,x-1)[c];
                temp+=2*mult*src.at<Vec3b>(y-1,x)[c];
                temp+=1*src.at<Vec3b>(y-1,x+1)[c];
                temp+=2*mult*src.at<Vec3b>(y,x-1)[c];
                temp+=4*mult*src.at<Vec3b>(y,x)[c];
                temp+=2*mult*src.at<Vec3b>(y,x+1)[c];
                temp+=1*src.at<Vec3b>(y+1,x-1)[c];
                temp+=2*mult*src.at<Vec3b>(y+1,x)[c];
                temp+=1*src.at<Vec3b>(y+1,x+1)[c];
                temp/=(4+mult*12);

                out.at<Vec3b>(y,x)[c]=temp;
            }
        }
    src=out.clone();
}


void Image::differenceBinary(Image *im1, Image *im2, int threshold){
    int dif;

    if(src.data)
        src.~Mat();
    src=Mat::zeros(im1->src.size(),im1->src.type());

    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++){
            dif=0;
            for(int c=0;c<3;c++)
                dif+=abs(im1->src.at<Vec3b>(y,x)[c]-im2->src.at<Vec3b>(y,x)[c]);
            if(dif>threshold)
                for(int c=0;c<3;c++)
                    src.at<Vec3b>(y,x)[c]=255;
        }
}
void Image::highlightMask(Image *mask){
    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++)
            if(mask->src.at<Vec3b>(y,x)[0]==255){
                src.at<Vec3b>(y,x)[0]=0;
                src.at<Vec3b>(y,x)[1]=0;
                src.at<Vec3b>(y,x)[2]=255;
            }
}

void Image::identifyWhite(){
    int thresh=25;

    int rgdif,rbdif,gbdif;
    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++){
            rgdif=abs(src.at<Vec3b>(y,x)[0]-src.at<Vec3b>(y,x)[1]);
            rbdif=abs(src.at<Vec3b>(y,x)[1]-src.at<Vec3b>(y,x)[2]);
            gbdif=abs(src.at<Vec3b>(y,x)[2]-src.at<Vec3b>(y,x)[0]);
            if(rgdif<thresh && rbdif<thresh && gbdif<thresh && src.at<Vec3b>(y,x)[0]>140){
                for(int c=0;c<3;c++){
                    src.at<Vec3b>(y,x)[c]=0;
                }
            } else {
                for(int c=0;c<3;c++){
                    src.at<Vec3b>(y,x)[c]=255;
                }
            }

        }
}
void Image::repaintMast(Image *cam, Image *mask){
    for(int y=0;y<src.rows;y++)
        for(int x=0;x<src.cols;x++){
           if(mask->src.at<Vec3b>(y,x)[0]==255)
               for(int c=0;c<3;c++){
                   src.at<Vec3b>(y,x)[c]=cam->src.at<Vec3b>(y,x)[c];
            }
        }
}

void Image::createHist(int bucketsIn){
    buckets=bucketsIn;
    hist=new long*[buckets];
    for(int i=0;i<buckets;i++){
        hist[i]=new long[buckets];
        for(int j=0;j<buckets;j++)
            hist[i][j]=0;
    }
}
void Image::clearHist(){
    for(int i=0;i<buckets;i++){
        for(int j=0;j<buckets;j++)
            hist[i][j]=0;
    }
}

void Image::addHSVfromFile(Mat histFeed){
    int H;     int S;     int V;     int h;     int s;
   // this->copyMat(histFeed);
   // this->invert();
   // histFeed = src.clone();
   // cvtColor(histFeed, histFeed, CV_BGR2HSV);

    for(int x = 0;x < histFeed.cols;x++){
        for(int y = 0;y < histFeed.rows;y++){
            int R = 255 - histFeed.at<Vec3b>(y,x)[0];
            int G = 255 - histFeed.at<Vec3b>(y,x)[1];
            int B = 255 - histFeed.at<Vec3b>(y,x)[2];

            hist[x][y]+= R;
        }
    }

}

void Image::addHSV(Image *cam, int xl, int xr, int yl, int yr){
    Mat temp;

    cvtColor(cam->srcClean,temp,CV_BGR2HSV);
    int H,S,V,h,s;

    for(int x=xl;x<xr;x++)
        for(int y=yl;y<yr;y++){
            H=temp.at<Vec3b>(y,x)[0];
            S=temp.at<Vec3b>(y,x)[1];
            V=temp.at<Vec3b>(y,x)[2];

            h=H*buckets/180;
            s=S*buckets/256;

            hist[h][s]++;
        }
}

void Image::histToSrc(){
    if(!src.data)
        src=Mat::zeros(buckets,buckets,CV_8UC3);

    int max=0;
    for(int x=0;x<buckets;x++)
        for(int y=0;y<buckets;y++){
            if(max<hist[x][y])
                max=hist[x][y];
        }
    if (max==0)
        max=1;
    for(int x=0;x<buckets;x++)
        for(int y=0;y<buckets;y++){
            src.at<Vec3b>(y,x)[0]=255-(255*hist[x][y]/max);
            src.at<Vec3b>(y,x)[1]=255-(255*hist[x][y]/max);
            src.at<Vec3b>(y,x)[2]=255-(255*hist[x][y]/max);
        }
}

void Image::colorHistHSV(Image *histIm, int thresh, Image *history){
    Mat temp;

    if(!path2Show.data)
        path2Show=Mat::zeros(src.size(),CV_8UC3);

    cvtColor(src,temp,CV_BGR2HSV);
    int H,S,V,h,s;

    buckets=histIm->buckets;
    int numGestures = 0;
    for(int x=0;x<temp.cols;x++)
        for(int y=0;y<temp.rows;y++){
            H=temp.at<Vec3b>(y,x)[0];
            S=temp.at<Vec3b>(y,x)[1];
            V=temp.at<Vec3b>(y,x)[2];

            h=H*buckets/179;
            s=S*buckets/255;

            if(histIm->src.at<Vec3b>(s,h)[0]<thresh){
                history->src.at<Vec3b>(y,x)[0]=255;
                history->src.at<Vec3b>(y,x)[1]=255;
                history->src.at<Vec3b>(y,x)[2]=255;
                path2Show.at<Vec3b>(y,x)[0] = 255;
                path2Show.at<Vec3b>(y,x)[1] = 255;
                path2Show.at<Vec3b>(y,x)[2] = 255;

            }

        }

}

void Image::paintScreen(Image *histIm, int thresh, Image *history){
    Mat temp;

    if(!path2Show.data)
        path2Show=Mat::zeros(src.size(),CV_8UC3);

    cvtColor(src,temp,CV_BGR2HSV);
    int H,S,V,h,s;

    buckets=histIm->buckets;
    int numGestures = 0;
    for(int x=0;x<temp.cols;x++){
        for(int y=0;y<temp.rows;y++){
            H=temp.at<Vec3b>(y,x)[0];
            S=temp.at<Vec3b>(y,x)[1];
            V=temp.at<Vec3b>(y,x)[2];

            h=H*buckets/179;
            s=S*buckets/255;

            if(histIm->src.at<Vec3b>(s,h)[0]<thresh){
                history->src.at<Vec3b>(y,x)[0]=bPath;
                history->src.at<Vec3b>(y,x)[1]=gPath;
                history->src.at<Vec3b>(y,x)[2]=rPath;
                bool hit = false;
                hit = collisionCheck(x, y, history);
                path2Show.at<Vec3b>(y,x)[0] = 255;
                path2Show.at<Vec3b>(y,x)[1] = 255;
                path2Show.at<Vec3b>(y,x)[2] = 255;
                src.at<Vec3b>(y,x)[0] = 255;
                src.at<Vec3b>(y,x)[1] = 0;
                src.at<Vec3b>(y,x)[2] = 0;





            }
        }
    }

}

bool Image::hasHistory(){
    int totUsed = 0;
    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){
            if(src.at<Vec3b>(y,x)[1] > 33){
                totUsed++;
            }
        }
    }
    if (totUsed > 200){
        return true;
    }
    return false;
}

void Image::checkPath(){
    if(this->hasHistory()){
       // Mat path2Check = this->prepPath();
       // savePath(path2Check);
        //compare against things
    }
}
bool hitShown1 = false;
bool hitShown2 = false;
bool hitShown3 = false;
bool hitShown4 = false;
bool hitShown5 = false;
bool hitShown6 = false;
bool hitShown7 = false;
bool hitShown8 = false;


bool hit1 = false;
bool hit2 = false;
bool hit3 = false;
bool hit4 = false;
bool hit5 = false;
bool hit6 = false;
bool hit7 = false;
bool hit8 = false;

void Image::showPath(Image *history){
    bool hit = false;

    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){

            //change to blue if the movement path has something there
            if(path2Show.at<Vec3b>(y,x)[0] == 255 && path2Show.at<Vec3b>(y,x)[1] == 255 && path2Show.at<Vec3b>(y,x)[2] == 255){
                hit = collisionCheck(x, y, history);

                src.at<Vec3b>(y,x)[0] = 255;
                src.at<Vec3b>(y,x)[1] = 0;
                src.at<Vec3b>(y,x)[2] = 255;

            }

        }

    }
    hitShown1 = false;
    hitShown2 = false;
    hitShown3 = false;
    hitShown4 = false;
    hitShown5 = false;
    hitShown6 = false;
    hitShown7 = false;
    hitShown8 = false;



    hit1 = false;
    hit2 = false;
    hit3 = false;
    hit4 = false;
    hit5 = false;
    hit6 = false;
    hit7 = false;
    hit8 = false;


    //do the blur

    GaussianBlur(path2Show, path2Show, Size(3,3), 5, 5, 0);
    Mat element = getStructuringElement( MORPH_CROSS, Size(9, 9 ), Point( 3, 3 ) );
    erode(path2Show, path2Show, element);


}

//should be called on a Copy of history or should return a copy of the src that has been modified, should not be applied to the history @ all times
//use when searching for shapes, nothing morei
Mat Image::prepPath(){
     Mat pathMat;
     this->gaussian(1);
     Mat element = getStructuringElement( MORPH_CROSS, Size(9, 9 ), Point( 3, 3 ) );
     dilate(src, pathMat, element);
     return pathMat;
}

void Image::clearHistory(){
    //sets the history trail back to an empty black mat
    for(int y = 0; y < src.rows; y++){
        for(int x = 0; x < src.cols; x++){
            for(int c = 0; c < 3; c++){
                src.at<Vec3b>(y,x)[c] = 0;
            }
        }
    }
}



void Image::savePath(Mat path2Save){
    time_t t = time(0);
    std::string fName;    //let's use the current time, t, as our file name.... stringstream will help with conversions
    std::stringstream timeStream;
    timeStream << t << ".png";
    fName = timeStream.str();
    Mat toFile = path2Save.clone();
    imwrite(fName, toFile);

}


void Image::saveHist(){
    cv::FileStorage fsHistos("histogram.xml", cv::FileStorage::WRITE);
    if(fsHistos.isOpened() == false){
        std::cout << "error, unable to open histograms \n";

    }else{
    this->histToSrc();
        fsHistos << "histogram" << src;
        fsHistos.release();
    }
}



bool Image::collisionCheck(int x, int y, Image *history){
    bool collision = false;
    for (int i=0; i<=numSq; i++){
       if (yStartLs[i] <= y && y<= yEndLs[i] && xStartLs[i] <= x && x<= xEndLs[i]){
           collision = true;
           hitCheck(xStartLs[i], xEndLs[i], yStartLs[i], yEndLs[i], history);

       }
    }

    return collision;
}




void Image::hitCheck(int xS, int xE, int yS, int yE, Image *history){

    if (yStartLs[0] == yS && yE == yEndLs[0] && xStartLs[0] == xS && xE == xEndLs[0]){
        hit1 = true;
        rPath=255; gPath=0; bPath=0;

    }
    if (yStartLs[1] == yS && yE == yEndLs[1] && xStartLs[1] == xS && xE == xEndLs[1]){
        rPath=0; gPath=255; bPath=0;

        hit2 =true;
    }
    if (yStartLs[2] == yS && yE == yEndLs[2] && xStartLs[2] == xS && xE == xEndLs[2]){
        hit3 = true;
        rPath=0; gPath=0; bPath=255;

    }
    if (yStartLs[3] == yS && yE == yEndLs[3] && xStartLs[3] == xS && xE == xEndLs[3]){
        hit4 = true;
        rPath=255; gPath=128; bPath=0;

    }
    if (yStartLs[4] == yS && yE == yEndLs[4] && xStartLs[4] == xS && xE == xEndLs[4]){
        hit5 = true;
        rPath=250; gPath=0; bPath=250;

    }
    if (yStartLs[5] == yS && yE == yEndLs[5] && xStartLs[5] == xS && xE == xEndLs[5]){
        hit6 = true;
        rPath=250; gPath=250; bPath=224;


    }
    if (yStartLs[6] == yS && yE == yEndLs[6] && xStartLs[6] == xS && xE == xEndLs[6]){
        hit7 = true;

    }
    if (yStartLs[7] == yS && yE == yEndLs[7] && xStartLs[7] == xS && xE == xEndLs[7]){
        hit8 = true;

    }


    if(hit1 == true && hitShown1 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown1 = true;
    }
    if(hit2 == true && hitShown2 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown2 = true;
    }
    if(hit3 == true && hitShown3 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown3 = true;
    }
    if(hit4 == true && hitShown4 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown4 = true;
    }
    if(hit5 == true && hitShown5 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown5 = true;

    }
    if(hit6 == true && hitShown6 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown6 = true;
    }
    if(hit7 == true && hitShown7 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown7 = true;
        history->clearHistory();
        rPath = 0;
        gPath = 0;
        bPath = 0;
    }
    if(hit8 == true && hitShown8 == false){
        drawRect2(xS, xE, yS, yE);
        hitShown8 = true;
        //savePath(history->src);
    }


}

void Image::rectList(){

    xStart = 10;
    yStart = 10;
    xEnd = xStart + 50;
    yEnd = yStart + 50;

    //draws squares for choosing colors
    for (int i=0; i<numSq; i++){
        drawRect(xStart, xEnd, yStart, yEnd);
        xStartLs[i]=xStart;
        xEndLs[i] = xEnd;
        yStartLs[i] = yStart;
        yEndLs[i] = yEnd;
        xStart += 110;
        yStart += 0;
        xEnd += 110;
        yEnd += 0;

    }
    //draws save and discard squares
    xStart =20;
    xEnd = xStart + 50;
    yStart += 400;
    yEnd = yStart + 50;
    xStartLs[numSq-2] = xStart;
    xEndLs[numSq-2] = xEnd;
    yStartLs[numSq-2] = yStart;
    yEndLs[numSq-2] = yEnd;
    drawRect(xStartLs[numSq-2], xEndLs[numSq-2], yStartLs[numSq-2], yEndLs[numSq-2]);


    xStart += 550;
    xEnd = xStart + 50;
    xStartLs[numSq-1] = xStart;
    xEndLs[numSq-1] = xEnd;
    yStartLs[numSq-1] = yStart;
    yEndLs[numSq-1] = yEnd;
    drawRect(xStartLs[numSq-1], xEndLs[numSq-1], yStartLs[numSq-1], yEndLs[numSq-1]);






}

void Image:: drawRect(int xS, int xE, int yS, int yE){
    int rCol=255; int bCol = 255; int gCol=255;

    if (yStartLs[0] == yS && yE == yEndLs[0] && xStartLs[0] == xS && xE == xEndLs[0]){
        rCol=255; gCol=0; bCol=0;

    }
    if (yStartLs[1] == yS && yE == yEndLs[1] && xStartLs[1] == xS && xE == xEndLs[1]){
        rCol=0; gCol=255; bCol=0;
    }
    if (yStartLs[2] == yS && yE == yEndLs[2] && xStartLs[2] == xS && xE == xEndLs[2]){
        rCol=0; gCol=0; bCol=255;

    }
    if (yStartLs[3] == yS && yE == yEndLs[3] && xStartLs[3] == xS && xE == xEndLs[3]){
        rCol=255; gCol=128; bCol=0;

    }
    if (yStartLs[4] == yS && yE == yEndLs[4] && xStartLs[4] == xS && xE == xEndLs[4]){
        rCol=250; gCol=0; bCol=250;

    }
    if (yStartLs[5] == yS && yE == yEndLs[5] && xStartLs[5] == xS && xE == xEndLs[5]){
        rCol=255; gCol=255; bCol=224;

    }
    if (yStartLs[6] == yS && yE == yEndLs[6] && xStartLs[6] == xS && xE == xEndLs[6]){
        rCol=148; gCol=15; bCol=17;

    }
    if (yStartLs[7] == yS && yE == yEndLs[7] && xStartLs[7] == xS && xE == xEndLs[7]){
        rCol=17; gCol=106; bCol=6;

    }



    rectangle(src, Point( xS + 2, yS + 2), Point( xE-2, yE-2), Scalar( bCol, gCol, rCol ), -1, 8 );
}

void Image:: drawRect2(int xS, int xE, int yS, int yE){
    rectangle(src, Point( xS-3, yS-3 ), Point( xE+3, yE+3), Scalar( 255, 255, 255 ), -1, 8 );
}



















