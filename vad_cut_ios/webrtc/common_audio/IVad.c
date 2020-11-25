/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *  Copyright (c) 2016 Daniel Pirch.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/IVad.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// valid sample rates in kHz
static const int valid_rates[] = { 8, 16, 32, 48 };

// VAD process functions for each valid sample rate
/*static int (*const process_funcs[])(VadInstT*, const int16_t*, size_t) = {
    WebRtcVad_CalcVad8khz,
    WebRtcVad_CalcVad16khz,
    WebRtcVad_CalcVad32khz,
    WebRtcVad_CalcVad48khz,
};*/

// valid frame lengths in ms
static const size_t valid_frame_times[] = { 10, 20, 30 };

#define arraysize(a) (sizeof (a) / sizeof *(a))

typedef struct Fvad_ {
    VadInstT core;
    int rate_idx; // index in valid_rates and process_funcs arrays
}Fvad;

//创建并初始化VAD实例。成功时，返回指向新VAD实例的指针，
//该实例最终应该使用fvad free()删除。如果内存分配错误，返回NULL。

int fvad_new(void **inst, int fs, int mode)  
{
    *inst = (Fvad *)malloc(sizeof (Fvad));
    if (*inst) {
		fvad_reset(*inst);
	} else {
		return -1;
	}
	if(fs != 16000) {
		printf("Only 16KHz sampling rate supported!\n");
		fvad_free(*inst);
		*inst = NULL;
		return -1;
	} else {
		fvad_set_mode(*inst, mode);
	}
    return 0;
}

//释放指定VAD实例的动态内存。

void fvad_free(void *inst) 
{
    assert(inst);
    free((Fvad *)inst);
}

//重新初始化VAD实例，清除所有状态并重新设置模式和默认采样率。

void fvad_reset(void *inst) 
{
	int rv;
    assert(inst);

    rv = WebRtcVad_InitCore(&((Fvad *)inst)->core);
    assert(rv == 0);
    //inst->rate_idx = 0;
	fvad_set_sample_rate((Fvad *)inst, 16000);
}


int fvad_set_mode(void* inst, int mode)
{
	int rv;
    assert(inst);
    rv = WebRtcVad_set_mode_core(&((Fvad *)inst)->core, mode);
    assert(rv == 0 || rv == -1);
    return rv;
}


int fvad_set_sample_rate(void* inst, int sample_rate)
{
	size_t i;
    assert(inst);
    for (i = 0; i < arraysize(valid_rates); i++) {
        if (valid_rates[i] * 1000 == sample_rate) {
            ((Fvad *)inst)->rate_idx = i;
            return 0;
        }
    }
    return -1;
}


static int valid_length(int rate_idx, size_t length)
{
	size_t i;
    int samples_per_ms = valid_rates[rate_idx];
    for (i = 0; i < arraysize(valid_frame_times); i++) {
        if (valid_frame_times[i] * samples_per_ms == length)
            return 0;
    }
    return -1;
}


int fvad_process(void* inst, const int16_t* frame, size_t length)
{
	int rv;
    assert(inst);
    if (valid_length(((Fvad *)inst)->rate_idx, length) < 0)
        return -1;

    //rv = process_funcs[inst->rate_idx](&inst->core, frame, length);
	rv = WebRtcVad_CalcVad16khz(&((Fvad *)inst)->core, (int16_t*)frame, length);
    assert (rv >= 0);
    if (rv > 0) rv = 1;

    return rv;
}
