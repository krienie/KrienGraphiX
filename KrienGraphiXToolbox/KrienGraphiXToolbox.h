
#pragma once

#include <QtWidgets/QMainWindow>


#include <Scene.h>

#include "parsers/KGTParserDefines.h"
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
		void createNewProject();
		void openProjectFile();
		void saveProjectFile();
		void saveProjectAsNewFile();
		void setProjectFolder();
		void exitProgram();

	private:
		void loadProject( const std::string &projFile );

		Ui::KrienGraphiXToolboxClass m_ui;

		//KgProjectData m_currentProject;
		std::string m_projectDir;

		kgx::Camera *m_mainCam;
		kgx::Scene *m_defaultScene;

		bool m_leftMouseBtnDown;
		bool m_wKeyDown;
		bool m_sKeyDown;
		bool m_aKeyDown;
		bool m_dKeyDown;
	};
}
