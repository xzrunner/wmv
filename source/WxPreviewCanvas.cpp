#include "wmv/WxPreviewCanvas.h"
#include "wmv/PreviewPage.h"
#include "wmv/WxGraphPage.h"
#include "wmv/Evaluator.h"
#include "wmv/MessageID.h"
#include "wmv/GrayRenderer.h"
#include "wmv/SplatRenderer.h"
#include "wmv/SplatPbrRenderer.h"
#include "wmv/TemplateBrushOP.h"
#include "wmv/RegistNodes.h"

#include <ee0/WxStagePage.h>
#include <ee0/SubjectMgr.h>
#include <blueprint/Node.h>
#include <blueprint/CompNode.h>
#include <blueprint/MessageID.h>

#include <node0/SceneNode.h>
#include <node3/RenderSystem.h>
#include <painting2/RenderSystem.h>
#include <painting3/MaterialMgr.h>
#include <painting3/Blackboard.h>
#include <painting3/WindowContext.h>
#include <painting3/PerspCam.h>
#include <painting3/Shader.h>
#include <tessellation/Painter.h>
#include <wm/Device.h>
#include <wm/device/TemplateBrush.h>

namespace
{

const float    NODE_RADIUS = 10;
const uint32_t LIGHT_SELECT_COLOR = 0x88000088;

}

namespace wmv
{

WxPreviewCanvas::WxPreviewCanvas(ee0::WxStagePage* stage, ECS_WORLD_PARAM
                                 const ee0::RenderContext& rc)
    : ee3::WxStageCanvas(stage, ECS_WORLD_VAR &rc, nullptr, true)
{
    //m_hf_rd = std::make_shared<GrayRenderer>();
    m_hf_rd = std::make_shared<SplatRenderer>();
    //m_hf_rd = std::make_shared<SplatPbrRenderer>();

    auto sub_mgr = stage->GetSubjectMgr();
    sub_mgr->RegisterObserver(MSG_HEIGHTMAP_CHANGED, this);
}

WxPreviewCanvas::~WxPreviewCanvas()
{
    if (m_graph_page)
    {
        auto sub_mgr = m_graph_page->GetSubjectMgr();
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
        sub_mgr->UnregisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);
    }

    auto sub_mgr = m_stage->GetSubjectMgr();
    sub_mgr->UnregisterObserver(MSG_HEIGHTMAP_CHANGED, this);
}

void WxPreviewCanvas::SetGraphPage(const WxGraphPage* graph_page)
{
    m_graph_page = graph_page;

    auto sub_mgr = m_graph_page->GetSubjectMgr();
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_INSERT, this);
    sub_mgr->RegisterObserver(ee0::MSG_NODE_SELECTION_CLEAR, this);

    auto brush_op = std::static_pointer_cast<TemplateBrushOP>(m_ops[OP_TEMP_BRUSH]);
    brush_op->SetGroupSubMgr(m_graph_page->GetSubjectMgr());
}

void WxPreviewCanvas::InitEditOP(const ee0::EditOPPtr& default_op)
{
    m_ops[OP_DEFAULT] = default_op;

    m_ops[OP_TEMP_BRUSH] = std::make_shared<TemplateBrushOP>(
        m_camera, GetViewport(), m_stage->GetSubjectMgr()
    );
}

void WxPreviewCanvas::OnNotify(uint32_t msg, const ee0::VariantSet& variants)
{
    ee3::WxStageCanvas::OnNotify(msg, variants);

	switch (msg)
	{
	case ee0::MSG_NODE_SELECTION_INSERT:
		OnSelectionInsert(variants);
		break;
    case ee0::MSG_NODE_SELECTION_CLEAR:
        OnSelectionClear(variants);
        break;

    case MSG_HEIGHTMAP_CHANGED:
        SetupRenderer();
        break;
	}
}

void WxPreviewCanvas::DrawBackground3D() const
{
//    ee3::WxStageCanvas::DrawBackgroundGrids(10.0f, 0.2f);
//    ee3::WxStageCanvas::DrawBackgroundCross();
}

