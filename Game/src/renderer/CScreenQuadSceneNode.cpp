#include "CScreenQuadSceneNode.h"

CScreenQuadSceneNode::CScreenQuadSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 id)
        : irr::scene::ISceneNode(parent, smgr, id)
{
    irr::core::dimension2d<irr::u32> currentResolution;

    // Here we initialize the vertices of the screen-aligned quad

    currentResolution = smgr->getVideoDriver()->getScreenSize();

    aabb.reset(0, 0, 0);

    irr::f32 shiftX =
            0.5 / currentResolution.Width;  // This small shift is necesary to compensate the texture sampling bias
    irr::f32 shiftY = 0.5 / currentResolution.Height; // It avoids that our effect becomes too blurry.

    vertices[0] = irr::video::S3DVertex2TCoords(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            irr::video::SColor(255, 255, 255, 255), shiftX, shiftY, shiftX, shiftY);
    vertices[1] = irr::video::S3DVertex2TCoords(1.0f, -1.0, 0.0f, 0.0f, 0.0f, -1.0f,
            irr::video::SColor(255, 255, 255, 255), 1.0f + shiftX, shiftY, 1.0f + shiftX, shiftY);
    vertices[2] = irr::video::S3DVertex2TCoords(-1.0f, 1.0, 0.0f, 0.0f, 0.0f, -1.0f,
            irr::video::SColor(255, 255, 255, 255), shiftX, 1.0f + shiftY, shiftX, 1.0f + shiftY);
    vertices[3] = irr::video::S3DVertex2TCoords(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            irr::video::SColor(255, 255, 255, 255), 1.0f + shiftX, 1.0f + shiftY, 1.0f + shiftX, 1.0f + shiftY);

    /**
     * Now we proceed to initialize the appropriate settings for the material we are going to use
     * We can alter these later, but for the time being, initializing them here will do no harm
     */

    material.Lighting = false; // no need for lighting
    material.MaterialType = irr::video::EMT_SOLID; // this will add both first and second textures
    material.BackfaceCulling = false; // not needed, but simplifies things
    setAutomaticCulling(
            irr::scene::EAC_OFF); // we don't need this scene node to be culled because we render it in screen space
    material.ZBuffer = irr::video::ECFN_ALWAYS;
    material.ZWriteEnable = false;
}

CScreenQuadSceneNode::~CScreenQuadSceneNode()
{
}

const irr::core::aabbox3df& CScreenQuadSceneNode::getBoundingBox() const
{
    return aabb;
}

void CScreenQuadSceneNode::OnRegisterSceneNode()
{
    // This method is empty because it is best for us to render this scene node manually.
    // So, it is never really rendered on its own, if we don't tell it to do so
}

void CScreenQuadSceneNode::ChangeMaterialType(irr::video::E_MATERIAL_TYPE newMaterial)
{
    material.MaterialType = newMaterial;
}

void CScreenQuadSceneNode::render()
{
    irr::video::IVideoDriver* driver = getSceneManager()->getVideoDriver();
    irr::core::matrix4 proj;
    irr::u16 indices[] = { 0, 1, 2, 3, 1, 2 }; // A triangle list

    driver->setMaterial(material);

    driver->setTransform(irr::video::ETS_PROJECTION, irr::core::IdentityMatrix);
    driver->setTransform(irr::video::ETS_VIEW, irr::core::IdentityMatrix);
    driver->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);

    driver->drawIndexedTriangleList(&vertices[0], 4, &indices[0], 2);
}

irr::u32 CScreenQuadSceneNode::getMaterialCount()
{
    return 1; // there is only one material
}

irr::video::SMaterial& CScreenQuadSceneNode::getMaterial(irr::u32 i)
{
    return material; // always return the same material
}

void CScreenQuadSceneNode::flipHorizontal()
{
    irr::core::vector2d<irr::f32> temp;

    temp = vertices[2].TCoords;
    vertices[2].TCoords = vertices[0].TCoords;
    vertices[0].TCoords = temp;

    temp = vertices[2].TCoords2;
    vertices[2].TCoords2 = vertices[0].TCoords2;
    vertices[0].TCoords2 = temp;

    temp = vertices[3].TCoords;
    vertices[3].TCoords = vertices[1].TCoords;
    vertices[1].TCoords = temp;

    temp = vertices[3].TCoords2;
    vertices[3].TCoords2 = vertices[1].TCoords2;
    vertices[1].TCoords2 = temp;
}
