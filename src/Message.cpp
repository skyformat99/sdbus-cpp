/**
 * (C) 2017 KISTLER INSTRUMENTE AG, Winterthur, Switzerland
 *
 * @file Message.cpp
 *
 * Created on: Nov 9, 2016
 * Project: sdbus-c++
 * Description: High-level D-Bus IPC C++ library based on sd-bus
 *
 * This file is part of sdbus-c++.
 *
 * sdbus-c++ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * sdbus-c++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sdbus-c++. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sdbus-c++/Message.h>
#include <sdbus-c++/Types.h>
#include <sdbus-c++/Error.h>
#include "MessageUtils.h"
#include "ScopeGuard.h"
#include <systemd/sd-bus.h>
#include <cassert>

namespace sdbus { /*namespace internal {*/

Message::Message(void *msg, Type type) noexcept
    : msg_(msg)
    , type_(type)
{
    assert(msg_ != nullptr);
    sd_bus_message_ref((sd_bus_message*)msg_);
}

Message::Message(const Message& other) noexcept
{
    *this = other;
}

Message& Message::operator=(const Message& other) noexcept
{
    msg_ = other.msg_;
    type_ = other.type_;
    ok_ = other.ok_;

    sd_bus_message_ref((sd_bus_message*)msg_);

    return *this;
}

Message::Message(Message&& other) noexcept
{
    *this = std::move(other);
}

Message& Message::operator=(Message&& other) noexcept
{
    msg_ = other.msg_;
    other.msg_ = nullptr;
    type_ = other.type_;
    other.type_ = {};
    ok_ = other.ok_;
    other.ok_ = true;

    return *this;
}

Message::~Message()
{
    if (msg_)
        sd_bus_message_unref((sd_bus_message*)msg_);
}

Message& Message::operator<<(bool item)
{
    int intItem = item;

    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_BOOLEAN, &intItem);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a bool value", -r);

    return *this;
}

Message& Message::operator<<(int16_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT16, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a int16_t value", -r);

    return *this;
}

Message& Message::operator<<(int32_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT32, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a int32_t value", -r);

    return *this;
}

Message& Message::operator<<(int64_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT64, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a int64_t value", -r);

    return *this;
}

Message& Message::operator<<(uint8_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_BYTE, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a byte value", -r);

    return *this;
}

Message& Message::operator<<(uint16_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT16, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a uint16_t value", -r);

    return *this;
}

Message& Message::operator<<(uint32_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT32, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a uint32_t value", -r);

    return *this;
}

Message& Message::operator<<(uint64_t item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT64, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a uint64_t value", -r);

    return *this;
}

Message& Message::operator<<(double item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_DOUBLE, &item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a double value", -r);

    return *this;
}

Message& Message::operator<<(const char* item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_STRING, item);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a C-string value", -r);

    return *this;
}

Message& Message::operator<<(const std::string& item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_STRING, item.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize a string value", -r);

    return *this;
}

Message& Message::operator<<(const Variant &item)
{
    item.serializeTo(*this);

    return *this;
}

Message& Message::operator<<(const ObjectPath &item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_OBJECT_PATH, item.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize an ObjectPath value", -r);

    return *this;
}

Message& Message::operator<<(const Signature &item)
{
    auto r = sd_bus_message_append_basic((sd_bus_message*)msg_, SD_BUS_TYPE_SIGNATURE, item.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to serialize an Signature value", -r);

    return *this;
}


Message& Message::operator>>(bool& item)
{
    int intItem;
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_BOOLEAN, &intItem);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a bool value", -r);

    item = static_cast<bool>(intItem);

    return *this;
}

Message& Message::operator>>(int16_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT16, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a int16_t value", -r);

    return *this;
}

Message& Message::operator>>(int32_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT32, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a int32_t value", -r);

    return *this;
}

Message& Message::operator>>(int64_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_INT64, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a bool value", -r);

    return *this;
}

Message& Message::operator>>(uint8_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_BYTE, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a byte value", -r);

    return *this;
}

Message& Message::operator>>(uint16_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT16, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a uint16_t value", -r);

    return *this;
}

Message& Message::operator>>(uint32_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT32, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a uint32_t value", -r);

    return *this;
}

Message& Message::operator>>(uint64_t& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_UINT64, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a uint64_t value", -r);

    return *this;
}

