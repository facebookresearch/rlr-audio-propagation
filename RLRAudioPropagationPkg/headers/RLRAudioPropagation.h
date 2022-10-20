/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifndef RLRA_EXPORT
	#if defined( __GNUC__ ) || defined( __clang__ )
		#define RLRA_EXPORT __attribute__((visibility("default")))
	#elif defined( _MSC_VER )
		#define RLRA_EXPORT __declspec(dllexport)
	#else
		#error Unsupported compiler
	#endif
#endif

// Macro indicating that functionality should no longer be used.
#ifndef RLRA_DEPRECATED
	#if defined( __GNUC__ ) || defined( __clang__ )
		#define RLRA_DEPRECATED __attribute__((deprecated))
	#elif defined( _MSC_VER )
		#define RLRA_DEPRECATED __declspec(deprecated)
	#else
		#error Unsupported compiler
	#endif
#endif

//******************************************************************************
//******************************************************************************
// C API

extern "C"
{

/** \brief Boolean type of well-defined size. */
typedef uint8_t RLR_Bool;

/** \brief Error codes that can be returned by RLRAudio functions. */
typedef enum
{
	RLRA_Success                                      = 0,
	RLRA_Error_Unknown                                = 2000, // An unknown error has occurred.
	RLRA_Error_InvalidParam                           = 2001, // An invalid parameter, e.g. NULL pointer or out of range variable, was passed
	RLRA_Error_BadSampleRate                          = 2002, // An unsupported sample rate was declared
	RLRA_Error_MissingDLL                             = 2003, // The DLL or shared library could not be found
	RLRA_Error_BadAlignment                           = 2004, // Buffers did not meet 16b alignment requirements
	RLRA_Error_Uninitialized                          = 2005, // audio function called before initialization
	RLRA_Error_BadAlloc                               = 2018, // memory allocation failure
	RLRA_Error_UnsupportedFeature                     = 2019, // Unsupported feature
	RLRA_Error_InternalEnd                            = 2099, // The last internal error code.
} RLRA_Error;

/** \brief A enum that describes the semantic type of an audio channel layout. */
typedef enum
{
	/** \brief An unknown channel layout type. */
	RLRA_ChannelLayoutType_Unknown = 0,
	/** \brief A monaural channel layout that does not have any spatial information. This layout usually has 1 channel. */
	RLRA_ChannelLayoutType_Mono = 1,
	/** \brief A channel layout with any number of channels (2 by default) that spatializes audio using an HRTF or ATF. */
	RLRA_ChannelLayoutType_Binaural = 3,
	/** \brief A channel layout that encodes fully spherical spatial audio as a set of spherical harmonic basis function coefficients.
	  *
	  * Ambisonics use N3D normalization, and use the same coordinate convention as world space.
	  */
	RLRA_ChannelLayoutType_Ambisonics = 7,
	RLRA_ChannelLayoutType_COUNT
} RLRA_ChannelLayoutType;

/** \brief A struct that describes the channel format for an audio stream. */
#pragma pack(push,1)
typedef struct
{
	/** \brief The number of channels that are in the layout.
	  *
	  * In most cases this should be set to the default number of channels for the layout type.
	  * For ambisonic layouts, the channel count is used to determine the ambisonic order N=sqrt(channelCount)-1.
	  * For binaural layouts, the channel count should match the number of channels in the HRTF or ATF.
	  */
	size_t channelCount;
	/** \brief The semantic type of the channel layout which determines how it is interpreted. */
	RLRA_ChannelLayoutType type;
} RLRA_ChannelLayout;
#pragma pack(pop)

/** \brief A struct that describes the configuration for an acoustic simulator.
  *
  * Call RLRA_ContextConfigurationDefault() to initialize the structure with the default configuration.
  * In all cases, the member thisSize should be initialized to sizeof(RLRA_ContextConfiguration) by the client,
  * for forward compatibility and a way to detect incompatibility between header and library.
  * The contents of this struct may be reordered in the future if new parameters are added.
  * For this reason, don't rely on member offsets (e.g. offsetof()).
  */
#pragma pack(push,1)
typedef struct
{
	/** \brief The size of this structure. This should be initialized to sizeof(RLRA_ContextConfiguration). This exists for forward/backward compatibility. */
	size_t thisSize;// = sizeof(RLRA_ContextConfiguration);
	/** \brief The number of log-spaced frequency bands that the simulation uses. 4 or 8 is recommended. */
	size_t frequencyBands;// = 4;
	/** \brief The spherical harmonic order used for direct sound. Relevant for area sound sources only. */
	size_t directSHOrder;// = 3;
	/** \brief The spherical harmonic order used for indirect sound (reflections, reverb). */
	size_t indirectSHOrder;// = 1;
	/** \brief The maximum number of rays that are used to compute direct sound between each source and listener.
      *
      * The actual number of rays is proportional to the source's solid angle.
      */
	size_t directRayCount;// = 500;
	/** \brief The number of indirect rays that are emitted from the listener. This is the main parameter for controlling quality and performance. */
	size_t indirectRayCount;// = 5000;
	/** \brief The maximum number of times that an indirect listener ray can reflect. Impacts quality and performance. */
	size_t indirectRayDepth;// = 200;
	/** \brief The number of indirect rays that are emitted from the source. Impacts quality and performance. */
	size_t sourceRayCount;// = 200;
	/** \brief The maximum number of times that an indirect source ray can reflect. Impacts quality and performance. */
	size_t sourceRayDepth;// = 10;
	/** \brief The maximum edge diffraction order that will be calculated (number of edges in diffraction path). */
	size_t maxDiffractionOrder;// = 10;
	/** \brief The number of CPU threads that are used to compute the simulation. */
	size_t threadCount;// = 1;
	/** \brief The sampling rate that is used internally for simulation and audio rendering. */
	float sampleRate;// = 44100;
	/** \brief The maximum length (in seconds) of any impulse response that is computed. */
	float maxIRLength;// = 4.f;
	/** \brief A scale factor that is applied to the scene geometry to make it so that 1 unit == 1 meter.
	  *
	  * For example, if scene units are specified in centimeters, the unitScale should be 0.01.
	  * Inches would need unitScale = 1 / 39.37.
	  */
	float unitScale;// = 1.f;
	/** \brief A global linear scale factor applied to the output audio or impulse response. */
	float globalVolume;// = 1.0f;
	/** \brief A vector pointing to the right in the listener-local coordinate system, which defines the HRTF orientation. */
	float hrtfRight[3];// = { 1.0f, 0.0f, 0.0f };
	/** \brief A vector pointing up in the listener-local coordinate system, which defines the HRTF orientation. */
	float hrtfUp[3];// = { 0.0f, 1.0f, 0.0f };
	/** \brief A vector pointing backward in the listener-local coordinate system, which defines the HRTF orientation. */
	float hrtfBack[3];// = { 0.0f, 0.0f, 1.0f };
	/** \brief Whether or not direct sound is simulated. */
	RLR_Bool direct;// = true;
	/** \brief Whether or not indirect sound is simulated (everything but direct sound: reflections, reverb, diffraction). */
	RLR_Bool indirect;// = true;
	/** \brief Turn on/off diffraction simulation. This makes occlusion smoother, but can have a big performance impact in some scenes. */
	RLR_Bool diffraction;// = true;
	/** \brief Turn on/off sound transmission through geometry. */
	RLR_Bool transmission;// = true;
	/** \brief Whether or not the input meshes are simplified before use. This takes some time upfront, but can make ray tracing faster. */
	RLR_Bool meshSimplification;// = false;
	/** \brief Whether or not temporal smoothing is applied to the IR to reduce path tracing noise.
	  *
	  * Enabling this parameter reduces the number of rays that are necessary for dynamic scenes
	  * by roughly a factor of 10. The drawback is that the IR may be spatially blurred if there is fast motion.
	  */
	RLR_Bool temporalCoherence;// = false;
} RLRA_ContextConfiguration;
#pragma pack(pop)

/** \brief A struct that describes the material categories assigned to each face of a box-shaped geometry.
  *
  * The material categories correspond to those provided in the material database via RLRA_SetMaterialDatabaseJSON().
  * If a NULL or invalid material name is provided, the default material is used for those faces.
  */
#pragma pack(push,1)
typedef struct
{
	const char* xMin;
	const char* xMax;
	const char* yMin;
	const char* yMax;
	const char* zMin;
	const char* zMax;
} RLRA_BoxMaterialCategories;
#pragma pack(pop)

/** \brief A enum that describes the types of ray intersections that can occur. */
typedef enum
{
	/** \brief An enum value indicating that a ray did not intersect with anything. */
	RLRA_RayHit_False = 0,
	/** \brief An enum value indicating that a ray intersected something. */
	RLRA_RayHit_True = 1
} RLRA_RayHit;

/** \brief A struct that represents a ray intersection query, containing both inputs and outputs.
  *
  * Before tracing a ray, the following members must be set. The rest can be uninitialized.
  * - 'origin', 'direction', 'tMin', 'tMax' should be set according to the desired ray query.
  *
  * After tracing a ray, the following values contain outputs.
  * - 'hit': if equal to RLRA_RayHit_True, an intersection was found.
  * - 'normal', 'tMax': if query type is "first hit" and an intersection was found.
  */
#pragma pack(push,1)
typedef struct
{
	/* Inputs */
	/** \brief The origin point of the ray in world space. */
	float origin[3];
	/** \brief The direction of the ray in world space. Does not need to be unit length. */
	float direction[3];
	/** \brief The minimum distance along the ray where intersection will be detected, as a multiple of the ray direction length. */
	float tMin;
	/** \brief The maximum distance along the ray where intersection will be detected, as a multiple of the ray direction length.
	  *
	  * If there is an intersection, and the query type is "first hit", then this will be
	  * set to the distance where the intersection occurred. The intersection point can be
	  * calculated as: hitPoint = origin + direction*tMax.
	  */
	float tMax;

	/* Outputs */
	/** \brief An enum that describes the type of ray intersection that was found.
	  *
	  * If the value is RLRA_RayHit_True, there was an intersection.
	  **/
	RLRA_RayHit hit;
	/** \brief If there is an intersection, and the query type is "first hit",
	  * this contains the 3D un-normalized vector perpendicular to the surface
	  * at the intersection point.
	  */
	float normal[3];
} RLRA_Ray;
#pragma pack(pop)

//******************************************************************************
// Context

/** \brief A opaque handle to an RLRAudio context. 0/NULL/nullptr represent an invalid handle. */
typedef struct RLRA_Context_* RLRA_Context;

/// Initialize a context configuration with the default parameters.
/**
  * Note: config->thisSize should be initialized to sizeof(RLRA_ContextConfiguration)
  * before calling this function. If there is a size mismatch, an error will be returned and the configuration
  * will not be modified.
  */
RLRA_EXPORT RLRA_Error RLRA_ContextConfigurationDefault( RLRA_ContextConfiguration* config );

/// Create and initialize a new RLRAudio context with the specified configuration.
/**
  * The configuration should be initialized by first calling RLRA_ContextConfigurationDefault(),
  * then setting any user configuration overrides.
  * The configuration struct is copied and is no longer needed after this function call.
  * After use, the context must be destroyed using RLRA_DestroyContext() to avoid leaks.
  * It is not necessary to call RLRA_Initialize() after calling this function.
  */
RLRA_EXPORT RLRA_Error RLRA_CreateContext( RLRA_Context* context, const RLRA_ContextConfiguration* config );

/// Completely destroy a RLRAudio context and any objects created within it.
RLRA_EXPORT RLRA_Error RLRA_DestroyContext( RLRA_Context context );

/// Reinitialize the context using the specified configuration structure.
/**
  * This causes the scene to be cleared, and the context to be reinitialized with the new configuration.
  * The configuration should be initialized by first calling RLRA_ContextConfigurationDefault(),
  * then setting any user configuration overrides.
  * The configuration struct is copied and is no longer needed after this function call.
  */
RLRA_EXPORT RLRA_Error RLRA_ResetContext( RLRA_Context context, const RLRA_ContextConfiguration* config );

//******************************************************************************
// Sources

/// Add a new source to the scene positioned at the origin.
/**
  * The new source will be placed at the end of the existing sources (i.e. at index RLRA_GetSourceCount()).
  */
RLRA_EXPORT RLRA_Error RLRA_AddSource( RLRA_Context context );

/// Remove all sources from the scene.
RLRA_EXPORT RLRA_Error RLRA_ClearSources( RLRA_Context context );

/// Return the number of sources that are in the scene.
RLRA_EXPORT size_t RLRA_GetSourceCount( const RLRA_Context context );

/// Set the 3D position of the source at the specified index.
RLRA_EXPORT RLRA_Error RLRA_SetSourcePosition( RLRA_Context context, size_t sourceIndex, const float position[3] );

/// Set the radius of the source's spherical geometry at the specified index.
/**
  * Using a non-zero radius causes the source to emit sound from a spherical surface rather than a point.
  * The quality of HRTF-based spatial sound may be slightly reduced when specifying a non-zero radius,
  * due to this using a different rendering method. The default radius is 0.
  */
RLRA_EXPORT RLRA_Error RLRA_SetSourceRadius( RLRA_Context context, size_t sourceIndex, float radius );

//******************************************************************************
// Listeners

/// Add a new listener to the scene with the specified channel layout.
/**
  * The channel layout determines the format of impulse responses computed for the listener,
  * as well as the spatialization method (e.g. ambisonics, binaural, or panning).
  * The new listener will be placed at the end of the existing listeners (i.e. at index RLRA_GetListenerCount()).
  */
RLRA_EXPORT RLRA_Error RLRA_AddListener( RLRA_Context context, const RLRA_ChannelLayout* channelLayout );

/// Remove all listeners from the scene.
RLRA_EXPORT RLRA_Error RLRA_ClearListeners( RLRA_Context context );

/// Return the number of listeners that are in the scene.
RLRA_EXPORT size_t RLRA_GetListenerCount( const RLRA_Context context );

/// Set the 3D position of the listener at the specified index.
RLRA_EXPORT RLRA_Error RLRA_SetListenerPosition( RLRA_Context context, size_t listenerIndex, const float position[3] );

/// Set the 3D orientation of the listener at the specified index as a normalized quaternion.
/**
  * The quaternion specifies the listener orientation as a rotation from local to world space.
  * The quaternion should be an array with contents [ w, x, y, z ]
  */
RLRA_EXPORT RLRA_Error RLRA_SetListenerOrientationQuaternion( RLRA_Context context, size_t listenerIndex, const float orientation[4] );

/// Set the radius of the listener's sphere at the specified index.
/**
  * This controls the radius of the spherical receiver that detects audio emitted by sources.
  * In most cases this has a very subtle effect on the propagation delay, distance, and spatialization.
  * The default radius is 0.1 meters, roughly the same size as a human head.
  */
RLRA_EXPORT RLRA_Error RLRA_SetListenerRadius( RLRA_Context context, size_t listenerIndex, float radius );

/// Set the HRTF file used by the listener at the specified index.
/**
  * This function should be called after adding the listener.
  * The HRTF can be in one of the following formats based on file name extension:
  * "sofa" -> AES SOFA format
  *
  * NOTE: The HRTF can have any number of channels, which allows for modeling of microphone arrays.
  * The number of channels in the HRTF should match the number of channels in the listener's channel layout.
  * A binaural layout type should always be used with HRTFs, regardless of how many channels.
  *
  * If a NULL or empty string is provided, this switches back to the default HRTF.
  */
RLRA_EXPORT RLRA_Error RLRA_SetListenerHRTF( RLRA_Context context, size_t listenerIndex, const char* hrtfFilePath );

//******************************************************************************
// Objects

/// Add a new object with no geometry to the scene positioned at the origin.
/**
  * The object's geometry can be defined after creation by calling RLRA_SetObjectMeshOBJ(), for example.
  * The new object will be placed at the end of the existing objects (i.e. at index RLRA_GetObjectCount()).
  */
RLRA_EXPORT RLRA_Error RLRA_AddObject( RLRA_Context context );

/// Remove all objects and their geometries from the scene.
RLRA_EXPORT RLRA_Error RLRA_ClearObjects( RLRA_Context context );

/// Return the number of objects that are in the scene.
RLRA_EXPORT size_t RLRA_GetObjectCount( const RLRA_Context context );

/// Set the 3D position of the object at the specified index.
/**
  * This is the position of the object relative to the origin of the parent coordinate system.
  * To go from object to world space, this vector is added to the vertex positions.
  */
RLRA_EXPORT RLRA_Error RLRA_SetObjectPosition( RLRA_Context context, size_t objectIndex, const float position[3] );

/// Set the 3D orientation of the object at the specified index as a normalized quaternion.
/**
  * The quaternion specifies the object orientation as a rotation from local to world space.
  * To convert from object to world space, the quaternion is applied to the vertex positions, then the position offset is added.
  * The quaternion should be an array with contents [ w, x, y, z ]
  */
RLRA_EXPORT RLRA_Error RLRA_SetObjectOrientationQuaternion( RLRA_Context context, size_t objectIndex, const float orientation[4] );

/// Load an object mesh in OBJ or PLY format with the specified material category assigned to all surfaces.
/**
  * If the function succeeds, the object's current mesh is replaced with the mesh at the specified location.
  * The material category should refer to an entry in the material database that is provided by RLRA_SetMaterialDatabaseJSON().
  * If a NULL material category is provided, this uses the default material.
  * Object indices range from 0 to RLRA_GetObjectCount()-1.
  */
RLRA_EXPORT RLRA_Error RLRA_SetObjectMeshOBJ( RLRA_Context context, size_t objectIndex, const char* objFilePath, const char* materialCategoryName );
RLRA_EXPORT RLRA_Error RLRA_SetObjectMeshPLY( RLRA_Context context, size_t objectIndex, const char* plyFilePath, const char* materialCategoryName );

/// Replace the mesh for an object with a rectangular box-shaped mesh.
/**
  * The box is specified in object-local space by its minimum and maximum vertex coordinates.
  * The box materials structure specifies the material category to use for each of the 6 faces.
  * The material category should refer to an entry in the material database that is provided by RLRA_SetMaterialDatabaseJSON().
  * If NULL is passed for the materials, the default material is used.
  * Object indices range from 0 to RLRA_GetObjectCount()-1.
  */
RLRA_EXPORT RLRA_Error RLRA_SetObjectBox( RLRA_Context context, size_t objectIndex,
	const float boxMin[3], const float boxMax[3], const RLRA_BoxMaterialCategories* materials );

/// Add the vertices for all or part of a mesh.
/**
  * The vertices are passed as a packed array of 32-bit floats, with every 3 values representing a 3D vertex.
  * The vertex array should have length 3*vertexCount.
  * RLRA_AddMeshIndices() must also be called to define the surface geometry and materials.
  * RLRA_UploadObjectMesh() must then be called after adding all mesh parts.
  */
RLRA_EXPORT RLRA_Error RLRA_AddMeshVertices( RLRA_Context context, const float* vertexData, size_t vertexCount );
/// Add the face indices for all or part of a mesh with the specified material category name.
/**
  * This function defines the surface geometry for vertices previously provided using RLRA_AddMeshVertices().
  * This function may be called multiple times to define multiple mesh parts that can each be assigned a different material.
  * The indices are passed as a packed array of unsigned 32-bit integers, that may represent either triangles or quads.
  * The number of faces is equal to floor(indexCount/verticesPerFace).
  * The category name should correspond to one listed in the audio material JSON file provided with RLRA_SetMaterialDatabaseJSON().
  * RLRA_UploadObjectMesh() must be called after adding all mesh parts.
  */
RLRA_EXPORT RLRA_Error RLRA_AddMeshIndices( RLRA_Context context, const uint32_t* indexData, size_t indexCount, size_t verticesPerFace, const char* materialCategoryName );
/// Transfer mesh data which was previously provided by RLRA_AddMeshVertices() and RLRA_AddMeshIndices() into the specified object's geometry.
/**
  * Object indices range from 0 to RLRA_GetObjectCount()-1.
  * If this function succeeds, any previously-added vertices/indices are discarded.
  */
RLRA_EXPORT RLRA_Error RLRA_FinalizeObjectMesh( RLRA_Context context, size_t objectIndex );

/// Specify the material database as a JSON file.
/**
  * The JSON file specifies a mapping from acoustic material category strings to the material data
  * (absorption, scattering, transmission coefficients).
  * This database is used by various object mesh functions that take a material category name as input.
  * A material is determined from a material category string by inspecting all materials in the database,
  * and finding the material which has the greatest number of label substring matches.
  * A match is counted if the lowercase category name contains a label as a substring.
  * NOTE: the material "name" attribute is not used for matching material categories to materials, is
  * intended as human-readable name, and will be ignored.
  *
  * The JSON format is:
  * {
  *   "materials":[
  *     {
  *       "name": "my custom material", // This is NOT the material category passed into various mesh functions. This is a human-readable name that is ignored.
  *       "labels": [ "my_category0", "my_category1" ], // These are the categories that will match with the material. The material is chosen that has the most number of label substrings in the category.
  *       "absorption": [ freq0, data0, freq1, data1, etc. ], // data in range [0,1], representing fraction of incident sound pressure that is absorbed.
  *       "scattering": [ freq0, data0, freq1, data1, etc. ], // data in range [0,1], representing fraction of incident sound pressure that is scattered diffusely.
  *       "transmission": [ freq0, data0, freq1, data1, etc. ], // data in range [0,1], representing fraction of incident sound pressure that is transmitted into material.
  *       "damping": [ freq0, data0, freq1, data1, etc. ], // Distance-dependent damping for propagation through material, expressed as (positive) decibels of attenuation per meter.
  *       "speed": 1484.0 // Sound speed in meters/second for propagation through the material (e.g. transmission)
  *     }
  *     .... more materials
  *   ]
  * }
  */
RLRA_EXPORT RLRA_Error RLRA_SetMaterialDatabaseJSON( RLRA_Context context, const char* jsonPath );

/// Write the mesh for the entire scene as an OBJ file, for debugging purposes.
/**
  * The vertices for all objects are transformed into world space and concatenated into a single mesh,
  * and this mesh is written at the specified output location. The mesh surfaces will be assigned
  * a random color corresponding to the material, using OBJ vertex colors.
  */
RLRA_EXPORT RLRA_Error RLRA_WriteSceneMeshOBJ( const RLRA_Context context, const char* outputPath );

//******************************************************************************
// Simulation

/// Compute IRs for all pairs of sources and listeners in the scene.
RLRA_EXPORT RLRA_Error RLRA_Simulate( RLRA_Context context );

/// Return the total number of IRs that are simulated (listenerCount * sourceCount)
RLRA_EXPORT size_t RLRA_GetIRCount( const RLRA_Context context );

/// Return the number of channels that are in the IR between the specified (listener, source) pair.
/**
  * Generally, this will match the listener's channel layout, but this behavior should not be relied on.
  */
RLRA_EXPORT size_t RLRA_GetIRChannelCount( const RLRA_Context context, size_t listenerIndex, size_t sourceIndex );

/// Return the number of samples per channel that are in the IR between the specified (listener, source) pair.
RLRA_EXPORT size_t RLRA_GetIRSampleCount( const RLRA_Context context, size_t listenerIndex, size_t sourceIndex );

/// Return a pointer to a channel of the IR between the specified (listener, source) pair.
/**
  * If there is an error, NULL will be returned. There are GetIRSampleCount() values in the returned array.
  * Accessing values outside that range will result in undefined behavior.
  * The returned pointer will remain valid until the next call to RLRA_Simulate().
  */
RLRA_EXPORT const float* RLRA_GetIRChannel( const RLRA_Context context, size_t listenerIndex, size_t sourceIndex, size_t channelIndex );

/// Write the IR for the specified (listener, source) pair as a .wav file at the specified location.
/**
  * The file is encoded as 32-bit floating point.
  */
RLRA_EXPORT RLRA_Error RLRA_WriteIRWave( const RLRA_Context context, size_t listenerIndex, size_t sourceIndex, const char* outputFilePath );

/// Write the IR metrics (e.g. RT60) for the specified (listener, source) pair as a .txt file at the specified location.
/**
  * This calculates various acoustic metrics for the impulse response (RT60, EDT, DRR, C80, C50, D50, TS)
  * for the frequency bands that are used in the simulation.
  */
RLRA_EXPORT RLRA_Error RLRA_WriteIRMetrics( const RLRA_Context context, size_t listenerIndex, size_t sourceIndex, const char* outputFilePath );

/// Return the fraction of indirect rays that hit geometry, as a value in the range [0,1].
/**
  * This is averaged across all sources and listeners, and can be used as a measure of how
  * enclosed an acoustic space is. A value close to 0 indicates a very open space (e.g. outdoors),
  * while a value closer to 1 indicates a closed geometry (e.g. indoors).
  */
RLRA_EXPORT float RLRA_GetIndirectRayEfficiency( const RLRA_Context context );

/// Trace a ray starting from an origin point along the given direction and find any intersection.
/**
  * This function detects if there are any intersections between the minimum and
  * maximum distance along the ray, but does not necessarily find the first intersection.
  * The ray structure will be populated with information about whether or not there
  * was a ray intersection, but will not contain any additional information (i.e. hit distance).
  * This function should be preferred if hit information is not needed because it will be
  * faster than RLRA_TraceRayFirstHit().
  *
  * The direction does not need to be a unit vector, but if not, then distances must be provided
  * and returned as a multiple of the direction length.
  *
  * NOTE: This function should not be called until after calling RLRA_Simulate(), because
  * the ray tracing data structures will not be initialized until that point.
  */
RLRA_EXPORT RLRA_Error RLRA_TraceRayAnyHit( const RLRA_Context context, RLRA_Ray* ray );

/// Trace a ray starting from an origin point along the given direction and find the first intersection.
/**
  * This will find the first intersection along the ray, if one exists, and then populate the
  * ray structure with information about the intersection.
  *
  * The direction does not need to be a unit vector, but if not, then distances must be provided
  * and returned as a multiple of the direction length.
  *
  * NOTE: This function should not be called until after calling RLRA_Simulate(), because
  * the ray tracing data structures will not be initialized until that point.
  */
RLRA_EXPORT RLRA_Error RLRA_TraceRayFirstHit( const RLRA_Context context, RLRA_Ray* ray );

}; // extern "C"


