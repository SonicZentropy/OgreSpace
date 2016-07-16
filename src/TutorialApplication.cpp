/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/
Tutorial Framework (for Ogre 1.9)
http://www.ogre3d.org/wiki/
-----------------------------------------------------------------------------
*/

#include "TutorialApplication.h"

using Zen::getTranslatedVec;

TutorialApplication::TutorialApplication()
	: mTerrainGroup(0),
	mTerrainGlobals(0),
	mInfoLabel(0)
{
}

TutorialApplication::~TutorialApplication()
{
}

void TutorialApplication::createScene()
{
	mSceneMgr->setAmbientLight(ColourValue(.0, .0, .0));
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_MODULATIVE);
	
	Vector3 shipPos(0, 600, 0);
	//mCamera->setPosition(0, 900, 0);
	//mCamera->lookAt(shipPos);
	//mCamera->setNearClipDistance(5);

	Vec3 aboveShipPos = getTranslatedVec(shipPos, 0, 200, 0);

	bool infiniteClip =
		mRoot->getRenderSystem()->getCapabilities()->hasCapability(
			RSC_INFINITE_FAR_PLANE);

	if (infiniteClip)
		mCamera->setFarClipDistance(0);
	else
		mCamera->setFarClipDistance(50000);


	Vector3 lightDir(.55, -.8, .75);
	lightDir.normalise();

	Light* light = mSceneMgr->createLight("TestLight");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(lightDir);
	light->setDiffuseColour(ColourValue::White);
	light->setSpecularColour(ColourValue(.4, .4, .4));


	//pointLight->setDirection(0, -1, 0);
	//pointLight->setSpotlightRange(Degree(35), Degree(50));
	// Fog
	//ColourValue fadeColour(.9, .9, .9);
	//mWindow->getViewport(0)->setBackgroundColour(fadeColour);

	//mSceneMgr->setFog(FOG_EXP2, fadeColour, 0.002);

	// Ship entity
	//Entity* ship = mSceneMgr->createEntity("ship", "ship.MESH.mesh");
	Entity* ship = mSceneMgr->createEntity("ship", "ship.mesh");
	ship->setCastShadows(true);
	SceneNode* shipNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("shipNode");
	shipNode->attachObject(ship);
	shipNode->setPosition(shipPos);
	Vec3 scaleV(20, 20, 20);
	shipNode->scale(scaleV);

	SceneNode* cameraNode = mSceneMgr->getSceneNode("shipNode")->createChildSceneNode("cameraNode");
	cameraNode->attachObject(mCamera);
	cameraNode->setPosition(0, 50, 75);
	mCamera->lookAt(shipNode->getPosition());

// 	Light* pointLight = mSceneMgr->createLight("PointLight");
// 	pointLight->setType(Light::LT_POINT);
// 
// 	SceneNode* pointLightNode = mSceneMgr->getSceneNode("shipNode")->createChildSceneNode("lightNode");
// 	pointLightNode->attachObject(pointLight);
// 	pointLightNode->setPosition(0, 200, 0);
// 	pointLight->setDiffuseColour(.1, .1, .99);
// 	pointLight->setSpecularColour(0.1, 0.1, 0.99);
// 	
// 	// Light pyramid
// 	Entity* lightPyr = mSceneMgr->createEntity("LightPyramid", "lightPyramid.mesh");
// 	lightPyr->setCastShadows(false);
// 	SceneNode* lightPyrNode = mSceneMgr->getSceneNode("lightNode")->createChildSceneNode("lightPyramidNode");
// 	lightPyrNode->attachObject(lightPyr);
// 	lightPyrNode->setPosition(0, 100, 0);
// 	lightPyrNode->scale(20, 20, 20);
	// Terrain
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
	
	mTerrainGroup = OGRE_NEW TerrainGroup(
		mSceneMgr,
		Terrain::ALIGN_X_Z,
		513, 12000.0);
	mTerrainGroup->setFilenameConvention(String("terrain"), String("dat"));
	mTerrainGroup->setOrigin(Vector3::ZERO);
	
	configureTerrainDefaults(light);
	
	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);
	
	mTerrainGroup->loadAllTerrains(true);
	
	if (mTerrainsImported)
	{
		TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	
		while (ti.hasMoreElements())
		{
			Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}
	mTerrainGlobals->setCastsDynamicShadows(true);
	
	mTerrainGroup->freeTemporaryResources();

	// Sky Techniques
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, true);
	//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	// Plane plane;
	// plane.d = 1000;
	// plane.normal = Vector3::NEGATIVE_UNIT_Y;

	// mSceneMgr->setSkyPlane(
	//   true, plane, "Examples/SpaceSkyPlane", 1500, 40, true, 1.5, 150, 150);

}

void TutorialApplication::createCamera()
{
	// create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");
	// set its position, direction  
	//mCamera->setPosition(Ogre::Vector3(0, 10, 500));
	//mCamera->lookAt(Ogre::Vector3(0, 0, 0));
	// set the near clip distance
	mCamera->setNearClipDistance(5);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}

void TutorialApplication::createViewports()
{
	Ogre::Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void TutorialApplication::createFrameListener()
{
	BaseApplication::createFrameListener();

	mInfoLabel = mTrayMgr->createLabel(OgreBites::TL_TOP, "TerrainInfo", "", 350);
}

void TutorialApplication::destroyScene()
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;
}

