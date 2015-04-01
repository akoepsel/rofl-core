#include "rofl/common/openflow/messages/cofmsg_hello.h"

using namespace rofl::openflow;

size_t
cofmsg_hello::length() const
{
	return (cofmsg::length() + helloelems.length());
}



void
cofmsg_hello::pack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::pack(buf, buflen);

	helloelems.pack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}



void
cofmsg_hello::unpack(uint8_t *buf, size_t buflen)
{
	if ((0 == buf) || (buflen == 0))
		return;

	if (buflen < length())
		throw eInval();

	cofmsg::unpack(buf, buflen);

	if (get_length() <= sizeof(struct rofl::openflow::ofp_header))
		return;

	helloelems.unpack(buf + sizeof(struct rofl::openflow::ofp_header),
			buflen - sizeof(struct rofl::openflow::ofp_header));
}


