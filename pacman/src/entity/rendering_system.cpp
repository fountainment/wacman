#include "rendering_system.h"
#include "config.h"
#include "components.h"
#include "rendering/renderer.h"

#include <gfx.h>
#include <entt/entity/registry.hpp>

namespace pac
{
void RenderingSystem::update(float dt, entt::registry& reg)
{
    /* Draw Regular (non-animated) Sprites */
    auto regular = reg.view<CSprite, CPosition>();
    regular.each([&reg](auto e, const CSprite& sprite, const CPosition& pos) {
        auto interp_pos = glm::vec2(pos.position);

        /* If we also have a movement comp, take that into consideration */
        if (reg.has<CMovement>(e))
        {
            auto& move = reg.get<CMovement>(e);
            interp_pos += move.progress * glm::vec2(move.current_direction);
        }

        /* Submit the draw request */
        get_renderer().draw({HALF_TILE + interp_pos * TILE_SIZE<float>, glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>),
                             glm::vec3(1.f), sprite.sprite});
    });

    /* Draw Animated Sprites */
    auto regular_moving_anim = reg.view<CAnimationSprite, CPosition>();
    regular_moving_anim.each([&reg](auto e, const CAnimationSprite& sprite, const CPosition& pos) {
        auto interp_pos = glm::vec2(pos.position);

        /* If we also have a movement comp, take that into consideration */
        if (reg.has<CMovement>(e))
        {
            auto& move = reg.get<CMovement>(e);
            interp_pos += move.progress * glm::vec2(move.current_direction);
        }

        /* Draw */
        get_renderer().draw({HALF_TILE + interp_pos * TILE_SIZE<float>, glm::vec2(TILE_SIZE<float>, TILE_SIZE<float>),
                             glm::vec3(1.f), sprite.active_animation});
    });

    /* Draw Player Icon as Lives */
    reg.view<CPlayer>().each([](const CPlayer& plr) {
        for (int i = 0; i < plr.lives; ++i)
        {
            get_renderer().draw({HALF_TILE + glm::vec2(i + 1, SCREEN_H / TILE_SIZE<int> - 2) * TILE_SIZE<float>,
                                 {TILE_SIZE<float>, TILE_SIZE<float>},
                                 {1.f, 1.f, 1.f},
                                 plr.icon});
        }
    });
}
}  // namespace pac