//******************************************************************************
//******************************************************************************
// Deprecated Interfaces

// It's not recommended to use this C++ interface for new programs
// because it has limitations like only allowing 1 source and listener,
// and it has potential issues with ABI compatibility.
// Use the above C API instead.

#ifdef __cplusplus

#include <cstdint>
#include <string>
#include <memory>
#include <ostream>

namespace RLRAudioPropagation
{

enum class RLRA_DEPRECATED ChannelLayoutType
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

enum class RLRA_DEPRECATED ErrorCodes
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

#pragma pack(push, 1)
struct RLRA_DEPRECATED Configuration
{
	std::size_t sampleRate = 44100;
	std::size_t frequencyBands = 4;
	std::size_t directSHOrder = 3;
	std::size_t indirectSHOrder = 1;
	std::size_t threadCount = 1;
	float updateDt = 0.02f;
	float irTime = 4.f;
	float unitScale = 1.f;
	float globalVolume = 0.25f;
	float listenerRadius = 0.1f;
	std::size_t indirectRayCount = 5000;
	std::size_t indirectRayDepth = 200;
	std::size_t sourceRayCount = 200;
	std::size_t sourceRayDepth = 10;
	std::size_t maxDiffractionOrder = 10;
	bool direct = true;
	bool indirect = true;
	bool diffraction = true;
	bool transmission = true;
	bool meshSimplification = false;
	bool temporalCoherence = false;
	bool dumpWaveFiles = false;
	bool enableMaterials = true;
	bool writeIrToFile = false;
};
#pragma pack(pop)
RLRA_DEPRECATED std::ostream& operator << ( std::ostream& os, const Configuration& config );


struct RLRA_DEPRECATED Vector3f
{
	float x;
	float y;
	float z;
};

struct RLRA_DEPRECATED Quaternion
{
	float s;
	float x;
	float y;
	float z;
};

#pragma pack(push, 1)
struct RLRA_DEPRECATED ChannelLayout
{
    RLRA_DEPRECATED ChannelLayout(); // Hide initialization of channelType to avoid deprecated warnings.
	RLRA_DEPRECATED ChannelLayoutType channelType; // Binaural default
	std::size_t channelCount = 2;
};
#pragma pack(pop)
RLRA_DEPRECATED std::ostream& operator << ( std::ostream& os, const ChannelLayout& channelLayout );

#pragma pack(push, 1)
struct RLRA_DEPRECATED VertexData
{
	const void* vertices;
	std::size_t byteOffset;
	std::size_t vertexCount;
	std::size_t vertexStride;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct RLRA_DEPRECATED IndexData
{
	const uint32_t* indices;
	std::size_t byteOffset;
	std::size_t indexCount;
};
#pragma pack(pop)


class RLRA_DEPRECATED Simulator
{
public:
    RLRA_DEPRECATED Simulator();
    RLRA_DEPRECATED ~Simulator();

