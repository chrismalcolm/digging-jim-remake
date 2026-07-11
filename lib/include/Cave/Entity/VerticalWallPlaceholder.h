#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    class VerticalWallPlaceholder : public Base {
    public:
        VerticalWallPlaceholder()
            : Base(Type::VerticalWallPlaceholder, 225) {}
    };
}
