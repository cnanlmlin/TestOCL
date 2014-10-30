#include <CL/cl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include "log.h"

static int loadedCL;
void printOutDetail();

void *getCLHandle(){
	void *res = NULL;
	res = dlopen("/system/vendor/lib/libPVROCL.so",RTLD_LAZY);
	if(res==NULL){
		res = dlopen("/system/vendor/lib/egl/libGLES_mali.so",RTLD_LAZY);
	}
	if(res==NULL){
		res = dlopen("/system/vendor/lib/libllvm-a3xx.so",RTLD_LAZY);
	}
	if(res==NULL) LOGD("Could not open library :(\n");
	else LOGD("loaded some library\n");
	return res;
}

cl_int (*rclGetPlatformIDs)(cl_uint          /* num_entries */,
                 cl_platform_id * /* platforms */,
                 cl_uint *        /* num_platforms */);


cl_int (*rclGetPlatformInfo)(cl_platform_id   /* platform */, 
                  cl_platform_info /* param_name */,
                  size_t           /* param_value_size */, 
                  void *           /* param_value */,
                  size_t *         /* param_value_size_ret */);

cl_int (*rclGetDeviceIDs)(cl_platform_id   /* platform */,
               cl_device_type   /* device_type */, 
               cl_uint          /* num_entries */, 
               cl_device_id *   /* devices */, 
               cl_uint *        /* num_devices */);


cl_int (*rclGetDeviceInfo)(cl_device_id    /* device */,
                cl_device_info  /* param_name */, 
                size_t          /* param_value_size */, 
                void *          /* param_value */,
                size_t *        /* param_value_size_ret */);


void initFuns(){
	loadedCL = 0;
	void *handle = getCLHandle();
	if(handle==NULL) return;
	rclGetPlatformIDs = (cl_int (*)(cl_uint,cl_platform_id *,cl_uint*))dlsym(handle,"clGetPlatformIDs");
	rclGetPlatformInfo = (cl_int (*)(cl_platform_id, cl_platform_info, size_t, void *, size_t*))dlsym(handle,"clGetPlatformInfo");
	rclGetDeviceIDs = (cl_int (*)(cl_platform_id, cl_device_type, cl_uint, cl_device_id *, cl_uint*))dlsym(handle,"clGetDeviceIDs");
	rclGetDeviceInfo = (cl_int (*)(cl_device_id, cl_device_info, size_t, void *, size_t*))dlsym(handle,"clGetDeviceInfo");
	loadedCL = 1;	
}

static void checkErr(cl_int err,const char* name){
	if(err != CL_SUCCESS){
		fprintf(stderr,"ERROR %s (%d)\n",name, err);
		exit(1);
	}
}

const char *getResultString(){
	initFuns();
	if(loadedCL==0){
		const char *ptr = "Did not find OpenCL\n";
		return ptr;
	}
	cl_platform_id platforms[10];
	cl_uint numPlats;
	rclGetPlatformIDs(10,platforms,&numPlats);
	int i;
	char *result = (char*)malloc(10000);
	result[0] = '\0';
	int index = 0;
	for(i=0;i<numPlats;i++){
			char platname[100];
			rclGetPlatformInfo(platforms[i],CL_PLATFORM_NAME,sizeof(platname),platname,NULL);
			int count = sprintf(&result[index],"%s\n",platname);
			index += count;

			char platversion[100];
			rclGetPlatformInfo(platforms[i],CL_PLATFORM_VERSION,sizeof(platversion),platversion,NULL);
			count = sprintf(&result[index],"%s\n",platversion);
			index += count;

			cl_device_id devices[10];
			cl_uint ndevices;
			rclGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_ALL,10,devices,&ndevices);
			int j;

			for(j=0;j<ndevices;j++){
					char devname[100];
					rclGetDeviceInfo(devices[j],CL_DEVICE_NAME,sizeof(devname),devname,NULL);
					count = sprintf(&result[index],"%s\n",devname);
					index += count;
			}
	}
	
	//more detail info in log
	printOutDetail();
	
	LOGD("  OCL Info display over.\n ");
	
	return result;
}

