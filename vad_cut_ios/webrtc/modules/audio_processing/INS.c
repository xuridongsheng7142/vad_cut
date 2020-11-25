#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "INS.h"
#include "webrtc/modules/audio_processing/ns/ns_core.h"
#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"
#include "webrtc/common_audio/signal_processing/include/signal_processing_library.h"
int fns_new(void** handler, uint32_t  sample)
{
	NsHandle **inst = (NsHandle **)handler;
	int rv = WebRtcNs_Create(inst);
	if (rv == -1) {
		return -1;
	}
	WebRtcNs_Init(*inst,sample);
	return rv;
}


int fns_set_policy(void * handler, int mode)
{
	NSinst_t *inst = (NSinst_t *)handler;
	return WebRtcNs_set_policy_core(inst, mode);
}

int fns_process(void* handler, const int16_t* pcm_in, int16_t* pcm_out)
{
	NsHandle* NS_inst = (NsHandle*) handler;
	int rv = WebRtcNs_Process(NS_inst, (short *)pcm_in, NULL, pcm_out, NULL);
	return rv;
}
 
void fns_free(void*handler)
{
	NsHandle *inst = (NsHandle *)handler;
	WebRtcNs_Free(inst);
}
