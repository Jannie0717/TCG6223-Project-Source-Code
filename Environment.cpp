#include <windows.h>
#include <GL/gl.h>
#include "Environment.hpp"

using namespace ProjectEnvironment;

Environment::Environment()
{
    skyBoxLoaded = false;
    groundLoaded = false;
}

bool Environment::loadSkyBox(const std::string& filePath)
{
    skyBoxLoaded = skyBoxModel.loadFromObjText(filePath);
    return skyBoxLoaded;
}

bool Environment::loadGround(const std::string& filePath)
{
    groundLoaded = groundModel.loadFromObjText(filePath);
    return groundLoaded;
}

void Environment::drawSkyBox() const
{
    if (!skyBoxLoaded)
        return;

    glPushMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glColor3ub(180, 210, 255);

    // Use your current working skybox transform here
    // Example only:
    glScalef(8.0f, 8.0f, 8.0f);

    skyBoxModel.draw();

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
}

void Environment::drawGround() const
{
    if (!groundLoaded)
        return;

    glPushMatrix();

    // No texture for now
    glDisable(GL_TEXTURE_2D);

    // Ground color for testing
    glColor3ub(120, 170, 100);

    // If the ground does not appear, keep culling disabled.
    glDisable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    //Transformation
    glTranslatef(0.0f, -18.7f, 0.0f);
    glScalef(8.0f, 8.0f, 8.0f);

    groundModel.draw();

    glDisable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    glPopMatrix();
}

void Environment::draw() const
{
    drawSkyBox();
    drawGround();
}
