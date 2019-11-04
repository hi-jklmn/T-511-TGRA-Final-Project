#ifndef ASSMAN_H
#define ASSMAN_H

#include <string>
#include <GL/gl.h>
#include <cstdio>
#include "stb_image.h"

struct Texture {
    unsigned int id;

    enum class Type {
        DIFFUSE,
        NORMAL,
        SPECULAR,
        UNSET
    };

    Texture::Type type = Texture::Type::UNSET;
    std::string path;

    static Texture FromPath(std::string path) {
        int w, h, dim;
        unsigned char* img_data = stbi_load(path.c_str(), &w, &h, &dim, 0);

        Texture t;

        if (img_data == NULL) {
            fprintf(stderr, "Error: Texture at path \'%s\' failed to load.\n", path.c_str());
            stbi_image_free(img_data);
            return t;
        }

        GLenum format;

        switch (dim) {
        case 1:
            format = GL_RED;  break;
        case 3:
            format = GL_RGB;  break;
        case 4:
            format = GL_RGBA; break;
        default:
            fprintf(stderr, "Error: could not determine pixel format for \'%s\', defaulting to GL_RGB", path.c_str());
            format = GL_RGB;  break;
        }

        glGenTextures(1, &t.id);
        glBindTexture(GL_TEXTURE_2D, t.id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, img_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(img_data);

        t.path = path;

        return t;
    }

    static Texture DiffuseFromPath(std::string path) {
        Texture t = Texture::FromPath(path);
        t.type = Texture::Type::DIFFUSE;
        return t;
    }

    static Texture SpecularFromPath(std::string path) {
        Texture t = Texture::FromPath(path);
        t.type = Texture::Type::SPECULAR;
        return t;
    }

    static Texture NormalFromPath(std::string path) {
        Texture t = Texture::FromPath(path);
        t.type = Texture::Type::NORMAL;
        return t;
    }
};

#endif
