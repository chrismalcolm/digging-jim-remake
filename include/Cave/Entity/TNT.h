#pragma once
#include "Cave/Entity/Base.h"
#include "Utils/Random.h"

namespace Cave::Entity {

    /**
     * @class TNT
     * @brief Left in the caves by long forgotten miners.
     * When you active the detonator all TNT boxes the cave will explode, so make sure these are in the right places before doing so.
     * Jim can push them around like boulders and they behave much in the same way.
     */
    class TNT : public Base {
    public:
        TNT()
            : Base(Type::TNT, Utils::randomInteger(128, 135)) {
            addTrait(Trait::Pushable);
            addTrait(Trait::Slippery);
        }
    };
}