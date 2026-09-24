/*
 * Company:    AW
 * Author:     Penng
 * Date:    2022/09/22
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <vector>


static unsigned short fp32_to_fp16(float in)
{
    uint32_t fp32 = 0;
    uint32_t t1 = 0;
    uint32_t t2 = 0;
    uint32_t t3 = 0;
    uint32_t fp16 = 0u;

    memcpy((uint8_t*)&fp32, (uint8_t*)&in, sizeof(uint32_t));

    t1 = (fp32 & 0x80000000u) >> 16;  /* sign bit. */
    t2 = (fp32 & 0x7F800000u) >> 13;  /* Exponent bits */
    t3 = (fp32 & 0x007FE000u) >> 13;  /* Mantissa bits, no rounding */

    if(t2 >= 0x023c00u )
    {
        fp16 = t1 | 0x7BFF;     /* Don't round to infinity. */
    }
    else if(t2 <= 0x01c000u )
    {
        fp16 = t1;
    }
    else
    {
        t2 -= 0x01c000u;
        fp16 = t1 | t2 | t3;
    }

    return (unsigned short) fp16;
}

void get_input_data(const char* image_file, unsigned short* input_data, int input_h, int input_w)
{
    cv::Mat sample = cv::imread(image_file, 1);
    cv::Mat img;

    if (sample.channels() == 1)
        cv::cvtColor(sample, img, cv::COLOR_GRAY2RGB);
    else
        cv::cvtColor(sample, img, cv::COLOR_BGR2RGB);

    if ((img.rows != input_h) || (img.cols != input_w))
        cv::resize(img, img, cv::Size(input_h, input_w));

    unsigned char* img_data = img.data;


    //unsigned short *input_ptr = input_data;

   
    for (int h = 0; h < input_h; h++)
    {
        for (int w = 0; w < input_w; w++)
        {
            for (int c = 0; c < 3; c++)
            {
                int index = h * input_w * 3 + w * 3 + c;

                // input dequant
                input_data[index] = fp32_to_fp16(1.0*img_data[index]/255.f);	//uint8
//                input_data[out_index] = (int8_t)(img_data[in_index] - 128);	//pcq int8
            }
        }
    }
}


uint8_t *class_preprocess(const char* imagepath, unsigned int *file_size)
{
	printf("class_preprocess.cpp run. \n");

	int img_c = 3;
	// const float mean[3] = {255, 255, 255};
	// const float scale[3] = {0.0039216, 0.0039216, 0.0039216};

	// set default  size
	int input_h = 128;	// 128 x 416
    int input_w = 416;
	int img_size = input_h * input_w * img_c;

	*file_size = img_size * sizeof(unsigned short);

	unsigned short *tensorData = NULL;
	tensorData = (unsigned short *)malloc(1 * img_size * sizeof(unsigned short));

	get_input_data(imagepath, tensorData, input_h, input_w);

    return (uint8_t*)tensorData;
}




