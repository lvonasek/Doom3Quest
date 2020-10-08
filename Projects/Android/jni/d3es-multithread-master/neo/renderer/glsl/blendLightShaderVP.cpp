/*
 * This file is part of the D3wasm project (http://www.continuation-labs.com/projects/d3wasm)
 * Copyright (c) 2019 Gabriel Cuvillier.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "glsl_shaders.h"

const char * const blendLightShaderVP = R"(
#version 300 es

// Multiview
#define NUM_VIEWS 2
#extension GL_OVR_multiview2 : enable
layout(num_views=NUM_VIEWS) in;

precision mediump float;

// In
in highp vec4 attr_Vertex;

// Uniforms
uniform ShaderMatrices
{
    uniform highp mat4 viewMatrix[NUM_VIEWS];
} u_shaderMatrices;
uniform highp mat4 u_modelMatrix;
uniform highp mat4 u_projectionMatrix;
uniform mat4 u_fogMatrix;

// Out
// gl_Position
out vec2 var_TexFog;
out vec2 var_TexFogEnter;

void main()
{
  gl_Position = u_projectionMatrix * (u_shaderMatrices.viewMatrix[gl_ViewID_OVR] * (u_modelMatrix * attr_Vertex));

  // What will be computed:
  //
  // vec4 tc;
  // tc.x = dot( u_fogMatrix[0], attr_Vertex );
  // tc.y = dot( u_fogMatrix[1], attr_Vertex );
  // tc.z = 0.0;
  // tc.w = dot( u_fogMatrix[2], attr_Vertex );
  // var_TexFog.xy = tc.xy / tc.w;
  //
  // var_TexFogEnter.x = dot( u_fogMatrix[3], attr_Vertex );
  // var_TexFogEnter.y = 0.5;

  // Optimized version:
  //
  var_TexFog = vec2(dot( u_fogMatrix[0], attr_Vertex ), dot( u_fogMatrix[1], attr_Vertex )) / dot( u_fogMatrix[2], attr_Vertex );
  var_TexFogEnter = vec2( dot( u_fogMatrix[3], attr_Vertex ), 0.5 );
}
)";