
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
	float vv[] = { -15.0, -10.0, 20.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, -15.0, -10.0, -20.0, 1.0, 0.0, 0.0, 0.0, -1.0, 0.0, 15.0, -10.0, -20.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 15.0, -10.0, 20.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, -15.0, 10.0, 20.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 15.0, 10.0, 20.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 15.0, 10.0, -20.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, -15.0, 10.0, -20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -15.0, -10.0, 20.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 15.0, -10.0, 20.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 15.0, 10.0, 20.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -15.0, 10.0, 20.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 15.0, -10.0, 20.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 15.0, -10.0, -20.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 15.0, 10.0, -20.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 15.0, 10.0, 20.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 15.0, -10.0, -20.0, 0.0, 1.0, 0.0, 0.0, 0.0, -1.0, -15.0, -10.0, -20.0, 1.0, 1.0, 0.0, 0.0, 0.0, -1.0, -15.0, 10.0, -20.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, 15.0, 10.0, -20.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, -15.0, -10.0, -20.0, 0.0, 1.0, 0.0, -1.0, 0.0, 0.0, -15.0, -10.0, 20.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, -15.0, 10.0, 20.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, -15.0, 10.0, -20.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0 };
	std::vector<float> v( vv, vv + sizeof(vv) / sizeof(float) );

	UINT ii[] = { 0, 2, 1, 0, 3, 2, 4, 6, 5, 4, 7, 6, 8, 10, 9, 8, 11, 10, 12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 22, 21, 20, 23, 22 };
	std::vector<UINT> i( ii, ii + sizeof(ii) / sizeof(UINT) );

	kgx::VertexInputLayout layout;
	layout.addInputType(kgx::VertexInputLayout::Position);
	layout.addInputType(kgx::VertexInputLayout::TextureCoordinate);
	layout.addInputType(kgx::VertexInputLayout::Normal);


	HRESULT res = E_FAIL;
	kgx::ResourceManager::MeshBufferID buffID = kgx::ResourceManager::getInst()->addMeshBuffer( v, i, layout, res );

	kgx::MeshBuffer mBuff = kgx::ResourceManager::getInst()->getBuffer(buffID);

	// setup material
	kgx::Material *material       = new kgx::Material( kgx::KGXCore::getInst()->getDxDevicePtr() );
	kgx::VertexShader *vertShader = material->createVertexShader( L"DefaultVertexShaderVS.cso", layout );
	kgx::PixelShader *pixShader   = material->createPixelShader( L"DefaultPixelShaderPS.cso" );
	material->addAutoShaderVar( vertShader, "viewMatrix", kgx::Material::CameraViewMatrix );
	material->addAutoShaderVar( vertShader, "projMatrix", kgx::Material::CameraProjectionMatrix );
	material->addAutoShaderVar( vertShader, "modelMatrix", kgx::Material::ObjectModelMatrix );

	kgx::ResourceManager::getInst()->claimMaterial(material);


	// create RenderableObject
	kgx::RenderableObject::ObjectContainer objContainer;
	objContainer.mat = material;
	kgx::RenderableObject::Mesh mesh = { "Object1", 0U, 36U };
	objContainer.meshes.push_back( mesh );
	std::vector<kgx::RenderableObject::ObjectContainer> objs;
	objs.push_back( objContainer );
	kgx::RenderableObject *renObj = new kgx::RenderableObject( kgx::KGXCore::getInst()->getDxDevicePtr(), mBuff, objs, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// add RenderableObject to scene
	defaultScene->claimRenderableObject( renObj );
}


void KrienGraphiXToolbox::openMaterialEditor()
{
	if ( !materialEditorWin )
		materialEditorWin = new MaterialEditor(this);

	materialEditorWin->show();
}
