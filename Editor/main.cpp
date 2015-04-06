#include "irrlicht.h"
#include "fstream"

#pragma comment(lib, "irrlicht.lib")

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

vector3df point[100];
int pointCnt = 0;
char* fileout = "";

IrrlichtDevice *device = 0;
IVideoDriver* driver = 0;
ISceneManager* smgr = 0;
IGUIEnvironment* guienv = 0;
IGUIStaticText* text = 0;
IAnimatedMesh* mesh = 0;
IAnimatedMeshSceneNode* node = 0;
ICameraSceneNode* camera = 0;
ILightSceneNode* light = 0;

void saveData(char* filename);

class EventReceiver : public IEventReceiver
{
    public:
        virtual bool OnEvent(const SEvent& event)
        {
            if (event.EventType == EET_MOUSE_INPUT_EVENT)
            {
                if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
                {
                    point[pointCnt++] = camera->getPosition();

                    smgr->addSphereSceneNode(10, 64, 0, 0, camera->getPosition(),
                        vector3df(0, 0, 0), vector3df(1, 1, 1));

                    smgr->addLightSceneNode(0, point[pointCnt], SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);
                }
            }

            if (event.EventType == EET_KEY_INPUT_EVENT)
            {
                if (event.KeyInput.Key == KEY_ESCAPE)
                {
                    device->drop();

                    saveData(fileout);

                    exit(1);
                }

                if (event.KeyInput.Key == KEY_F2)
                {
                    saveData(fileout);
                }
            }

            return false;
        }
};

void saveData(char* filename)
{
    ofstream outf(filename);

    outf<<pointCnt<<endl;

    for (int i = 0; i <= pointCnt-1; i++)
        outf<<point[i].X<<" "<<point[i].Y<<" "<<point[i].Z<<endl;

    outf.close();
}

int main(int argc, char* argv[])
{
    fileout = argv[2];

    EventReceiver receiver;

	device = createDevice(EDT_OPENGL, dimension2d<s32>(640, 480), 32,
			false, false, false, 0);

    device->setEventReceiver(&receiver);

	device->setWindowCaption(L"Shoot Them! Editor");

	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	light = smgr->addLightSceneNode(0, vector3df(0, 0, 0), SColorf(0.5f, 0.5f, 0.5f, 0), 50, 0);

	device->getCursorControl()->setVisible(false);

	text = guienv->addStaticText(L"", rect<int>(10,10,200,22), true);

    mesh = smgr->getMesh(argv[1]);
    node = smgr->addAnimatedMeshSceneNode(mesh);

	camera = smgr->addCameraSceneNodeFPS();

	while(device->run())
	{
		driver->beginScene(true, true, SColor(0,200,200,200));

		smgr->drawAll();
		guienv->drawAll();

		stringw str = L"Points: ";
		str += pointCnt;
		text->setText(str.c_str());

		light->setPosition(camera->getPosition());

		driver->endScene();
	}

	device->drop();

	return 0;
}

