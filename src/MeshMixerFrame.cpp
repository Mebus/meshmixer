// -*- C++ -*- generated by wxGlade 0.6.3 on Fri Nov 14 23:01:51 2008

#include "MeshMixerFrame.h"

#include <wx/filedlg.h>
#include <wx/bitmap.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/wizard.h>
#include <wx/aui/auibook.h>
#include <wx/aui/framemanager.h>
#include <wx/log.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include <assimp.h>
#include <aiScene.h>
#include <aiMesh.h>
#include <aiPostProcess.h>

#include "OgreCamera.h"
#include "OgreColourValue.h"
#include "OgreConfigFile.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"

#include "OgreMeshManager.h"

#include "OgreRoot.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreMesh.h"
#include "OgreVector3.h"
#include "OgreMeshSerializer.h"
#include "OgreSkeletonSerializer.h"


#include "boost/format.hpp"


#include "SceneClientData.h"
#include "MeshMaker.h"
#include "LogPanel.h"
#include "OptionsPanel.h"
#include "ScenePanel.h"
#include "wxOgre.h"


using Ogre::Camera;
using Ogre::ColourValue;
using Ogre::RenderSystemList;
using Ogre::Root;
using Ogre::String;
using Ogre::Vector3;
using Ogre::ConfigFile;

extern const long ID_MAIN_FRAME;

const long ID_FILE_MENU_NEW = wxNewId();
const long ID_FILE_MENU_OPEN = wxNewId();
const long ID_FILE_MENU_SAVE = wxNewId();
const long ID_FILE_MENU_SAVE_AS = wxNewId();
const long ID_FILE_MENU_CLOSE = wxNewId();
const long ID_FILE_MENU_EXIT = wxNewId();
const long ID_VIEW_MENU_FREE_CAMERA = wxNewId();
const long ID_VIEW_MENU_WIREFRAME = wxNewId();
const long ID_SCENE_PANEL = wxNewId();

BEGIN_EVENT_TABLE(MeshMixerFrame, wxFrame)
    EVT_ACTIVATE(MeshMixerFrame::OnActivate)
    EVT_MENU (ID_FILE_MENU_NEW,          MeshMixerFrame::OnFileNew)
    EVT_MENU (ID_FILE_MENU_OPEN,         MeshMixerFrame::OnFileOpen)
    EVT_MENU (ID_FILE_MENU_SAVE,         MeshMixerFrame::OnFileSave)
    EVT_MENU (ID_FILE_MENU_CLOSE,        MeshMixerFrame::OnFileClose)
    EVT_MENU (ID_FILE_MENU_EXIT,         MeshMixerFrame::OnFileExit)
    EVT_MENU (ID_VIEW_MENU_FREE_CAMERA,  MeshMixerFrame::OnViewFreeCamera)
    EVT_MENU (ID_VIEW_MENU_WIREFRAME,    MeshMixerFrame::OnViewWireframe)
	EVT_COMMAND(ID_SCENE_PANEL, SceneEventType, MeshMixerFrame::OnSceneChange)
END_EVENT_TABLE()

MeshMixerFrame::MeshMixerFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
wxFrame(parent, id, title, pos, size, wxDEFAULT_FRAME_STYLE)
{
    mRoot = NULL;
    mMeshNode = NULL;
    mEntity = NULL;	
    mScene = NULL;
	mScenePanel = NULL;
    mMesh.setNull();

    createAuiManager();
    createMenuBar();
    if (!createOgrePane()) {
		throw std::exception();
    }
    createOptionsPane();
    createInformationPane();
    mAuiManager->Update();

}



void MeshMixerFrame::createAuiManager()
{
    mAuiManager = new wxAuiManager();
    mAuiManager->SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG);
    mAuiManager->SetManagedWindow(this);

    wxAuiDockArt* art = mAuiManager->GetArtProvider();
    art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
    art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
    art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 17);
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(49, 106, 197));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(90, 135, 208));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxColour(255, 255, 255));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(200, 198, 183));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(228, 226, 209));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(0, 0, 0));

    mAuiManager->Update();
}

