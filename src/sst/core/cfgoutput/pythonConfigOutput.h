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
//

#ifndef SST_CORE_PYTHON_CONFIG_OUTPUT_H
#define SST_CORE_PYTHON_CONFIG_OUTPUT_H

#include "sst/core/configGraph.h"
#include "sst/core/configGraphOutput.h"

#include <cstddef>
#include <map>
#include <string>

namespace SST::Core {

class PythonConfigGraphOutput : public ConfigGraphOutput
{
public:
    explicit PythonConfigGraphOutput(const char* path);

    virtual void generate(const Config* cfg, ConfigGraph* graph) override;

protected:
    ConfigGraph* getGraph() { return graph_; }
    void         generateParams(const Params& params);
    void         generateCommonComponent(const char* objName, const ConfigComponent* comp);
    void         generateSubComponent(const char* owner, const ConfigComponent* comp);
    void         generateComponent(const ConfigComponent* comp, bool output_parition_info);
    void         generateStatGroup(const ConfigGraph* graph, const ConfigStatGroup& grp);

    const std::string& getLinkObject(LinkId_t id, const std::string& name, bool no_cut);

    char* makePythonSafeWithPrefix(const std::string& name, const std::string& prefix) const;
    void  makeBufferPythonSafe(char* buffer) const;
    char* makeEscapeSafe(const std::string& input) const;
    bool  strncmp(const char* a, const char* b, const size_t n) const;
    bool  isMultiLine(const char* check) const;
    bool  isMultiLine(const std::string& check) const;

private:
    ConfigGraph*                         graph_;
    std::map<LinkId_t, std::string>      link_map_;
    std::string                          py_parent_name_;
    std::map<StatisticId_t, std::string> shared_stat_map_;
};

} // namespace SST::Core

#endif // SST_CORE_PYTHON_CONFIG_OUTPUT_H
