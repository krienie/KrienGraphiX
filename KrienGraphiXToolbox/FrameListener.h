
#pragma once

namespace kgt
{
    class FrameListener
    {
        public:
            FrameListener()          = default;
            virtual ~FrameListener() = default;

            virtual void frameUpdate( double deltaTime ) = 0;
    };
}