void MeshMixerFrame::createMenuBar()
{
    mMenuBar = new wxMenuBar();

    createFileMenu();
    createViewMenu();
//  createEditMenu();
//  createToolsMenu();
//  createWindowMenu();
//  createHelpMenu();
    SetMenuBar(mMenuBar);
}

void MeshMixerFrame::createFileMenu()
{
    mFileMenu = new wxMenu();


    wxMenuItem *menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_OPEN, wxT("&Open"));
    mFileMenu->Append(menuItem);

    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_SAVE, wxT("&Save"));
    //menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE));
    mFileMenu->Append(menuItem);

    //menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_SAVE_AS, wxT("Save &As..."));
    //menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE_AS));
    //mFileMenu->Append(menuItem);

    mFileMenu->AppendSeparator();

    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_CLOSE, wxT("&Close"));
    //menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::CLOSE));
    mFileMenu->Append(menuItem);

    mFileMenu->UpdateUI();

    mMenuBar->Append(mFileMenu, wxT("&File"));

}


void MeshMixerFrame::createViewMenu()
{
    mViewMenu = new wxMenu();

    wxMenuItem *menuItem = new wxMenuItem(mViewMenu, ID_VIEW_MENU_FREE_CAMERA,
                                          wxT("&Free Camera"),
                                          wxT("Switch between free or orbital camera"),
                                          wxITEM_CHECK);

    mViewMenu->Append(menuItem);

    menuItem = new wxMenuItem(mViewMenu, ID_VIEW_MENU_WIREFRAME, wxT("&Wireframe"),
                              wxT("Switch between Wireframe or solid"),
                              wxITEM_CHECK);
    mViewMenu->Append(menuItem);

    mViewMenu->UpdateUI();

    mMenuBar->Append(mViewMenu, wxT("&View"));

    mViewMenu->Check(ID_VIEW_MENU_FREE_CAMERA, true);

}


bool MeshMixerFrame::createOgrePane()
{
    mRoot = new Ogre::Root();

    // Find Render Systems
    // Testing only, this will be deleted once Projects can tell us
    // which rendering system they want used
#ifndef __unix__
    mDirectXRenderSystem = NULL;
#endif
    mOpenGLRenderSystem = NULL;
    RenderSystemList *rl = mRoot->getAvailableRenderers();
    if (rl->empty())
    {
        wxMessageBox(wxT("No render systems found"), wxT("Error"));
        return false;
    }
    for(RenderSystemList::iterator it = rl->begin(); it != rl->end(); ++it)
    {
        Ogre::RenderSystem *rs = (*it);
        rs->setConfigOption("Full Screen", "No");
        rs->setConfigOption("VSync", "No");
        rs->setConfigOption("Video Mode", "512 x 512 @ 32-bit");

        if(rs->getName() == "OpenGL Rendering Subsystem")
            mOpenGLRenderSystem = *it;
#ifndef __unix__
        else if(rs->getName() == "Direct3D9 Rendering Subsystem")
            mDirectXRenderSystem = *it;
#endif
    }

    // We'll see if there is already and Ogre.cfg, if not we'll
    // default to OpenGL since we know that will work on all
    // platforms
    if(!mRoot->restoreConfig())
    {
        mRoot->setRenderSystem(mOpenGLRenderSystem);
    }

    mOgreControl = new wxOgre(this);
    ConfigFile cf;
    cf.load("resources.cfg");

    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while(seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for(i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }


    wxString caption;
    String rs = mRoot->getRenderSystem()->getName();
    if(rs == "OpenGL Rendering Subsystem") 
		caption = wxT("OGRE - OpenGL");
    else caption = wxT("OGRE - DirectX");

    mAuiManager->AddPane(mOgreControl, wxCENTER, wxT("Ogre Pane"));

    return true;
}

void MeshMixerFrame::createOgreRenderWindow()
{
    mOgreControl->createOgreRenderWindow();

    Ogre::ResourceGroupManager::getSingleton().createResourceGroup("Converted");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation( "../../../converted" , "FileSystem" , "Converted");
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	Ogre::SceneManager *sceneMgr = wxOgre::getSingleton().getSceneManager();    
	Ogre::SceneNode *rootNode = sceneMgr->getRootSceneNode();
	mMeshNode = rootNode->createChildSceneNode();	
    mOgreControl->toggleTimerRendering();

}

void MeshMixerFrame::updateOgre()
{
    mOgreControl->update();
}

void MeshMixerFrame::createInformationPane()
{

    Ogre::LogManager* logMgr = Ogre::LogManager::getSingletonPtr();
    
    mInformationNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER);

    mLogPanel = new LogPanel(mInformationNotebook);
    mInformationNotebook->AddPage(mLogPanel, wxT("Ogre Log"));

    mLogPanel->attachLog(logMgr->getDefaultLog());

    mImportLogPanel = new LogPanel(mInformationNotebook);
    mInformationNotebook->AddPage(mImportLogPanel, wxT("Import Log"));

    Ogre::Log *importLog = logMgr->createLog("Import.log");
    mImportLogPanel->attachLog(importLog);
    
    wxAuiPaneInfo info;
    info.Caption(wxT("Information"));
    info.MaximizeButton(true);
    info.BestSize(256, 128);
    info.Bottom();

    mAuiManager->AddPane(mInformationNotebook, info);
    return;
}

