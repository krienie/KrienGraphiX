
#pragma once

#include <QWidget>

#include <RenderWindow.h>

#include "MouseListener.h"

namespace kgt
{
	class FrameListener;
	class KeyboardListener;

	class KGTRenderWidget : public QWidget
	{
		Q_OBJECT

			public slots:
		void startRendering();

	public:
		KGTRenderWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
		//~KGTRenderWidget();

		void initialize();
		kgx::RenderWindow* getRenderWindow() const;

		void toggleFullscreen();

		void addFrameListener( FrameListener *listener );
		void addMouseListener( MouseListener *listener );
		void addKeyboardListener( KeyboardListener *listener );

	protected:
		QPaintEngine* paintEngine() const;
		void paintEvent( QPaintEvent *evt ) {}
		void resizeEvent( QResizeEvent *evt ) {}
		void focusInEvent( QFocusEvent *evt ) {}
		void mouseMoveEvent( QMouseEvent *evt );
		void mousePressEvent( QMouseEvent *evt );
		void mouseReleaseEvent( QMouseEvent *evt );
		void wheelEvent( QWheelEvent *evt );
		void keyPressEvent( QKeyEvent *evt );
		void keyReleaseEvent( QKeyEvent *evt );

	private:
		MouseEvent createMouseEvent( QWheelEvent *qtEvt ) const;
		MouseEvent createMouseEvent( QMouseEvent *qtEvt, MouseEvent::ButtonState state ) const;

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
