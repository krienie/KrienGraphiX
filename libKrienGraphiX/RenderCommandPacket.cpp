
#include "RenderCommandPacket.h"

namespace kgx
{
	namespace rendercommandpacket
	{
		RenderCommandPacket* getNextCommandPacket( RenderCommandPacket packet )
		{
			return reinterpret_cast<RenderCommandPacket*>(reinterpret_cast<char*>(packet) + OFFSET_NEXT_COMMAND_PACKET);
		}

		BackendDispatchFunction* getBackendDispatchFunction( RenderCommandPacket packet )
		{
			return reinterpret_cast<BackendDispatchFunction*>(reinterpret_cast<char*>(packet) + OFFSET_BACKEND_DISPATCH_FUNCTION);
		}

		void storeNextCommandPacket( RenderCommandPacket packet, RenderCommandPacket nextPacket )
		{
			*rendercommandpacket::getNextCommandPacket( packet ) = nextPacket;
		}

		void storeBackendDispatchFunction( RenderCommandPacket packet, BackendDispatchFunction dispatchFunction )
		{
			*rendercommandpacket::getBackendDispatchFunction( packet ) = dispatchFunction;
		}

		const RenderCommandPacket loadNextCommandPacket( const RenderCommandPacket packet )
		{
			return *getNextCommandPacket( packet );
		}

		const BackendDispatchFunction loadBackendDispatchFunction( const  RenderCommandPacket packet )
		{
			return *getBackendDispatchFunction( packet );
		}

		const void* loadCommand( const RenderCommandPacket packet )
		{
			return reinterpret_cast<char*>(packet) + OFFSET_COMMAND;
		}
	}
}
