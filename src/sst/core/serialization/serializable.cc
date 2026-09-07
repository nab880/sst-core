// Copyright 2009-2026 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2026, NTESS
// All rights reserved.
//
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#include "sst_config.h"

#include "sst/core/serialization/serializable.h"

#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

namespace SST::Core::Serialization::pvt {

static const long null_ptr_id = -1;

void
size_serializable(serializable_base* s, serializer& ser)
{
    long dummy = 0;
    ser.size(dummy);
    if ( s ) {
        s->serialize_order(ser);
    }
}

void
pack_serializable(serializable_base* s, serializer& ser)
{
    if ( s ) {
        long cls_id = s->cls_id();
        ser.pack(cls_id);
        s->serialize_order(ser);
    }
    else {
        long id = null_ptr_id;
        ser.pack(id);
    }
}

void
unpack_serializable(serializable_base*& s, serializer& ser)
{
    long cls_id;
    ser.unpack(cls_id);
    if ( cls_id == null_ptr_id ) {
        s = nullptr;
    }
    else {
        if constexpr ( sizeof(long) > sizeof(uint32_t) ) {
            if ( cls_id < 0 || static_cast<uint64_t>(cls_id) > std::numeric_limits<uint32_t>::max() ) {
                throw std::runtime_error("serialized class ID is outside the supported range");
            }
        }
        std::unique_ptr<serializable_base> unpacked(
            serializable_factory::get_serializable(static_cast<uint32_t>(cls_id)));
        const size_t publication_mark = ser.unpacker().mark();
        try {
            if ( ser.is_pointer_tracking_enabled() ) {
                ser.unpacker().report_new_pointer(reinterpret_cast<uintptr_t>(unpacked.get()));
            }
            unpacked->serialize_order(ser);
        }
        catch ( ... ) {
            // Remove nested publications before destruction can delete the corresponding owned objects.
            ser.unpacker().rollback(publication_mark);
            throw;
        }
        s = unpacked.release();
    }
}

void
map_serializable(serializable_base*& s, serializer& ser)
{
    if ( s ) {
        ObjectMap* obj_map = new ObjectMapClass(s, s->cls_name());
        ser.mapper().report_object_map(obj_map);
        ser.mapper().map_hierarchy_start(ser.getMapName(), obj_map);
        s->serialize_order(ser);
        ser.mapper().map_hierarchy_end();
    }
}

} // namespace SST::Core::Serialization::pvt
