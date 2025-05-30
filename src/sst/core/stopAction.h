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

#ifndef SST_CORE_STOPACTION_H
#define SST_CORE_STOPACTION_H

#include "sst/core/action.h"
#include "sst/core/output.h"

#include <cinttypes>
#include <iostream>

namespace SST {

/**
 * Action which causes the Simulation to end
 */
class StopAction : public Action
{
private:
    std::string message;
    bool        print_message;

public:
    StopAction()
    {
        setPriority(STOPACTIONPRIORITY);
        print_message = false;
    }

    /** Create a new StopAction which includes a message to be printed when it fires
     */
    explicit StopAction(const std::string& msg)
    {
        setPriority(STOPACTIONPRIORITY);
        print_message = true;
        message       = msg;
    }

    void execute() override
    {
        if ( print_message ) { Output::getDefaultObject().output("%s\n", message.c_str()); }
        endSimulation();
        delete this;
    }

    void print(const std::string& header, Output& out) const override
    {
        out.output("%s StopAction to be delivered at %" PRIu64 "\n", header.c_str(), getDeliveryTime());
    }

    void serialize_order(SST::Core::Serialization::serializer& ser) override
    {
        Action::serialize_order(ser);
        SST_SER(message);
        SST_SER(print_message);
    }
    ImplementSerializable(SST::StopAction)
};

} // namespace SST

#endif // SST_CORE_STOPACTION_H
