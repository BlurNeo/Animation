#pragma once

#include "glm/glm.hpp"
#include "gravity_soft_body.h"
#include "obj_renderer.h"
#include "soft_world.h"


struct GLInstanceVertex
{
	float xyzw[4];
	float normal[3];
	float uv[2];
};
// Physical obj with rendering
class ObjSoftBody {
 public:
    ObjSoftBody(float mass, std::string obj_path, std::string texture_path) :
            gravity_soft_body(SoftWorld::get()->dynamicsWorld, SoftWorld::get()->m_softBodyWorldInfo) {
        obj_renderer.init(obj_path, texture_path);
        std::vector<float> vertices = obj_renderer.get_vertices();
        std::vector<unsigned int> vertice_idxs = obj_renderer.get_vertice_idxs();
	    gravity_soft_body.init(mass, vertices, vertice_idxs);
    }
    ~ObjSoftBody() {}
    void set_controler(InputControl *_controler) {
        obj_renderer.set_controler(_controler);
    }
    bool init() {
        return true;
    }
    void draw() {
        get_vertices_from_world();
        obj_renderer.draw();
    }
 
 private:
    void get_vertices_from_world() {
        auto rbWorld = SoftWorld::get()->dynamicsWorld;
        int numCollisionObjects = rbWorld->getNumCollisionObjects();
        //B3_PROFILE("writeSingleInstanceTransformToCPU");
        int world_id = gravity_soft_body.get_world_id();
        btCollisionObject* colObj = rbWorld->getCollisionObjectArray()[world_id];
        btCollisionShape* collisionShape = colObj->getCollisionShape();
        printf("collisionShape->getShapeType(): %d", collisionShape->getShapeType());
        printf("SOFTBODY_SHAPE_PROXYTYPE: %d", SOFTBODY_SHAPE_PROXYTYPE);
        printf("collisionShape->getUserIndex(): %d", collisionShape->getUserIndex());
        if (collisionShape->getShapeType() == SOFTBODY_SHAPE_PROXYTYPE && collisionShape->getUserIndex() >= 0)
        {
            const btSoftBody* psb = (const btSoftBody*)colObj;
            btAlignedObjectArray<GLInstanceVertex> gfxVertices;
            
            if (psb->m_renderNodes.size() > 0)
            {
                gfxVertices.resize(psb->m_renderNodes.size());
                for (int i = 0; i < psb->m_renderNodes.size(); i++)  // Foreach face
                {
                    gfxVertices[i].xyzw[0] = psb->m_renderNodes[i].m_x[0];
                    gfxVertices[i].xyzw[1] = psb->m_renderNodes[i].m_x[1];
                    gfxVertices[i].xyzw[2] = psb->m_renderNodes[i].m_x[2];
                    gfxVertices[i].xyzw[3] = psb->m_renderNodes[i].m_x[3];
                    gfxVertices[i].uv[0] = psb->m_renderNodes[i].m_uv1[0];
                    gfxVertices[i].uv[1] = psb->m_renderNodes[i].m_uv1[1];
                    //gfxVertices[i].normal[0] = psb->m_renderNodes[i].
                    gfxVertices[i].normal[0] = psb->m_renderNodes[i].m_normal[0];
                    gfxVertices[i].normal[1] = psb->m_renderNodes[i].m_normal[1];
                    gfxVertices[i].normal[2] = psb->m_renderNodes[i].m_normal[2];
                }
            }
            else
            {
                btAlignedObjectArray<int> indices;
                // computeSoftBodyVertices(collisionShape, gfxVertices, indices);
            }
        }
    }


 private:
    glm::vec3 pos, half_extension;
    GravitySoftBody gravity_soft_body;
    ObjRenderer obj_renderer;
};