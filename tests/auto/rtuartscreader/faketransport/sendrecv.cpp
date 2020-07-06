// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include "sendrecv.h"

#include <faketransport/faketransport.h>

using namespace std;

unique_ptr<FakeSendRecv> gFakeSendRecv = make_unique<FakeSendRecv>();

transport_status_t transport_recv_byte_impl(const transport_t* transport, uint8_t* byte) {
    return gFakeSendRecv->recv_byte(transport, byte);
}

transport_status_t transport_send_byte_impl(const transport_t* transport, uint8_t byte) {
    return gFakeSendRecv->send_byte(transport, byte);
}

transport_status_t transport_recv_bytes_impl(const transport_t* transport, uint8_t* buf, size_t len) {
    return gFakeSendRecv->recv_bytes(transport, buf, len);
}

transport_status_t transport_send_bytes_impl(const transport_t* transport, const uint8_t* buf, size_t len) {
    return gFakeSendRecv->send_bytes(transport, buf, len);
}

transport_sendrecv_impl_t gSendRecvImpl = {
    .transport_recv_byte = transport_recv_byte_impl,
    .transport_send_byte = transport_send_byte_impl,
    .transport_recv_bytes = transport_recv_bytes_impl,
    .transport_send_bytes = transport_send_bytes_impl
};

transport_status_t FakeSendRecv::recv_byte(const transport_t*, uint8_t* byte) {
    return recv(byte, 1);
}

transport_status_t FakeSendRecv::send_byte(const transport_t*, uint8_t byte) {
    return send(&byte, 1);
};

transport_status_t FakeSendRecv::recv_bytes(const transport_t*, uint8_t* buf, size_t len) {
    return recv(buf, len);
}

transport_status_t FakeSendRecv::send_bytes(const transport_t*, const uint8_t* buf, size_t len) {
    return send(buf, len);
}

transport_status_t FakeSendRecv::send(const uint8_t* buf, size_t len) {
    try {
        if (!mCard) throw runtime_error("You need to set card");

        if (buf == nullptr && len != 0) throw runtime_error("Invalid buffer");

        if (!len) return transport_status_ok;

        mCard->input(buf, len);

        return transport_status_ok;
    } catch (const exception&) {
    }

    return transport_status_communication_error;
}

transport_status_t FakeSendRecv::recv(uint8_t* buf, size_t len) {
    try {
        if (!mCard) throw runtime_error("You need to set card");

        if (buf == nullptr && len != 0) throw runtime_error("Invalid buffer");

        if (!len) return transport_status_ok;

        mCard->output(buf, len);

        return transport_status_ok;
    } catch (const exception&) {
    }

    return transport_status_communication_error;
}

void FakeSendRecv::setCard(const shared_ptr<rt::faketransport::Card>& card) {
    mCard = card;
}

void FakeSendRecv::resetCard() {
    mCard = nullptr;
}

namespace rt {
namespace faketransport {

void initializeSendRecv() {
    transport_sendrecv_impl_set(&gSendRecvImpl);
}
void deinitializeSendRecv() {
    transport_sendrecv_impl_reset();
}

void setCard(const std::shared_ptr<Card>& card) {
    gFakeSendRecv->setCard(card);
}

void resetCard() {
    gFakeSendRecv->resetCard();
}


} // namespace faketransport
} // namespace rt