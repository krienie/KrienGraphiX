
#pragma once

namespace kgt
{
	//TODO: this prob better belongs in libKrienGraphiX, combined with a RenderWindow or in KGXCore
	class FrameListener
	{
		public:
			virtual void frameUpdate( double deltaTime ) = 0;

		protected:
			FrameListener() {}
			virtual ~FrameListener() {}

		private:
	};
}
