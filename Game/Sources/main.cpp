#include "irrlicht.h"
#include "fstream"
#include "stdlib.h"
#include "irrKlang.h"

// #pragma comment(lib, "irrlicht.lib")
// #pragma comment(lib, "irrKlang.lib")

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace std;
using namespace irrklang;

#define maxAmmo 10
#define maxTime 6000

int targetCnt = 0, Tm = maxTime, levelNumber = 0;
int points = 0, targetLeft = 0, ammo = maxAmmo;
int Tms = 0, Pnts = 0;

bool endLevel = false;
bool hiscoremnu = false;

char* maps[10];
/*char* musics[1000];
int musicCnt = -1;
int musicNum = 0;*/
vector3df positions[100];
int targets[100] = {-1}, levelCnt, shoots = 0;

ISound* music = 0;

struct TRecord
{
    char name[128];
    int time;
    int points;
};

int hiscoreCnt = -1;
TRecord hiscores[100];

IGUIStaticText* indicator = 0;
ITimer* timer = 0;
IGUIWindow* msgbox = 0;
IGUIListBox* hiscoreTable = 0;

IrrlichtDevice *device = 0;
IVideoDriver* driver = 0;
ISceneManager* smgr = 0;
IGUIEnvironment* guienv = 0;
IAnimatedMesh* levelmesh = 0;
IAnimatedMeshSceneNode* level = 0;
ICameraSceneNode* camera = 0;

IAnimatedMesh* playermesh = 0;
IAnimatedMeshSceneNode* player = 0;

ILightSceneNode* light[10] = {0};

IBillboardSceneNode* bill = 0;
ITriangleSelector* selector = 0;

ISoundEngine* engine = 0;

ISceneNode* target[10] = {0};

ITriangleSelector* trisel = 0;

void gotoMap(int mapNum);
void showHiscores();
void saveHiscores();

class EventReceiver : public IEventReceiver
{
    public:
        virtual bool OnEvent(const SEvent& event)
        {
            if (event.EventType == EET_GUI_EVENT)
            {
                if (event.GUIEvent.EventType == EGET_MESSAGEBOX_OK)
                {
                    if (levelNumber+1 == levelCnt)
                    {
                        saveHiscores();
                        device->drop();
                        engine->drop();

                        exit(0);
                    }

                    if (endLevel == true)
                    {
                        //showHiscores();

                        endLevel = false;
                        gotoMap(++levelNumber);
                        Tm = maxTime;

						return true;
                    }
                }
            }

            if (event.EventType == EET_KEY_INPUT_EVENT)
            {
                if (event.KeyInput.Key == KEY_ESCAPE)
                {
                    saveHiscores();
                    device->drop();
                    engine->drop();

                    exit(0);
                }

                if (event.KeyInput.Key == KEY_RETURN)
                {
                    if (hiscoremnu == true)
                    {
                        hiscoremnu = false;
                    }
                }
            }

            if (event.EventType == EET_MOUSE_INPUT_EVENT)
            {
                ISceneNode* object = 0;

                if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                {
                    object = smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera);

                    if (ammo <= 0)
                    {
                        engine->play2D("Resources/Sounds/noammo.wav", false);
                        return false;
                    }

                    engine->play2D("Resources/Sounds/shot.wav", false);
                    ammo--;
                    shoots++;

                    if (object == level || object == player)
                        return false;

                    for (int i = 0; i <= 9; i++)
                        if (target[i] == object)
                        {
                            object->setVisible(false);

                            points++;
                            targetLeft--;

                            engine->play2D("Resources/Sounds/bell.wav", false);

                            return true;
                        }
                }

                if (event.MouseInput.Event == EMIE_RMOUSE_PRESSED_DOWN)
                {
                    if (ammo < maxAmmo)
                    {
                        ammo = maxAmmo;
                        engine->play2D("Resources/Sounds/reload.wav", false);

						return true;
                    }
                }
            }

            return false;
        }
};

EventReceiver receiver;

