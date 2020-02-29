#pragma once

#include "wmv/BrushDrawOP.h"

namespace wmv
{

namespace node { class TemplateBrush; }

class NoiseBrushOP : public BrushDrawOP
{
public:
    NoiseBrushOP(const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseLeftUp(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;

private:
    void AddToBrush(int x, int y) const;

private:
    bool m_left_pressed = false;

}; // NoiseBrushOP

}