## Tiny Data Client

### Prerequisites

1. CMake >= `3.10.0`
2. A reasonably standards compliant C++ compiler and development environment you're comfortable with, such as:
  * `gcc` or `clang` on `Linux` or `OS X`
  * `MSVC` or `Visual Studio Code` on `Windows`
  
### Setup

1. Depending on your platform, execute either `./scripts/generate_makefiles.sh` or `./scripts/generate_msvc_project.bat` to write build files to `./build/`
2. Either `cd build; make -j4` or open up whatever project files you may have generated in your favorite editor

**Warning**: `./scripts/build_linux.sh` and `./scripts/build_windows.bat` delete the `./build/` folder before compiling!

### Your Task

This project represents an idealized, possibly even *dumb*, representation of a data pipeline that's responsible for consuming data from a source, applying some transformations, and then feeding the transformed data into an upstream sink.  This data is ordered, and in the real-world is time sensitive, so in addition to maintaining the correct pipeline behavior, we are looking at performance (throughput).

In this case the data represents chunks of a point cloud - points in 3D space.
  
The chunks come from the `source::samples_generator` function in `data_source.h`.

The (unwritten :)) data processing lives in `data_processing.h`, specifically `processing::transform`.

The sink that accepts the transformed data is in `data_sink.h`.

Your task is to write a function in `data_processing.h` that accepts data in *spherical coordinates* from `source::samples_generator`, converts them to *Cartesian*, and then passes the Cartesian samples to `sink::data_sink` in `data_sink.h`.

The skeleton of such a pipeline is already defined in `main.cpp`. Feel free to restructure that however you see fit as long as the data still flows correctly through the pipeline.

Now `source::samples_generator` is a stream of never ending data that will start to come in faster and faster - the game is to see how long your program can run before the pipeline grinds to a halt.

Then once that happens, you need to figure out how to make the pipeline more efficient! Feel free to use any profiling tools you like, but please try to avoid changing any implementation details in either `data_source.h` or `data_sink.h`.

### Spending Your Time

This coding chalenge is designed to be a multi-faceted challenge against multiple traits that we value in our engineers.  A fast, clean, submission that is a technological wonder is always preferred, but for us to get a better picture of your abilities do not spend more than a few hours on this project.  You may bump up against hard limits set by the design decisions made in those files - that's okay! If that happens please consider the changes you *would* make to the overall pipeline and discuss them in the report I'm about to tell you more about.

### Deliverables

We would like the following from you:

1. a cleanly compiling and running program that can correctly propagate transformed data from source to sink for some amount of time, no matter how small.
2. a short report discussing your initial implementation and indicating how you improved performance, what profiling tools and techniques you used, and what changes you might make to the overall pipeline architecture to increase throughput.  Please include this as a markdown (.md) text file in the root of your submission.
3. if you never ran into any performance issues, OR the task was hopelessly difficult to understand, OR the initial code we gave you didn't work, OR you just have ideas for how this test could be improved, please also include those thoughts!

Feel free to send us back a complete `git` repo of your work that includes the history of your implementation, but please explain any branch usage that you may have included in your report document.

### Licensing / Permission

The code inside of this challenge is owned and controlled by Luminar Technologies Inc.  Any changes or additions you make to this code must be compatible with this license.  Please do not include work for which you do not hold rights over, or add/include libraries that would violate our license or theirs.

This coding challenge is confidential, and is meant to measure your abilities within a reasonable time period and without prior knowledge.  Please do not distribute or discuss your submission outside of the Luminar Technilogies interview process.
