#ifndef PIPELINE_H
#define PIPELINE_H

#include "threadsafe_queue.h"
#include "types.h"

#include <vector>

//
// Pipeline acts as temporary storage and saves the data from the producer 
// to be read by the consumer at later point in time.
//
class pipeline_t
{
private:
    typedef std::vector<types::Sample> buffer_t;

    threadsafe_queue_t<std::shared_ptr<buffer_t>> write_queue;
    threadsafe_queue_t<std::shared_ptr<buffer_t>> read_queue;

    pipeline_t( pipeline_t const& ) = delete;
    pipeline_t& operator=( pipeline_t const& ) = delete;

public:
    static constexpr size_t max_buffer_size = 25000;

    explicit pipeline_t( size_t buffer_count = 5000 );

    // write samples from [start, end)
    size_t write(types::Samples const& values, size_t start, size_t end);

    // read samples
    void read(types::Samples & values);
};

#endif
