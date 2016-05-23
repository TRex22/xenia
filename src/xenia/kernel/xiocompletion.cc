/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2015 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/kernel/xiocompletion.h"

namespace xe {
namespace kernel {

XIOCompletion::XIOCompletion(KernelState* kernel_state)
    : XObject(kernel_state, kTypeIOCompletion) {
  notification_semaphore_ = threading::Semaphore::Create(0, kMaxNotifications);
}

XIOCompletion::~XIOCompletion() = default;

void XIOCompletion::QueueNotification(IONotification& notification) {
  std::unique_lock<std::mutex> lock(notification_lock_);

  notifications_.push(notification);
  notification_semaphore_->Release(1, nullptr);
}

XIOCompletion::IONotification XIOCompletion::DequeueNotification() {
  std::unique_lock<std::mutex> lock(notification_lock_);
  assert_false(notifications_.empty());

  auto notification = notifications_.front();
  notifications_.pop();

  return notification;
}

bool XIOCompletion::WaitForNotification(uint64_t wait_ticks) {
  auto ms = std::chrono::milliseconds(TimeoutTicksToMs(wait_ticks));
  auto res = threading::Wait(notification_semaphore_.get(), false, ms);
  if (res == threading::WaitResult::kSuccess) {
    return true;
  }

  return false;
}

}  // namespace kernel
}  // namespace xe
