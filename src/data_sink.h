/// data_sink.h
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#ifndef DATA_SINK_H
#define DATA_SINK_H

#include "types.h"

namespace sink {
void data_sink(types::Samples samples);
bool order_lost();
} // namespace sink

#endif
