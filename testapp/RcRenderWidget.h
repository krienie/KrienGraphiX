
#pragma once

#include <QWidget>

namespace RenderCore::Test
{
    class RcRenderWidget : public QWidget
    {
        Q_OBJECT

        public:
            RcRenderWidget( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
            //~RcRenderWidget();

        protected:
            QPaintEngine* paintEngine() const override;
            void paintEvent( QPaintEvent * ) override {}
            void resizeEvent( QResizeEvent * ) override {}
            void focusInEvent( QFocusEvent * ) override {}
            //void mouseMoveEvent( QMouseEvent *evt ) override;
            //void mousePressEvent( QMouseEvent *evt ) override;
            //void mouseReleaseEvent( QMouseEvent *evt ) override;
            //void wheelEvent( QWheelEvent *evt ) override;
            //void keyPressEvent( QKeyEvent *evt ) override;
            //void keyReleaseEvent( QKeyEvent *evt ) override;
    };
}
