#include "pch.h"
#include"ffaLine.h"

using namespace std;
using namespace cv;
void CalculateDirectionVector(pixel* PixelSet,float &VectorMeans, float &VectorStandardDeviation, Mat source,int distance)
{
	int count = 0; int sum = 0;
	int GrayValue[30];
	for(int i=0;i< (distance-1)*2;i++)
	{
		sum += PixelSet[i].grayValue;
		count++;
	}
	VectorMeans = (float)(sum / count);
	sum = 0;
	for (int i = 0; i < (distance - 1) * 2; i++)
	{
		sum += (PixelSet[i].grayValue - VectorMeans)*(PixelSet[i].grayValue - VectorMeans);
	}
	VectorStandardDeviation = sqrt(sum / count);
}
float Calculate_P_Value(float TotalMeans, float TotalStandardDeviation,float VectorMeans, float VectorStandardDeviation)
{
	float D, Delta;
	D = sqrt(TotalMeans*TotalMeans + TotalStandardDeviation * TotalStandardDeviation);
	Delta = sqrt((TotalMeans - VectorMeans)*(TotalMeans - VectorMeans)
		+ (TotalStandardDeviation - VectorStandardDeviation)
		* (TotalStandardDeviation - VectorStandardDeviation));
	float ReturnValue = (abs(D - Delta) / D);
	return ReturnValue;
}
float CalculateTotalMeans(Mat pic) {
	int row = pic.rows;
	int col = pic.cols;
	float means = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++)
		{
			int grayvalue = (int)pic.at<uchar>(i, j);
			means += grayvalue;
		}
	}
	return means / (row*col);
}
float CalculateStandardDeviation(float means, Mat pic) {
	float sum = 0;
	for (int i = 0; i < pic.rows; i++) {
		for (int j = 0; j < pic.cols; j++) {
			float temp = (float)((int)pic.at<uchar>(i, j)) - means;
			sum = sum + (temp*temp);
		}
		return sqrt(sum / (pic.rows*pic.cols));
	}
}
int Projection01to0255(float cta) {
	int grey = 0;
	grey = (int)(cta * 255);
	return grey;
}
int thresholdPic(Mat pic) {
	int outValue = 110;
	return outValue;
}
int** segmentToMatrix(Mat pic) {
	int** segMatrix = new int* [pic.rows];
	for (int i = 0; i < pic.rows; i++)
		segMatrix[i] = new int[pic.cols];
	int threshold = thresholdPic(pic);
	for (int i = 0; i < pic.rows;i++) 
		for(int j=0;j<pic.cols;j++)
	{
			int pixelGrayValue = pic.at<uchar>(i, j);
			if (pixelGrayValue < threshold) {//小于阈值认为是裂缝区
				segMatrix[i][j] = 1;//开启线扫描
			}
			else {
				segMatrix[i][j] = 0;//关闭先扫描
			}
	}
	return segMatrix;
}
Mat CTA(Mat source, int distance, float TotalMeans, float TotalStandardDeviation)
{
	int highdensity = 1; int lowdensity = distance;
	//cvtColor(source, GrayScaleMap, COLOR_RGB2GRAY);
	Mat outputTemp = Mat(source.rows + 2 * distance, source.cols + 2 * distance, CV_8U);
	Mat GrayScaleMap = Mat(source.rows+2*distance, source.cols + 2*distance, CV_8U);
	for (int row = 0; row < GrayScaleMap.rows - 0; row++)  // GrayScaleMap.rows-distance
		for (int col = 0; col < GrayScaleMap.cols - 0; col++) {
			GrayScaleMap.at<uchar>(row, col) = TotalMeans;
		}
	Rect rect(distance, distance, source.cols, source.rows);
	source.copyTo(GrayScaleMap(rect));
	int** segMatrix = segmentToMatrix(GrayScaleMap);///重复调用函数，应修改
	for (int row = 0 + distance; row < GrayScaleMap.rows - distance; row++)  // GrayScaleMap.rows-distance
		for (int col = 0 + distance; col < GrayScaleMap.cols - distance; col++)
		{
			int density = 1;
				int Switch=segMatrix[row][col];
			if (Switch ==1) {//开启扫描
				density = highdensity;
			}
			else {
				density = lowdensity;
			}
				pixel* PixelSet01 = new pixel[distance * 2]();
				pixel* PixelSet02 = new pixel[distance * 2]();
				pixel* PixelSet03 = new pixel[distance * 2]();
				pixel* PixelSet04 = new pixel[distance * 2]();
				float p[5];
				float VectorMeans01 = 0; float VectorStandardDeviation01 = 0;
				diagonal_135_left(GrayScaleMap, PixelSet01, row, col, distance, density);
				diagonal_135_right(GrayScaleMap, PixelSet01, row, col, distance, density);
				CalculateDirectionVector(PixelSet01, VectorMeans01, VectorStandardDeviation01, source,distance);
				p[1] = Calculate_P_Value(VectorMeans01, VectorStandardDeviation01, TotalMeans, TotalStandardDeviation);
				float VectorMeans02 = 0; float VectorStandardDeviation02 = 0;
				diagonal_045_left(GrayScaleMap, PixelSet02, row, col, distance, density);
				diagonal_045_right(GrayScaleMap,PixelSet02, row, col, distance, density);
				CalculateDirectionVector(PixelSet02, VectorMeans02, VectorStandardDeviation02, source, distance);
				p[2] = Calculate_P_Value(VectorMeans02, VectorStandardDeviation02, TotalMeans, TotalStandardDeviation);
				float VectorMeans03 = 0; float VectorStandardDeviation03 = 0;
				longitudinal_up(GrayScaleMap, PixelSet03, row, col, distance, density);
				longitudinal_down(GrayScaleMap, PixelSet03, row, col, distance, density);
				CalculateDirectionVector(PixelSet03, VectorMeans03, VectorStandardDeviation03, source, distance);
				p[3] = Calculate_P_Value(VectorMeans03, VectorStandardDeviation03, TotalMeans, TotalStandardDeviation);
				float VectorMeans04 = 0; float VectorStandardDeviation04 = 0;
				tansversal_left(GrayScaleMap, PixelSet04, row, col, distance, density);
				tansversal_right(GrayScaleMap,PixelSet04, row, col, distance, density);
				CalculateDirectionVector(PixelSet04, VectorMeans04, VectorStandardDeviation04, source, distance);
				p[4] = Calculate_P_Value(VectorMeans04, VectorStandardDeviation04, TotalMeans, TotalStandardDeviation);
				float min, max;
				std::sort(p + 1, p + 5);
				min = p[1];max = p[4];
				float cta_num =(max - min) / max;
				int gray = Projection01to0255(cta_num);
				outputTemp.at<uchar>(row, col) = gray;
			
		}
	Rect rect2(distance, distance, source.cols, source.rows);
	Mat output=outputTemp(rect2);
	return output;
}
