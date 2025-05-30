// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef SST_CORE_HEARTBEAT_H
#define SST_CORE_HEARTBEAT_H

#include "sst/core/action.h"
#include "sst/core/config.h"
#include "sst/core/cputimer.h"
#include "sst/core/output.h"
#include "sst/core/sst_types.h"

#include <set>

namespace SST {

class Simulation_impl;
class TimeConverter;

/**
  \class SimulatorHeartbeat
    An optional heartbeat to show progress in a simulation
*/
class SimulatorHeartbeat : public Action
{
public:
    /**
    Create a new heartbeat object for the simulation core to show progress
    */
    SimulatorHeartbeat(Config* cfg, int this_rank, Simulation_impl* sim, TimeConverter* period);
    ~SimulatorHeartbeat() = default;

    // Used to re-schedule the new heartbeat event during restart
    void schedule();

    void serialize_order(SST::Core::Serialization::serializer& ser) override;
    ImplementSerializable(SST::SimulatorHeartbeat)

private:
    SimulatorHeartbeat()                          = default;
    SimulatorHeartbeat(const SimulatorHeartbeat&) = delete;
    SimulatorHeartbeat& operator=(const SimulatorHeartbeat&) = delete;

    void           execute() override;
    int            rank;
    TimeConverter* m_period;
    double         lastTime;
};

} // namespace SST

#endif // SST_CORE_HEARTBEAT_H
