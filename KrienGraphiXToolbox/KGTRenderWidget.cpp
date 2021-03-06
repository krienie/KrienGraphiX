
#include "KGTRenderWidget.h"

#include <Windows.h>
#include <QTimer>

#include <kriengraphix/Core/KGXCore.h>

#include "FrameListener.h"
#include "KeyboardListener.h"


namespace kgt
{
    KGTRenderWidget::KGTRenderWidget( QWidget *parent, Qt::WindowFlags f )
        : QWidget( parent, f | Qt::MSWindowsOwnDC ), m_isInit( false ), m_lastFrameTime( 0.0 ),
        m_prevMousePosX( 0.0f ), m_prevMousePosY( 0.0f ), m_mouseListeners(), m_keyboardListeners()
    {
        QPalette colourPalette = palette();
        colourPalette.setColor( QPalette::Active, QPalette::WindowText, Qt::black );
        colourPalette.setColor( QPalette::Active, QPalette::Window, Qt::black );
        setPalette( colourPalette );

        setAttribute( Qt::WA_PaintOnScreen );
        setAttribute( Qt::WA_NoSystemBackground );
        setFocusPolicy( Qt::StrongFocus );
        setMouseTracking( true );
    }

    /*KGTRenderWidget::~KGTRenderWidget()
    {
    }*/

    //for ( auto &frameListener : m_frameListeners )
    //    frameListener->frameUpdate( deltaTime );

    void KGTRenderWidget::toggleFullscreen()
    {
        //if ( m_renderWin )
        //    m_renderWin->setFullscreen( !m_renderWin->isFullscreen() );
    }

    void KGTRenderWidget::addMouseListener( MouseListener *listener )
    {
        m_mouseListeners.push_back( listener );
    }

    void KGTRenderWidget::addKeyboardListener( KeyboardListener *listener )
    {
        m_keyboardListeners.push_back( listener );
    }


    QPaintEngine* KGTRenderWidget::paintEngine() const
    {
        return nullptr;
    }


    void KGTRenderWidget::mouseMoveEvent( QMouseEvent *evt )
    {
        for ( auto &mouseListener : m_mouseListeners )
            mouseListener->mouseMoved( createMouseEvent( evt, MouseEvent::ButtonState::None ) );

        m_prevMousePosX = evt->x();
        m_prevMousePosY = evt->y();
    }

    void KGTRenderWidget::mousePressEvent( QMouseEvent *evt )
    {
        m_prevMousePosX = evt->x();
        m_prevMousePosY = evt->y();

        for ( auto &mouseListener : m_mouseListeners )
            mouseListener->mousePressed( createMouseEvent( evt, MouseEvent::ButtonState::Down ) );
    }

    void KGTRenderWidget::mouseReleaseEvent( QMouseEvent *evt )
    {
        m_prevMousePosX = evt->x();
        m_prevMousePosY = evt->y();

        for ( auto &mouseListener : m_mouseListeners )
            mouseListener->mouseReleased( createMouseEvent( evt, MouseEvent::ButtonState::Up ) );
    }

    void KGTRenderWidget::wheelEvent( QWheelEvent *evt )
    {
        for ( auto &mouseListener : m_mouseListeners )
            mouseListener->wheelEvent( createMouseEvent( evt ) );
    }

    void KGTRenderWidget::keyPressEvent( QKeyEvent *evt )
    {
        int mod = evt->modifiers();
        for ( auto &keyboardListener : m_keyboardListeners )
            keyboardListener->keyPressed( KeyEvent( static_cast<KeyEvent::Key>(evt->key()), KeyEvent::KeyState::Pressed, static_cast<KeyEvent::KeyModifiers>(mod) ) );
    }

    void KGTRenderWidget::keyReleaseEvent( QKeyEvent *evt )
    {
        int mod = evt->modifiers();
        for ( auto &keyboardListener : m_keyboardListeners )
            keyboardListener->keyReleased( KeyEvent( static_cast<KeyEvent::Key>(evt->key()), KeyEvent::KeyState::Released, static_cast<KeyEvent::KeyModifiers>(mod) ) );
    }

    MouseEvent KGTRenderWidget::createMouseEvent( QWheelEvent *qtEvt ) const
    {
        float normX = qtEvt->x() / float( this->width() );
        float normY = qtEvt->y() / float( this->height() );

        MouseEvent::Axis xAxis( qtEvt->x(), m_prevMousePosX - qtEvt->x(), normX );
        MouseEvent::Axis yAxis( qtEvt->y(), m_prevMousePosY - qtEvt->y(), normY );
        MouseEvent::Axis zAxis( qtEvt->angleDelta().y(), qtEvt->angleDelta().y(), 0.0f );

        return MouseEvent( xAxis, yAxis, zAxis, this->width(), this->height(), MouseEvent::Button::MiddleButton, MouseEvent::ButtonState::None );
    }

    MouseEvent KGTRenderWidget::createMouseEvent( QMouseEvent *qtEvt, MouseEvent::ButtonState state )  const
    {
        float normX = qtEvt->x() / float( this->width() );
        float normY = qtEvt->y() / float( this->height() );

        MouseEvent::Axis xAxis( qtEvt->x(), m_prevMousePosX - qtEvt->x(), normX );
        MouseEvent::Axis yAxis( qtEvt->y(), m_prevMousePosY - qtEvt->y(), normY );
        MouseEvent::Axis zAxis( 0, 0, 0.0f );

        return MouseEvent( xAxis, yAxis, zAxis, this->width(), this->height(), qtEvt->button(), state );
    }
}
