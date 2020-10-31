#pragma once

#include <irrlicht.h>

class CScreenQuadSceneNode : public irr::scene::ISceneNode
{
public:
    CScreenQuadSceneNode(irr::scene::ISceneNode* parent, irr::scene::ISceneManager* smgr, irr::s32 id);

    ~CScreenQuadSceneNode();

    const irr::core::aabbox3df& getBoundingBox() const;

    void OnRegisterSceneNode();

    void ChangeMaterialType(irr::video::E_MATERIAL_TYPE newMatetrial);

    void render();

    irr::u32 getMaterialCount();

    irr::video::SMaterial& getMaterial(irr::u32 i);

    void flipHorizontal();

protected:
    irr::core::aabbox3df aabb;                 // An axis aligned bounding box; not really needed
    irr::video::SMaterial material;            // The material used to render the SceneNode
    irr::video::S3DVertex2TCoords vertices[4]; // The vertices of the scene node. Normally we wouldn't need more than one set of UV coordinates. But if we are to use the builtin materials, this is necesary
};
