/// data_source.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "data_source.h"
#include "secret_channel.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

namespace {
// Some implementation details, kindly do your best to ignore
//
std::chrono::milliseconds start_time;

constexpr int double_rate_every = 6; // this is in chunks
constexpr float update_interval_scale_factor = 0.3f;
constexpr int min_update_interval = 32;
int update_interval = 1024;

constexpr int double_chunks_every = 3; // this is in chunks
constexpr float chunk_scale_factor = 0.2f;
int chunk_size = 2048;

long samples_sent = 0;
long chunks_sent = 0;

std::random_device rnd;
std::mt19937 gen;

unsigned long sequence_number = 0;

std::uniform_real_distribution<float> azimuth_distribution(0.0f, 180.0f);
std::uniform_real_distribution<float> elevation_distribution(0.0f, 30.0f);
std::uniform_real_distribution<float> range_distribution(0.0f, 100.0f);

void initialize() {
  start_time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());

  gen.seed(rnd());
}

types::Sample random_sample() {
  samples_sent++;

  types::Sample s;

  s.azimuth = azimuth_distribution(gen);
  s.elevation = elevation_distribution(gen);
  s.range = range_distribution(gen);
  s.sequence_no = sequence_number;
  ++sequence_number;

  return s;
}

types::Samples read_point_cloud_chunk(int chunk_size_to_read) {
  types::Samples chunk(chunk_size_to_read);
  std::generate(chunk.begin(), chunk.end(), random_sample);

  return chunk;
}
} // namespace

namespace source {
void samples_generator(types::Buffer &buffer) {
  initialize();

  std::thread([&]() {
    bool keep_going = true;

    while (keep_going) {
      // wait a tick
      std::this_thread::sleep_for(std::chrono::milliseconds(update_interval));

      // false means sink has already processed the previous chunk
      auto ready_to_send = secret_channel::setDataSent() == false;

      if (!ready_to_send) {
        const auto end_time =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
        const auto delta =
            std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                  start_time);
        std::cout << "Pipeline went " << delta.count()
                  << " milliseconds before dropping a packet!" << std::endl;

        buffer.dropped_a_packet.store(true);

        return;
      }

      buffer.data->clear();
      buffer.data->reserve(chunk_size);

      types::Samples chunk = read_point_cloud_chunk(chunk_size);
      *(buffer.data) = chunk;

      chunks_sent++;

      buffer.wait_list.notify_all();

      if (((chunks_sent % double_rate_every) == 0) &&
          update_interval > min_update_interval) {
        update_interval =
            std::lround(static_cast<float>(update_interval) -
                        (update_interval * update_interval_scale_factor));
      }

      if ((chunks_sent % double_chunks_every) == 0) {
        chunk_size += std::lround(chunk_size * chunk_scale_factor);
      }
    }
  })
      .detach();
}
} // namespace source
