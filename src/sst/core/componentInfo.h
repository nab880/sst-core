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

#ifndef SST_CORE_COMPONENTINFO_H
#define SST_CORE_COMPONENTINFO_H

#include "sst/core/params.h"
#include "sst/core/serialization/serializer_fwd.h"
#include "sst/core/sst_types.h"
#include "sst/core/timeConverter.h"

#include <functional>
#include <map>
#include <string>
#include <unordered_set>

namespace SST {

class BaseComponent;
class ComponentInfoMap;
class LinkMap;

class ConfigPortModule;
class ConfigComponent;
class ConfigStatistic;

class Simulation_impl;

namespace Core::Serialization::pvt {
class SerializeBaseComponentHelper;
} // namespace Core::Serialization::pvt

class ComponentInfo
{

public:
    using statEnableList_t = std::vector<ConfigStatistic>; /*!< List of Enabled Statistics */

    // Share Flags for SubComponent loading
    static const uint64_t SHARE_PORTS  = 0x1;
    static const uint64_t SHARE_STATS  = 0x2;
    static const uint64_t INSERT_STATS = 0x4;
    static const uint64_t SHARE_NONE   = 0x0;

private:
    // Mask to make sure users are only setting the flags that are
    // available to them
    static const uint64_t USER_FLAGS = 0x7;

    // Friend classes
    friend class Simulation_impl;
    friend class BaseComponent;
    friend class ComponentInfoMap;
    friend class Core::Serialization::pvt::SerializeBaseComponentHelper;

    /**
       Component ID.

       SubComponents share the lower bits (defined by macros in
       sst_types.h) with their Component parent.  However, every
       SubComponent has a unique ID.
    */
    const ComponentId_t id_;

    ComponentInfo*    parent_info;
    /**
       Name of the Component/SubComponent.
     */
    const std::string name;

    /**
       Type of the Component/SubComponent.
     */
    const std::string type;

    /**
       LinkMap containing the links assigned to this
       Component/SubComponent in the python file.

       This field is not used for SubComponents loaded with
       loadAnonymousSubComponent().
     */
    LinkMap* link_map;

    /**
       Pointer to the Component created using this ComponentInfo.
     */
    BaseComponent* component;

    /**
       SubComponents loaded into the Component/SubComponent.
     */
    std::map<ComponentId_t, ComponentInfo> subComponents;

    /**
       Parameters defined in the python file for the (Sub)Component.

       This field is used for only a short time while loading
       SubComponents via loadUserSubComponent().

       This pointer is invalid after simulation wire-up.
    */
    Params* params;

    TimeConverter defaultTimeBase;

    std::map<std::string, std::vector<ConfigPortModule>>* portModules         = nullptr;
    std::map<StatisticId_t, ConfigStatistic>*             stat_configs_       = nullptr;
    std::map<std::string, StatisticId_t>*                 enabled_stat_names_ = nullptr;
    bool                                                  enabled_all_stats_  = false;
    ConfigStatistic*                                      all_stat_config_    = nullptr;

    uint8_t statLoadLevel;

    std::vector<double> coordinates;

    uint64_t subIDIndex;

    // Variables only used by SubComponents

    /**
       Name of the slot this SubComponent was loaded into.  This field
       is not used for Components.
     */
    const std::string slot_name;

    /**
       Index in the slot this SubComponent was loaded into.  This field
       is not used for Components.
     */
    int slot_num;

    /**
       Sharing flags.

       Determines whether various data is shared from parent to child.
     */
    uint64_t share_flags;

    bool sharesPorts() { return (share_flags & SHARE_PORTS) != 0; }

    bool sharesStatistics() { return (share_flags & SHARE_STATS) != 0; }

    bool canInsertStatistics() { return (share_flags & INSERT_STATS) != 0; }

    inline void setComponent(BaseComponent* comp) { component = comp; }
    // inline void setParent(BaseComponent* comp) { parent = comp; }

    /* Lookup Key style constructor */
    ComponentInfo(ComponentId_t id, const std::string& name);
    void finalizeLinkConfiguration() const;
    void prepareForComplete() const;

    ComponentId_t addAnonymousSubComponent(
        ComponentInfo* parent_info, const std::string& type, const std::string& slot_name, int slot_num,
        uint64_t share_flags);

public:
    /**
       Constructor used only for serialization
     */
    ComponentInfo();

    /**
       Function used to serialize the class
     */
    void serialize_order(SST::Core::Serialization::serializer& ser);
    void serialize_comp(SST::Core::Serialization::serializer& ser);

    /* Old ELI Style subcomponent constructor */
    ComponentInfo(const std::string& type, const Params* params, const ComponentInfo* parent_info);

