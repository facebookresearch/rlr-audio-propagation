/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <ostream>
#include <cstdint>

#include "RLRAudioPropagationEnums.h"

namespace RLRAudioPropagation
{

struct Configuration
{
public:
	friend std::ostream& operator<<(std::ostream& os, const Configuration& config)
	{
		return os
			<< "sampleRate:" << config.sampleRate
			<< ", frequencyBands:" << config.frequencyBands
			<< ", directSHOrder:" << config.directSHOrder
			<< ", indirectSHOrder:" << config.indirectSHOrder
			<< ", threadCount:" << config.threadCount
			<< ", updateDt:" << config.updateDt
			<< ", irTime:" << config.irTime
			<< ", unitScale:" << config.unitScale
			<< ", globalVolume:" << config.globalVolume
			<< ", listenerRadius:" << config.listenerRadius
			<< ", indirectRayCount:" << config.indirectRayCount
			<< ", indirectRayDepth:" << config.indirectRayDepth
			<< ", sourceRayCount:" << config.sourceRayCount
			<< ", sourceRayDepth:" << config.sourceRayDepth
			<< ", maxDiffractionOrder:" << config.maxDiffractionOrder
			<< ", direct:" << config.direct
			<< ", indirect:" << config.indirect
			<< ", diffraction:" << config.diffraction
			<< ", transmission:" << config.transmission
			<< ", temporalCoherence:" << config.temporalCoherence
			<< ", meshSimplification:" << config.meshSimplification
			<< ", dumpWaveFiles:" << config.dumpWaveFiles
			<< ", enableMaterials:" << config.enableMaterials
			<< ", writeIrToFile:" << config.writeIrToFile;
	}

	std::size_t sampleRate = 44100;
	std::size_t frequencyBands = 4;
	std::size_t directSHOrder = 3;
	std::size_t indirectSHOrder = 1;
	std::size_t threadCount = 1;
	float updateDt = 0.02f;
	float irTime = 4.f;
	float unitScale = 1.f;
	float globalVolume = 4.f;
	float listenerRadius = 0.1f;
	std::size_t indirectRayCount = 5000;
	std::size_t indirectRayDepth = 200;
	std::size_t sourceRayCount = 200;
	std::size_t sourceRayDepth = 10;
	std::size_t maxDiffractionOrder = 10;
	bool direct = true;
	bool indirect = true;
	bool diffraction = true;
	bool transmission = false;
	bool meshSimplification = true;
	bool temporalCoherence = false;
	bool dumpWaveFiles = false;
	bool enableMaterials = true;
	bool writeIrToFile = false;
};


struct Vector3f
{
	float x;
	float y;
	float z;
};


struct Quaternion
{
	float s;
	float x;
	float y;
	float z;
};


#pragma pack(push, 1)
struct ChannelLayout
{
friend std::ostream& operator<<(std::ostream& os, const ChannelLayout& channelLayout)
	{
		return os
			<< "channelType:" << static_cast<int>(channelLayout.channelType)
			<< ", channelCount:" << channelLayout.channelCount;
	}
public:
	ChannelLayoutType channelType = ChannelLayoutType::Binaural;
	std::size_t channelCount = 2;
};
#pragma pack(pop)
static_assert((sizeof(std::size_t) + sizeof(int)) == sizeof(ChannelLayout), "RLRAudioPropagation::ChannelLayout size is incorrect");


#pragma pack(push, 1)
struct VertexData
{
	const void* vertices;
	std::size_t byteOffset;
	std::size_t vertexCount;
	std::size_t vertexStride;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct IndexData
{
	const uint32_t* indices;
	std::size_t byteOffset;
	std::size_t indexCount;
};
#pragma pack(pop)


namespace Constants
{
	const std::int32_t SUCCESS = 0;
}

} // namespace RLRAudioPropagation
