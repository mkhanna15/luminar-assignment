/// data_processing.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "data_processing.h"
#include <cmath>

namespace processing {
types::Samples transform(types::Samples samples) {
  types::Samples cartesian;

  // TODO: this is simply passing data through
  // it needs to be modified so that it returns samples
  // in Cartesian space
  for (int i = 0; i < samples.size(); i++) {
    cartesian.push_back(samples[i]);
  }

  return samples;
}

types::Sample cartesian_transform_t::operator()(types::Sample sample) {
    float azimuth = sample.azimuth;
    float elevation = sample.elevation;
    float range = sample.range;

    float x = range*cos(elevation)*cos(azimuth);
    float y = range*cos(elevation)*sin(azimuth);
    float z = range*sin(elevation);

    types::Sample cartesian_sample;

    // Because data_sink expects, Sample object, hence using existing fields of Sample
    // to have Cartesian coordinates.
    cartesian_sample.azimuth = x;
    cartesian_sample.elevation = y;
    cartesian_sample.range = z;
    cartesian_sample.sequence_no = sample.sequence_no;
    
    return cartesian_sample;
}
} // namespace processing