void printOutDetail(){
	cl_int err;
	cl_uint num_platforms;
	cl_platform_id platforms[4];

	//Platform info
	err = rclGetPlatformIDs(4,platforms,&num_platforms);
	checkErr(err, "Unable to get platform list");

	//Iteratate over platforms
	LOGD("Number of platforms:\t\t\t\t %d\n", num_platforms);

	unsigned int i;
	for (i=0; i<num_platforms; i++)
	{
		char str[10240];

		err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(str), str, NULL);
		checkErr(err, "clGetPlatformInfo(CL_PLATFORM_PROFILE)");
		LOGD("  Plaform Profile:\t\t\t\t %s\n", str);

		err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(str), str, NULL);
		checkErr(err, "clGetPlatformInfo(CL_PLATFORM_VERSION)");
		LOGD("  Plaform Version:\t\t\t\t %s\n", str);

		err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(str), str, NULL);
		checkErr(err, "clGetPlatformInfo(CL_PLATFORM_NAME)");
		LOGD("  Plaform NAME:\t\t\t\t %s\n", str);

		err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(str), str, NULL);
		checkErr(err, "clGetPlatformInfo(CL_PLATFORM_VENDOR)");
		LOGD("  Plaform VENDOR:\t\t\t\t %s\n", str);

		err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(str), str, NULL);
		checkErr(err, "clGetPlatformInfo(CL_PLATFORM_EXTENSIONS)");
		LOGD("  Plaform EXTENSIONS:\t\t\t\t %s\n", str);
	}
	LOGD("\n\n");

	// Now Iteratate over each platform and its devices
	{
		unsigned int i;
		for (i=0; i<num_platforms; i++) {
			char str[10240];
			cl_device_id devices[4];
			cl_uint num_devices;

			err = rclGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(str), &str, NULL);
			checkErr(err, "clGetPlatformInfo(CL_PLATFORM_NAME)");
			LOGD("  Plaform Name:\t\t\t\t\t %s\n", str);

			rclGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 4, devices, &num_devices);
			LOGD("  Number of devices:\t\t\t\t %d\n", num_devices);

			{
				unsigned int j;
				for (j=0; j<num_devices; j++){
					cl_device_type dev_type;
					LOGD("\n  DEVICE %d\n", j);

					err = rclGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(dev_type), &dev_type, NULL);
					checkErr(err, "clGetDeviceInfo(CL_DEVICE_TYPE)");
					LOGD("  Device Type:\t\t\t\t ");

					if (dev_type & CL_DEVICE_TYPE_ACCELERATOR){
						LOGD("CL_DEVICE_TYPE_ACCELERATOR ");
					}
					else if (dev_type & CL_DEVICE_TYPE_CPU){
						LOGD("CL_DEVICE_TYPE_CPU ");
					}
					else if (dev_type & CL_DEVICE_TYPE_GPU){
						LOGD("CL_DEVICE_TYPE_GPU ");
					}
					else if (dev_type & CL_DEVICE_TYPE_DEFAULT){
						LOGD("CL_DEVICE_TYPE_DEFAULT ");
					}
					LOGD("\n");

					{
						cl_uint vendor_id;
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_VENDOR_ID, sizeof(vendor_id), &vendor_id, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_VENDOR_ID)");
						LOGD("  Device ID:\t\t\t\t %d\n", vendor_id);
					}

					{
						cl_uint units;
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(units), &units, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_MAX_COMPUTE_UNITS)");
						LOGD("  Max compute units:\t\t\t\t %d\n", units);
					}

					{
						cl_uint dims;
						size_t *sizes;
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(dims), &dims, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS)");
						LOGD("  Max work item dimensions:\t\t\t %d\n", dims);
						                             
						sizes = (size_t*)malloc(dims * sizeof(size_t));
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t)*dims, sizes, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES)");
						LOGD("  Max work item dimensions:\t\t\t %d\n", dims);

						{
							unsigned int k;
							LOGD("    Max work items:\t\t\t\t (");
							for (k=0; k<dims; k++){
								LOGD("%u", (unsigned int)sizes[k]);
								if (k != dims-1){
									LOGD(",");
								}
									 
							}
							LOGD(")\n");
						}
					}

					#define GET_SIZET(CL_D,str) { \
						size_t val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, (unsigned int)val); \
					}

					#define GET_STRING(CL_D,str,size) { \
						char val[size]; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, val); \
					}

					#define GET_UINT(CL_D,str) { \
						cl_uint val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, val); \
					}

					#define GET_ULONG(CL_D,str) { \
						cl_ulong val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, val); \
					}

					#define GET_BOOL(CL_D,str) { \
						cl_bool val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, (val == CL_TRUE ? "Yes" : "No")); \
					}

					#define GET_BOOL_CUSTOM(CL_D,str,t,f) { \
						cl_bool val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, (val == CL_TRUE ? t : f)); \
					}

					#define GET_BITSET_AND(TYPE,CL_D,test,str) { \
						TYPE val; \
						err = rclGetDeviceInfo(devices[j], CL_D, sizeof(val), &val, NULL); \
						checkErr(err, "clGetDeviceInfo(" #CL_D ")"); \
						LOGD(str, ((val & test) == CL_TRUE ? "Yes" : "No")); \
					}

					GET_SIZET(CL_DEVICE_MAX_WORK_GROUP_SIZE, "  Max work group size:\t\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, "  Preferred vector width char:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, "  Preferred vector width short:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, "  Preferred vector width int:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, "  Preferred vector width long:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, "  Preferred vector width float:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, "  Preferred vector width double:\t\t %u\n")
					GET_UINT(CL_DEVICE_MAX_CLOCK_FREQUENCY, "  Max clock frequency:\t\t\t\t %uMHz\n")
					GET_UINT(CL_DEVICE_ADDRESS_BITS, "  Address bits:\t\t\t\t\t %ubits\n")
					GET_ULONG(CL_DEVICE_MAX_MEM_ALLOC_SIZE, "  Max memory allocation:\t\t\t %lu bytes\n")
					GET_BOOL(CL_DEVICE_IMAGE_SUPPORT, "  Image support:\t\t\t\t %s\n")
						                              
					GET_SIZET(CL_DEVICE_MAX_PARAMETER_SIZE, "  Max size of kernel argument:\t\t\t %u\n")
					GET_UINT(CL_DEVICE_MEM_BASE_ADDR_ALIGN, "  Alignment of base addres:\t\t\t %u bits\n")
					GET_UINT(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, "  Minimum alignment for any datatype:\t\t %u bytes\n")

					LOGD("  Single precision floating point capability\n");
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_DENORM, "    Denorms:\t\t\t\t\t %s\n")
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_INF_NAN, "    Quiet NaNs:\t\t\t\t\t %s\n")
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_ROUND_TO_NEAREST, "    Round to nearest even:\t\t\t %s\n")
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_ROUND_TO_ZERO, "    Round to zero:\t\t\t\t %s\n")
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_ROUND_TO_INF, "    Round to +ve and infinity:\t\t\t %s\n")
					GET_BITSET_AND(cl_device_fp_config,CL_DEVICE_SINGLE_FP_CONFIG, CL_FP_FMA, "    IEEE754-2008 fused multiply-add:\t\t %s\n")
					
					{
						cl_device_mem_cache_type cache;
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, sizeof(cache), &cache, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE)");
						LOGD("  Cache type:\t\t\t\t\t ");
						switch(cache){
							case CL_NONE:
								LOGD("NONE\n");
								break;
							case CL_READ_ONLY_CACHE:
								LOGD("Read only");
								break;
							case CL_READ_WRITE_CACHE:
								LOGD("Read/Write\n");
								break;
						}
					}
					GET_UINT(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, "  Cache line size:\t\t\t\t %u bytes\n")
					GET_ULONG(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, "  Cache size:\t\t\t\t\t %lu bytes\n")
					GET_ULONG(CL_DEVICE_GLOBAL_MEM_SIZE, "  Global memory size:\t\t\t\t %lu bytes\n")
					GET_ULONG(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, "  Constant buffer size:\t\t\t\t %lu bytes\n")
					GET_UINT(CL_DEVICE_MAX_CONSTANT_ARGS, "  Max number of constant args:\t\t\t %u\n")

					{
						cl_device_local_mem_type cache;
						err = rclGetDeviceInfo(devices[j], CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cache), &cache, NULL);
						checkErr(err, "clGetDeviceInfo(CL_DEVICE_LOCAL_MEM_TYPE)");
						LOGD("  Local memory type:\t\t\t\t ");
						switch(cache){
							case CL_LOCAL:
								LOGD("Local\n");
								break;
							case CL_GLOBAL:
								LOGD("Global\n");
								break;
						}
					}
					
					GET_ULONG(CL_DEVICE_LOCAL_MEM_SIZE, "  Local memory size\t\t\t\t %lu bytes\n")
					GET_SIZET(CL_DEVICE_PROFILING_TIMER_RESOLUTION, "  Profiling timer resolution\t\t\t %u\n")
					GET_BOOL_CUSTOM(CL_DEVICE_ENDIAN_LITTLE, "  Device endianess\t\t\t\t %s\n", "Little", "Big")
					GET_BOOL(CL_DEVICE_AVAILABLE, "  Available\t\t\t\t\t %s\n")
					GET_BOOL(CL_DEVICE_COMPILER_AVAILABLE, "  Compiler available\t\t\t\t %s\n")
					
					LOGD("  Execution capabilities\t\t\t\t \n");
					GET_BITSET_AND(cl_device_exec_capabilities, CL_DEVICE_EXECUTION_CAPABILITIES, CL_EXEC_KERNEL, "  Execute OpenCL kernels\t\t\t %s\n")
					GET_BITSET_AND(cl_device_exec_capabilities, CL_DEVICE_EXECUTION_CAPABILITIES, CL_EXEC_NATIVE_KERNEL, "  Execute native kernels\t\t\t %s\n")

					LOGD("  Queue properties\t\t\t\t\n ");
					GET_BITSET_AND(cl_command_queue_properties, CL_DEVICE_QUEUE_PROPERTIES, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "   Out-of-Order\t\t\t\t %s\n")
					GET_BITSET_AND(cl_command_queue_properties, CL_DEVICE_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, "    Profiling\t\t\t\t\t %s\n")

					GET_STRING(CL_DEVICE_NAME, "  Name\t\t\t\t\t\t %s\n", 256);
					GET_STRING(CL_DEVICE_VENDOR, "  Vendor\t\t\t\t\t %s\n", 256);
					GET_STRING(CL_DRIVER_VERSION, "  Driver version\t\t\t\t %s\n", 1000);
					GET_STRING(CL_DEVICE_PROFILE, "  Profile\t\t\t\t\t %s\n", 30);
					GET_STRING(CL_DEVICE_VERSION, "  Version\t\t\t\t\t %s\n", 50);
					GET_STRING(CL_DEVICE_EXTENSIONS, "  Extensions\t\t\t\t\t %s\n", 4096);

					LOGD("\n");
				}
			}
		}
	}
}



