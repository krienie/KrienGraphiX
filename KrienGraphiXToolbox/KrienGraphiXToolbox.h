
#pragma once

#include <QtWidgets/QMainWindow>

#include "MaterialEditor.h"
#include "Scene.h"
#include "ui_KrienGraphiXToolbox.h"


class KrienGraphiXToolbox : public QMainWindow
{
	Q_OBJECT

	public slots:
		void openMaterialEditor();

	public:
		KrienGraphiXToolbox( QWidget *parent = nullptr );
		~KrienGraphiXToolbox();

	private:
		void setupTestScene();

		Ui::KrienGraphiXToolboxClass ui;

		MaterialEditor *materialEditorWin;

		kgx::Camera *mainCam;
		kgx::Scene *defaultScene;
};
