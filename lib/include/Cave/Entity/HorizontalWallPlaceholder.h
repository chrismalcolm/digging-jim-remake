#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    class HorizontalWallPlaceholder : public Base {
    public:
        HorizontalWallPlaceholder()
            : Base(Type::HorizontalWallPlaceholder, 224) {}
    };
}