	// Don't allow copying.
    Simulator( const Simulator& other ) = delete;
    Simulator( Simulator&& other ) = delete;
    Simulator& operator = ( const Simulator& other ) = delete;
    Simulator& operator = ( Simulator&& other ) = delete;

public:
    // Public API
    // Returns the error code.
    RLRA_DEPRECATED ErrorCodes Configure(const Configuration& config);

    // Load the material database from a JSON file
    RLRA_DEPRECATED ErrorCodes LoadAudioMaterialJSON(const std::string& jsonPath);

    // Load the mesh using an obj file. Returns the error code.
    RLRA_DEPRECATED ErrorCodes LoadMesh(const std::string& meshPath);

    // Load the mesh using an obj file using the specific material category. Returns the error code.
    RLRA_DEPRECATED ErrorCodes LoadMesh(const std::string& meshPath, const std::string& materialCategoryName);

    // Load the mesh with vertices and indices. Returns the error code.
    // Vertices are always 3 floats each, and indices are always uint32_t and refer to triangles (3 indices per face).
    RLRA_DEPRECATED ErrorCodes LoadMeshData(const VertexData& vertices, const IndexData& indices);

    // Load mesh with material properties. Returns the error code.
    // Vertices are always 3 floats each, and indices are always uint32_t and refer to triangles (3 indices per face)
    RLRA_DEPRECATED ErrorCodes LoadMeshDataWithMaterial(const VertexData& vertices, const IndexData& indices, const float absorption, const float transmission, const float scattering);

