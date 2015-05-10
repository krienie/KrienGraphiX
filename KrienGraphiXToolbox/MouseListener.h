
#pragma once

#include <QMouseEvent>

namespace kgxt
{
	class MouseEvent
	{
		public:
			typedef Qt::MouseButton Button;

			enum class ButtonState
			{
				Up,
				Down,
				None
			};

			struct Axis
			{
				public:
					Axis( int _abs, int _rel, float _norm )
						: absolute(_abs), relative(_rel), normalized(_norm) {}

					int abs() const { return absolute; }
					int rel() const { return relative; }
					float norm() const { return normalized; }

				private:
					int absolute;
					int relative;
					float normalized;
			};

			MouseEvent( Axis xAxis, Axis yAxis, Axis zAxis, int width, int height, Button button, ButtonState state )
				: m_xAxis( xAxis ), m_yAxis( yAxis ), m_zAxis( zAxis ), m_w( width ), m_h( height ), m_b( button ), m_s( state )
			{
			}
			//~MouseEvent();

			Axis X() const { return m_xAxis; }
			Axis Y() const { return m_yAxis; }
			Axis Z() const { return m_zAxis; }
			int width() const { return m_w; }
			int height() const { return m_h; }
			Button button() const { return m_b; }
			ButtonState state() const { return m_s; }

		private:
			Axis m_xAxis;
			Axis m_yAxis;
			Axis m_zAxis;
			int m_w;
			int m_h;
			Button m_b;
			ButtonState m_s;
	};

	class MouseListener
	{
		public:
			virtual void mouseMoved( const MouseEvent &evt )    = 0;
			virtual void mousePressed( const MouseEvent &evt )  = 0;
			virtual void mouseReleased( const MouseEvent &evt ) = 0;
			virtual void wheelEvent( const MouseEvent &evt )    = 0;

		protected:
			MouseListener() {}
			virtual ~MouseListener() {}

		private:
	};
}
