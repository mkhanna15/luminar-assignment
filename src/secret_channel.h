/// secret_channel.h
/// Copyright (c) 2020, Luminar Technologies, Inc.
/// This material contains confidential and trade secret information of Luminar
/// Technologies. Reproduction, adaptation, and distribution are prohibited,
/// except to the extent expressly permitted in writing by Luminar Technologies.

#ifndef SECRET_CHANNEL_H
#define SECRET_CHANNEL_H

#include "types.h"

#include <atomic>
#include <memory>

namespace secret_channel {
bool setDataSent();
void setDataReceived();
} // namespace secret_channel

#endif