Message& Message::operator>>(double& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_DOUBLE, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a double value", -r);

    return *this;
}

Message& Message::operator>>(char*& item)
{
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_STRING, &item);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a string value", -r);

    return *this;
}

Message& Message::operator>>(std::string& item)
{
    char* str{};
    (*this) >> str;

    if (str != nullptr)
        item = str;

    return *this;
}

Message& Message::operator>>(Variant &item)
{
    item.deserializeFrom(*this);

    // Empty variant is normally prohibited. Users cannot send empty variants.
    // Therefore in this context an empty variant means that we are at the end
    // of deserializing a container, and thus we shall set ok_ flag to false.
    if (item.isEmpty())
        ok_ = false;

    return *this;
}

Message& Message::operator>>(ObjectPath &item)
{
    char* str{};
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_OBJECT_PATH, &str);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize an ObjectPath value", -r);

    if (str != nullptr)
        item = str;

    return *this;
}

Message& Message::operator>>(Signature &item)
{
    char* str{};
    auto r = sd_bus_message_read_basic((sd_bus_message*)msg_, SD_BUS_TYPE_SIGNATURE, &str);
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to deserialize a Signature value", -r);

    if (str != nullptr)
        item = str;

    return *this;
}


Message& Message::openContainer(const std::string& signature)
{
    auto r = sd_bus_message_open_container((sd_bus_message*)msg_, SD_BUS_TYPE_ARRAY, signature.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to open a container", -r);

    return *this;
}

Message& Message::closeContainer()
{
    auto r = sd_bus_message_close_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to close a container", -r);

    return *this;
}

Message& Message::openDictEntry(const std::string& signature)
{
    auto r = sd_bus_message_open_container((sd_bus_message*)msg_, SD_BUS_TYPE_DICT_ENTRY, signature.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to open a dictionary entry", -r);

    return *this;
}

Message& Message::closeDictEntry()
{
    auto r = sd_bus_message_close_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to close a dictionary entry", -r);

    return *this;
}

Message& Message::openVariant(const std::string& signature)
{
    auto r = sd_bus_message_open_container((sd_bus_message*)msg_, SD_BUS_TYPE_VARIANT, signature.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to open a variant", -r);

    return *this;
}

Message& Message::closeVariant()
{
    auto r = sd_bus_message_close_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to close a variant", -r);

    return *this;
}

Message& Message::openStruct(const std::string& signature)
{
    auto r = sd_bus_message_open_container((sd_bus_message*)msg_, SD_BUS_TYPE_STRUCT, signature.c_str());
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to open a struct", -r);

    return *this;
}

Message& Message::closeStruct()
{
    auto r = sd_bus_message_close_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to close a struct", -r);

    return *this;
}


Message& Message::enterContainer(const std::string& signature)
{
    auto r = sd_bus_message_enter_container((sd_bus_message*)msg_, SD_BUS_TYPE_ARRAY, signature.c_str());
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to enter a container", -r);

    return *this;
}

Message& Message::exitContainer()
{
    auto r = sd_bus_message_exit_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to exit a container", -r);

    return *this;
}

Message& Message::enterDictEntry(const std::string& signature)
{
    auto r = sd_bus_message_enter_container((sd_bus_message*)msg_, SD_BUS_TYPE_DICT_ENTRY, signature.c_str());
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to enter a dictionary entry", -r);

    return *this;
}

Message& Message::exitDictEntry()
{
    auto r = sd_bus_message_exit_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to exit a dictionary entry", -r);

    return *this;
}

Message& Message::enterVariant(const std::string& signature)
{
    auto r = sd_bus_message_enter_container((sd_bus_message*)msg_, SD_BUS_TYPE_VARIANT, signature.c_str());
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to enter a variant", -r);

    return *this;
}

Message& Message::exitVariant()
{
    auto r = sd_bus_message_exit_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to exit a variant", -r);

    return *this;
}

Message& Message::enterStruct(const std::string& signature)
{
    auto r = sd_bus_message_enter_container((sd_bus_message*)msg_, SD_BUS_TYPE_STRUCT, signature.c_str());
    if (r == 0)
        ok_ = false;
    else if (r < 0)
        SDBUS_THROW_ERROR("Failed to enter a struct", -r);

    return *this;
}

Message& Message::exitStruct()
{
    auto r = sd_bus_message_exit_container((sd_bus_message*)msg_);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to exit a struct", -r);

    return *this;
}


Message::operator bool() const
{
    return ok_;
}

void Message::clearFlags()
{
    ok_ = true;
}

void Message::copyTo(Message& destination, bool complete) const
{
    auto r = sd_bus_message_copy((sd_bus_message*)destination.msg_, (sd_bus_message*)msg_, complete);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to copy the message", -r);
}

void Message::seal()
{
    const auto messageCookie = 1;
    const auto sealTimeout = 0;
    auto r = sd_bus_message_seal((sd_bus_message*)msg_, messageCookie, sealTimeout);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to seal the message", -r);
}

void Message::rewind(bool complete)
{
    auto r = sd_bus_message_rewind((sd_bus_message*)msg_, complete);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to rewind the message", -r);
}

Message Message::send() const
{
    if (type_ == Type::eMethodCall)
    {
        sd_bus_message* sdbusReply{};
        SCOPE_EXIT{ sd_bus_message_unref(sdbusReply); }; // Returned message will become an owner of sdbusReply
        sd_bus_error sdbusError = SD_BUS_ERROR_NULL;
        SCOPE_EXIT{ sd_bus_error_free(&sdbusError); };

        auto r = sd_bus_call(nullptr, (sd_bus_message*)msg_, 0, &sdbusError, &sdbusReply);

        if (sd_bus_error_is_set(&sdbusError))
        {
            throw sdbus::Error(sdbusError.name, sdbusError.message);
        }

        SDBUS_THROW_ERROR_IF(r < 0, "Failed to call method", -r);

        return Message(sdbusReply);
    }
    else if (type_ == Type::eMethodReply)
    {
        auto r = sd_bus_send(nullptr, (sd_bus_message*)msg_, nullptr);
        if (r < 0)
            SDBUS_THROW_ERROR("Failed to send reply", -r);

        return Message();
    }
    else if (type_ == Type::eSignal)
    {
        auto r = sd_bus_send(nullptr, (sd_bus_message*)msg_, nullptr);
        if (r < 0)
            SDBUS_THROW_ERROR("Failed to emit signal", -r);

        return Message();
    }
    else
    {
        assert(false);
        return Message();
    }
}

Message Message::createReply() const
{
    sd_bus_message *sdbusReply{};
    SCOPE_EXIT{ sd_bus_message_unref(sdbusReply); }; // Returned message will become an owner of sdbusReply
    auto r = sd_bus_message_new_method_return((sd_bus_message*)msg_, &sdbusReply);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to create method reply", -r);

    assert(sdbusReply != nullptr);

    return Message(sdbusReply, Type::eMethodReply);
}

std::string Message::getInterfaceName() const
{
    return sd_bus_message_get_interface((sd_bus_message*)msg_);
}

std::string Message::getMemberName() const
{
    return sd_bus_message_get_member((sd_bus_message*)msg_);
}

void Message::peekType(std::string& type, std::string& contents) const
{
    char typeSig;
    const char* contentsSig;
    auto r = sd_bus_message_peek_type((sd_bus_message*)msg_, &typeSig, &contentsSig);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to peek message type", -r);
    type = typeSig;
    contents = contentsSig;
}

bool Message::isValid() const
{
    return msg_ != nullptr;
}

bool Message::isEmpty() const
{
    return sd_bus_message_is_empty((sd_bus_message*)msg_);
}

Message::Type Message::getType() const
{
    return type_;
}

Message createPlainMessage()
{
    int r;

    sd_bus* bus{};
    SCOPE_EXIT{ sd_bus_unref(bus); }; // sdbusMsg will hold reference to the bus
    r = sd_bus_default_system(&bus);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to get default system bus", -r);

    sd_bus_message* sdbusMsg{};
    SCOPE_EXIT{ sd_bus_message_unref(sdbusMsg); }; // Returned message will become an owner of sdbusMsg
    r = sd_bus_message_new(bus, &sdbusMsg, _SD_BUS_MESSAGE_TYPE_INVALID);
    if (r < 0)
        SDBUS_THROW_ERROR("Failed to create a new message", -r);

    return Message(sdbusMsg, Message::Type::ePlainMessage);
}

/*}*/}
