/// main.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "data_processing.h"
#include "data_sink.h"
#include "data_source.h"

#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  types::Buffer buffer;
  source::samples_generator(buffer);

  std::thread t1([&buffer]() {
    std::mutex m;
    std::unique_lock<std::mutex> l(m);

    while (true) {
      buffer.wait_list.wait(l);

      // We have new data, let's process it
      const auto cartesian_samples = processing::transform(*buffer.data);

      sink::data_sink(cartesian_samples);

      if (buffer.dropped_a_packet.load()) {
        std::cout << "Oops, we dropped a packet!" << std::endl;
        return false;
      }

      if (sink::order_lost()) {
        std::cout << "Oops, points lost their order!" << std::endl;
        return false;
      }
    }
  });
      t1.join();

  return 0;
}
