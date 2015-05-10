
#pragma once

#include <QtWidgets/QMainWindow>

#include "MaterialEditor.h"
#include "Scene.h"
#include "MouseListener.h"
#include "KeyboardListener.h"
#include "FrameListener.h"
#include "ui_KrienGraphiXToolbox.h"

namespace kgxt
{
	class KrienGraphiXToolbox : public QMainWindow, MouseListener, KeyboardListener, FrameListener
	{
		Q_OBJECT

			public slots:
		void openMaterialEditor();

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

	private:
		void setupTestScene();

		Ui::KrienGraphiXToolboxClass m_ui;

		MaterialEditor *m_materialEditorWin;

		kgx::Camera *m_mainCam;
		kgx::Scene *m_defaultScene;

		bool m_leftMouseBtnDown;
		bool m_wKeyDown;
		bool m_sKeyDown;
		bool m_aKeyDown;
		bool m_dKeyDown;
	};
}
