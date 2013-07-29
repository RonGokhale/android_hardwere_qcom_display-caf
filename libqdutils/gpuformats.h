/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef QCOM_GPUFORMATS_H
#define QCOM_GPUFORMATS_H
#include <gralloc_priv.h>
#include "qdMetaData.h"
namespace qdutils {
/*
 * Checks if the format is supported by the GPU.
 *
 * @param: pointer to native_handle
 *
 * @return true if the format is supported by the GPU.
 */
static inline bool isGPUSupportedFormat(const native_handle *handle)
{
    const private_handle_t *hnd = static_cast<const private_handle_t *>(handle);
    MetaData_t *metadata = (MetaData_t *)hnd->base_metadata;
    int format = hnd->format;
    if ((format == HAL_PIXEL_FORMAT_RGB_888)      ||
        (format == HAL_PIXEL_FORMAT_YCrCb_422_SP) ||
        (format == HAL_PIXEL_FORMAT_YCbCr_422_SP) ||
        (format == HAL_PIXEL_FORMAT_YCbCr_444_I)  ||
        (format == HAL_PIXEL_FORMAT_YCbCr_444_I)  ||
        (metadata->operation & PP_PARAM_S3D_VIDEO)) {
        return false;
    }
    return true;
}
}; //namespace qdutils



#endif /* end of include guard: QCOM_GPUFORMATS_H */