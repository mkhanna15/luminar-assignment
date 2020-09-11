/// secret_channel.cpp
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#include "secret_channel.h"

#include <thread>

namespace {
std::atomic_flag signal;
}

namespace secret_channel {
bool setDataSent() { return signal.test_and_set(); }

void setDataReceived() { signal.clear(); }
} // namespace secret_channel
