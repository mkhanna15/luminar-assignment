/// data_sink.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "data_sink.h"
#include "secret_channel.h"

#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

namespace {
std::mutex mut;
unsigned long sequence_number = 0;
std::atomic_bool unordered = ATOMIC_VAR_INIT(false);
} // namespace

namespace sink {
void data_sink(types::Samples const& samples) {
  std::lock_guard<std::mutex> l(mut);

  //secret_channel::setDataReceived();

  for (auto &p : samples) {
    if (p.sequence_no != sequence_number) {
      unordered.store(true);
    }
    ++sequence_number;
  }

  std::cout << "Got " << samples.size() << " samples" << std::endl;
}

bool order_lost() { return unordered.load(); }
} // namespace sink