void init()
{
    device = createDevice(EDT_OPENGL, dimension2d<s32>(640, 480), 32,
			false, false, false, 0);

	device->setWindowCaption(L"ShootThem!");

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	device->setEventReceiver(&receiver);

	engine = createIrrKlangDevice();

	device->getFileSystem()->addZipFileArchive("Resources/Packs/data.pk3");

	IAnimatedMesh* mesh = 0;
	mesh = smgr->getMesh("chicken.3ds");

	for (int i = 0; i <= 9; i++)
    {
        target[i] = smgr->addAnimatedMeshSceneNode(mesh);
        //target[i]->setScale(vector3df(2, 2, 2));
        target[i]->setVisible(false);

        target[i]->setMaterialTexture(0, driver->getTexture("Chick02.bmp"));
        target[i]->setMaterialFlag(EMF_ANISOTROPIC_FILTER, true);
    }

    bill = smgr->addBillboardSceneNode();
	bill->setMaterialType(EMT_TRANSPARENT_ADD_COLOR );
	bill->setMaterialTexture(0, driver->getTexture("cross.bmp"));
	bill->setMaterialFlag(EMF_LIGHTING, false);
	bill->setMaterialFlag(EMF_ZBUFFER, false);
	bill->setSize(dimension2d<f32>(20.0f, 20.0f));

    smgr->addLightSceneNode(0, vector3df(0, 10, 0), SColorf(0.5f, 0.5f, 0.5f, 0.5f), 1000, 0);

    driver->setFog(SColor(0, 138, 125, 81), true, 250, 1000, 0, true);
}

void createPlayer()
{
    camera = smgr->addCameraSceneNodeFPS(0, 100, 0, 0);
    device->getCursorControl()->setVisible(false);

    stringw str = L"Ammo: ";
    str += ammo;
    str += "/";
    str += maxAmmo;
    str += ";  Points: ";
    str += points;
    str += "/";
    str += targetCnt;

    indicator = guienv->addStaticText(str.c_str(), rect<s32>(10, 10, 260, 22), true, true, 0, 0, true);
    timer = device->getTimer();
    timer->start();

    playermesh = smgr->getMesh("rifle.3ds");
    player = smgr->addAnimatedMeshSceneNode(playermesh);

    player->setPosition(vector3df(0.5f, -1.0f, 1.0f));

    player->setParent(camera);
}

void loadCoords(char* filename)
{
    ifstream inf(filename);

    int i = 0, oldI = 0;

    inf>>levelCnt;

    for (int t = 0; t <= levelCnt - 1; t++)
    {
        inf>>targets[t];

        for (i = oldI; i <= oldI + targets[t] - 1; i++)
            inf>>positions[i].X>>positions[i].Y>>positions[i].Z;

        oldI = i;
    }

    inf.close();
}

void loadHiscores(char* filename)
{
    ifstream inf(filename);

    inf>>hiscoreCnt;

    for (int i = 0; i <= hiscoreCnt - 1; i++)
    {
        inf>>hiscores[i].name;
        inf>>hiscores[i].time;
        inf>>hiscores[i].points;
    }

    inf.close();
}

void saveHiscores()
{
    ofstream outf("Data/hiscores.dat");

    outf<<++hiscoreCnt;

    //hiscores[hiscoreCnt].name = "Player";
    hiscores[hiscoreCnt].time = Tms;
    hiscores[hiscoreCnt].points = Pnts;

    for (int i = 0; i <= hiscoreCnt - 1; i++)
    {
        outf<<hiscores[i].name<<endl;
        outf<<hiscores[i].time<<endl;
        outf<<hiscores[i].points<<endl;
    }

    outf.close();
}

/*void loadMusicList(char* filename)
{
    ifstream inf(filename);

    while (!inf.eof())
        inf.getline(musics[++musicCnt], '\n');

    inf.close();
}*/

void createConfig()
{
    maps[0] = "room1.x";
    maps[1] = "egypt1.x";
    maps[2] = "forest1.x";
    maps[3] = "square1.x";
}

void loadMap(char* mapname)
{
	if (level)
		level->setVisible(false);

    levelmesh = smgr->getMesh(mapname);
    level = smgr->addAnimatedMeshSceneNode(levelmesh);

    selector = smgr->createOctTreeTriangleSelector(levelmesh->getMesh(0), level, 128);
}

void pasteTargets(int levelNumber)
{
    for (int i = 0; i <= targets[levelNumber]-1; i++)
    {
        target[i]->setVisible(true);

        int k = targets[levelNumber-1];

        target[i]->setPosition(positions[k+i]);

        //light[i]->setPosition(vector3df(positions[k+i].X+25, positions[k+i].Y + 25, positions[k+i].Z));
    }

	targetLeft = targets[levelNumber];
	targetCnt = targetLeft;
	points = 0;
	shoots = 0;
}