void WxPreviewCanvas::DrawForeground3D() const
{
    auto& shaders = m_hf_rd->GetAllShaders();
    if (!shaders.empty()) {
        assert(shaders.size() == 1);
        auto& wc = std::const_pointer_cast<pt3::WindowContext>(GetWidnowContext().wc3);
        if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
            std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
        }
    }
    {
        auto& shaders = m_overlay_rd.GetAllShaders();
        if (!shaders.empty()) {
            assert(shaders.size() == 1);
            auto& wc = std::const_pointer_cast<pt3::WindowContext>(GetWidnowContext().wc3);
            if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
                std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
            }
        }
    }
    if (m_ops[OP_TEMP_BRUSH])
    {
        auto renderer = std::static_pointer_cast<TemplateBrushOP>(m_ops[OP_TEMP_BRUSH])->GetRenderer();
        auto& shaders = renderer->GetAllShaders();
        if (!shaders.empty()) {
            assert(shaders.size() == 1);
            auto& wc = std::const_pointer_cast<pt3::WindowContext>(GetWidnowContext().wc3);
            if (shaders[0]->get_type() == rttr::type::get<pt3::Shader>()) {
                std::static_pointer_cast<pt3::Shader>(shaders[0])->AddNotify(wc);
            }
        }
    }

    pt0::RenderContext rc;
    rc.AddVar(
        pt3::MaterialMgr::PositionUniforms::light_pos.name,
        pt0::RenderVariant(sm::vec3(0, 2, -4))
    );
    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto persp = std::static_pointer_cast<pt3::PerspCam>(m_camera);
        rc.AddVar(
            pt3::MaterialMgr::PositionUniforms::cam_pos.name,
            pt0::RenderVariant(persp->GetPos())
        );
    }
    auto& wc = pt3::Blackboard::Instance()->GetWindowContext();
    assert(wc);
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::view.name,
        pt0::RenderVariant(wc->GetViewMat())
    );
    rc.AddVar(
        pt3::MaterialMgr::PosTransUniforms::projection.name,
        pt0::RenderVariant(wc->GetProjMat())
    );

    tess::Painter pt;

    auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();

    DrawSelected(pt, cam_mat, rc);

    pt2::RenderSystem::DrawPainter(pt);
}

void WxPreviewCanvas::DrawForeground2D() const
{
}

void WxPreviewCanvas::OnSelectionInsert(const ee0::VariantSet& variants)
{
    auto var_obj = variants.GetVariant("obj");
    GD_ASSERT(var_obj.m_type == ee0::VT_PVOID, "no var in vars: obj");
    const ee0::GameObj obj = *static_cast<const ee0::GameObj*>(var_obj.m_val.pv);
    GD_ASSERT(GAME_OBJ_VALID(obj), "err scene obj");

    m_selected = obj;

    auto node = GetSelectedNode();
    if (node)
    {
        auto type = node->get_type();
        if (type == rttr::type::get<node::TemplateBrush>())
        {
            std::shared_ptr<wm::HeightField> hf = nullptr;

            auto eval = m_graph_page->GetEval();
            if (eval) {
                auto back_node = eval->QueryBackNode(*node);
                if (back_node) {
                    hf = std::static_pointer_cast<wm::device::TemplateBrush>(back_node)->GetBrush();
                }
            }

            if (hf)
            {
                auto brush_op = std::static_pointer_cast<TemplateBrushOP>(m_ops[OP_TEMP_BRUSH]);
                brush_op->SetBrush(std::dynamic_pointer_cast<node::TemplateBrush>(node), hf, obj);
                m_stage->GetImpl().SetEditOP(m_ops[OP_TEMP_BRUSH]);
            }
        }
        else
        {
            m_stage->GetImpl().SetEditOP(m_ops[OP_DEFAULT]);
        }
    }

    SetupRenderer();
}

void WxPreviewCanvas::OnSelectionClear(const ee0::VariantSet& variants)
{
    m_selected.reset();

    m_hf_rd->Clear();
    m_img_rd.Clear();
    m_overlay_rd.Clear();
}

void WxPreviewCanvas::DrawSelected(tess::Painter& pt, const sm::mat4& cam_mat,
                                   const pt0::RenderContext& rc) const
{
    auto device = GetSelectedDevice();
    if (!device) {
        return;
    }

    auto hf = device->GetHeightField();
    auto bmp = device->GetBitmap();
    auto mask = device->GetMask();
    if (hf && bmp) {
        m_overlay_rd.Draw();
    } else if (hf) {
        m_hf_rd->Draw();
    } else if (bmp) {
        m_img_rd.Draw();
    } else if (mask) {
        m_img_rd.Draw();
    }
}

void WxPreviewCanvas::SetupRenderer()
{
    auto device = GetSelectedDevice();
    if (!device) {
        return;
    }

    auto hf = device->GetHeightField();
    auto bmp = device->GetBitmap();
    auto mask = device->GetMask();
    if (hf && bmp) {
        m_overlay_rd.Setup(hf, bmp);
    } else if (hf) {
        m_hf_rd->Setup(hf);
    } else if (bmp) {
        m_img_rd.Setup(bmp);
    } else if (mask) {
        m_img_rd.Setup(mask);
    }

    SetDirty();
}

bp::NodePtr WxPreviewCanvas::GetSelectedNode() const
{
    if (!m_selected || !m_selected->HasUniqueComp<bp::CompNode>()) {
        return nullptr;
    }

    auto eval = m_graph_page->GetEval();
    if (!eval) {
        return nullptr;
    }

    auto& cnode = m_selected->GetUniqueComp<bp::CompNode>();
    auto bp_node = cnode.GetNode();
    if (!bp_node) {
        return nullptr;
    }

    return cnode.GetNode();
}

wm::DevicePtr WxPreviewCanvas::GetSelectedDevice() const
{
    if (!m_graph_page) {
        return nullptr;
    }

    auto eval = m_graph_page->GetEval();
    if (!eval) {
        return nullptr;
    }

    auto front_node = GetSelectedNode();
    return front_node ? eval->QueryBackNode(*front_node) : nullptr;
}

}
