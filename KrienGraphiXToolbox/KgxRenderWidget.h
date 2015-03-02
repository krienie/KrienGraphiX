
#pragma once

#include "RenderWindow.h"

#include <QWidget>

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

	protected:
		QPaintEngine *paintEngine() const;
		void paintEvent( _In_ QPaintEvent *evt ) { }
		void resizeEvent( _In_ QResizeEvent *evt );
		void focusInEvent( _In_ QFocusEvent *evt );
		/*void mouseMoveEvent( _In_ QMouseEvent *evt );
		void mousePressEvent( _In_ QMouseEvent *evt );
		void mouseReleaseEvent( _In_ QMouseEvent *evt );
		void wheelEvent( _In_ QWheelEvent *evt );
		void keyPressEvent( _In_ QKeyEvent *evt );
		void keyReleaseEvent( _In_ QKeyEvent *evt );*/

	private:
		bool isInit;
		kgx::RenderWindow *renderWin;
};
