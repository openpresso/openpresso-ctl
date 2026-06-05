#pragma once

#include <cstdint>
#include <print>

#include <google/protobuf/duration.pb.h>
#include <grpcpp/grpcpp.h>

inline bool checkStatus(const grpc::Status& status)
{
  if (!status.ok()) {
    std::println(stderr, "RPC error [{}]: {}", static_cast<int>(status.error_code()), status.error_message());
    return false;
  }
  return true;
}

// Converts a floating-point seconds value (e.g. 0.5) to google::protobuf::Duration.
// seconds() carries whole seconds, nanos() carries the sub-second remainder —
// both are additive components of the same value, not alternative representations.
inline google::protobuf::Duration secondsToDuration(double secs)
{
  google::protobuf::Duration duration;
  const auto whole = static_cast<int64_t>(secs);
  const auto frac = static_cast<int32_t>((secs - static_cast<double>(whole)) * 1'000'000'000);
  duration.set_seconds(whole);
  duration.set_nanos(frac);
  return duration;
}