    /* Anonymous SubComponent */
    ComponentInfo(
        ComponentId_t id, ComponentInfo* parent_info, const std::string& type, const std::string& slot_name,
        int slot_num, uint64_t share_flags /*, const Params& params_in*/);

    /* New ELI Style */
    ComponentInfo(ConfigComponent* ccomp, const std::string& name, ComponentInfo* parent_info, LinkMap* link_map);
    ComponentInfo(ComponentInfo&& o);
    ~ComponentInfo();

    bool isAnonymous() { return COMPDEFINED_SUBCOMPONENT_ID_MASK(id_); }

    bool isUser() { return !COMPDEFINED_SUBCOMPONENT_ID_MASK(id_); }

    inline ComponentId_t getID() const { return id_; }

    inline const std::string& getName() const
    {
        if ( name.empty() && parent_info ) return parent_info->getName();
        return name;
    }

    inline const std::string& getParentComponentName() const
    {
        // First, get the actual component (parent pointer will be
        // nullptr).
        const ComponentInfo* real_comp = this;
        while ( real_comp->parent_info != nullptr )
            real_comp = real_comp->parent_info;
        return real_comp->getName();
    }

    /**
       Get the short name for this SubComponent (name not including
       any parents, so just slot_name[index])
     */
    inline std::string getShortName() const { return name.substr(name.find_last_of(':') + 1); }

    inline const std::string& getSlotName() const { return slot_name; }

    inline int getSlotNum() const { return slot_num; }

    inline const std::string& getType() const { return type; }

    inline BaseComponent* getComponent() const { return component; }

    LinkMap* getLinkMap();

    inline const Params* getParams() const { return params; }

    // inline std::map<std::string, ComponentInfo>& getSubComponents() { return subComponents; }
    inline std::map<ComponentId_t, ComponentInfo>& getSubComponents() { return subComponents; }

    ComponentInfo* findSubComponent(const std::string& slot, int slot_num);
    ComponentInfo* findSubComponent(ComponentId_t id);
    bool           hasLinks() const;

    uint8_t getStatisticLoadLevel() { return statLoadLevel; }

    struct HashName
    {
        size_t operator()(const ComponentInfo* info) const
        {
            std::hash<std::string> hash;
            return hash(info->name);
        }
    };

    struct EqualsName
    {
        bool operator()(const ComponentInfo* lhs, const ComponentInfo* rhs) const { return lhs->name == rhs->name; }
    };

    struct HashID
    {
        size_t operator()(const ComponentInfo* info) const
        {
            std::hash<ComponentId_t> hash;
            return hash(info->id_);
        }
    };

    struct EqualsID
    {
        bool operator()(const ComponentInfo* lhs, const ComponentInfo* rhs) const { return lhs->id_ == rhs->id_; }
    };

    //// Functions used only for testing, they will not create valid
    //// ComponentInfos for simulation

    /**
       (DO NOT USE) Constructor used only for serialization testing
     */
    ComponentInfo(
        ComponentId_t id, const std::string& name, const std::string& slot_name, TimeConverter tv = TimeConverter());

    ComponentInfo*
    test_addSubComponentInfo(const std::string& name, const std::string& slot_name, TimeConverter tv = TimeConverter());

    void test_printComponentInfoHierarchy(int index = 0);
};

class ComponentInfoMap
{
private:
    std::unordered_set<ComponentInfo*, ComponentInfo::HashID, ComponentInfo::EqualsID> dataByID;

public:
    using const_iterator =
        std::unordered_set<ComponentInfo*, ComponentInfo::HashID, ComponentInfo::EqualsID>::const_iterator;

    const_iterator begin() const { return dataByID.begin(); }

    const_iterator end() const { return dataByID.end(); }

    ComponentInfoMap() {}

    void insert(ComponentInfo* info) { dataByID.insert(info); }

    ComponentInfo* getByID(const ComponentId_t key) const
    {
        ComponentInfo infoKey(COMPONENT_ID_MASK(key), "");
        auto          value = dataByID.find(&infoKey);
        if ( value == dataByID.end() ) return nullptr;
        if ( SUBCOMPONENT_ID_MASK(key) != 0 ) {
            // Looking for a subcomponent
            return (*value)->findSubComponent(key);
        }
        return *value;
    }

    bool empty() { return dataByID.empty(); }

    void clear()
    {
        for ( auto i : dataByID ) {
            delete i;
        }
        dataByID.clear();
    }

    size_t size() { return dataByID.size(); }
};

} // namespace SST

#endif // SST_CORE_COMPONENTINFO_H
