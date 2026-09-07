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

#ifndef SST_CORE_SERIALIZATION_SERIALIZABLE_H
#define SST_CORE_SERIALIZATION_SERIALIZABLE_H

#include "sst/core/serialization/serializable_base.h"
#include "sst/core/serialization/serialize.h"

#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace SST::Core::Serialization {

class serializable : public serializable_base
{
public:
    static constexpr uint32_t NullClsId = std::numeric_limits<uint32_t>::max();

    // virtual const char* cls_name() const = 0;

    // virtual void serialize_order(serializer& ser) = 0;

    // virtual uint32_t    cls_id() const             = 0;
    // virtual std::string serialization_name() const = 0;

    virtual ~serializable() {}
};

namespace pvt {

void size_serializable(serializable_base* s, serializer& ser);

void pack_serializable(serializable_base* s, serializer& ser);

void unpack_serializable(serializable_base*& s, serializer& ser);

void map_serializable(serializable_base*& s, serializer& ser);

} // namespace pvt

/**
   Unpack an exclusively owned polymorphic pointer written by SST_SER(pointer).
   Family must opt into family-aware construction. An already unpacked pointer
   is rejected rather than introducing a second owner. With pointer tracking,
   the new Family* is published before decoding its fields, allowing non-owning
   self-references and nested back-references through the same Family* type.

   validate() receives the decoded pointer (including nullptr) before ownership
   is returned. If decoding or validation throws, newly published pointer keys
   (including those of nested objects) are removed before the object is destroyed.
   The failed unpack session must still be discarded or reset; this does not
   restore external objects or rewind the input. This permits callers to validate
   an enclosing discriminator without depending on pointer framing or factory
   internals.
 */
template <class Family, class Validator>
std::unique_ptr<Family>
unpack_exclusive_serializable(serializer& ser, Validator validate)
{
    static_assert(std::is_base_of_v<serializable, Family>, "Family must derive from serializable");
    if ( ser.mode() != serializer::UNPACK ) {
        throw std::logic_error("unpack_exclusive_serializable requires unpack mode");
    }

    uintptr_t stored_pointer = 0;
    bool      nonnull        = false;
    if ( !ser.is_pointer_tracking_enabled() ) {
        ser.primitive(nonnull);
    }
    else {
        ser.unpack(stored_pointer);
        nonnull = stored_pointer != 0;
        if ( nonnull && ser.unpacker().check_pointer_unpack(stored_pointer) != 0 ) {
            throw std::runtime_error("cannot unpack an aliased exclusively owned serializable");
        }
    }

    std::unique_ptr<Family> object;
    const size_t            publication_mark = ser.unpacker().mark();
    try {
        if ( nonnull ) {
            long serialized_class_id = -1;
            ser.unpack(serialized_class_id);
            if constexpr ( sizeof(long) > sizeof(uint32_t) ) {
                if ( serialized_class_id < 0 ||
                     static_cast<uint64_t>(serialized_class_id) > std::numeric_limits<uint32_t>::max() ) {
                    throw std::runtime_error("serialized class ID is outside the supported range");
                }
            }

            object.reset(serializable_factory::get_serializable_as<Family>(static_cast<uint32_t>(serialized_class_id)));
            if ( object->cls_id() != static_cast<uint32_t>(serialized_class_id) ) {
                throw std::runtime_error("exclusively owned serializable reported the wrong class ID");
            }
            if ( ser.is_pointer_tracking_enabled() ) {
                ser.unpacker().report_real_pointer(stored_pointer, reinterpret_cast<uintptr_t>(object.get()));
            }
            object->serialize_order(ser);
        }
        validate(static_cast<const Family*>(object.get()));
    }
    catch ( ... ) {
        ser.unpacker().rollback(publication_mark);
        throw;
    }
    return object;
}


template <class T>
class serialize_impl<T*, std::enable_if_t<std::is_base_of_v<serializable, T>>>
{
    void operator()(T*& s, serializer& ser, ser_opt_t UNUSED(options))
    {
        serializable_base* sp = static_cast<serializable_base*>(s);
        switch ( ser.mode() ) {
        case serializer::SIZER:
            pvt::size_serializable(sp, ser);
            break;
        case serializer::PACK:
            pvt::pack_serializable(sp, ser);
            break;
        case serializer::UNPACK:
            pvt::unpack_serializable(sp, ser);
            break;
        case serializer::MAP:
            pvt::map_serializable(sp, ser);
            break;
        }
        s = static_cast<T*>(sp);
    }

    SST_FRIEND_SERIALIZE();
};

template <class T>
void
serialize_intrusive_ptr(T*& t, serializer& ser)
{
    serializable_base* s = t;
    switch ( ser.mode() ) {
    case serializer::SIZER:
        pvt::size_serializable(s, ser);
        break;
    case serializer::PACK:
        pvt::pack_serializable(s, ser);
        break;
    case serializer::UNPACK:
        pvt::unpack_serializable(s, ser);
        t = dynamic_cast<T*>(s);
        break;
    case serializer::MAP:
        // Add your code here
        break;
    }
}

template <class T>
class serialize_impl<T, std::enable_if_t<std::is_base_of_v<serializable, T>>>
{
    inline void operator()(T& t, serializer& ser, ser_opt_t UNUSED(options))
    {
        // T* tmp = &t;
        // serialize_intrusive_ptr(tmp, ser);
        t.serialize_order(ser);

        // TODO: Need to figure out how to handle mapping mode for
        // classes inheriting from serializable that are not pointers.
        // For the core, this really only applies to SharedObjects,
        // which may actually need their own specific serialization.

        // For now mapping mode won't provide any data
    }

    SST_FRIEND_SERIALIZE();
};

} // namespace SST::Core::Serialization

// #include "sst/core/serialization/serialize_serializable.h"

#endif
