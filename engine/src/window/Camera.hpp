#pragma once

#include <vector>
#include <glm/glm.hpp>

using glm::vec3;

class Camera {
    private:
        float fov = 45.0f;
        float near = 0.1f;
        float far = 100.0f;
        vec3 position = vec3(0.0f, 0.0f, 0.0f);
        vec3 lookingAt = vec3(0.0f, 0.0f, 0.0f);
        vec3 up = vec3(0.0f, 1.0f, 0.0f);
    public:
        Camera(float fov = 45.0f, float near = 0.1f, float far = 100.0f,
               vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 lookingAt = vec3(0.0f, 0.0f, 0.0f),
               vec3 up = vec3(0.0f, 1.0f, 0.0f))
            : fov(fov), near(near), far(far), position(position), lookingAt(lookingAt), up(up) {}
};