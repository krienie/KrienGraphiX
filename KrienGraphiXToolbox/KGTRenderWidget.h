
#pragma once

#include <QWidget>

#include "RenderWindow.h"
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
		KGTRenderWidget( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
		//~KGTRenderWidget();

		void initialize();
		kgx::RenderWindow* getRenderWindow() const;

		void toggleFullscreen();

		void addFrameListener( FrameListener *listener );
		void addMouseListener( MouseListener *listener );
		void addKeyboardListener( KeyboardListener *listener );

	protected:
		QPaintEngine* paintEngine() const override;
		void paintEvent( QPaintEvent *evt ) override {}
		void resizeEvent( QResizeEvent *evt ) override {}
		void focusInEvent( QFocusEvent *evt ) override {}
		void mouseMoveEvent( QMouseEvent *evt ) override;
		void mousePressEvent( QMouseEvent *evt ) override;
		void mouseReleaseEvent( QMouseEvent *evt ) override;
		void wheelEvent( QWheelEvent *evt ) override;
		void keyPressEvent( QKeyEvent *evt ) override;
		void keyReleaseEvent( QKeyEvent *evt ) override;

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
