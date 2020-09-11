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
} // namespace processing
