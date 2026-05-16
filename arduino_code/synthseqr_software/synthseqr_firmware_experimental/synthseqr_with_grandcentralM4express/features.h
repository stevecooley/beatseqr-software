// features.h — compile-time feature enable/disable.
//
// Default: all features enabled (1).
// CI overrides to 0 via:
//   --build-property "compiler.cpp.extra_flags=-DFEATURE_X=0 -DFEATURE_Y=0"
//
// All guards use #if rather than #ifdef so that an explicit -DFEATURE_X=0
// correctly disables the feature (whereas #ifdef only tests for existence).

#ifndef FEATURE_ADVANCED_MODE
#define FEATURE_ADVANCED_MODE 1
#endif

#ifndef FEATURE_CC_MODE
#define FEATURE_CC_MODE 1
#endif

#ifndef FEATURE_PROBABILITY
#define FEATURE_PROBABILITY 1
#endif

#ifndef FEATURE_GATE_MODE
#define FEATURE_GATE_MODE 1
#endif

#ifndef FEATURE_SCALE_QUANTIZATION
#define FEATURE_SCALE_QUANTIZATION 1
#endif

#ifndef FEATURE_PITCH_DRIFT
#define FEATURE_PITCH_DRIFT 1
#endif

#ifndef FEATURE_PATTERN_DIRECTION
#define FEATURE_PATTERN_DIRECTION 1
#endif

#ifndef FEATURE_VARIABLE_PATTERN_LENGTH
#define FEATURE_VARIABLE_PATTERN_LENGTH 1
#endif

#ifndef FEATURE_SWING
#define FEATURE_SWING 1
#endif

#ifndef FEATURE_EXTERNAL_CLOCK
#define FEATURE_EXTERNAL_CLOCK 1
#endif

#ifndef FEATURE_OCTAVE_NOTE_SHIFT
#define FEATURE_OCTAVE_NOTE_SHIFT 1
#endif

#ifndef FEATURE_DIAGNOSTICS
#define FEATURE_DIAGNOSTICS 1
#endif

#ifndef FEATURE_NOTE_AUDITION
#define FEATURE_NOTE_AUDITION 1
#endif

// All feature code is always compiled. Feature flags control runtime behaviour
// only — toggled from the Features submenu in the device config menu.
