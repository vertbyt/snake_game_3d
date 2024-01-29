
#ifndef MATERIAL_H
#define MATERIAL_H

namespace material {

struct Material {
  texture::Texture diffuse, specular;
  f32 shine;
};

}

#endif


