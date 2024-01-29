
#ifndef LIGHT_H
#define LIGHT_H

namespace light {

struct Light {
  Vec3 dir;
  Vec3 ambient, diffuse, specular;
};

}

#endif

