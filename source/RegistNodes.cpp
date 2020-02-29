#include "terrainlab/RegistNodes.h"

#include <ee0/ReflectPropTypes.h>

#include <js/RTTR.h>

RTTR_REGISTRATION
{

// base

rttr::registration::class_<terrainlab::Node>("terrainlab::node")
.property("name", &terrainlab::Node::GetName, &terrainlab::Node::SetName)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Name"))
)
.property("display", &terrainlab::Node::GetDisplay, &terrainlab::Node::SetDisplay)
(
	rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo(terrainlab::Node::STR_PROP_DISPLAY))
)
;

#define EXE_FILEPATH "terrainlab/node_rttr_gen.h"
#define SKIP_FILE_NODE
#include "terrainlab/node_regist_cfg.h"
#undef SKIP_FILE_NODE
#undef EXE_FILEPATH

rttr::registration::class_<terrainlab::node::FileInput>("terrainlab::file_input")
    .constructor<>()
    .property("filepath", &terrainlab::node::FileInput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

rttr::registration::class_<terrainlab::node::HeightOutput>("terrainlab::height_output")
    .constructor<>()
    .property("filepath", &terrainlab::node::HeightOutput::m_filepath)
    (
	    rttr::metadata(ee0::UIMetaInfoTag(), ee0::UIMetaInfo("Filepath")),
        rttr::metadata(js::RTTR::FilePathTag(), true),
        rttr::metadata(ee0::PropOpenFileTag(), ee0::PropOpenFile("*.*"))
    )
;

}

namespace terrainlab
{

void nodes_regist_rttr()
{
}

}