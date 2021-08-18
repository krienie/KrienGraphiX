
#pragma once

#include <QMainWindow>

#include "ui_RenderCoreTestWindow.h"

namespace RenderCore::Test
{
    class RenderCoreTestWindow : public QMainWindow
    {
        Q_OBJECT

        public:
            explicit RenderCoreTestWindow( QWidget *parent = nullptr );
            ~RenderCoreTestWindow();

            //void mouseMoved( const MouseEvent &evt ) override;
            //void mousePressed( const MouseEvent &evt ) override;
            //void mouseReleased( const MouseEvent &evt ) override;
            //void wheelMoved( const MouseEvent &evt ) override;
            //void keyPressed( const KeyEvent &evt ) override;
            //void keyReleased( const KeyEvent &evt ) override;

        private:
            Ui::RenderCoreTestWindow m_ui;
    };
}
