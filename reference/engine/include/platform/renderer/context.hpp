#pragma once
#include <cstdint>
#include <glm/glm.hpp>

bool pde_renderer_create_context();
bool pde_renderer_destroy_context();
void pde_renderer_swap_buffers();
void pde_renderer_begin_frame(glm::vec2 framebuffer, glm::vec4 color);
void pde_renderer_finalize_frame();

