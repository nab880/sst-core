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

#ifndef SST_CORE_CONFIGGRAPH_OUTPUT_H
#define SST_CORE_CONFIGGRAPH_OUTPUT_H

#include "sst/core/configGraph.h"
#include "sst/core/params.h"
#include "sst/core/util/filesystem.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <map>
#include <string>
#include <vector>

namespace SST {
class ConfigGraph;

namespace Core {

/**
 * Exception handler class for graph configuration.
 * Provides the interface to handle errors through the throw exception.
 */
class ConfigGraphOutputException : public std::exception
{
public:
    explicit ConfigGraphOutputException(const char* msg)
    {
        exMsg = (char*)malloc(sizeof(char) * (strlen(msg) + 1));
        std::strcpy(exMsg, msg);
    }

    /**
     * @return Exception Message
     */
    virtual const char* what() const noexcept override { return exMsg; }

    /**
     * Exception message generated on call.
     */
protected:
    char* exMsg;
};

/**
 * Outputs configuration data to a specified file path.
 */
class ConfigGraphOutput
{
public:
    explicit ConfigGraphOutput(const char* path);

    virtual ~ConfigGraphOutput() { fclose(outputFile); }

    /**
     * @param cfg Constant pointer to SST configuration
     * @param graph Constant pointer to SST configuration graph
     * @return void
     */
    virtual void generate(const Config* cfg, ConfigGraph* graph) = 0;

protected:
    FILE* outputFile;

    /**
     * Get a named shared parameter set.
     *
     * @param name Name of the set to get
     *
     * @return returns a copy of the reqeusted shared param set
     *
     */
    static std::map<std::string, std::string> getSharedParamSet(const std::string& name)
    {
        return Params::getSharedParamSet(name);
    }


    [[deprecated("getGlobalParamSet() has been deprecated and will be removed in SST 16.  Please use "
                 "getSharedParamSet()")]] static std::map<std::string, std::string>
    getGlobalParamSet(const std::string& name)
    {
        return getSharedParamSet(name);
    }
    /**
     * Get a vector of the names of available shared parameter sets.
     *
     * @return returns a vector of the names of available shared param
     * sets
     *
     */
    static std::vector<std::string> getSharedParamSetNames() { return Params::getSharedParamSetNames(); }

    [[deprecated("getGlobalParamSetNames() has been deprecated and will be removed in SST 16.  Please use "
                 "getSharedParamSetNames()")]] static std::vector<std::string>
    getGlobalParamSetNames()
    {
        return getSharedParamSetNames();
    }

    /**
     * Get a vector of the local keys
     *
     * @return returns a vector of the local keys in this Params
     * object
     *
     */
    std::vector<std::string> getParamsLocalKeys(const Params& params) const { return params.getLocalKeys(); }


    /**
     * Get a vector of the shared param sets this Params object is
     * subscribed to
     *
     * @return returns a vector of the shared param sets his Params
     * object is subscribed to
     *
     */
    std::vector<std::string> getSubscribedSharedParamSets(const Params& params) const
    {
        return params.getSubscribedSharedParamSets();
    }

    [[deprecated("getSubscribedGlobalParamSets() has been deprecated and will be removed in SST 16.  Please use "
                 "getSubscribedSharedParamSets()")]] std::vector<std::string>
    getSubscribedGlobalParamSets(const Params& params) const
    {
        return getSubscribedSharedParamSets(params);
    }
};

} // namespace Core
} // namespace SST

#endif // SST_CORE_CONFIGGRAPH_OUTPUT_H