bool TutorialApplication::frameRenderingQueued(const FrameEvent& fe)
{
	bool ret = BaseApplication::frameRenderingQueued(fe);

	if (mTerrainGroup->isDerivedDataUpdateInProgress())
	{
		mTrayMgr->moveWidgetToTray(mInfoLabel, OgreBites::TL_TOP, 0);
		mInfoLabel->show();
	
		if (mTerrainsImported)
			mInfoLabel->setCaption("Building terrain...");
		else
			mInfoLabel->setCaption("Updating textures...");
	}
	else
	{
		mTrayMgr->removeWidgetFromTray(mInfoLabel);
		mInfoLabel->hide();
	
		if (mTerrainsImported)
		{
			mTerrainGroup->saveAllTerrains(true);
			mTerrainsImported = false;
		}
	}

	if (!processUnbufferedInput(fe))
		return false;

	return ret;
}

bool TutorialApplication::processUnbufferedInput(const FrameEvent& fe)
{
	static bool mouseDownLastFrame = false;
	static Ogre::Real toggleTimer = 0.0;
	static Ogre::Real rotate = .13;
	static Ogre::Real move = 500;

	// First toggle method
	bool leftMouseDown = mMouse->getMouseState().buttonDown(OIS::MB_Left);

	if (leftMouseDown && !mouseDownLastFrame)
	{
		Ogre::Light* light = mSceneMgr->getLight("PointLight");
		light->setVisible(!light->isVisible());
	}

	mouseDownLastFrame = leftMouseDown;

	// Second toggle method
	toggleTimer -= fe.timeSinceLastFrame;

	if ((toggleTimer < 0) && mMouse->getMouseState().buttonDown(OIS::MB_Right))
	{
		toggleTimer = .5;

		Ogre::Light* light = mSceneMgr->getLight("PointLight");
		light->setVisible(!light->isVisible());
	}

	// Moving the Ninja
	Ogre::Vector3 dirVec = Ogre::Vector3::ZERO;

	if (mKeyboard->isKeyDown(OIS::KC_I))
		dirVec.z -= move;

	if (mKeyboard->isKeyDown(OIS::KC_K))
		dirVec.z += move;

	if (mKeyboard->isKeyDown(OIS::KC_U))
		dirVec.y += move;

	if (mKeyboard->isKeyDown(OIS::KC_O))
		dirVec.y -= move;

	if (mKeyboard->isKeyDown(OIS::KC_J))
	{
		if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			mSceneMgr->getSceneNode("shipNode")->yaw(Ogre::Degree(9 * rotate));
		else
			dirVec.x -= move;
	}

	if (mKeyboard->isKeyDown(OIS::KC_L))
	{
		if (mKeyboard->isKeyDown(OIS::KC_LSHIFT))
			mSceneMgr->getSceneNode("shipNode")->yaw(Ogre::Degree(-9 * rotate));
		else
			dirVec.x += move;
	}

	mSceneMgr->getSceneNode("shipNode")->translate(
		dirVec * fe.timeSinceLastFrame,
		Ogre::Node::TS_LOCAL);

	return true;
}

void getTerrainImage(bool flipX, bool flipY, Image& img)
{
	img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();

}

void TutorialApplication::defineTerrain(long x, long y)
{
	String filename = mTerrainGroup->generateFilename(x, y);

	bool exists =
		ResourceGroupManager::getSingleton().resourceExists(
			mTerrainGroup->getResourceGroup(),
			filename);

	if (exists)
		mTerrainGroup->defineTerrain(x, y);
	else
	{
		Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);

		mTerrainsImported = true;
	}
}

void TutorialApplication::initBlendMaps(Terrain* terrain)
{
	Real minHeight0 = 70;
	Real fadeDist0 = 400;
	Real minHeight1 = 70;
	Real fadeDist1 = 150;

	TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);

	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();

	for (uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Real val = (height - minHeight0) / fadeDist0;
			val = Math::Clamp(val, (Real)0, (Real)1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Math::Clamp(val, (Real)0, (Real)1);
			*pBlend1++ = val;
		}
	}

	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();

}

void TutorialApplication::configureTerrainDefaults(Light* light)
{
	mTerrainGlobals->setMaxPixelError(0.0001);
	mTerrainGlobals->setCompositeMapDistance(13000);

	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	Terrain::ImportData& importData = mTerrainGroup->getDefaultImportSettings();
	importData.terrainSize = 513;
	importData.worldSize = 12000.0;
	importData.inputScale = 600;
	importData.minBatchSize = 33;
	importData.maxBatchSize = 65;

	importData.layerList.resize(3);
	importData.layerList[0].worldSize = 1000;
	importData.layerList[0].textureNames.push_back(
		"dirt_grayrocky_diffusespecular.dds");
	importData.layerList[0].textureNames.push_back(
		"dirt_grayrocky_normalheight.dds");
	importData.layerList[1].worldSize = 300;
	importData.layerList[1].textureNames.push_back(
		"grass_green-01_diffusespecular.dds");
	importData.layerList[1].textureNames.push_back(
		"grass_green-01_normalheight.dds");
	importData.layerList[2].worldSize = 2000;
	importData.layerList[2].textureNames.push_back(
		"growth_weirdfungus-03_diffusespecular.dds");
	importData.layerList[2].textureNames.push_back(
		"growth_weirdfungus-03_normalheight.dds");

}


//==============================================================================
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch(Exception& e)  {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occurred: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------
