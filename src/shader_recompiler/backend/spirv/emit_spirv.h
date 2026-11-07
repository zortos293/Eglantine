// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <vector>

#include "common/common_types.h"
#include "shader_recompiler/backend/bindings.h"
#include "shader_recompiler/frontend/ir/program.h"
#include "shader_recompiler/profile.h"
#include "shader_recompiler/runtime_info.h"

namespace Shader::Backend::SPIRV {

constexpr u32 NUM_TEXTURE_SCALING_WORDS = 4;
constexpr u32 NUM_IMAGE_SCALING_WORDS = 2;
constexpr u32 NUM_TEXTURE_AND_IMAGE_SCALING_WORDS =
    NUM_TEXTURE_SCALING_WORDS + NUM_IMAGE_SCALING_WORDS;

struct RescalingLayout {
    alignas(16) std::array<u32, NUM_TEXTURE_SCALING_WORDS> rescaling_textures;
    alignas(16) std::array<u32, NUM_IMAGE_SCALING_WORDS> rescaling_images;
    u32 down_factor;
};
struct RenderAreaLayout {
    std::array<f32, 4> render_area;
};
constexpr u32 RESCALING_LAYOUT_WORDS_OFFSET = offsetof(RescalingLayout, rescaling_textures);
constexpr u32 RESCALING_LAYOUT_DOWN_FACTOR_OFFSET = offsetof(RescalingLayout, down_factor);
constexpr u32 RENDERAREA_LAYOUT_OFFSET = offsetof(RenderAreaLayout, render_area);

[[nodiscard]] std::vector<u32> EmitSPIRV(const Profile& profile, const RuntimeInfo& runtime_info, IR::Program& program, Bindings& bindings);
[[nodiscard]] inline std::vector<u32> EmitSPIRV(const Profile& profile, IR::Program& program) {
    Bindings binding;
    return EmitSPIRV(profile, {}, program, binding);
}

/// Mesh emulation without VK_EXT_mesh_shader. The mesh program runs as a compute shader
/// (RuntimeInfo::mesh_as_compute) that writes one block per workgroup to a u32 storage buffer:
/// max_vertices * vertex_lines words (vertex v, line l at v * vertex_lines + l), then
/// max_primitives * 3 vertex indices (0 0 0 past the primitive count, a degenerate triangle).
[[nodiscard]] inline u32 MeshEmulationBlockWords(const MeshInfo& mesh) {
    return mesh.max_vertices * static_cast<u32>(mesh.vertex_lines.size()) +
           mesh.max_primitives * 3;
}

/// Vertex shader drawing the emulation's output as a triangle list of
/// groups * max_primitives * 3 vertices, with the mesh program's vertex outputs.
[[nodiscard]] std::vector<u32> EmitMeshEmulationVertex(const Profile& profile,
                                                       const MeshInfo& mesh, u32 binding,
                                                       bool convert_depth);

/// Mesh emulation with a task program: push constants of the distribution pass.
struct MeshEmulationDistributionConstants {
    u32 tasks;              ///< task workgroups of the draw
    u32 payload_words;      ///< words per task in the payload buffer (count at word 1)
    u32 capacity;           ///< mesh workgroups the output buffer holds
    u32 vertices_per_group; ///< draw vertices per mesh workgroup
};
/// Byte offset of the vkCmdDrawIndirect arguments in the distribution's argument buffer (the
/// vkCmdDispatchIndirect ones are at 0).
constexpr u32 MESH_EMULATION_DRAW_ARGS_OFFSET = 16;

/// Distribution pass (one invocation) between the task and mesh dispatches, set 0: payload
/// buffer (binding 0), offsets (1: first mesh workgroup of each task), map (2: task of each mesh
/// workgroup), arguments (3: mesh dispatch, then the draw). Each task's mesh workgroup count is
/// clamped so the total fits the capacity.
[[nodiscard]] std::vector<u32> EmitMeshEmulationDistribution(const Profile& profile);

} // namespace Shader::Backend::SPIRV
