// Copyright (c) Meta Platforms, Inc. and affiliates.
// All rights reserved.

// This source code is licensed under the license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <string>
#include <memory>

#include "RLRAudioPropagationStructs.h"

class PropagationWrapper;

namespace RLRAudioPropagation
{

class Simulator
{
public: // Constructor and destructor
    Simulator();
    ~Simulator();

public:
    // Public API
    // Returns the error code.
    ErrorCodes Configure(const Configuration& config);

    // Load the material database from a JSON file
    ErrorCodes LoadAudioMaterialJSON(const std::string& jsonPath);

    // Load the mesh using an obj file. Returns the error code.
    ErrorCodes LoadMesh(const std::string& meshPath);

    // Load the mesh using an obj file using the specific material category. Returns the error code.
    ErrorCodes LoadMesh(const std::string& meshPath, const std::string& materialCategoryName);

    // Load the mesh with vertices and indices. Returns the error code.
    ErrorCodes LoadMeshData(const VertexData& vertices, const IndexData& indices);

    // Load mesh with material properties. Returns the error code.
    ErrorCodes LoadMeshDataWithMaterial(const VertexData& vertices, const IndexData& indices, const float absorption, const float transmission, const float scattering);

    // Load mesh using material category name. Returns the error code.
    ErrorCodes LoadMeshDataWithMaterial(const VertexData& vertices, const IndexData& indices, const std::string& materialCategoryName);

    // Load mesh vertices. Returns the error code.
    ErrorCodes LoadMeshVertices(const VertexData& vertices);

    // Load mesh indices along with the material category name. Returns the error code.
    ErrorCodes LoadMeshIndices(const IndexData& indices, const std::string& materialCategoryName);

    // Last call to uplaod the mesh to the propagation engine. Returns the error code.
    ErrorCodes UploadMesh();

    // Add audio source at the given location. Returns the error code.
    ErrorCodes AddSource(const Vector3f& sourcePos);

    // Add listener at a given position with a given channelLayout. Returns the error code.
    ErrorCodes AddListener(const Vector3f& listenerPos, const Quaternion& listenerRotQuat, const ChannelLayout& channelLayout);

    // Run the simulation
    void RunSimulation(const std::string& outputDir);

    // Return the channel count in the output impulse response.
    std::size_t GetChannelCount();

    // Return the sample count in the output impulse response
    std::size_t GetSampleCount();

    // Return the impulse response at a given channel + sample index pair
    float GetImpulseResponse(const std::size_t channelIndex, const std::size_t sampleIndex);

    // Return the impulse response at a given channel
    const float * GetImpulseResponseForChannel(const std::size_t channelIndex);

private:
    std::shared_ptr<PropagationWrapper> propagationWrapper;
};

} // namespace RLRAudioPropagation
