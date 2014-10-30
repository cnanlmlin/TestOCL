/*Copyright 2013 Rahul Garg

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.*/

#include "helloCL.h"
extern const char *getResultString(void);

JNIEXPORT jstring JNICALL Java_org_code_testopencl_MainActivity_testOCL
  (JNIEnv *env, jobject obj){


	const char* result = getResultString();
	return (jstring)(*env)->NewStringUTF(env,result);
		
}
