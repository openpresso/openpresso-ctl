#include "commands.hpp"

#include "openpresso_proto/openpresso.pb.h"
#include "util.hpp"

#include <cstdio>
#include <fstream>
#include <print>
#include <string>

#include <google/protobuf/empty.pb.h>
#include <google/protobuf/json/json.h>
#include <grpcpp/grpcpp.h>

using namespace openpressod;
using grpc::ClientContext;

// ─── local helpers ────────────────────────────────────────────────────────────

namespace
{

BrewProfile profileFromJson(const std::string& jsonText)
{
  BrewProfile profile;
  const auto status = google::protobuf::json::JsonStringToMessage(jsonText, &profile);
  if (!status.ok()) {
    throw std::runtime_error(std::string(status.message()));
  }
  return profile;
}

std::string readText(const std::string& path)
{
  if (path.empty()) {
    return {std::istreambuf_iterator<char>(std::cin), {}};
  }

  std::ifstream file(path);
  if (!file) {
    throw std::runtime_error("Cannot open: " + path);
  }
  return {std::istreambuf_iterator<char>(file), {}};
}

} // namespace

// ─── commands ─────────────────────────────────────────────────────────────────

namespace commands
{

int resetScales(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  google::protobuf::Empty resp;
  if (!checkStatus(stub.resetScales(&ctx, req, &resp))) {
    return 1;
  }
  std::println("Scales zeroed.");
  return 0;
}

int getProfile(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  BrewProfile profile;
  if (!checkStatus(stub.getBrewProfile(&ctx, req, &profile))) {
    return 1;
  }

  std::string profileJson;
  google::protobuf::json::PrintOptions opts;
  opts.add_whitespace = true;
  opts.always_print_fields_with_no_presence = true;

  if (const auto status = google::protobuf::json::MessageToJsonString(profile, &profileJson, opts); !status.ok()) {
    std::println(stderr, "JSON serialisation failed: {}", std::string(status.message()));
    return 1;
  }
  std::println("{}", profileJson);
  return 0;
}

int setProfile(OpenpressoDaemon::Stub& stub, const std::string& file)
{
  std::string jsonText;
  try {
    jsonText = readText(file);
  }
  catch (const std::exception& e) {
    std::println(stderr, "Read error: {}", e.what());
    return 1;
  }

  BrewProfile profile;
  try {
    profile = profileFromJson(jsonText);
  }
  catch (const std::exception& e) {
    std::println(stderr, "Profile parse error: {}", e.what());
    return 1;
  }

  ClientContext ctx;
  google::protobuf::Empty result;
  if (!checkStatus(stub.setBrewProfile(&ctx, profile, &result))) {
    return 1;
  }
  std::println("Profile uploaded.");
  return 0;
}

int getUserSettings(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  UserSettings settings;
  if (!checkStatus(stub.getUserSettings(&ctx, req, &settings))) {
    return 1;
  }
  std::println("Steam Temperature: {}", settings.steamttemperature());
  return 0;
}

int setUserSettings(OpenpressoDaemon::Stub& stub, int steamTemp)
{
  ClientContext ctx;
  UserSettings req;
  req.set_steamttemperature(steamTemp);
  google::protobuf::Empty resp;
  if (!checkStatus(stub.setUserSettings(&ctx, req, &resp))) {
    return 1;
  }
  std::println("User settings updated.");
  return 0;
}

int setPower(OpenpressoDaemon::Stub& stub, bool powerOn)
{
  ClientContext ctx;
  PowerState req;
  req.set_value(powerOn);
  google::protobuf::Empty resp;
  if (!checkStatus(stub.setPowerState(&ctx, req, &resp))) {
    return 1;
  }
  std::println("Power: {}", powerOn ? "ON" : "OFF");
  return 0;
}

int getPower(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  PowerState state;
  if (!checkStatus(stub.getPowerState(&ctx, req, &state))) {
    return 1;
  }
  std::println("Power: {}", state.value() ? "ON" : "OFF");
  return 0;
}

int startBrew(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  google::protobuf::Empty resp;
  if (!checkStatus(stub.startBrew(&ctx, req, &resp))) {
    return 1;
  }
  std::println("Brew started.");
  return 0;
}

int stopBrew(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  google::protobuf::Empty resp;
  if (!checkStatus(stub.stopBrew(&ctx, req, &resp))) {
    return 1;
  }
  std::println("Brew stopped.");
  return 0;
}

int getBrewState(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  BrewState state;
  if (!checkStatus(stub.getBrewState(&ctx, req, &state))) {
    return 1;
  }
  std::println("Brew: {}", state.value() ? "ACTIVE" : "IDLE");
  return 0;
}

int setSteam(OpenpressoDaemon::Stub& stub, bool steamOn)
{
  ClientContext ctx;
  SteamModeState req;
  req.set_isactive(steamOn);
  google::protobuf::Empty resp;
  if (!checkStatus(stub.setSteamModeState(&ctx, req, &resp))) {
    return 1;
  }
  std::println("Steam: {}", steamOn ? "ON" : "OFF");
  return 0;
}

int getSteam(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  SteamModeState state;
  if (!checkStatus(stub.getSteamModeState(&ctx, req, &state))) {
    return 1;
  }
  std::println("Steam: {}", state.isactive() ? "ON" : "OFF");
  return 0;
}

int events(OpenpressoDaemon::Stub& stub)
{
  ClientContext ctx;
  google::protobuf::Empty req;
  auto stream = stub.events(&ctx, req);

  std::println("Watching events  (Ctrl+C to stop)\u2026");

  Event event;
  google::protobuf::json::PrintOptions opts;
  opts.add_whitespace = true;
  opts.always_print_fields_with_no_presence = true;

  while (stream->Read(&event)) {
    std::string eventJson;
    if (const auto status = google::protobuf::json::MessageToJsonString(event, &eventJson, opts);
        !status.ok()) {
      std::println(stderr, "[events] JSON serialisation failed: {}", std::string(status.message()));
      continue;
    }
    std::println("{}", eventJson);
  }

  return checkStatus(stream->Finish()) ? 0 : 1;
}

int metrics(OpenpressoDaemon::Stub& stub, double rateSecs, PidSource pidSource)
{
  ClientContext ctx;
  MetricsRequest req;
  *req.mutable_updaterate() = secondsToDuration(rateSecs);
  req.set_pidsource(pidSource);

  auto stream = stub.metrics(&ctx, req);

  std::println(stderr, "Streaming metrics at {} s interval  (Ctrl+C to stop)\u2026", rateSecs);

  static constexpr auto millisMult = 0.001F;

  Metrics metrics;
  while (stream->Read(&metrics)) {
    // \r rewrites the same terminal line on every update.
    // std::print does not guarantee a flush, so fflush explicitly.
    std::print("\rTemp: {:5.2f} \xc2\xb0"
               "C   "
               "Pres: {:5.2f} mbar   "
               "Wght: {:5.2f} g   "
               "Flow: {:5.2f} ml/s",
               millisMult * static_cast<float>(metrics.temperature()),
               millisMult * static_cast<float>(metrics.pressure()),
               millisMult * static_cast<float>(metrics.weight()),
               millisMult * static_cast<float>(metrics.flowrate()));
    
    if (metrics.has_pidmetrics()) {
      const auto& pid = metrics.pidmetrics();
      std::print("   PID[P:{:5.2f} I:{:5.2f} D:{:5.2f} F:{:5.2f} S:{:5.2f}]",
                 pid.p(), pid.i(), pid.d(), pid.f(), pid.sum());
    }
    std::print("  ");
    std::fflush(stdout); // NOLINT(cert-err33-c)
  }
  std::println("");

  return checkStatus(stream->Finish()) ? 0 : 1;
}

} // namespace commands
