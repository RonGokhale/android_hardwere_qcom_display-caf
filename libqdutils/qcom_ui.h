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
#ifndef QCOM_UTIL_H
#define QCOM_UTIL_H

#include <gralloc_priv.h>
#include <comptype.h>
#include <ui/Region.h>
#include <hardware/hwcomposer.h>
#include <genlock.h>
#include "egl_handles.h"

namespace qdutils {
class CBUtils {

private:
  static bool sGPUlayerpresent;
  static bool sIsMDPComp;

public:
  enum { // S3D formats
      eS3D_SIDE_BY_SIDE   = 0x10000,
      eS3D_TOP_BOTTOM     = 0x20000
  };
  static void checkforGPULayer(const hwc_layer_list_t* list);
  static void updateStat(const hwc_layer_list_t* list, bool mdp_comp_used);
  static bool isUpdatingFB(int compositionType);
  static int qcomuiClearRegion(Region region, EGLDisplay dpy);
  static int getS3DCompositionFormat(int hints);
  static void unlock_lastGpuSupportedBuffer(void *handle);
};
}//namespace qdutils
#endif /* end of include guard: QCOM_UTIL_H*/