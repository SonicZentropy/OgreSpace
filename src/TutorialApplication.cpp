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
using namespace Ogre;

//---------------------------------------------------------------------------
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
	DBG("Creating Scene:");
	mCamera->setPosition(Vec3(1683, 50, 2116));
	mCamera->lookAt(Vec3(1963, 50, 1660));
	mCamera->setNearClipDistance(0.1);

	bool infiniteClip = mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE);

	if (infiniteClip)
	{
		DBG("Infinite clipping");
		mCamera->setFarClipDistance(0);
	}
	else
	{
		DBG("Non-infinite clipping");
		mCamera->setFarClipDistance(50000);
	}

	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	Vec3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	Light* light = mSceneMgr->createLight("TestLight");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(lightdir);
	light->setDiffuseColour(ColourValue::White);
	light->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	ColourValue fadeColor(0.9, 0.9, 0.9);
	mWindow->getViewport(0)->setBackgroundColour(fadeColor);

	//ColourValue fadeColor(1.0, 0.1, 0.1);

	//mSceneMgr->setFog(FOG_LINEAR, fadeColor, 0, 600, 900);
	//mSceneMgr->setFog(FOG_EXP2, fadeColor, 0.002);

	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();

	mTerrainGroup = OGRE_NEW TerrainGroup(
		mSceneMgr,
		Terrain::ALIGN_X_Z,
		513, 12000.0);
	mTerrainGroup->setFilenameConvention(String("terrain"), String("dat"));
	mTerrainGroup->setOrigin(Vec3::ZERO);

	configureTerrainDefaults(light);

	//loop to define all terrains - only one terrain atm
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

	mTerrainGroup->freeTemporaryResources();
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 300, true);
	//mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	//Plane skyPlane;
	//skyPlane.d = 1000;
	//skyPlane.normal = Vec3::NEGATIVE_UNIT_Y;
	//mSceneMgr->setSkyPlane(true, skyPlane, "Examples/SpaceSkyPlane", 1500, 50, true, 1.5, 150, 150);
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
			mInfoLabel->setCaption("Building Terrain...");
		else
			mInfoLabel->setCaption("Updating Terrain...");
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

	return ret;
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
	bool exists = ResourceGroupManager::getSingleton().resourceExists(
			mTerrainGroup->getResourceGroup(),
			filename);

	if (exists)
		mTerrainGroup->defineTerrain(x, y);
	else
	{
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);

		mTerrainsImported = true;
	}
}

void TutorialApplication::initBlendMaps(Terrain* terrain)
{
	Real minHeight0 = 70;
	Real fadeDist0 = 40;
	Real minHeight1 = 70;
	Real fadeDist1 = 15;

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
	mTerrainGlobals->setMaxPixelError(8);
	mTerrainGlobals->setCompositeMapDistance(3000);

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

	importData.layerList[0].worldSize = 100;
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	importData.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	importData.layerList[1].worldSize = 30;
	importData.layerList[1].textureNames.push_back(
		"grass_green-01_diffusespecular.dds");
	importData.layerList[1].textureNames.push_back(
		"grass_green-01_normalheight.dds");
	importData.layerList[2].worldSize = 200;
	importData.layerList[2].textureNames.push_back(
		"growth_weirdfungus-03_diffusespecular.dds");
	importData.layerList[2].textureNames.push_back(
		"growth_weirdfungus-03_normalheight.dds");
}

//---------------------------------------------------------------------------
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
