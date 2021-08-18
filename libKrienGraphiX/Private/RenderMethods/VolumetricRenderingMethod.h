
#pragma once

#include "RenderMethod.h"

namespace kgx::RenderMethods
{
class RcScene;

class VolumetricRenderingMethod final : public RenderMethod
{
    public:
        ~VolumetricRenderingMethod() override = default;

        void render(const RcScene & scene /*, RHIRenderTargetView, GpuVolumeStore, QualityProfile*/) override;
};
}
