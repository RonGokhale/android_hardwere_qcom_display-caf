/*
* Copyright (C) 2008 The Android Open Source Project
* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <cutils/properties.h>
#include "overlayCtrlData.h"
#include "fb_priv.h"
#include "gralloc_priv.h" //for interlace

namespace overlay{

bool Ctrl::init(uint32_t fbnum) {
    // MDP/FD init
    if(!mMdp.init(fbnum)) {
        ALOGE("Ctrl failed to init fbnum=%d", fbnum);
        return false;
    }

    if(!getScreenInfo(mInfo)) {
        ALOGE("Ctrl failed to getScreenInfo");
        return false;
    }

    return true;
}

bool Ctrl::setSource(const utils::PipeArgs& args)
{
    return mMdp.setSource(args);
}

bool Ctrl::setPosition(const utils::Dim& dim)
{
    if(!dim.check(mInfo.mFBWidth, mInfo.mFBHeight)) {
        ALOGE("Ctrl setPosition error in dim");
        dim.dump();
        return false;
    }

    if(!mMdp.setPosition(dim, mInfo.mFBWidth, mInfo.mFBHeight)) {
        ALOGE("Ctrl failed MDP setPosition");
        return false;
    }
    return true;
}

bool Ctrl::setTransform(const utils::eTransform& orient)
{
    if(!mMdp.setTransform(orient)) {
        ALOGE("Ctrl setTransform failed for Mdp");
        return false;
    }
    return true;
}

void Ctrl::setRotatorUsed(const bool& rotUsed) {
    mMdp.setRotatorUsed(rotUsed);
}

bool Ctrl::setCrop(const utils::Dim& d)
{
    if(!mMdp.setCrop(d)) {
        ALOGE("Data setCrop failed in MDP setCrop");
        return false;
    }
    return true;
}

utils::ActionSafe* utils::ActionSafe::sActionSafe = NULL;
utils::OverScanCompensation* utils::OverScanCompensation::sOverScanCompensation = NULL;

bool Ctrl::setVisualParams(const MetaData_t& data)
{
    if(!mMdp.setVisualParams(data)) {
        ALOGD("setVisualParam failed in MDP setVisualParam");
        return false;
    }
    return true;
}

utils::Dim Ctrl::getAspectRatio(const utils::Whf& whf) const
{
    utils::Whf inWhf(whf.w, whf.h, mMdp.getSrcWhf().format);
    utils::Whf tmpwhf(inWhf);
    uint32_t fbWidth  = mInfo.mFBWidth;
    uint32_t fbHeight = mInfo.mFBHeight;

    /* Calculate the width and height if it is YUV TILE format*/
    if (inWhf.format == HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED) {
        tmpwhf.w = whf.w - (utils::alignup(whf.w, 64) - whf.w);
        tmpwhf.h = whf.h - (utils::alignup(whf.h, 32) - whf.h);
    }
    if (inWhf.w * fbHeight > fbWidth * inWhf.h) {
        inWhf.h = fbWidth * inWhf.h / inWhf.w;
        inWhf.w = fbWidth;
    } else if (inWhf.w * fbHeight < fbWidth * inWhf.h) {
        inWhf.w = fbHeight * inWhf.w / inWhf.h;
        inWhf.h = fbHeight;
    } else {
        inWhf.w = fbWidth;
        inWhf.h = fbHeight;
    }
    /* Scaling of upto a max of 8 times supported */
    if (inWhf.w > (tmpwhf.w * utils::HW_OV_MAGNIFICATION_LIMIT)){
        inWhf.w = utils::HW_OV_MAGNIFICATION_LIMIT * tmpwhf.w;
    }
    if(inWhf.h > (tmpwhf.h * utils::HW_OV_MAGNIFICATION_LIMIT)) {
        inWhf.h = utils::HW_OV_MAGNIFICATION_LIMIT * tmpwhf.h;
    }
    if (inWhf.w > fbWidth) inWhf.w = fbWidth;
    if (inWhf.h > fbHeight) inWhf.h = fbHeight;

    float asWidth = utils::ActionSafe::getInstance()->getHeight();
    float asHeight = utils::ActionSafe::getInstance()->getWidth();

    inWhf.w = inWhf.w * (1.0f - asWidth / 100.0f);
    inWhf.h = inWhf.h * (1.0f - asHeight / 100.0f);

    uint32_t x = (fbWidth - inWhf.w) / 2.0;
    uint32_t y = (fbHeight - inWhf.h) / 2.0;
    return utils::Dim(x, y, inWhf.w, inWhf.h);
}

utils::FrameBufferInfo* utils::FrameBufferInfo::sFBInfoInstance = 0;

