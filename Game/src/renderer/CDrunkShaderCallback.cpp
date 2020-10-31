#include "CDrunkShaderCallback.h"

CDrunkShaderCallback::CDrunkShaderCallback() : time(0.0f)
{
}

CDrunkShaderCallback::~CDrunkShaderCallback()
{
}

void CDrunkShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    services->setVertexShaderConstant("time", &time, 1);
}

void CDrunkShaderCallback::setTime(irr::f32 _time)
{
    time = _time;
}
