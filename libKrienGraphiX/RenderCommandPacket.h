
/**
* Inspired by Stefan Reinalter's blog on stateless, layered, multi-threaded rendering.
* https://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/
*/

#pragma once

#include "RenderBackend.h"

namespace kgx
{
	typedef void* RenderCommandPacket;

	namespace rendercommandpacket
	{
		static const size_t OFFSET_NEXT_COMMAND_PACKET       = 0u;
		static const size_t OFFSET_BACKEND_DISPATCH_FUNCTION = OFFSET_NEXT_COMMAND_PACKET + sizeof(RenderCommandPacket);
		static const size_t OFFSET_COMMAND                   = OFFSET_BACKEND_DISPATCH_FUNCTION + sizeof(BackendDispatchFunction);

		RenderCommandPacket* getNextCommandPacket( RenderCommandPacket packet );
		BackendDispatchFunction* getBackendDispatchFunction( RenderCommandPacket packet );
		void storeNextCommandPacket( RenderCommandPacket packet, RenderCommandPacket nextPacket );
		void storeBackendDispatchFunction( RenderCommandPacket packet, BackendDispatchFunction dispatchFunction );
		const RenderCommandPacket loadNextCommandPacket( const RenderCommandPacket packet );
		const BackendDispatchFunction loadBackendDispatchFunction( const  RenderCommandPacket packet );
		const void* loadCommand( const RenderCommandPacket packet );

		template <typename T>
		RenderCommandPacket create( size_t auxMemorySize )
		{
			return operator new ( getSize<T>(auxMemorySize) );
		}

		template <typename T>
		size_t getSize( size_t auxMemorySize )
		{
			return OFFSET_COMMAND + sizeof(T) + auxMemorySize;
		};

		template <typename T>
		RenderCommandPacket* getNextCommandPacket( T* command )
		{
			return reinterpret_cast<RenderCommandPacket*>(reinterpret_cast<char*>(command) - OFFSET_COMMAND + OFFSET_NEXT_COMMAND_PACKET);
		}

		template <typename T>
		T* getCommand( RenderCommandPacket packet )
		{
			return reinterpret_cast<T*>(reinterpret_cast<char*>(packet) + OFFSET_COMMAND);
		}

		template <typename T>
		char* getAuxiliaryMemory( T* command )
		{
			return reinterpret_cast<char*>(command) + sizeof(T);
		}

		template <typename T>
		void storeNextCommandPacket( T* command, RenderCommandPacket nextPacket )
		{
			*rendercommandpacket::getNextCommandPacket<T>(command) = nextPacket;
		}
	}
}
