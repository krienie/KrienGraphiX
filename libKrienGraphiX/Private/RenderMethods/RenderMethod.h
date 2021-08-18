
#pragma once

namespace kgx::RenderMethods
{
class RcScene;

class RenderMethod
{
    public:
        virtual ~RenderMethod() = default;

        virtual void preRenderSetup() = 0;
        virtual void update() = 0;
        virtual void render(const RcScene & scene /*, RHIRenderTargetView, GpuVolumeStore, QualityProfile*/) = 0;
};
}
