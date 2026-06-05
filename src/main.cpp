#include "commands.hpp"

#include <CLI/CLI.hpp>
#include <grpcpp/grpcpp.h>

int main(int argc, char** argv)
{
  CLI::App app{"openpresso-ctl \u2014 CLI client for OpenpressoDaemon"};
  app.set_help_all_flag("--help-all", "Expand all subcommand help");
  app.require_subcommand(1);

  std::string socketPath;
  app.add_option("-s,--socket", socketPath, "unix://path or host:port [env: OPENPRESSO_SOCK]")
    ->envname("OPENPRESSO_SOCK")
    ->default_val("unix://run/openpresso/openpresso.sock");

  // ── scale ────────────────────────────────────────────────────────────────
  app.add_subcommand("reset-scales", "Zero the weight scales");

  // ── profile ──────────────────────────────────────────────────────────────
  app.add_subcommand("get-profile", "Print active brew profile JSON");

  auto* cmdSetProfile =
    app.add_subcommand("set-profile",
                       "Upload a brew profile from proto-JSON (file or stdin).\n"
                       "Field names follow proto3 JSON mapping (camelCase or snake_case).\n"
                       "Duration values use the \"Xs\" string form, e.g. \"30s\" or \"4.5s\".\n"
                       "Example: {\"name\":\"Espresso\",\"temperature\":93,\"totalWeight\":36,\n"
                       "          \"steps\":[{\"pressure\":200,\"stepTime\":\"5s\"},\n"
                       "                    {\"pressure\":900,\"totalWeight\":36}]}");
  std::string profileFile;
  cmdSetProfile->add_option("file", profileFile, "Path to JSON file (omit to read from stdin)");

  // ── settings ─────────────────────────────────────────────────────────────
  app.add_subcommand("get-user-settings", "Print user settings");

  auto* cmdSetSettings = app.add_subcommand("set-user-settings", "Update user settings");
  int steamTemp = 0;
  cmdSetSettings->add_option("--steam-temp", steamTemp, "Steam temperature")->required();

  // ── power ────────────────────────────────────────────────────────────────
  app.add_subcommand("power-on", "Turn the machine on");
  app.add_subcommand("power-off", "Turn the machine off");
  app.add_subcommand("get-power", "Query power state");

  // ── brew ─────────────────────────────────────────────────────────────────
  app.add_subcommand("start-brew", "Start the brew sequence");
  app.add_subcommand("stop-brew", "Abort the brew sequence");
  app.add_subcommand("get-brew-state", "Query whether a brew is running");

  // ── steam ────────────────────────────────────────────────────────────────
  app.add_subcommand("steam-on", "Enable steam mode");
  app.add_subcommand("steam-off", "Disable steam mode");
  app.add_subcommand("get-steam", "Query steam mode state");

  // ── streaming ────────────────────────────────────────────────────────────
  app.add_subcommand("events", "Stream state-change events  (Ctrl+C to quit)");

  auto* cmdMetrics = app.add_subcommand("metrics", "Stream live sensor metrics  (Ctrl+C to quit)");
  double metricsRate = 1.0;
  cmdMetrics->add_option("-r,--rate", metricsRate, "Update interval in seconds (e.g. 0.5, 1, 2)")
    ->default_val(1.0)
    ->check(CLI::PositiveNumber);
    
  std::map<std::string, openpressod::PidSource> pidSourceMap{
    {"none", openpressod::PidSource_UNSPECIFIED},
    {"brew", openpressod::BrewTemperatureController},
    {"steam-preheat", openpressod::SteamPreheatTemperatureController},
    {"steam-active", openpressod::SteamActiveTemperatureController}
  };
  openpressod::PidSource pidSource = openpressod::PidSource_UNSPECIFIED;
  cmdMetrics->add_option("--pid-source", pidSource, "Include PID metrics for a specific controller")
    ->transform(CLI::CheckedTransformer(pidSourceMap, CLI::ignore_case));

  CLI11_PARSE(app, argc, argv);

  // ── gRPC channel ─────────────────────────────────────────────────────────
  auto stub = openpressod::OpenpressoDaemon::NewStub(
    grpc::CreateChannel(socketPath, grpc::InsecureChannelCredentials()));

  // ── dispatch ─────────────────────────────────────────────────────────────
  const std::string cmd = app.get_subcommands().front()->get_name();

  if (cmd == "reset-scales") {
    return commands::resetScales(*stub);
  }
  if (cmd == "get-profile") {
    return commands::getProfile(*stub);
  }
  if (cmd == "set-profile") {
    return commands::setProfile(*stub, profileFile);
  }
  if (cmd == "get-user-settings") {
    return commands::getUserSettings(*stub);
  }
  if (cmd == "set-user-settings") {
    return commands::setUserSettings(*stub, steamTemp);
  }
  if (cmd == "power-on") {
    return commands::setPower(*stub, true);
  }
  if (cmd == "power-off") {
    return commands::setPower(*stub, false);
  }
  if (cmd == "get-power") {
    return commands::getPower(*stub);
  }
  if (cmd == "start-brew") {
    return commands::startBrew(*stub);
  }
  if (cmd == "stop-brew") {
    return commands::stopBrew(*stub);
  }
  if (cmd == "get-brew-state") {
    return commands::getBrewState(*stub);
  }
  if (cmd == "steam-on") {
    return commands::setSteam(*stub, true);
  }
  if (cmd == "steam-off") {
    return commands::setSteam(*stub, false);
  }
  if (cmd == "get-steam") {
    return commands::getSteam(*stub);
  }
  if (cmd == "events") {
    return commands::events(*stub);
  }
  if (cmd == "metrics") {
    return commands::metrics(*stub, metricsRate, pidSource);
  }

  return 0;
}