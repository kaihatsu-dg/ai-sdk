/****************************************************************************
*  npulib header file
****************************************************************************/
#ifndef _NPULIB_H_
#define _NPULIB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _npu_network  *npu_network;
typedef struct _npu_buffer   *npu_buffer;

typedef enum _save_file_type_e
{
    SAVE_NONE,
    SAVE_BINARY,
    SAVE_TEXT
} save_file_type_e;

#define _CHECK_STATUS( stat )  do {\
    if( 0 != stat ) {\
        printf("Error: %s: %s at %d\n", __FILE__, __FUNCTION__, __LINE__);\
    }\
} while(0)


class NpuUint
{
public:
	NpuUint(void);
    ~NpuUint(void);

	unsigned int get_driver_version(void);
	int npu_init();
    int query_hardware_info(void);
    int npu_destroy(void);
};

class NetworkItem
{
public:
	NetworkItem(void);
	~NetworkItem(void);

	int network_create(char *model_file, unsigned int network_id);
	int network_prepare(void);
	int network_input_output_set(void);

	// input file such as: xxx.tensor, xxx.dat, xxx.bin, xxx.txt
	int network_load_input_file(char **input_path);

	// one input, eg: BGR, RGB
	int network_load_input_buffer(void *input_data, unsigned int input_size);

	// input yuv buffer
	int network_load_input_yuv_buffer(void *yuv_data, int w, int h);


	int network_run(void);

	float **get_output(save_file_type_e save_type=SAVE_NONE);    //default: SAVE_NONE

	char *get_ngb_name(void);

	int get_output_cnt(void);


	void network_finish(void);

	void network_destroy(void);

private:
	/* network information. */
	int             m_nbg_name;
	int             m_input_count;
	int             m_output_count;

	/* NPU buffer objects. */
	npu_network     m_network;
	npu_buffer     *m_input_buffers;
	npu_buffer     *m_output_buffers;

//	uint32_t   loop_count;
//	uint32_t   infer_cycle;
//	uint32_t   infer_time;
//	uint64_t   total_infer_cycle;
//	long long   total_infer_time;
};



#ifdef __cplusplus
}
#endif

#endif
