// (c) 2016 ZaKlaus; All Rights Reserved

#include "hftw.h"

#include "Shlwapi.h"

#include "common.hpp"
#include "vec3.hpp"
#include "mat4x4.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/quaternion.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/matrix_access.hpp"
#include "gtx/transform.hpp"
#include "gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "f3d_camera.h"
#include "f3d_async.h"
#include "f3d_asset.h"
#include "f3d_window.h"
#include "f3d_texture.h"
#include "f3d_render.h"
#include "f3d_shader.h"
#include "f3d_render_4ds.h"
#include "f3d_scene.h"
                                                                   