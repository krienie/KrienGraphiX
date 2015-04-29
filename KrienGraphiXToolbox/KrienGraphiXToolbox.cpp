
#include "KgParser.h"

#include "KGXCore.h"
#include "Camera.h"
#include "Material.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "VertexInputLayout.h"
#include "ResourceManager.h"
#include "RenderableObject.h"
#include "KrienGraphiXToolbox.h"


KrienGraphiXToolbox::KrienGraphiXToolbox(QWidget *parent)
	: QMainWindow(parent), materialEditorWin(nullptr), mainCam(nullptr), defaultScene(nullptr)
{
	ui.setupUi(this);


	ui.renderWidget1->initialize();

	mainCam      = new kgx::Camera( DirectX::XM_PIDIV4, 1.0f, 0.001f, 500.0f, 
										DirectX::XMFLOAT3(50.0f, 50.0f, 50.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) );
	defaultScene = new kgx::Scene();
	mainCam->setParentScene(defaultScene);

	ui.renderWidget1->getRenderWindow()->setViewport(mainCam);

	setupTestScene();

	ui.renderWidget1->startRendering();

	QObject::connect( ui.actionMaterial_Editor, &QAction::triggered, this, &KrienGraphiXToolbox::openMaterialEditor );
}

KrienGraphiXToolbox::~KrienGraphiXToolbox()
{
	if ( materialEditorWin )
	{
		materialEditorWin->close();
		delete materialEditorWin;
	}

	if ( mainCam )
		delete mainCam;
	if ( defaultScene )
		delete defaultScene;

	kgx::KGXCore::destroy();
}


void KrienGraphiXToolbox::setupTestScene()
{
	kgx::RenderableObject *renObj = kgx::KgParser::loadKGO("..\\..\\Assets\\box.kgo");

	// add RenderableObject to scene
	defaultScene->claimRenderableObject( renObj );
}


void KrienGraphiXToolbox::openMaterialEditor()
{
	if ( !materialEditorWin )
		materialEditorWin = new MaterialEditor(this);

	materialEditorWin->show();
}
