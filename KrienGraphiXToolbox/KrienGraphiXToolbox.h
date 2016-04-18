
#pragma once

#include <QtWidgets/QMainWindow>

#include <Scene.h>

#include "MouseListener.h"
#include "KeyboardListener.h"
#include "FrameListener.h"
#include "ui_KrienGraphiXToolbox.h"

namespace kgt
{
	class KrienGraphiXToolbox : public QMainWindow, MouseListener, KeyboardListener, FrameListener
	{
		Q_OBJECT

	public:
		explicit KrienGraphiXToolbox( QWidget *parent = nullptr );
		~KrienGraphiXToolbox();

		void frameUpdate( double deltaTime );
		void mouseMoved( const MouseEvent &evt );
		void mousePressed( const MouseEvent &evt );
		void mouseReleased( const MouseEvent &evt );
		void wheelEvent( const MouseEvent &evt );
		void keyPressed( const KeyEvent &evt );
		void keyReleased( const KeyEvent &evt );

	private slots:
		void createNewScene();
		void openSceneFile();
		void saveSceneFile();
		void saveSceneAsNewFile();
		void setProjectFolder();
		void exitProgram();

	private:
		void loadScene( const std::string &sceneFile );

		Ui::KrienGraphiXToolboxClass m_ui;

		std::string m_projectDir;

		kgx::Camera *m_mainCam;
		kgx::Scene *m_currentScene;

		std::vector<float> m_mouseXFilterBuffer;
		std::vector<float> m_mouseYFilterBuffer;

		bool m_leftMouseBtnDown;
		bool m_wKeyDown;
		bool m_sKeyDown;
		bool m_aKeyDown;
		bool m_dKeyDown;
	};
}
