#include <faketransport/card.h>

#include "sendrecv.h"

namespace rt {
namespace faketransport {

void setCard(const std::shared_ptr<Card>& card) {
    gFakeSendRecv->setCard(card);
}

void resetCard() {
    gFakeSendRecv->resetCard();
}

} // namespace faketransport
} // namespace rt