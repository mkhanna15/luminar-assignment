/// main.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "data_processing.h"
#include "data_sink.h"
#include "data_source.h"
#include "pipeline.h"
#include "secret_channel.h"

#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  // create pipeline to temporarily store data
  // pipeline allocates memory for fixed size buffers
  pipeline_t pipeline;

  // atomic to flag when pipeline thread ends
  std::atomic_bool pipeline_thread_done = ATOMIC_VAR_INIT(false);

  types::Buffer buffer;
  source::samples_generator(buffer);

  // determine how many threads can be created to transform read data
  unsigned long const hardware_threads = std::thread::hardware_concurrency();
  // we need setup thread for main, consumer, producer and prepare data
  unsigned long const setup_threads = 4;
  unsigned long const transform_threads_count = hardware_threads > setup_threads ? hardware_threads - setup_threads : 0; 

  types::ConsumerBuffer consumer_buffer;
  consumer_buffer.threads_finished.store(0); // currently no thread has finished
  consumer_buffer.read_data.reserve( pipeline_t::max_buffer_size );

  // create thread that writes data to the pipeline
  std::thread ([&buffer, &pipeline, &pipeline_thread_done]() {
    std::mutex m;
    std::unique_lock<std::mutex> l(m);

    while (true) {
      buffer.wait_list.wait(l);

      // we have new data, let's process it
      types::Samples const& samples = *buffer.data;

      std::chrono::milliseconds start_time = 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());

      // add data to the pipeline to temporarily store the data
      size_t chunk_pos = 0;
      while ( chunk_pos < samples.size() ) {
        size_t const data_stored = pipeline.write( samples, chunk_pos, samples.size() );
        chunk_pos += data_stored;
      }

      // mark the secret channel as data received
      secret_channel::setDataReceived();

      std::chrono::milliseconds end_time_copy = 
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch());

      const auto delta_copy = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_copy - start_time);

      // std::cout << " Producer thread time " << delta_copy.count() << std::endl;

      if (buffer.dropped_a_packet.load()) {
        std::cout << "Oops, we dropped a packet!" << std::endl;
        pipeline_thread_done.store(true);
        return false;
      }

      if (sink::order_lost()) {
        std::cout << "Oops, points lost their order!" << std::endl;
        pipeline_thread_done.store(true);
        return false;
      }
    }
  })
    .detach();

  // create tranform threads that will apply tranform on read data
  std::vector<std::thread> transform_threads_coll;

  for ( size_t thread_index = 0; thread_index < transform_threads_count; ++thread_index ) {
    transform_threads_coll.push_back( std::thread ([&consumer_buffer, &transform_threads_count, thread_index]() {
      std::unique_lock<std::mutex> l(consumer_buffer.shared_mutex);

      while (true) {
        // wait for the read data to be ready for transform
        consumer_buffer.wait_list.wait(l);

        // determine portion of the data that this thread will process
        unsigned long const object_count = consumer_buffer.read_data.size();
        unsigned long const stripe_size = std::max( 1ul, object_count/transform_threads_count );
        unsigned long const start_index = stripe_size*thread_index;
        unsigned long const end_index = std::min( object_count,
                ( thread_index == transform_threads_count - 1 ) ? object_count : start_index + stripe_size );
        // std::cout << "thread index=" << thread_index << " start=" << start_index << " end=" << end << std::endl; 

        std::chrono::milliseconds start_time = 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());
        // apply transform
        for( size_t i = start_index; i < end_index; ++i ) {
          consumer_buffer.read_data[i] = processing::cartesian_transform_t()(consumer_buffer.read_data[i]);
        }

        std::chrono::milliseconds end_time_copy = 
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch());

        const auto delta_copy = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_copy - start_time);

        //std::cout << " Transform thread time " << delta_copy.count() << std::endl;

        // inform waiting thread that I am done with the transform
        ++consumer_buffer.threads_finished;

        // if waiting thread has terminated, end this thread
        if ( consumer_buffer.end_run.load() ) {
          return false;
        }
      }
    }));
  }

  // create consumer thread that reads data from the pipeline,
  // signals transform thread to apply transform, and then sends data to the sink
  std::thread ([&pipeline, &pipeline_thread_done, &consumer_buffer, &transform_threads_count]() {
    while (true) {
      std::chrono::milliseconds start_time = 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());

      // read data from the pipeline when ready
      pipeline.read(consumer_buffer.read_data);

      if ( transform_threads_count > 1 ) {
        // notify transform threads that data is ready for transform
        consumer_buffer.wait_list.notify_all();

        // wait for all transform threads to finish
        while ( consumer_buffer.threads_finished.load() != transform_threads_count ) {}

        // all transform threads are done. reset counter for next run
        consumer_buffer.threads_finished.store(0);
      }
      else {
        // apply transform
        for( size_t index = 0; index < consumer_buffer.read_data.size(); ++index ) {
          consumer_buffer.read_data[index] = processing::cartesian_transform_t()(consumer_buffer.read_data[index]);
        }
      }

      // propagate transformed data to sink
      sink::data_sink(consumer_buffer.read_data);

      // clear the read data to store new data
      //consumer_buffer.read_data.clear();

      while ( !consumer_buffer.read_data.empty() ) {
        consumer_buffer.read_data.pop_back();
      }

      std::chrono::milliseconds end_time_copy = 
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch());

      const auto delta_copy = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_copy - start_time);

      // std::cout << " Consumer thread time " << delta_copy.count() << std::endl;

      // if producer thread has terminated, exit this thread and signal all the transform threads to end run
      if ( pipeline_thread_done.load() ) {
        consumer_buffer.end_run.store(true);
        consumer_buffer.wait_list.notify_all(); // called again, because transform threads may be waiting for next run
        return false;
      }
    }
  })
    .detach();

  // wait for all the transform threads to finish, as they will be the last one to end their run
  for ( size_t i = 0; i < transform_threads_count; ++i ) {
    transform_threads_coll[i].join();
  }

  return 0;
}
