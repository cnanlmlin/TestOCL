#include "helloCL.h"
extern const char *getResultString(void);

JNIEXPORT jstring JNICALL Java_org_code_testopencl_MainActivity_testOCL
  (JNIEnv *env, jobject obj){


	const char* result = getResultString();
	return (jstring)(*env)->NewStringUTF(env,result);
		
}
