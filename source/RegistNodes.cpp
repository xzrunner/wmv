#include "terrview/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<terrv::Node>("terrv::node")
.property("name", &terrv::Node::GetName, &terrv::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
.property("display", &terrv::Node::GetDisplay, &terrv::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(terrv::Node::STR_PROP_DISPLAY))
)
;

// generator

rttr::registration::class_<terrv::node::BasicNoise>("terrv::basic_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/BasicNoise.parm.h"
#define PARM_NODE_CLASS BasicNoise
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terrv::node::PerlinNoise>("terrv::perlin_noise")
.constructor<>()
#define PARM_FILEPATH "terr/device/PerlinNoise.parm.h"
#define PARM_NODE_CLASS PerlinNoise
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

rttr::registration::class_<terrv::node::PlasmaFractal>("terrv::plasma_fractal")
.constructor<>()
#define PARM_FILEPATH "terr/device/PlasmaFractal.parm.h"
#define PARM_NODE_CLASS PlasmaFractal
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

// natural

rttr::registration::class_<terrv::node::Erosion>("terrv::erosion")
.constructor<>()
#define PARM_FILEPATH "terr/device/Erosion.parm.h"
#define PARM_NODE_CLASS Erosion
#include "terrview/node_rttr_gen.h"
#undef PARM_NODE_CLASS
#undef PARM_FILEPATH
;

}

namespace terrv
{

void nodes_regist_rttr()
{
}

}