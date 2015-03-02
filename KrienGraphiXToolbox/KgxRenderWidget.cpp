
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <Windows.h>

#include <QTimer>

#include "KGXCore.h"
#include "KgxRenderWidget.h"

KgxRenderWidget::KgxRenderWidget( _In_ QWidget *parent, Qt::WindowFlags f )
	: QWidget(parent, f | Qt::MSWindowsOwnDC), isInit(false), renderWin(0)
{
	QPalette colourPalette = palette();
	colourPalette.setColor(QPalette::Active, QPalette::WindowText, Qt::black);
	colourPalette.setColor(QPalette::Active, QPalette::Window, Qt::black);
	setPalette(colourPalette);
}

/*KgxRenderWidget::~KgxRenderWidget()
{
}*/


void KgxRenderWidget::startRendering()
{
	if ( !isInit )
		return;

	renderWin->update();

	//TODO: add FPS upper limit
	//TODO: kijken of dit beter kan.. of iig op een andere manier. Ziet er beetje habby-babby uit...
	QTimer::singleShot( 0, this, SLOT(startRendering()) );
}


void KgxRenderWidget::initialize()
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);

	//Accept input focus
	setFocusPolicy(Qt::StrongFocus);

	renderWin = kgx::KGXCore::getInst()->createRenderWindow( HWND(winId()) );

	setMouseTracking(true);

	isInit = true;
}


kgx::RenderWindow* KgxRenderWidget::getRenderWindow() const
{
	return renderWin;
}



QPaintEngine *KgxRenderWidget::paintEngine() const
{
	return 0;
}

void KgxRenderWidget::resizeEvent( _In_ QResizeEvent *evt )
{
	/*if( renderWin )
	{
		renderWin->resize(width(), height());
		renderWin->windowMovedOrResized();

		for(int ct = 0; ct < renderWin->getNumViewports(); ++ct)
		{
			Ogre::Viewport* pViewport = renderWin->getViewport(ct);
			Ogre::Camera* pCamera = pViewport->getCamera();
			pCamera->setAspectRatio(static_cast<Ogre::Real>(pViewport->getActualWidth()) / static_cast<Ogre::Real>(pViewport->getActualHeight()));
		}
	}*/
}

void KgxRenderWidget::focusInEvent( _In_ QFocusEvent *evt )
{
	/*const int NUM_KEY_STATES = 3;
	short keyStates[NUM_KEY_STATES] = { VK_MENU, VK_SHIFT, VK_CONTROL };
	int keyInt[NUM_KEY_STATES]      = { Qt::Key::Key_Alt, Qt::Key::Key_Shift, Qt::Key::Key_Control };
		
	for ( int i = 0; i < NUM_KEY_STATES; ++i )
	{
		bool altDown = GetKeyState(keyStates[i]) >> 7;

		QKeyEvent *pKeyEvt = 0;
		ButtonState bState = ButtonState::Up;
		if ( altDown )
		{
			pKeyEvt = new QKeyEvent( QEvent::Type::KeyPress, keyInt[i], Qt::KeyboardModifier::NoModifier );
			bState  = ButtonState::Down;
		} else
		{
			pKeyEvt = new QKeyEvent( QEvent::Type::KeyRelease, keyInt[i], Qt::KeyboardModifier::NoModifier );
		}
		std::vector<InputListener*>::iterator it;
		for ( it = inputListeners.begin(); it != inputListeners.end(); ++it )
			(*it)->keyboardEvent( pKeyEvt, bState );

		delete pKeyEvt;
	}*/
} 
