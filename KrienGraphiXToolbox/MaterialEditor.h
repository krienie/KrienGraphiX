
#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MaterialEditor.h"


class MaterialEditor : public QMainWindow
{
	Q_OBJECT

	public:
		MaterialEditor( QWidget *parent = nullptr );
		~MaterialEditor();

	private:
		void setupTestScene();

		Ui::MaterialEditorWindow ui;
};
