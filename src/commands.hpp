#pragma once

#include "openpresso_proto/openpresso.grpc.pb.h"

#include <string>

namespace commands
{

int resetScales(openpressod::OpenpressoDaemon::Stub& stub);
int getProfile(openpressod::OpenpressoDaemon::Stub& stub);
int setProfile(openpressod::OpenpressoDaemon::Stub& stub, const std::string& file);

int getUserSettings(openpressod::OpenpressoDaemon::Stub& stub);
int setUserSettings(openpressod::OpenpressoDaemon::Stub& stub, int steamTemp);

int setPower(openpressod::OpenpressoDaemon::Stub& stub, bool powerOn);
int getPower(openpressod::OpenpressoDaemon::Stub& stub);

int startBrew(openpressod::OpenpressoDaemon::Stub& stub);
int stopBrew(openpressod::OpenpressoDaemon::Stub& stub);
int getBrewState(openpressod::OpenpressoDaemon::Stub& stub);

int setSteam(openpressod::OpenpressoDaemon::Stub& stub, bool steamOn);
int getSteam(openpressod::OpenpressoDaemon::Stub& stub);

int events(openpressod::OpenpressoDaemon::Stub& stub);
int metrics(openpressod::OpenpressoDaemon::Stub& stub, double rateSecs, openpressod::PidSource pidSource);

} // namespace commands
