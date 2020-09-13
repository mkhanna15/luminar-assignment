/// types.h
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#ifndef TYPES_H
#define TYPES_H

#include <atomic>
#include <condition_variable>
#include <vector>

namespace types {
struct Sample {
  float azimuth = -1.0f;
  float elevation = -1.0f;
  float range = -1.0f;
  unsigned long sequence_no = -1;
};

using Samples = std::vector<Sample>;

struct Buffer {
  std::condition_variable wait_list;
  std::shared_ptr<Samples> data = std::make_shared<Samples>();

  std::atomic_bool dropped_a_packet = ATOMIC_VAR_INIT(false);
};

struct ConsumerBuffer {
  Samples read_data;
  std::mutex shared_mutex;
  std::condition_variable wait_list;
  std::atomic_bool end_run = ATOMIC_VAR_INIT(false);
  std::atomic<int> threads_finished;
};

} // namespace types

#endif
