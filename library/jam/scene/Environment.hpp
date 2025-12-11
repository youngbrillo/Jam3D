#pragma once
#include "jam/core/uuid.hpp"
#include <raylib.h>

namespace YAML {
    class Emitter;
    class Node;
}

namespace jam
{
    struct Skybox
    {
        bool enabled= true;
        UUID shader_id = 0;
        UUID texture_id = 0;
        Model model;

        Skybox();
        ~Skybox();

        bool Load();    //loads resources from the resource manager based on the shader and texture_id's
        void Unload();
        void render();

        void serialize(YAML::Emitter& out);
        void deserialize(const YAML::Node& node);
        bool valid() const;
    private:
        bool _valid = false;

    };

    struct Environment
    {
        Skybox skybox;


        void serialize(YAML::Emitter& out);
        void deserialize(const YAML::Node& node);
    };
}