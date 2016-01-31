
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include <qfiledialog.h>

#include <KGObjectParser.h>
#include <KGXCore.h>
#include <Camera.h>
#include <RenderableObject.h>
#include <PixelShader.h>
#include <TextureManager.h>

#include "parsers/KGTProjectParser.h"
#include "parsers/KGSceneParser.h"
#include "generators/KGTProjectGenerator.h"
#include "generators/KGSceneGenerator.h"
#include "IOManager.h"
#include "KrienGraphiXToolbox.h"

namespace bfs = boost::filesystem;

namespace
{
	const std::string SCENE_FOLDER    = "scenes";
	const std::string RESOURCE_FOLDER = "resources";
}


namespace kgt
{
	KrienGraphiXToolbox::KrienGraphiXToolbox( QWidget *parent )
		: QMainWindow(parent), m_projectDir(), m_mainCam(nullptr),
			m_defaultScene(nullptr), m_leftMouseBtnDown(false), m_wKeyDown(false), m_sKeyDown(false),
			m_aKeyDown(false), m_dKeyDown(false)
	{
		m_ui.setupUi( this );
		m_ui.renderWidget1->initialize();
		m_ui.renderWidget1->addFrameListener( this );
		m_ui.renderWidget1->addMouseListener( this );
		m_ui.renderWidget1->addKeyboardListener( this );

		QRect widgetGeom = m_ui.renderWidget1->geometry();
		float aspectRatio = static_cast<float>(widgetGeom.width()) / widgetGeom.height();

		m_ui.renderWidget1->startRendering();

		QObject::connect( m_ui.actionNew, &QAction::triggered, this, &KrienGraphiXToolbox::createNewProject );
		QObject::connect( m_ui.actionOpen, &QAction::triggered, this, &KrienGraphiXToolbox::openProjectFile );
		QObject::connect( m_ui.actionSave, &QAction::triggered, this, &KrienGraphiXToolbox::saveProjectFile );
		QObject::connect( m_ui.actionSave_as, &QAction::triggered, this, &KrienGraphiXToolbox::saveProjectAsNewFile );
		QObject::connect( m_ui.actionSetProjectFolder, &QAction::triggered, this, &KrienGraphiXToolbox::setProjectFolder );
		QObject::connect( m_ui.actionExit, &QAction::triggered, this, &KrienGraphiXToolbox::exitProgram );
	}

	KrienGraphiXToolbox::~KrienGraphiXToolbox()
	{
		if ( m_defaultScene )
			delete m_defaultScene;

		kgx::KGXCore::destroy();
	}


	void KrienGraphiXToolbox::frameUpdate( double deltaTime )
	{
		if ( !m_mainCam )
			return;

		float speed = static_cast<float>(150.0 * deltaTime);
		
		if ( m_wKeyDown )
			m_mainCam->moveForward( speed );
		if ( m_sKeyDown )
			m_mainCam->moveBackward( speed );
		if ( m_aKeyDown )
			m_mainCam->moveLeft( speed );
		if ( m_dKeyDown )
			m_mainCam->moveRight( speed );
	}


	void KrienGraphiXToolbox::mouseMoved( const MouseEvent &evt )
	{
		if ( m_mainCam && m_leftMouseBtnDown )
		{
			m_mainCam->rotateRight( float(-evt.X().rel()) * 0.5f );
			m_mainCam->rotateUp( float(evt.Y().rel()) * 0.5f );
		}
	}

	void KrienGraphiXToolbox::mousePressed( const MouseEvent &evt )
	{
		if ( evt.button() == MouseEvent::Button::LeftButton )
			m_leftMouseBtnDown = true;
	}

	void KrienGraphiXToolbox::mouseReleased( const MouseEvent &evt )
	{
		if ( evt.button() == MouseEvent::Button::LeftButton )
			m_leftMouseBtnDown = false;
	}

	void KrienGraphiXToolbox::wheelEvent( const MouseEvent &evt )
	{
		//TODO: implement
	}

