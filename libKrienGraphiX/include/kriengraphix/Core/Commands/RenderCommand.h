
#pragma once

#include <d3d11.h>

#include "Core/Commands/CommandBase.h"

namespace kgx
{
template<class Lambda>
class RenderCommand final : public CommandBase
{
    public:
        RenderCommand(ID3D11DeviceContext *dxDevCont, Lambda func)
            : CommandBase(), m_dxDevCont(dxDevCont), m_commandFunc(std::forward<Lambda>(func))
        {
        }

        virtual ~RenderCommand() = default;
        void execute() override
        {
            m_commandFunc(m_dxDevCont);
        }

    private:
        ID3D11DeviceContext *m_dxDevCont;
        Lambda m_commandFunc;
};
}
