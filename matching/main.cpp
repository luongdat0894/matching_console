#include <stdio.h>
#include <iostream>
#include <fstream>
#include <QDir>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QRect>
#include <QIODevice>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

char *fullname; //duong dan file anh
double x,y,w,h; //cac toa do xen anh
Mat image1; //doc anh tu file
Mat imrect; //anh da xen
int minHessian = 400;
SurfFeatureDetector detector( minHessian );
SurfDescriptorExtractor extractor;

void folder();
void readfile();
void cropimage();
void feature_imgcrop();
void feature_dataset();

void demo();

int main()
{
    folder();
    readfile();
    cropimage();
    feature_imgcrop();
    feature_dataset();

    //    demo();
    waitKey(0);
    return 0;
}

//Xoa va khoi tao nhung thu muc can thiet
void folder()
{
    QDir dir("../dataset_feature");
    QDir dir1("../data");
    QDir dir2("../output");
    dir.removeRecursively();
    dir1.removeRecursively();
    dir2.removeRecursively();
    dir.mkpath("../dataset_feature");
    dir1.mkpath("../data");
    dir2.mkpath("../output");
}

//Doc file de bai
void readfile()
{
    QString filename, x1, y1, w1, h1;
    QFile inputfile("../input/input.txt");
    inputfile.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream in(&inputfile);

    if(!inputfile.isOpen())
    {
        cout<<"File not found!"<<endl;
    }
    else
    {
        in>>filename;
        in>>dec>>x1>>y1>>w1>>h1;

        const char *name = filename.toLocal8Bit().data();
        x = x1.toDouble();
        y = y1.toDouble();
        w = w1.toDouble();
        h = h1.toDouble();

        fullname = new char[30];
        sprintf(fullname,"../dataset/%s.jpg",name);
        cout<<"Dia chi anh can tim kiem: "<<fullname<<endl;
        cout<<"Toa do xen anh: x = "<<x<<", y = "<<y<<", width = "<<w<<", height = "<<h<<endl;
    }
    inputfile.close();
}

//Cat anh
void cropimage()
{
    image1 = imread(fullname, CV_LOAD_IMAGE_COLOR);

    double rowimg1 = image1.rows;
    double colimg1 = image1.cols;

    cout<<"Image Size: "<<colimg1<<" x "<<rowimg1<<endl;

    if(rowimg1<(y+h)||colimg1<(x+w))
    {
        w =colimg1-x;
        h =rowimg1-y;
    }
    imrect = image1(Rect(x,y,w,h));
    imwrite("../data/imagecroped.jpg",imrect);
    FileStorage fs("../data/imagecroped.yml",FileStorage::WRITE);
    write(fs,"Image Crop",imrect);
    fs.release();
    namedWindow("Image Source", CV_WINDOW_AUTOSIZE);
    namedWindow("Image Croped", CV_WINDOW_AUTOSIZE);
    imshow("Image Source",image1);
    imshow("Image Croped",imrect);
}

//Rut trich dac trung anh da cat
void feature_imgcrop()
{
    vector<KeyPoint> keypoints_imrect;
    detector.detect(imrect,keypoints_imrect);
    Mat descriptors_imrect;
    extractor.compute(imrect,keypoints_imrect,descriptors_imrect);


    FileStorage fs("../data/descriptors_imrect.yml",FileStorage::WRITE);
    write(fs,"descriptors_imrect",descriptors_imrect);
    fs.release();
}

//Rut trich dac trung dataset va luu vao thu muc dataset_feature
void feature_dataset()
{

}

//example2
void demo()
{
    Mat imgA, imgB;
    imgA = imread("../dataset/all_souls_000006.jpg");
    imgB = imread("../dataset/all_souls_000017.jpg");

    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( imgA, keypoints_object );
    detector.detect( imgB, keypoints_scene );

    SurfDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute( imgA, keypoints_object, descriptors_object );
    extractor.compute( imgB, keypoints_scene, descriptors_scene );

    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );
    double max_dist = 0; double min_dist = 100;
    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );

    FileStorage fs("../data/descriptors_objectyml",FileStorage::WRITE);
    write(fs,"descriptors_object",descriptors_object);
    fs.release();


    //    namedWindow("Image GrayScale", CV_WINDOW_AUTOSIZE);
    //    imshow("Image GrayScale",descriptors_object);
    //    waitKey(0);
    //    destroyWindow("Image GrayScale");
}
