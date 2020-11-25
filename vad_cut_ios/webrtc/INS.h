 

#include <stdint.h>
#include <stddef.h>

#ifndef INS_H_
#define INS_H_

#ifdef __cplusplus
extern "C" {
#endif 
typedef struct NsHandleT NsHandle;

int fns_process(void* handler, const int16_t* pcm_in, int16_t* pcm_out); 

int fns_new(void** handler,  uint32_t  sample);

int fns_set_policy(void * handler, int mode);

void fns_free(void*handler);



#ifdef __cplusplus
}
#endif

#endif  //ISINWTNS_H_
 		   