    // Load mesh using material category name. Returns the error code.
    // Vertices are always 3 floats each, and indices are always uint32_t and refer to triangles (3 indices per face)
    RLRA_DEPRECATED ErrorCodes LoadMeshDataWithMaterial(const VertexData& vertices, const IndexData& indices, const std::string& materialCategoryName);

    // Load mesh vertices. Returns the error code.
    // Vertices are always 3 floats each.
    RLRA_DEPRECATED ErrorCodes LoadMeshVertices(const VertexData& vertices);

    // Load mesh indices along with the material category name. Returns the error code.
    // Indices are always uint32_t and refer to triangles (3 indices per face)
    RLRA_DEPRECATED ErrorCodes LoadMeshIndices(const IndexData& indices, const std::string& materialCategoryName);

    // Last call to upload the mesh to the propagation engine. Returns the error code.
    RLRA_DEPRECATED ErrorCodes UploadMesh();

    // Add audio source at the given location. Returns the error code.
    // Calling this function more than once does NOT add a new source, but instead repositions the first source.
    RLRA_DEPRECATED ErrorCodes AddSource(const Vector3f& sourcePos);

    // Add listener at a given position with a given channelLayout. Returns the error code.
    // Calling this function more than once does NOT add a new listener, but instead repositions the first listener.
    RLRA_DEPRECATED ErrorCodes AddListener(const Vector3f& listenerPos, const Quaternion& listenerRotQuat, const ChannelLayout& channelLayout);

    // Run the simulation. IRs will be written to the output directory if the configuration writeIrToFile is true.
    RLRA_DEPRECATED void RunSimulation(const std::string& outputDir);

    // Return the channel count in the output impulse response.
    RLRA_DEPRECATED std::size_t GetChannelCount();

    // Return the sample count in the output impulse response
    RLRA_DEPRECATED std::size_t GetSampleCount();

    // Return the impulse response at a given channel + sample index pair
    RLRA_DEPRECATED float GetImpulseResponse(const std::size_t channelIndex, const std::size_t sampleIndex);

    // Return the specified channel of the impulse response, or nullptr if there was an error.
    // The returned array has GetSampleCount() entries. Out-of-bounds access results in undefined behavior.
    RLRA_DEPRECATED const float* GetImpulseResponseForChannel(const std::size_t channelIndex);

    // Return the fraction of indirect rays that intersected with scene geometry.
    RLRA_DEPRECATED float GetRayEfficiency();

private:
    RLRA_Context context = nullptr;
    RLRA_DEPRECATED Configuration config;
};

} // namespace RLRAudioPropagation


#endif // __cplusplus
