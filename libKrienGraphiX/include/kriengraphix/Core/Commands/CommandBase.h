
#pragma once

namespace kgx
{
    class CommandBase
    {
        public:
            CommandBase() = default;
            virtual ~CommandBase() = default;
            virtual void execute() = 0;
            CommandBase* next = nullptr;
    };
}
