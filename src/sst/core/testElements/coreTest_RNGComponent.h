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

#ifndef SST_CORE_CORETEST_RNGCOMPONENT_H
#define SST_CORE_CORETEST_RNGCOMPONENT_H

#include "sst/core/component.h"
#include "sst/core/rng/rng.h"

#include <string>

using namespace SST;
using namespace SST::RNG;

namespace SST::CoreTestRNGComponent {

class coreTestRNGComponent : public SST::Component
{
public:
    // REGISTER THIS COMPONENT INTO THE ELEMENT LIBRARY
    SST_ELI_REGISTER_COMPONENT(
        coreTestRNGComponent,
        "coreTestElement",
        "coreTestRNGComponent",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Random number generation component",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "seed_w",  "The seed to use for the random number generator", "7" },
        { "seed_z",  "The seed to use for the random number generator", "5" },
        { "seed",    "The seed to use for the random number generator.", "11" },
        { "rng",     "The random number generator to use (Marsaglia or Mersenne), default is Mersenne", "Mersenne"},
        { "count",   "The number of random numbers to generate, default is 1000", "1000" },
        { "verbose", "Sets the output verbosity of the component", "0" }
    )

    // Optional since there is nothing to document
    SST_ELI_DOCUMENT_STATISTICS(
    )

    // Optional since there is nothing to document
    SST_ELI_DOCUMENT_PORTS(
    )

    // Optional since there is nothing to document
    SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    )

    coreTestRNGComponent(SST::ComponentId_t id, SST::Params& params);
    ~coreTestRNGComponent();
    void setup() override {}
    void finish() override {}

private:
    coreTestRNGComponent();                                                // for serialization only
    coreTestRNGComponent(const coreTestRNGComponent&) = delete;            // do not implement
    coreTestRNGComponent& operator=(const coreTestRNGComponent&) = delete; // do not implement

    virtual bool tick(SST::Cycle_t);

    Output*     output;
    Random*     rng;
    std::string rng_type;
    int         rng_max_count;
    int         rng_count;
};

} // namespace SST::CoreTestRNGComponent

#endif // SST_CORE_CORETEST_RNGCOMPONENT_H
