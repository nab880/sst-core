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

#ifndef SST_CORE_CORETEST_PARAMCOMPONENT_H
#define SST_CORE_CORETEST_PARAMCOMPONENT_H

#include "sst/core/component.h"

namespace SST::CoreTestParamComponent {

class coreTestParamComponent : public SST::Component
{
public:
    // REGISTER THIS COMPONENT INTO THE ELEMENT LIBRARY
    SST_ELI_REGISTER_COMPONENT(
        coreTestParamComponent,
        "coreTestElement",
        "coreTestParamComponent",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Param Check Component",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "int32t_param",  "Check for integer values", "-1" },
        { "uint32t_param",  "Check for integer values", "0" },
        { "int64t_param",  "Check for integer values", "-1" },
        { "uint64t_param",  "Check for integer values", "0" },
        { "bool_true_param",  "Check for bool values", "true" },
        { "bool_false_param",  "Check for bool values", "false" },
        { "float_param",  "Check for float values", "1.0" },
        { "double_param",  "Check for double values", "1.0" },
        { "string_param",  "Check for string values",  "test" },
        { "scope.int32", "Check scoped params", "-1" },
        { "scope.bool", "Check scoped params", "true" },
        { "scope.string", "Check scoped params", "test" }
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

    coreTestParamComponent(SST::ComponentId_t id, SST::Params& params);
    ~coreTestParamComponent() {}
    void setup() override {}
    void finish() override {}

private:
    coreTestParamComponent();                                                  // for serialization only
    coreTestParamComponent(const coreTestParamComponent&) = delete;            // do not implement
    coreTestParamComponent& operator=(const coreTestParamComponent&) = delete; // do not implement
};

} // namespace SST::CoreTestParamComponent

#endif // SST_CORE_CORETEST_PARAMCOMPONENT_H
