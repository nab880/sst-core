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

#ifndef SST_CORE_SERIALIZATION_IMPL_UNPACKER_H
#define SST_CORE_SERIALIZATION_IMPL_UNPACKER_H

#ifndef SST_INCLUDING_SERIALIZER_H
#warning \
    "The header file sst/core/serialization/impl/unpacker.h should not be directly included as it is not part of the stable public API.  The file is included in sst/core/serialization/serializer.h"
#endif

#include "sst/core/serialization/impl/ser_buffer_accessor.h"
#include "sst/core/serialization/impl/ser_shared_ptr_tracker.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

namespace SST::Core::Serialization::pvt {

class ser_unpacker : public ser_buffer_accessor, public ser_shared_ptr_unpacker
{
    std::map<uintptr_t, uintptr_t> ser_pointer_map;
    std::vector<uintptr_t>         pointer_publications;
    uintptr_t                      split_key = 0;

public:
    // inherit ser_buffer_accessor constructors
    using ser_buffer_accessor::ser_buffer_accessor;

    template <typename T>
    void unpack(T& t)
    {
        memcpy(&t, buf_next(sizeof(t)), sizeof(t));
    }

    template <typename T, typename SIZE_T>
    void unpack_buffer(T*& buffer, SIZE_T& size)
    {
        size = 0;
        unpack(size);
        if ( size != 0 ) {
            using ELEM_T = std::conditional_t<std::is_void_v<T>, char, T>; // Use char if T == void
            buffer       = new ELEM_T[size];
            memcpy(buffer, buf_next(size * sizeof(ELEM_T)), size * sizeof(ELEM_T));
        }
        else
            buffer = nullptr;
    }

    uintptr_t check_pointer_unpack(uintptr_t ptr);
    void      unpack_string(std::string& str);
    void      report_new_pointer(uintptr_t real_ptr) { report_real_pointer(split_key, real_ptr); }
    void      report_real_pointer(uintptr_t ptr, uintptr_t real_ptr);

    // Rollback removes keys first published after the marker, including nested objects. Updating an existing
    // key does not publish it again. Neither old values nor object mutations are restored; an unpack failure
    // invalidates the session even after rollback.
    size_t mark() const noexcept { return pointer_publications.size(); }
    void   rollback(size_t mark) noexcept;
}; // class ser_unpacker

} // namespace SST::Core::Serialization::pvt

#endif // SST_CORE_SERIALIZATION_IMPL_UNPACKER_H