void MeshMixerFrame::createOptionsPane()
{
    mOptionsNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER ); 

    mOptionsPanel = new OptionsPanel(mOptionsNotebook);
    mOptionsNotebook->AddPage(mOptionsPanel, wxT("Post Process Options"));


    wxAuiPaneInfo info;         
    info.Caption(wxT("Options"));
    info.MaximizeButton(true);
    info.BestSize(512,128);
    info.Left();
    mAuiManager->AddPane(mOptionsNotebook, info);
    return;
}
// -- MENU EVENT HANDLERS -----------------------------

void MeshMixerFrame::OnFileNew(wxCommandEvent& event)
{
	if (mScenePanel != NULL)
		mOptionsNotebook->DeletePage(mOptionsNotebook->GetPageIndex(mScenePanel));
	mScenePanel = NULL;
    mOgreControl->resetCamera();
    mMeshNode->detachAllObjects();
    return;
}

void MeshMixerFrame::OnFileOpen(wxCommandEvent& event)
{
	
    wxFileDialog *fd = new wxFileDialog(this, wxT("Open An Asset"));
    if (fd->ShowModal() == wxID_OK)
    {

		std::string meshPath(fd->GetPath().fn_str());
        wxFileName fn(fd->GetPath());
        std::string meshName(fn.GetName().fn_str());
		mWorkingDir = wxString(fn.GetPath());

		if (mScene != NULL) {
			aiReleaseImport(mScene);
			mScene = NULL;
		}
		mScene = aiImportFile( meshPath.c_str(),  mOptionsPanel->getOptions());

        if (!mScene)
        {
            mImportLogPanel->messageLogged( aiGetErrorString() );

        } else {

			if (mScenePanel == NULL) {
				mScenePanel = new ScenePanel(mOptionsNotebook, ID_SCENE_PANEL);		
				mOptionsNotebook->AddPage(mScenePanel, wxT("File Structure"));
			} 				
						
            mImportLogPanel->messageLogged(( boost::format("Read file %s ") % fd->GetPath().c_str() ).str() );
            mImportLogPanel->messageLogged(( boost::format("Animations %d ") % mScene->mNumAnimations).str() );
            mImportLogPanel->messageLogged(( boost::format("Materials %d ") % mScene->mNumMaterials).str() );
            mImportLogPanel->messageLogged(( boost::format("Meshes %d ") %  mScene->mNumMeshes).str() );
            mImportLogPanel->messageLogged(( boost::format("Textures %d ") % mScene->mNumTextures).str() );
			mScenePanel->SetScene(mScene);
        }
    };
    delete fd;
    
}

