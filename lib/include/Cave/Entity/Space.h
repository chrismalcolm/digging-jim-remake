#pragma once
#include "Cave/Entity/Base.h"

namespace Cave::Entity {

    /**
     * @class Space
     * @brief Empty space.
     */
    class Space : public Base {
    public:
        Space()
            : Base(Type::Space, 223) {
            addTrait(Trait::Static);
            addTrait(Trait::Empty);
            addTrait(Trait::Free);
            addTrait(Trait::Traversable);
        }
    };
}