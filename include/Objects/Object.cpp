#include "Object.hpp"
#include "Engine.hpp"

    std::shared_ptr<Shader> Object::getShader(char* name){
            return Engine::getInstance()->getShader(name);
    };
    std::shared_ptr<Camera> Object::getCurrentCamera(){
            return Engine::getInstance()->getCurrentCamera();
    };