void MeshMixerFrame::OnFileSave(wxCommandEvent& event)
{
    wxFileDialog *fdlg = new wxFileDialog(this, wxT("Save a mesh"), wxEmptyString, wxEmptyString,
                                          wxT("Ogre Mesh File|*.mesh|Any File|*.*"), wxOVERWRITE_PROMPT | wxFD_SAVE);

	std::vector<Ogre::String> exportedNames;

    if (fdlg->ShowModal() == wxID_OK)
    {
        Ogre::String fname(std::string(fdlg->GetPath().mb_str(wxConvUTF8)));
        wxFileName fn(fdlg->GetPath());
		wxDir fdir(fn.GetPath());
        if (!mMesh.isNull())
        {
            // serialise the materials
            Ogre::Mesh::SubMeshIterator it = mMesh->getSubMeshIterator();
            while(it.hasMoreElements())
            {
                Ogre::SubMesh* sm = it.getNext();
                Ogre::String matName(sm->getMaterialName());
				if (std::find(exportedNames.begin(), exportedNames.end(), matName) == exportedNames.end())
				{
					Ogre::MaterialManager *mmptr = Ogre::MaterialManager::getSingletonPtr();
					Ogre::MaterialPtr materialPtr = mmptr->getByName(matName);
					Ogre::MaterialSerializer ms;
					Ogre::String matPath(Ogre::String(fdir.GetName().fn_str()));
					matPath = matPath + Ogre::String("\\");
					matPath = matPath + matName;
					matPath = matPath + Ogre::String(".material");
					ms.exportMaterial(materialPtr, matPath, true);
					exportedNames.push_back(matName);
				}
            }
            Ogre::MeshSerializer *meshSerializer = new Ogre::MeshSerializer();
            meshSerializer->exportMesh(mMesh.getPointer(), Ogre::String(fn.GetFullPath().fn_str()),  Ogre::Serializer::ENDIAN_NATIVE);
        }

    }

}



void MeshMixerFrame::OnFileClose(wxCommandEvent& event)
{
	wxOgre::getSingleton().destroyOgreRenderWindow();
    this->Close();
}

void MeshMixerFrame::OnFileExit(wxCommandEvent& event)
{
}

void MeshMixerFrame::OnViewFreeCamera(wxCommandEvent& event)
{

    if (event.IsChecked())
    {
        mOgreControl->cameraTrackNode(mMeshNode);
    } else {
        mOgreControl->cameraTrackNode();
    }

}

void MeshMixerFrame::OnViewWireframe(wxCommandEvent& event)
{
    mOgreControl->wireFrame(event.IsChecked());
}

void MeshMixerFrame::OnSceneChange(wxCommandEvent& event)
{
	SceneClientData *info = dynamic_cast<SceneClientData*>( event.GetClientObject() );
	if (info != NULL)
	{
		// yay!
		MeshMaker maker;
		maker.setDirectory(std::string(mWorkingDir.fn_str()));
		maker.setName(std::string(info->meshNode->mName.data));
	    Ogre::LogManager* logMgr = Ogre::LogManager::getSingletonPtr();
		Ogre::Log* importLog = logMgr->getLog("Import.log");
	    maker.setLog(importLog);
		mMesh = maker.createMesh(mScene, info->meshNode);
		if (mEntity != NULL) {
			mMeshNode->detachObject(mEntity->getName());
			wxOgre::getSingleton().getSceneManager()->destroyEntity(mEntity);
			mEntity = NULL;
		}
		mEntity = wxOgre::getSingleton().getSceneManager()->createEntity(Ogre::String(mMesh->getName()) + Ogre::String("Mesh"), mMesh->getName());	 
		mMeshNode->attachObject(mEntity);
	}
}
