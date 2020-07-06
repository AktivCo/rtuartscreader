// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <memory>

#include <rtuartscreader/transport/sendrecv.h>

#include <faketransport/card.h>

class SendRecv {
public:
    virtual transport_status_t recv_byte(const transport_t* transport, uint8_t* byte) = 0;
    virtual transport_status_t send_byte(const transport_t* transport, uint8_t byte) = 0;
    virtual transport_status_t recv_bytes(const transport_t* transport, uint8_t* buf, size_t len) = 0;
    virtual transport_status_t send_bytes(const transport_t* transport, const uint8_t* buf, size_t len) = 0;

    virtual ~SendRecv() = default;
};

class FakeSendRecv : public SendRecv {
public:
    FakeSendRecv()
        : mCard(nullptr) {}

    virtual transport_status_t recv_byte(const transport_t* transport, uint8_t* byte) override;
    virtual transport_status_t send_byte(const transport_t* transport, uint8_t byte) override;
    virtual transport_status_t recv_bytes(const transport_t* transport, uint8_t* buf, size_t len) override;
    virtual transport_status_t send_bytes(const transport_t* transport, const uint8_t* buf, size_t len) override;

    void setCard(const std::shared_ptr<rt::faketransport::Card>& card);
    void resetCard();

private:
    transport_status_t send(const uint8_t* buf, size_t len);
    transport_status_t recv(uint8_t* buf, size_t len);

    std::shared_ptr<rt::faketransport::Card> mCard;
};