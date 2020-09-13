/// data_processing.h
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "types.h"

namespace processing {
// TODO: update this function so that it correctly transforms data
// from spherical to Cartesian coordinates
types::Samples transform(types::Samples samples);

struct cartesian_transform_t {
    types::Sample operator()(types::Sample sample);
};
} // namespace processing

#endif
