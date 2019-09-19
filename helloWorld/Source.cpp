#include <stdint.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include  <conio.h>

int main(int argc, char* argv[])
{
	const char* wName = "PEN"; // window name 
	const char* wName2 = "Histogram Plot";
	const char* wName3 = "TPen";
	cvNamedWindow(wName, 0); // create simple window    
	cvNamedWindow(wName2, 0); // create simple window   
	cvNamedWindow(wName3, 0); // create simple window 

	CvCapture* capture = 0;

	double capProp = 0;

	IplImage *frame, *img, *frame_copy = 0; // pointers to images 
	capture = cvCaptureFromCAM(0); // initialize capture device 

	img = cvLoadImage("PEN.png", 1); // = cvLoadImage("ariane5_1b.jpg", 1); //

	IplImage *imgT = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	cvCvtColor(img, imgT, CV_RGB2GRAY);
	printf("img width: %d Hight: %d nSize: %d \n", img->height, img->width, img->nSize);

	cvResizeWindow(wName, img->width, img->height);
	cvMoveWindow(wName, 0, 150);
	
	cvResizeWindow(wName3, imgT->width, imgT->height);
	cvMoveWindow(wName3, 650, 150);


	uchar * data = (uchar *)img->imageData;
	int h = img->height;
	int w = img->width;
	int step = img->widthStep / sizeof(uchar);
	int color = 1; // red = 0, Green = 1,  Blue = 2 
	int c = 0 + color;
	int r = 0;

/*
	for (size_t i = 0; i < img->imageSize; i=i+3)
	{
		data[i + color]=255 - data[i + color];
		data[i + color+1] = 255- data[i + color + 1];
		data[i + color + 2] = 255 - data[i + color + 2];
	}
	img->imageData = (char *)data;
	//data[c + step * r]
*/

	unsigned char *channel, *channel2; // pre-allocated array holding image data for the color channel or the grayscale image. 
	unsigned char value=0; // index value for the histogram (not really needed) 
	int histogram[256]; // histogram array - remember to set to zero initially 
	int width; // say, 320 
	int height; // say, 240 
	channel = (uchar *)img->imageData;
	channel2 = (uchar *)imgT->imageData;

	width = w;
	height = h;
	int k = 256; 
	int t = 110;//107
	while (k-- > 0)
	{
		histogram[k] = 0; // reset histogram entry 
	}
	int maxVal = 0;
	for (int i = 0; i < width*height; i++)
	{
		value = channel[i];
		histogram[value] += 1;
		if (channel2[i] <= t)
		{
			imgT -> imageData[i] = 255;
		}
		else { imgT->imageData[i] = 0;}
		
		if (histogram[value] > maxVal)
		{
			maxVal = histogram[value];
		}
		//printf("%d ", histogram[value]);
	}
	printf("%d ", maxVal);
	CvSize histImSiz = cvSize(256, maxVal*1.01); //cvSize(width, height); 

	IplImage *histPlot = cvCreateImage(histImSiz, IPL_DEPTH_8U, img->nChannels);
	
	for (int i = 0; i < 256; i++)
	{
		int mag = histogram[i];
		cvLine(histPlot, cvPoint(i, 0), cvPoint(i, mag), cvScalar(255, 0, 0), 1);
	}

	cvLine(histPlot, cvPoint(t, 0), cvPoint(t, maxVal), cvScalar(0, 255, 0), 1);
	//cvLine(histPlot, cvPoint(100, 0), cvPoint(100, 100), cvScalar(255, 0, 0), 1, 8, CV_AA);
	/*cvLine(		histPlot, cvPoint(400, 0), cvPoint(400, 200), cvScalar(255, 0, 0), 1);
	cvRectangle(histPlot, cvPoint(10, 10), cvPoint(20, 20), cvScalar(255, 0, 0), 1);*/
	cvResizeWindow(wName2, img->width, h);
	cvMoveWindow(wName2, 250, 150);



	// center of mass
	step = imgT->widthStep / sizeof(uchar);
	double M00 = 0;
	double M01 = 0;
	double M10 = 0;
	double M11 = 0;
	double M02 = 0;
	double M20 = 0;

	int x;
	int y;

	FILE * pFile;
	int n;
	char name[100];

	pFile = fopen("myfile.txt", "w");

	for (size_t i = 0; i < width * h; i++)
	{
		if (channel2[i] >= t)
		{
			//data[c + step * r]
			x = i % step;
			y = i / (step);
			//r = i % step;
			//c = i - step * r; 
				M00++;
				M10 = M10 + x+1;
				M01 = M01 + y+1;
				M11 = M11 + x * (y+1);
				M20 = M20 + pow((x+1), 2);
				M02 = M02 + pow((y+1), 2);
				//printf("\n x= %d y = %d", x, channel2[i]);
		}
//		printf("\n %d", channel2[i]);

//			fprintf(pFile, "\n %d", channel2[i], name);
	}
	fclose(pFile);

	double xCM = M10 / M00;
	double yCM = M01 / M00;
	double xCMd = (double)M10 / (double)M00;
	double yCMd = (double)M01 / (double)M00;
	printf("double M10 =%d  M01= %d M00= %d \n", M10, M01, M00);
	double my00 = (double)M00;
	double my01 = 0;
	double my10 = 0;
	double my20 = ((double)M20 - (double)xCM * (double)M10)/M00;
	double my02 = ((double)M02 - (double)yCM * (double)M01) / M00;
	double my11 = ((double)M11 - (double)yCM * (double)M10) / M00;
	printf("my11 = %f my20-my02 %f\n", my11, (my20 - my02));
	double theta = 0.5*atan2( 2*my11, (my20 - my02)); // 0.5*atan(2 * my11/( my20 - my02));// 

	printf("\ntheta =%f theta =%f degree \n", theta, theta*180/3.14);
	//cvLine(imgT, cvPoint(xCM, yCM), cvPoint(w, yCM), cvScalar(0, 0, 255), 1);


	printf("xCM =%d yCM = %d", xCM, yCM);
	cvLine(imgT, cvPoint(xCM, 0), cvPoint(xCM, h), cvScalar(0, 0, 255), 1);
	cvLine(imgT, cvPoint(0, yCM), cvPoint(w, yCM), cvScalar(0, 0, 255), 1);


	while (1)
	{
		if (!img)
		{
			printf("no img");
			break;
		}

		cvShowImage(wName, img);
		cvShowImage(wName2, histPlot);
		cvShowImage(wName3, imgT);
		if (cvWaitKey(0))
			break;
	}

	cvReleaseImage(&frame_copy);    
	cvDestroyWindow("Hello World");
	cvReleaseImage(&histPlot);
	cvDestroyWindow(wName2);

	return 0;
}