	void KrienGraphiXToolbox::keyPressed( const KeyEvent &evt )
	{
		switch ( evt.keyCode() )
		{
			case KeyEvent::Key::Key_W:
				m_wKeyDown = true;
				break;
			case KeyEvent::Key::Key_S:
				m_sKeyDown = true;
				break;
			case KeyEvent::Key::Key_A:
				m_aKeyDown = true;
				break;
			case KeyEvent::Key::Key_D:
				m_dKeyDown = true;
				break;
			case KeyEvent::Key::Key_Return:
				if ( (evt.modifiers() & KeyEvent::KeyModifiers::Alt) == KeyEvent::KeyModifiers::Alt )
					m_ui.renderWidget1->toggleFullscreen();
				break;
			default:
				break;
		}
	}

	void KrienGraphiXToolbox::keyReleased( const KeyEvent &evt )
	{
		switch ( evt.keyCode() )
		{
			case KeyEvent::Key::Key_W:
				m_wKeyDown = false;
				break;
			case KeyEvent::Key::Key_S:
				m_sKeyDown = false;
				break;
			case KeyEvent::Key::Key_A:
				m_aKeyDown = false;
				break;
			case KeyEvent::Key::Key_D:
				m_dKeyDown = false;
				break;
			default:
				break;
		}
	}


	void KrienGraphiXToolbox::createNewProject()
	{
		std::cout << "Error (KrienGraphiXToolbox::createNewProject): Not implemented." << std::endl;
	}
	void KrienGraphiXToolbox::openProjectFile()
	{
		std::cout << "Error (KrienGraphiXToolbox::openProjectFile): Not implemented." << std::endl;
	}
	void KrienGraphiXToolbox::saveProjectFile()
	{
		std::cout << "Error (KrienGraphiXToolbox::saveProjectFile): Not implemented." << std::endl;
	}
	void KrienGraphiXToolbox::saveProjectAsNewFile()
	{
		std::cout << "Error (KrienGraphiXToolbox::saveProjectAsNewFile): Not implemented." << std::endl;
	}

	void KrienGraphiXToolbox::setProjectFolder()
	{
		std::string selectedDir = QFileDialog::getExistingDirectory( this, tr( "Select project directory" ), "" ).toStdString();

		if ( !bfs::is_directory( selectedDir ) )
		{
			std::cout << "Error (KrienGraphiXToolbox::setProjectFolder): Not a valid directory: " << selectedDir << std::endl;
			return;
		}
		m_projectDir = selectedDir;

		// create scene folders
		bfs::path scenePath = bfs::path( m_projectDir ).append( SCENE_FOLDER );
		if ( !bfs::exists(scenePath) )
			bfs::create_directory( scenePath );

		// create resource folder
		bfs::path resourcePath = bfs::path( m_projectDir ).append( RESOURCE_FOLDER );
		if ( !bfs::exists( resourcePath ) )
			bfs::create_directory( resourcePath );


	}

	void KrienGraphiXToolbox::exitProgram()
	{
		this->close();
	}


	void KrienGraphiXToolbox::loadProject( const std::string &projFile )
	{
		// unload current scene
		if ( m_defaultScene )
			delete m_defaultScene;
		kgx::KGXCore::getInst()->clearManagers();


		// parse and load project file
		KgProjectData projData;
		if ( !KGTProjectParser::loadKGProject( projFile, projData ) )
		{
			std::cout << "Error (KrienGraphiXToolbox::loadProject): Error parsing KgProject file " << projFile << std::endl;
			return;
		}

		std::cout << "Loaded project: " << projData.projectName << ": " << projData.sceneFile << std::endl;

		// set project folder
		bfs::path projectPath( projFile );
		kgx::IOManager::getInst()->addSearchPath( projectPath.parent_path().string() );

		// set window title
		std::stringstream titleSS;
		titleSS << "KrienGraphiX Toolbox - " << projData.projectName;
		setWindowTitle( QString( titleSS.str().c_str() ) );

		// parse and load scene file
		m_defaultScene = kgx::KGSceneParser::loadKGScene( projData.sceneFile, m_ui.renderWidget1->getRenderWindow() );
		if ( m_defaultScene && m_defaultScene->getDefaultCamera() )
		{
			m_mainCam = m_defaultScene->getDefaultCamera();

			//TODO: change interface for setting output for a camera below to something like: m_mainCam->setRenderTarget( m_ui.renderWidget1->getRenderWindow() );
			m_ui.renderWidget1->getRenderWindow()->setViewport( m_mainCam );
		}
	}
}
