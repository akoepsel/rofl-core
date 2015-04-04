#include "rofl/common/openflow/messages/cofmsg_packet_out.h"

using namespace rofl::openflow;

size_t
cofmsg_packet_out::length() const
{
	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {
		return (sizeof(struct rofl::openflow10::ofp_packet_out) + actions.length() + packet.length());
	} break;
	default: {
		return (sizeof(struct rofl::openflow12::ofp_packet_out) + actions.length() + packet.length());
	};
	}
}



void
cofmsg_packet_out::pack(
		uint8_t *buf, size_t buflen)
{
	cofmsg::pack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	if (buflen < get_length())
		throw eMsgInval("cofmsg_packet_out::pack()");

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		struct rofl::openflow10::ofp_packet_out* hdr =
				(struct rofl::openflow10::ofp_packet_out*)buf;

		hdr->buffer_id   = htobe32(buffer_id);
		hdr->in_port     = htobe16(in_port & 0x0000ffff);
		hdr->actions_len = htobe16(actions.length());

		if (not actions.empty()) {
			actions.pack((uint8_t*)(hdr->actions), actions.length());
		}

		if (not packet.empty()) {
			packet.pack(buf + sizeof(struct rofl::openflow10::ofp_packet_out)
				+ actions.length(), packet.length());
		}

	} break;
	default: {

		struct rofl::openflow12::ofp_packet_out* hdr =
				(struct rofl::openflow12::ofp_packet_out*)buf;

		hdr->buffer_id   = htobe32(buffer_id);
		hdr->in_port     = htobe32(in_port);
		hdr->actions_len = htobe16(actions.length());

		if (not actions.empty()) {
			actions.pack((uint8_t*)(hdr->actions), actions.length());
		}

		if (not packet.empty()) {
			packet.pack(buf + sizeof(struct rofl::openflow12::ofp_packet_out)
				+ actions.length(), packet.length());
		}

	};
	}
}



void
cofmsg_packet_out::unpack(
		uint8_t *buf, size_t buflen)
{
	actions.clear();
	packet.clear();

	cofmsg::unpack(buf, buflen);

	if ((0 == buf) || (0 == buflen))
		return;

	switch (get_version()) {
	case rofl::openflow10::OFP_VERSION: {

		if (get_length() < sizeof(struct rofl::openflow10::ofp_packet_out))
			throw eBadSyntaxTooShort("cofmsg_packet_out::unpack()");

		struct rofl::openflow10::ofp_packet_out* hdr =
				(struct rofl::openflow10::ofp_packet_out*)buf;

		buffer_id = be32toh(hdr->buffer_id);
		in_port   = be16toh(hdr->in_port);

		uint16_t actions_len = be16toh(hdr->actions_len);
		uint16_t packet_offset = sizeof(struct rofl::openflow10::ofp_packet_out) + actions_len;

		if (get_length() < packet_offset)
			throw eBadSyntaxTooShort("cofmsg_packet_out::unpack()");

		actions.unpack((uint8_t*)hdr->actions, actions_len);

		if (rofl::openflow10::OFP_NO_BUFFER != get_buffer_id())
			return;

		packet.unpack(buf + packet_offset, buflen - packet_offset);

	} break;
	default: {

		if (get_length() < sizeof(struct rofl::openflow12::ofp_packet_out))
			throw eBadSyntaxTooShort("cofmsg_packet_out::unpack()");

		struct rofl::openflow12::ofp_packet_out* hdr =
				(struct rofl::openflow12::ofp_packet_out*)buf;

		buffer_id = be32toh(hdr->buffer_id);
		in_port   = be32toh(hdr->in_port);

		uint16_t actions_len = be16toh(hdr->actions_len);
		uint16_t packet_offset = sizeof(struct rofl::openflow12::ofp_packet_out) + actions_len;

		if (get_length() < packet_offset)
			throw eBadSyntaxTooShort("cofmsg_packet_out::unpack()");

		actions.unpack((uint8_t*)hdr->actions, actions_len);

		if (rofl::openflow12::OFP_NO_BUFFER != get_buffer_id())
			return;

		packet.unpack(buf + packet_offset, buflen - packet_offset);

	};
	}
}



