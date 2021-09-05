
#include "RcRenderWidget.h"

namespace RenderCore::Test
{
    RcRenderWidget::RcRenderWidget( QWidget *parent, Qt::WindowFlags f )
        : QWidget( parent, f | Qt::MSWindowsOwnDC )
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

    QPaintEngine* RcRenderWidget::paintEngine() const
    {
        return nullptr;
    }
}
