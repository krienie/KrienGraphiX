
#pragma once

#include <QWidget>

#include <RenderWindow.h>

#include "MouseListener.h"

namespace kgxt
{
	class FrameListener;
	class KeyboardListener;

	class KgxRenderWidget : public QWidget
	{
		Q_OBJECT

			public slots:
		void startRendering();

	public:
		KgxRenderWidget( _In_ QWidget *parent = 0, Qt::WindowFlags f = 0 );
		//~KgxRenderWidget();

		void initialize();
		kgx::RenderWindow* getRenderWindow() const;

		void toggleFullscreen();

		void addFrameListener( _In_ FrameListener *listener );
		void addMouseListener( _In_ MouseListener *listener );
		void addKeyboardListener( _In_ KeyboardListener *listener );

	protected:
		QPaintEngine* paintEngine() const;
		void paintEvent( _In_ QPaintEvent *evt ) {}
		void resizeEvent( _In_ QResizeEvent *evt ) {}
		void focusInEvent( _In_ QFocusEvent *evt ) {}
		void mouseMoveEvent( _In_ QMouseEvent *evt );
		void mousePressEvent( _In_ QMouseEvent *evt );
		void mouseReleaseEvent( _In_ QMouseEvent *evt );
		void wheelEvent( _In_ QWheelEvent *evt );
		void keyPressEvent( _In_ QKeyEvent *evt );
		void keyReleaseEvent( _In_ QKeyEvent *evt );

	private:
		MouseEvent createMouseEvent( _In_ QWheelEvent *qtEvt ) const;
		MouseEvent createMouseEvent( _In_ QMouseEvent *qtEvt, MouseEvent::ButtonState state ) const;

		bool m_isInit;
		kgx::RenderWindow *m_renderWin;
		double m_lastFrameTime;
		float m_prevMousePosX;
		float m_prevMousePosY;

		std::vector<FrameListener*> m_frameListeners;
		std::vector<MouseListener*> m_mouseListeners;
		std::vector<KeyboardListener*> m_keyboardListeners;
	};
}
