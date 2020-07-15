
#pragma once

#include <kriengraphix/Simulation/Scene.h>

#include "MouseListener.h"
#include "KeyboardListener.h"
#include "ui_KrienGraphiXToolbox.h"

namespace kgt
{
    class KrienGraphiXToolbox : public QMainWindow, MouseListener, KeyboardListener
    {
        Q_OBJECT

        public:
            explicit KrienGraphiXToolbox( QWidget *parent = nullptr );
            ~KrienGraphiXToolbox();

            void mouseMoved( const MouseEvent &evt ) override;
            void mousePressed( const MouseEvent &evt ) override;
            void mouseReleased( const MouseEvent &evt ) override;
            void wheelEvent( const MouseEvent &evt ) override;
            void keyPressed( const KeyEvent &evt ) override;
            void keyReleased( const KeyEvent &evt ) override;

        private slots:
            void createNewScene();
            void openSceneFile();
            void saveSceneFile();
            void saveSceneAsNewFile();
            void setProjectFolder();
            void exitProgram();

        private:
            Ui::KrienGraphiXToolboxClass m_ui;

            std::string m_projectDir;

            //kgx::Camera *m_mainCam;

            std::vector<float> m_mouseXFilterBuffer;
            std::vector<float> m_mouseYFilterBuffer;

            bool m_leftMouseBtnDown;
            bool m_wKeyDown;
            bool m_sKeyDown;
            bool m_aKeyDown;
            bool m_dKeyDown;
    };
}
