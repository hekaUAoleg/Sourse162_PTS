#pragma once

#include <cstdint>

class CUserSocket;
class CSocket;

namespace runtime::client_protocol {

inline constexpr std::int32_t kCurrentRevision = 162;
inline constexpr std::int32_t kNorthAmericanRevision = 152;

// The current CUserSocket layout has no protocol-revision lane because the
// shipping server accepts only revision 162. Alternate portable revisions
// therefore live outside the ABI-exact object.
void RecordNegotiatedRevision(const CUserSocket* socket, std::int32_t revision);

std::int32_t NegotiatedRevision(const CUserSocket* socket) noexcept;

inline bool UsesNorthAmericanProtocol(const CUserSocket* socket) noexcept {
    return NegotiatedRevision(socket) == kNorthAmericanRevision;
}

// Lets mixed-revision broadcasts keep the exact current fast path until a
// P152 socket is actually present. The shipping object layout remains free
// of portable revision state.
bool HasNorthAmericanClients() noexcept;

void ForgetSocket(const CUserSocket* socket) noexcept;

using IncomingPacketCallback = bool (*)(CSocket*, const std::uint8_t*);

// Portable multi-revision dispatch. The shipping revision-162 callback maps
// remain the source of truth and are never mutated. The P152 maps copy that
// baseline and override only audited revision differences.
IncomingPacketCallback ResolveBasicIncomingCallback(
    const CUserSocket* socket, int opcode, IncomingPacketCallback currentCallback
) noexcept;

IncomingPacketCallback ResolveExtendedIncomingCallback(
    const CUserSocket* socket, int opcode, int payloadSize, IncomingPacketCallback currentCallback
) noexcept;

int ExtendedPolicyOpcode(const CUserSocket* socket, int opcode) noexcept;

} // namespace runtime::client_protocol
