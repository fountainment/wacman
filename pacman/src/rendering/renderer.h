#pragma once

#include "shader_program.h"
#include "vertex_array_object.h"

#include <vector>
#include <memory>
#include <string_view>

#include <cglutil.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace pac
{
/*!
 * \brief The TextureID struct represents a texture ID, and is a bitfield of a single unsigned, where 12 bits are used
 * to encode the array and frame number of the texture. Supports up to 16 textures with up to 64 animation frames each.
 */
struct TextureID
{
    uint32_t _ : 14;
    uint32_t frame_count : 6;
    uint32_t frame_number : 6;
    uint32_t array_index : 4;
};

/*!
 * \brief The Renderer class is a specialized "renderer" designed to render this pacman game efficiently. It
 * uses a single, large buffer starting with vertex and index data for the one instanced quad. Then the rest
 * of the buffer is per-instance data for all the sprites in the game.
 * \note This class should be instanced once, and act as a Singleton ish (taken care of with get_renderer function)
 */
class Renderer
{
private:
    /*!
     * \brief The Vertex struct is the vertex layout for the sprite quad
     */
    struct Vertex
    {
        glm::vec2 pos = {};
        glm::vec2 uv = {};
    };

    /*!
     * \brief The InstanceVertex struct is the vertex layout of per-instance sprite instantiations
     */
    struct InstanceVertex
    {
        glm::vec2 pos = {};
        glm::vec2 size = {};
        glm::vec3 col = {};
        TextureID texture_id = {};
    };

    /* Buffer that contains sprite data, vertices and indices like [INDEX DATA ... VERTEX DATA] */
    unsigned m_sprite_buffer = 0u;

    /* Buffer that contains per-instance data */
    unsigned m_instance_buffer = 0u;

    /* Vector of all currently available textures */
    std::vector<unsigned> m_textures = {};

    /* Instance data, added as you draw, and drawn once you submit the draw */
    std::vector<InstanceVertex> m_instance_data = {};

    /* Vertex attribute layout */
    VertexArray m_vao = {};

    /* Shader program (unique_ptr) since it has no default Ctor */
    std::unique_ptr<ShaderProgram> prog = nullptr;

public:
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    ~Renderer();

    /*!
     * \brief draw adds a sprite to the draws that will appear the next time work is submitted
     * \param data is the vertex data to add
     * \param texture_id is the id of the texture that this sprite should use
     */
    void draw(const InstanceVertex& data);

    /*!
     * \brief submit_work submits the collected draws for rendering and renders it
     */
    void submit_work();

    /*!
     * \brief load_texture loads the texture at the given relative file path
     * \param relative_fp is the relative file path
     * \return a handle to the new texture, you do not own this, so please do not delete it or otherwise be careless with it
     */
    TextureID load_texture(std::string_view relative_fp);

    /*!
     * \brief load_animation_texture loads a texture with an animated sprite in it that can later be used with an animation
     * \param relative_fp is the relative file path
     * \return a handle to the new texture, you do not own this, so please do not delete it or otherwise be careless with it
     */
    TextureID load_animation_texture(std::string_view relative_fp);

private:
    /* Private because we want the singleton function to be the only one able to create a Renderer */
    explicit Renderer(unsigned max_sprites = 2048u);
    friend Renderer& get_renderer();

    void init(unsigned max_sprites);
};

/*!
 * \brief get_renderer returns a reference to the active renderer Singleton class
 * \note singleton is accessible through this function so we can control the lifetime and creation, unlike a global which
 * is created statically and can mess with the GL Context or otherwise act weirdly.
 * \return The Renderer
 */
Renderer& get_renderer();
}  // namespace pac