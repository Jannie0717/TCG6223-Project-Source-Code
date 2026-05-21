#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include "ObjModel.hpp"
#include <string>

namespace ProjectEnvironment
{
    class Environment
    {
    private:
        ObjModel skyBoxModel;
        ObjModel groundModel;

        bool skyBoxLoaded;
        bool groundLoaded;

    public:
        Environment();

        bool loadSkyBox(const std::string& filePath);
        bool loadGround(const std::string& filePath);

        void drawSkyBox() const;
        void drawGround() const;
        void draw() const;
    };
}

#endif
