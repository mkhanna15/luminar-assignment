#include "pipeline.h"
#include <iostream>

pipeline_t::pipeline_t( size_t buffer_count )
{
    for ( size_t i = 0; i < buffer_count; ++i ) {
        std::shared_ptr<buffer_t> buffer = std::make_shared<buffer_t>();
        buffer->resize( max_buffer_size );
        write_queue.push( buffer );
    }
}

size_t pipeline_t::write(types::Samples const& values, size_t start, size_t end)
{
    // std::cout << "WRITE queue size=" << write_queue.size() << std::endl;

    // get buffer from write queue to copy data
    std::shared_ptr<buffer_t> buffer;
    write_queue.wait_and_pop(buffer);

    // copy data
    buffer->resize( std::min( end - start, max_buffer_size ) );
    for( size_t index = 0; index < buffer->size(); ++index ) {
        (*buffer)[index] = values[start + index];
    }

    // add buffer to read queue
    read_queue.push( buffer );

    // return number of samples written
    return buffer->size();
}

void pipeline_t::read(types::Samples & values)
{
    // std::cout << "READ queue size=" << read_queue.size() << std::endl;

    // get buffer from read queue to read data
    std::shared_ptr<buffer_t> buffer;
    read_queue.wait_and_pop(buffer);

    // read data
    for( size_t index = 0; index < buffer->size(); ++index ) {
        values.push_back( (*buffer)[index] );
    }

    // add free buffer back to write queue
    write_queue.push( buffer );
}
