/*
 * Company:    AW
 * Author:     Penng
 * Date:    2022/09/23
 */
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <sys/time.h>

// #include "label.h"

using namespace std;

int run(const char *imagepath, float *output)
{
	cv::Mat bgr= cv::imread(imagepath, 1);
	if (bgr.empty())
	{
		fprintf(stderr, "cv::imread %s failed\n", imagepath);
		return -1;
	}
	
	// float _y=0.01;
	float _scale = 4.745981907844543f;	//can modify
	int size = 416 * 128;
	float *depth=output;

    // for (int i=0; i<416*128; i++)
    // {
    //     depth[i] = 1.0/depth[i];
    // }
	for (int i=0; i<size; i++)
	{
        if (i < 20)
			printf("[%d]:%f\n" ,i,depth[i]);
		// depth[i] =depth[i] * _scale + _y;
		depth[i] *=_scale;
		depth[i] = 1.0f / depth[i];
		
		// if (i < 20)
		// 	printf("[%d]:%f\n" ,i,depth[i]);
		
		if (depth[i] < 0.0f)
			depth[i] = 0.0f;
		else if (depth[i] > 1.0f)
			depth[i] = 1.0f;
		else
			;
	}
	uint8_t disp_buff[416 * 128] = {0};
	// FILE *fp=fopen("./depth_chip.txt","w+");
	for (int i=0; i<size; i++)
	{
		disp_buff[i] = depth[i] * 255;
	// 	fprintf(fp,"%.16f \n",disp_buff[i]);		
		
		if (i < 20)
			printf("[%d]: %f, %d. \n", i, depth[i], disp_buff[i]);
	}


	cv::Mat im_gray(128, 416, CV_8UC1, disp_buff);

	cv::imwrite("./disp_gray.jpg", im_gray);


	cv::Mat im_color;
	cv::applyColorMap(im_gray, im_color, cv::COLORMAP_PLASMA);
	cv::imwrite("./disp_color.jpg", im_color);	//rgb
	
	cv::cvtColor(im_color, im_color, cv::COLOR_RGB2BGR);
	
	//与原图拼接展示
	cv::Mat bgr_resize;
	cv::resize(bgr, bgr_resize, cv::Size(416, 128));

	cv::Mat im_show(128*2, 416, CV_8UC3);	
	memcpy(im_show.data, bgr_resize.data, 416*128*3);
	memcpy(im_show.data + 416*128*3, im_color.data, 416*128*3);

	cv::imwrite("./disp_show.jpg", im_show);
	
	return 0;
}

// 假设 output 是一个 float** 类型的二维数组，rows 和 cols 分别为行数和列数
float* convert2dTo1d(float **output, int rows, int cols) {
   // 计算一维数组所需的空间大小，并分配空间
    float* depth = new float[rows * cols];
    // 使用 memcpy 函数进行数据复制
    memcpy(depth, *output, rows * cols * sizeof(float));
    return depth; // 返回一维数组的指针
}

int class_postprocess(const char *imagepath, float **output)
{
	printf("class_postprocess.cpp run. \n");

	// cv::Mat m = cv::imread(imagepath, 1);
	// if (m.empty())
	// {
	// 	fprintf(stderr, "cv::imread %s failed\n", imagepath);
	// 	return -1;
	// }

    //float* depth = convert2dTo1d(output, sizeof(output[0]), sizeof(output) / sizeof(output[0]));

	run(imagepath, output[3]);

    // 释放内存
    // delete[] depth;

    return 0;
}

