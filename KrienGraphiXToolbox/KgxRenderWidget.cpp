
#include <Windows.h>
#include <QTimer>

#include <KGXCore.h>

#include "FrameListener.h"
#include "KeyboardListener.h"
#include "KgxRenderWidget.h"


namespace kgxt
{
	KgxRenderWidget::KgxRenderWidget( _In_ QWidget *parent, Qt::WindowFlags f )
		: QWidget(parent, f | Qt::MSWindowsOwnDC), m_isInit(false), m_renderWin(0), m_lastFrameTime(0.0),
		m_prevMousePosX( 0.0f ), m_prevMousePosY( 0.0f ), m_frameListeners(), m_mouseListeners(), m_keyboardListeners()
	{
		QPalette colourPalette = palette();
		colourPalette.setColor( QPalette::Active, QPalette::WindowText, Qt::black );
		colourPalette.setColor( QPalette::Active, QPalette::Window, Qt::black );
		setPalette( colourPalette );
	}

	/*KgxRenderWidget::~KgxRenderWidget()
	{
	}*/


	void KgxRenderWidget::startRendering()
	{
		if ( !m_isInit )
			return;

		double currentTime = static_cast<double>(timeGetTime());
		double deltaTime   = (currentTime - m_lastFrameTime) * 0.001;

		std::vector<FrameListener*>::iterator it;
		for ( it = m_frameListeners.begin(); it != m_frameListeners.end(); ++it )
			(*it)->frameUpdate( deltaTime );

		m_renderWin->update();

		m_lastFrameTime = currentTime;

		//TODO: add FPS upper limit
		//TODO: kijken of dit beter kan.. of iig op een andere manier. Ziet er beetje habby-babby uit...
		QTimer::singleShot( 0, this, SLOT( startRendering() ) );
	}


	void KgxRenderWidget::initialize()
	{
		setAttribute( Qt::WA_PaintOnScreen );
		setAttribute( Qt::WA_NoSystemBackground );

		//Accept input focus
		setFocusPolicy( Qt::StrongFocus );

		//TODO: might want to force the user to explicitly initialize KGXCore..
		m_renderWin = kgx::KGXCore::getInst()->createRenderWindow( HWND( winId() ) );

		setMouseTracking( true );

		m_lastFrameTime = static_cast<double>(timeGetTime());

		m_isInit = true;
	}


	kgx::RenderWindow* KgxRenderWidget::getRenderWindow() const
	{
		return m_renderWin;
	}

	void KgxRenderWidget::toggleFullscreen()
	{
		if ( m_renderWin )
			m_renderWin->setFullscreen( !m_renderWin->isFullscreen() );
	}

	void KgxRenderWidget::addFrameListener( _In_ FrameListener *listener )
	{
		m_frameListeners.push_back( listener );
	}

	void KgxRenderWidget::addMouseListener( _In_ MouseListener *listener )
	{
		m_mouseListeners.push_back( listener );
	}

	void KgxRenderWidget::addKeyboardListener( _In_ KeyboardListener *listener )
	{
		m_keyboardListeners.push_back( listener );
	}


	QPaintEngine* KgxRenderWidget::paintEngine() const
	{
		return 0;
	}


	void KgxRenderWidget::mouseMoveEvent( _In_ QMouseEvent *evt )
	{
		std::vector<MouseListener*>::iterator it;
		for ( it = m_mouseListeners.begin(); it != m_mouseListeners.end(); ++it )
			(*it)->mouseMoved( createMouseEvent( evt, MouseEvent::ButtonState::None ) );

		m_prevMousePosX = evt->x();
		m_prevMousePosY = evt->y();

	}

	void KgxRenderWidget::mousePressEvent( _In_ QMouseEvent *evt )
	{
		m_prevMousePosX = evt->x();
		m_prevMousePosY = evt->y();

		std::vector<MouseListener*>::iterator it;
		for ( it = m_mouseListeners.begin(); it != m_mouseListeners.end(); ++it )
			(*it)->mousePressed( createMouseEvent( evt, MouseEvent::ButtonState::Down ) );
	}

	void KgxRenderWidget::mouseReleaseEvent( _In_ QMouseEvent *evt )
	{
		m_prevMousePosX = evt->x();
		m_prevMousePosY = evt->y();

		std::vector<MouseListener*>::iterator it;
		for ( it = m_mouseListeners.begin(); it != m_mouseListeners.end(); ++it )
			(*it)->mouseReleased( createMouseEvent( evt, MouseEvent::ButtonState::Up ) );
	}

	void KgxRenderWidget::wheelEvent( _In_ QWheelEvent *evt )
	{
		std::vector<MouseListener*>::iterator it;
		for ( it = m_mouseListeners.begin(); it != m_mouseListeners.end(); ++it )
			(*it)->wheelEvent( createMouseEvent( evt ) );
	}

	void KgxRenderWidget::keyPressEvent( _In_ QKeyEvent *evt )
	{
		int mod = evt->modifiers();

		std::vector<KeyboardListener*>::iterator it;
		for ( it = m_keyboardListeners.begin(); it != m_keyboardListeners.end(); ++it )
			(*it)->keyPressed( KeyEvent(static_cast<KeyEvent::Key>(evt->key()), KeyEvent::KeyState::Pressed, static_cast<KeyEvent::KeyModifiers>(mod)) );
	}

	void KgxRenderWidget::keyReleaseEvent( _In_ QKeyEvent *evt )
	{
		int mod = evt->modifiers();

		std::vector<KeyboardListener*>::iterator it;
		for ( it = m_keyboardListeners.begin(); it != m_keyboardListeners.end(); ++it )
			(*it)->keyReleased( KeyEvent( static_cast<KeyEvent::Key>(evt->key()), KeyEvent::KeyState::Released, static_cast<KeyEvent::KeyModifiers>(mod) ) );
	}

	MouseEvent KgxRenderWidget::createMouseEvent( _In_ QWheelEvent *qtEvt ) const
	{
		float normX = qtEvt->x() / float( this->width() );
		float normY = qtEvt->y() / float( this->height() );

		MouseEvent::Axis xAxis( qtEvt->x(), m_prevMousePosX - qtEvt->x(), normX );
		MouseEvent::Axis yAxis( qtEvt->y(), m_prevMousePosY - qtEvt->y(), normY );
		MouseEvent::Axis zAxis( qtEvt->angleDelta().y(), qtEvt->angleDelta().y(), 0.0f );

		return MouseEvent( xAxis, yAxis, zAxis, this->width(), this->height(), MouseEvent::Button::MiddleButton, MouseEvent::ButtonState::None );
	}

	MouseEvent KgxRenderWidget::createMouseEvent( _In_ QMouseEvent *qtEvt, MouseEvent::ButtonState state )  const
	{
		float normX = qtEvt->x() / float( this->width() );
		float normY = qtEvt->y() / float( this->height() );

		MouseEvent::Axis xAxis( qtEvt->x(), m_prevMousePosX - qtEvt->x(), normX );
		MouseEvent::Axis yAxis( qtEvt->y(), m_prevMousePosY - qtEvt->y(), normY );
		MouseEvent::Axis zAxis( 0, 0, 0.0f );

		return MouseEvent( xAxis, yAxis, zAxis, this->width(), this->height(), qtEvt->button(), state );
	}
}