// This function gets the destination position for external display
// based on the position and aspect ratio of the primary
utils::Dim Ctrl::getAspectRatio(const utils::Dim& dim) const {
    float priWidth  = utils::FrameBufferInfo::getInstance()->getWidth();
    float priHeight = utils::FrameBufferInfo::getInstance()->getHeight();
    float fbWidth = mInfo.mFBWidth;
    float fbHeight = mInfo.mFBHeight;
    float wRatio = 1.0;
    float hRatio = 1.0;
    float xRatio = 1.0;
    float yRatio = 1.0;
    utils::Dim inDim(dim);

    int xPos = 0;
    int yPos = 0;
    int tmp = 0;
    utils::Dim tmpDim;
    switch(inDim.o) {
        case MDP_ROT_NOP:
        case MDP_ROT_180:
            {
                utils::Whf whf((uint32_t) priWidth, (uint32_t) priHeight, 0);
                tmpDim = getAspectRatio(whf);
                xPos = tmpDim.x;
                yPos = tmpDim.y;
                fbWidth = tmpDim.w;
                fbHeight = tmpDim.h;

                if (inDim.o == MDP_ROT_180) {
                    inDim.x = priWidth - (inDim.x + inDim.w);
                    inDim.y = priHeight - (inDim.y + inDim.h);
                }
                break;
            }
        case MDP_ROT_90:
        case MDP_ROT_270:
            {
                if(inDim.o == MDP_ROT_90) {
                    tmp = inDim.y;
                    inDim.y = priWidth - (inDim.x + inDim.w);
                    inDim.x = tmp;
                }
                else if (inDim.o == MDP_ROT_270) {
                    tmp = inDim.x;
                    inDim.x = priHeight - (inDim.y + inDim.h);
                    inDim.y = tmp;
                }

                // Swap the destination width/height
                utils::swapWidthHeight(inDim.w, inDim.h);
                // Swap width/height for primary
                utils::swapWidthHeight(priWidth, priHeight);
                utils::Whf whf((uint32_t) priWidth, (uint32_t) priHeight, 0);
                tmpDim = getAspectRatio(whf);
                xPos = tmpDim.x;
                yPos = tmpDim.y;
                fbWidth = tmpDim.w;
                fbHeight = tmpDim.h;
                break;
            }
        default:
            ALOGE("%s: Unknown Orientation", __FUNCTION__);
            break;
    }

    // Calculate the position
    xRatio = inDim.x/priWidth;
    yRatio = inDim.y/priHeight;
    wRatio = inDim.w/priWidth;
    hRatio = inDim.h/priHeight;

    return utils::Dim((xRatio * fbWidth) + xPos,   // x
            (yRatio * fbHeight) + yPos,  // y
            (wRatio * fbWidth),          // width
            (hRatio * fbHeight),         // height
            inDim.o);                    // orientation
}

utils::Dim Ctrl::getOSCPosition(const utils::Dim& dim) const {

    float prifbWidth  = utils::FrameBufferInfo::getInstance()->getWidth();
    float prifbHeight = utils::FrameBufferInfo::getInstance()->getHeight();

    int oscx = 0;
    int oscy = 0;
    int oscwidth = 0;
    int oscheight = 0;

    float x_ratio = ((float)dim.x/prifbWidth);
    float y_ratio = ((float)dim.y/prifbHeight);
    float w_ratio = ((float)dim.w/prifbWidth);
    float h_ratio = ((float)dim.h/prifbHeight);

    if(utils::OverScanCompensation::getInstance()->isOSCDimensionsSet()) {
        utils::OverScanCompensation* oscinstance = utils::OverScanCompensation::getInstance();
        oscinstance->getDimension(oscx,oscy,oscwidth,oscheight);

        /* Don't rely too much on the osc values set via binder. Check for outliers */
        if(oscx < 0 or oscx > prifbWidth)
            oscx = 0;
        if(oscy < 0 or oscy > prifbHeight)
            oscy = 0;
        if(oscwidth <= 0 or oscwidth >= prifbWidth)
            oscwidth = prifbWidth;
        if(oscheight <= 0 or oscheight >= prifbHeight)
            oscheight = prifbHeight;
        if(oscx + oscwidth > prifbWidth) {
            oscx = 0;
            oscwidth =  prifbWidth;
        }
        if(oscy + oscheight > prifbHeight) {
            oscy = 0;
            oscheight = prifbHeight;
        }

        utils::Dim oscdim;
        oscdim.x = (x_ratio * oscwidth) + oscx;
        oscdim.y = (y_ratio * oscheight) + oscy;
        oscdim.w = (w_ratio * oscwidth);
        oscdim.h = (h_ratio * oscheight);
        oscdim.o = dim.o;

        return oscdim;
    } else {
        return utils::Dim(dim);
    }

}

void Ctrl::dump() const {
    ALOGE("== Dump Ctrl start ==");
    mInfo.dump("mInfo");
    mMdp.dump();
    ALOGE("== Dump Ctrl end ==");
}

} // overlay
