//
// Created by johnk on 2025/1/8.
//

#include <Render/View.h>

namespace Render {
    ViewData::ViewData()
        : viewMatrix(Common::FMat4x4Consts::identity)
        , projectionMatrix(Common::FMat4x4Consts::identity)
        , viewport()
    {
    }

    ViewState::ViewState() {}

    View::View() {}
}