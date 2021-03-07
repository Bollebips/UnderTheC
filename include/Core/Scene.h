#ifndef SCENE_H
#define SCENE_H

// Entity nextEntityID;

typedef struct Scene Scene;

Scene* SceneNew();
void SceneFree(Scene* scene);

// Entity SceneAddEntity(Scene* scene);
// void SceneRegisterComponent(Scene* scene, char* componentName, size_t componentNameSize, size_t componentSize);
// ComponentID SceneAddComponent(Scene* scene, ComponentTypeID componentTypeID, void* component, Entity entity);

#endif