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

#ifndef SST_CORE_SYNC_THREADSYNCSIMPLESKIP_H
#define SST_CORE_SYNC_THREADSYNCSIMPLESKIP_H

#include "sst/core/action.h"
#include "sst/core/sst_types.h"
#include "sst/core/sync/syncManager.h"
#include "sst/core/sync/syncQueue.h"
#include "sst/core/threadsafe.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace SST {

class ActivityQueue;
class Link;
class TimeConverter;
class Exit;
class Event;
class Simulation_impl;
class ThreadSyncQueue;

class ThreadSyncSimpleSkip : public ThreadSync
{
public:
    /** Create a new ThreadSync object */
    ThreadSyncSimpleSkip(int num_threads, int thread, Simulation_impl* sim);
    ThreadSyncSimpleSkip() {} // For serialization only
    ~ThreadSyncSimpleSkip();

    void setMaxPeriod(TimeConverter* period);

    void before() override;
    void after() override;
    void execute() override;

    /** Set signals to exchange during sync */
    void setSignals(int end, int usr, int alrm) override;
    /** Return exchanged signals after sync */
    bool getSignals(int& end, int& usr, int& alrm) override;

    /** Cause an exchange of Untimed Data to occur */
    void processLinkUntimedData() override;
    /** Finish link configuration */
    void finalizeLinkConfigurations() override;
    void prepareForComplete() override;

    /** Register a Link which this Sync Object is responsible for */
    void           registerLink(const std::string& name, Link* link) override;
    ActivityQueue* registerRemoteLink(int tid, const std::string& name, Link* link) override;

    uint64_t getDataSize() const;


    // static void disable() { disabled = true; barrier.disable(); }

private:
    // Stores the links until they can be intialized with the right
    // remote data.  It will hold whichever thread registers the link
    // first and will be removed after the second thread registers and
    // the link is properly initialized with the remote data.
    std::unordered_map<std::string, Link*> link_map;

    std::vector<ThreadSyncQueue*>    queues;
    SimTime_t                        my_max_period;
    int                              num_threads;
    int                              thread;
    static SimTime_t                 localMinimumNextActivityTime;
    Simulation_impl*                 sim;
    static Core::ThreadSafe::Barrier barrier[3];
    double                           totalWaitTime;
    bool                             single_rank;
    Core::ThreadSafe::Spinlock       lock;
    static int                       sig_end_;
    static int                       sig_usr_;
    static int                       sig_alrm_;
};

} // namespace SST

#endif // SST_CORE_SYNC_THREADSYNCSIMPLESKIP_H