void gotoMap(int mapNum)
{
	loadMap(maps[levelNumber]);
    pasteTargets(mapNum);

    timer->start();
}

void showResult()
{
    stringw title = L"Level complete!";
    stringw msg = L"Your time: ";

    msg += (maxTime/100) - abs(Tm/100);
    msg += "sec;  Shots: ";
    msg += shoots;
    msg += "/";
    msg += targets[levelNumber];
    msg += " min";
    msg += ";  Target hit: ";
    msg += points;
    msg += "/";
    msg += targets[levelNumber];

    Tms += (maxTime/100) - abs(Tm/100);
    Pnts += points;

    guienv->addMessageBox(title.c_str(), msg.c_str(), true, EMBF_OK, 0, 0);

    endLevel = true;

    timer->stop();

    if (levelNumber + 1 == levelCnt)
    {
        guienv->getSkin()->setFont(guienv->getFont("fontcourier.bmp"));
        guienv->clear();
        guienv->addMessageBox(L"Congratulations!", L"Game over!", true, EMBF_OK, 0, 0);

        while(device->run())
        {
            driver->beginScene(true, true, SColor(0,200,200,200));

            smgr->drawAll();
            guienv->drawAll();

            driver->endScene();
        }
    }
}

void showHiscores()
{
    hiscoremnu = true;

    hiscoreTable = guienv->addListBox(rect<s32>(10, 30, 300, 100), 0, 0, true);

    for (int i = 0; i <= hiscoreCnt-1; i++)
    {
        stringw str = L"";

        str += (i+1);
        str += " - ";
        str += hiscores[i].name;
        str += " - ";
        str += hiscores[i].time;
        str += " sec. - ";
        str += hiscores[i].points;
        str += " pts.";

        hiscoreTable->addItem(str.c_str());
    }

    timer->stop();

    while (hiscoremnu == true && device->run())
    {
        driver->beginScene(true, true, SColor(0,200,200,200));

        smgr->drawAll();
		guienv->drawAll();

        driver->endScene();
    }

    //hiscoreTable->drop();
    timer->start();
}

void refreshIndicator()
{
    stringw str = L"Ammo: ";
    str += ammo;
    str += "/";
    str += maxAmmo;
    str += ";  Points: ";
    str += points;
    str += "/";
    str += targetCnt;
    str += ";  Time:";
    str += (Tm/100);
    str += ";  Level:";
    str += levelNumber+1;
    str += "/";
    str += levelCnt;

    if (timer->isStopped() && endLevel == false)
    {
        Tm--;
    }

    if (Tm <= 0 || points == targetCnt)
        if (endLevel == false)
            showResult();

    indicator->setText(str.c_str());

    float k = (sin(abs(Tm)/100)/(10-levelNumber));

    camera->setRotation(vector3df(camera->getRotation().X+k,
        camera->getRotation().Y, camera->getRotation().Z));

    line3d<f32> line;
	line.start = camera->getPosition();
	line.end = line.start + (camera->getTarget() - line.start).normalize() * 10000.0f;

	vector3df intersection;
	triangle3df tri;

	/*if (engine)
	{
	    if (music->isFinished())
            music = engine->play2D(musics[++musicNum]);

        if (musicNum >= musicCnt)
            musicNum = 0;
	}*/

	if (smgr->getSceneCollisionManager()->getCollisionPoint(line, selector, intersection, tri))
        bill->setPosition(intersection);
}

int main()
{
    init();

    /*IParticleSystemSceneNode* ps = 0;
    ps = smgr->addParticleSystemSceneNode(true, 0, 0, vector3df(0, 5, 5), vector3df(0, 0, 0), vector3df(2, 2, 2));
    ps->setParticleSize(dimension2d<f32>(20.0f, 20.0f));
    ps->setMaterialTexture(0, driver->getTexture("fire.bmp"));*/

    createPlayer();
    createConfig();
	loadCoords("Data/coords.dat");
	loadHiscores("Data/hiscores.dat");
	//loadMusicList("music.dat");

	//music = engine->play2D(musics[0]);

    gotoMap(0);

	while(device->run())
	{
		driver->beginScene(true, true, SColor(0,200,200,200));

		smgr->drawAll();
		guienv->drawAll();

		refreshIndicator();

		driver->endScene();
	}

    saveHiscores();

	device->drop();
	engine->drop();

	timer->stop();

	return 0;
}

