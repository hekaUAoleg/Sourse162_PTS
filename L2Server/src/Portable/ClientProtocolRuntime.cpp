#include "Portable/ClientProtocolRuntime.h"

#include <mutex>
#include <unordered_map>

namespace runtime::client_protocol {

namespace {

struct RuntimeState final {
    std::mutex lock;
    std::unordered_map<const CUserSocket*, std::int32_t> revisions;
};

RuntimeState& State() {
    // CUserSocket globals can be destroyed after function-local statics.
    // Keep this process-lifetime compatibility state available until exit.
    static RuntimeState* const state = new RuntimeState;
    return *state;
}

} // namespace

void RecordNegotiatedRevision(const CUserSocket* socket, const std::int32_t revision) {
    if (socket == nullptr) {
        return;
    }

    RuntimeState& state = State();
    const std::lock_guard<std::mutex> guard(state.lock);
    if (revision == kCurrentRevision) {
        state.revisions.erase(socket);
    } else {
        state.revisions[socket] = revision;
    }
}

std::int32_t NegotiatedRevision(const CUserSocket* socket) noexcept {
    if (socket == nullptr) {
        return kCurrentRevision;
    }

    RuntimeState& state = State();
    const std::lock_guard<std::mutex> guard(state.lock);
    const auto iterator = state.revisions.find(socket);
    return iterator != state.revisions.end() ? iterator->second : kCurrentRevision;
}

bool HasNorthAmericanClients() noexcept {
    RuntimeState& state = State();
    const std::lock_guard<std::mutex> guard(state.lock);
    for (const auto& [socket, revision] : state.revisions) {
        if (socket != nullptr && revision == kNorthAmericanRevision) {
            return true;
        }
    }
    return false;
}

void ForgetSocket(const CUserSocket* socket) noexcept {
    if (socket == nullptr) {
        return;
    }

    RuntimeState& state = State();
    const std::lock_guard<std::mutex> guard(state.lock);
    state.revisions.erase(socket);
}

} // namespace runtime::client_protocol
