#include "rofl/common/openflow/messages/cofmsg_barrier.h"

using namespace rofl::openflow;

size_t
cofmsg_barrier_request::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_barrier_request::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eMsgInval("cofmsg_barrier_request::pack()");

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct rofl::openflow::ofp_header), body.somem(), body.memlen());
}



void
cofmsg_barrier_request::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	cofmsg::unpack(buf, buflen);

	if (get_length() <= sizeof(struct openflow::ofp_header)) {
		return;
	}
	body.assign(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



size_t
cofmsg_barrier_reply::length() const
{
	return (cofmsg::length() + body.memlen());
}



void
cofmsg_barrier_reply::pack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eMsgInval("cofmsg_barrier_reply::pack()");

	cofmsg::pack(buf, buflen);

	memcpy(buf + sizeof(struct rofl::openflow::ofp_header), body.somem(), body.memlen());
}



void
cofmsg_barrier_reply::unpack(
		uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	cofmsg::unpack(buf, buflen);

	if (get_length() <= sizeof(struct openflow::ofp_header)) {
		return;
	}
	body.assign(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}


