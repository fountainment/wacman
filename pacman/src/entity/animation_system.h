#pragma once

#include "system.h"

namespace pac
{
class AnimationSystem : public System
{
public:
    using System::System;

    void update(float dt) override;
};
}  // namespace pac