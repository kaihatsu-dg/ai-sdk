/*
 * Company:    AW
 * Author:     Penng
 * Date:    2023/01/16
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __linux__
#include <sys/time.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

#include "npulib.h"


/*-------------------------------------------
        Macros and Variables
-------------------------------------------*/

extern uint8_t *class_preprocess(const char* imagepath, unsigned int *file_size);
extern int class_postprocess(const char *imagepath, float **output);

const char *usage =
    "profiler-demo -b modle_path -i input_path -l loop_run_count \n"
    "-b modle_path:     the NBG file path.\n"
    "-i input_path:     the input file path.\n"
    "-l loop_run_count: the number of loop run network.\n"
    "-h : help\n"
    "example: profiler-demo -b model.nb -i input.tensor -l 10 -m 20 \n";

enum time_idx_e {
    NPU_INIT = 0,
    NETWORK_CREATE,
    NETWORK_PREPARE,
    NETWORK_RUN,
    NETWORK_LOOP,
    TIME_IDX_MAX = 9
};


#if defined(__linux__)
#define TIME_SLOTS   10
static uint64_t time_begin[TIME_SLOTS];
static uint64_t time_end[TIME_SLOTS];
static uint64_t GetTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (uint64_t)(time.tv_usec + time.tv_sec * 1000000);
}

static void TimeBegin(int id)
{
    time_begin[id] = GetTime();
}

static void TimeEnd(int id)
{
    time_end[id] = GetTime();
}

static uint64_t TimeGet(int id)
{
    return time_end[id] - time_begin[id];
}
#endif



int main(int argc, char** argv)
{
    int status = 0;
    int i = 0, j = 0;
    unsigned int count = 0;
    float **output_float = nullptr;
    long long total_infer_time = 0;

    char *model_file = NULL;
    char *input_file = NULL;
    unsigned int loop_count = 1;

    if (argc < 2) {
        printf("%s\n", usage);
        return -1;
    }

    for (i = 0; i< argc; i++) {
        if (!strcmp(argv[i], "-b")) {
            model_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-i")) {
            input_file = argv[++i];
        }
        else if (!strcmp(argv[i], "-l")) {
            loop_count = atoi(argv[++i]);
        }
        else if (!strcmp(argv[i], "-h")) {
            printf("%s\n", usage);
            return 0;
        }
    }
    printf("model_file=%s, input=%s, loop_count=%d \n", model_file, input_file, loop_count);

    /* NPU init*/
    NpuUint npu_uint;
    unsigned int version = npu_uint.get_driver_version();
    printf("npu driver version=0x%08x...\n", version);

    int ret = npu_uint.npu_init();
    if (ret != 0) {
        return -1;
    }
    npu_uint.query_hardware_info();


    NetworkItem struct2depth;
    unsigned int network_id = 0;
    status = struct2depth.network_create(model_file, network_id);
    if (status != 0) {
        printf("network %d create failed.\n", network_id);
    }

    status = struct2depth.network_prepare();
    if (status != 0) {
        printf("network prepare fail, status=%d\n", status);
    }

    // load jpg file
    void *file_data = nullptr;
    unsigned int file_size = 0;
    file_data = (void *)class_preprocess(input_file, &file_size);

    status = struct2depth.network_load_input_buffer(file_data, file_size);
    if (status != 0) {
        printf("network load input file fail, status=%d\n", status);
    }
    if (file_data != nullptr) {
        free(file_data);
        file_data = nullptr;
    }

    //struct2depth.network_load_input_file(&input_file);


    i = network_id;
    /* run network */
    TimeBegin(NETWORK_LOOP);
    while (count < loop_count) {
        count++;

        printf("network: %d, loop count: %d\n", i, count);
        status = struct2depth.network_input_output_set();
        if (status != 0) {
            printf("set network input/output %d failed.\n", i);
            ret = -1;
            return ret;
        }

        #if defined (__linux__)
        TimeBegin(NETWORK_RUN);
        #endif


        status = struct2depth.network_run();
        if (status != 0) {
            printf("fail to run network, status=%d, batchCount=%d\n", status, i);
            ret = -2;
            return ret;
        }

        #if defined (__linux__)
        TimeEnd(NETWORK_RUN);
        printf("run time for this network %d: %lu us.\n", i, (unsigned long)TimeGet(NETWORK_RUN));
        #endif

        total_infer_time += (unsigned long)TimeGet(NETWORK_RUN);

        output_float = struct2depth.get_output(SAVE_TEXT);
        printf("get output finished. \n");


        class_postprocess(input_file, output_float);


        // free output buffer
        for (j=0; j<struct2depth.get_output_cnt(); j++) {
            free(output_float[j]);
        }
        free(output_float);
    }
	TimeEnd(NETWORK_LOOP);

	if (loop_count > 1) {
	    printf("network: %d, this network run avg inference time=%d us,  total avg cost: %d us\n", i,
           (uint32_t)(total_infer_time / loop_count),  (unsigned int)(TimeGet(NETWORK_LOOP) / loop_count));
	}



// exit
#if 1
    /* exit function run in NetworkItem::~NetworkItem()*/
#else
//    struct2depth.network_finish();
//    struct2depth.network_destroy();
#endif

    return ret;
}
