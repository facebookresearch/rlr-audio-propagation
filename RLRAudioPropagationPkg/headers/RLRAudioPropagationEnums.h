// Copyright (c) Meta Platforms, Inc. and affiliates.
// All rights reserved.

// This source code is licensed under the license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

namespace RLRAudioPropagation
{

enum class ChannelLayoutType
{
	// unknown channel layout type.
	Unknown = 0,
	// monaural channel layout that does not have any spatial information. This layout usually has 1 channel.
	Mono,
	// channel layout with 2 channels (e.g. speakers) that does not use any HRTF.
	Stereo,
	// channel layout with 2 channels that spatializes audio using an HRTF.
	Binaural,
	// channel layout with 4 channels (speakers) arranged at +-30 and +-95 degrees in the horizontal plane.
	Quad,
	// channel layout with 6 channels (speakers) arranged at 0, +-30, and +-110 degrees in the horizontal plane, with unpositioned low frequency channel.
	Surround_5_1,
	// channel layout with 8 channels (speakers) arranged at 0, +-30, +-90, and +-135 degrees in the horizontal plane, with unpositioned low frequency channel.
	Surround_7_1,
	// channel layout that encodes fully spherical spatial audio as a set of spherical harmonic basis function coefficients.
	Ambisonics,
	COUNT
};

enum class ErrorCodes
{
	Success                                = 0,
	Unknown                                = 2000, // An unknown error has occurred.
   	InvalidParam                           = 2001, // An invalid parameter, e.g. NULL pointer or out of range variable, was passed
   	BadSampleRate                          = 2002, // An unsupported sample rate was declared
   	MissingDLL                             = 2003, // The DLL or shared library could not be found
   	BadAlignment                           = 2004, // Buffers did not meet 16b alignment requirements
   	Uninitialized                          = 2005, // audio function called before initialization
   	HRTFInitFailure                        = 2006, // HRTF provider initialization failed
   	BadVersion                             = 2007, // Mismatched versions between header and libs
   	SymbolNotFound                         = 2008, // Couldn't find a symbol in the DLL
   	SharedReverbDisabled                   = 2009, // Late reverberation is disabled
   	NoAvailableAmbisonicInstance           = 2017,
   	MemoryAllocFailure                     = 2018,
   	UnsupportedFeature                     = 2019, // Unsupported feature
   	InternalEnd                            = 2099, // Internal errors used by propagation engine defined down towards public errors
                                                        // NOTE: Since we do not define a beginning range for Internal codes, make sure
                                                        // not to hard-code range checks (since that can vary based on build)
};

} // namespace RLRAudioPropagation
