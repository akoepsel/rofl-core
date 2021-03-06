/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "rofl/common/croflexp.h"

using namespace rofl::openflow;

croflexp::croflexp(
		uint8_t *buf,
		size_t buflen) :
		mem(buf, buflen)
{
	rext_header = (struct ofp_rofl_ext_header*)mem.somem();

	validate();
}



croflexp::croflexp(
		size_t size) :
		mem(size)
{
	rext_header = (struct ofp_rofl_ext_header*)mem.somem();
}


croflexp::~croflexp()
{

}



void
croflexp::pack(uint8_t *__body, size_t __bodylen)
{
	size_t len = mem.memlen();

	if (__bodylen < len) {
		throw eRoflExpInval();
	}

	memcpy(__body, mem.somem(), mem.memlen());
}


void
croflexp::unpack(uint8_t *__body, size_t __bodylen)
{
	mem.resize(__bodylen);

	mem.assign(__body, __bodylen);

	rext_header = (struct ofp_rofl_ext_header*)mem.somem();
}


size_t
croflexp::length()
{
	return mem.memlen();
}


void
croflexp::validate()
		throw (eRoflExpInval)
{
	if (mem.memlen() < sizeof(struct ofp_rofl_ext_header))
	{
		throw eRoflExpInval();
	}

	switch (rext_header->type) {
	case OFPRET_FLOWSPACE:
		{
			validate_flowspace();
		}
		break;
	default:
		{
			throw eRoflExpInval();
		}
		break;
	}


}


void
croflexp::validate_flowspace()
		throw (eRoflExpInval)
{
	if (mem.memlen() < sizeof(struct ofp_rofl_ext_flowspace)) {
		throw eRoflExpInval();
	}

	switch (rext_fsp->command) {
	case OFPRET_FSP_ADD:
	case OFPRET_FSP_DELETE: {
		match.clear();
		if (mem.memlen() > sizeof(struct ofp_rofl_ext_flowspace)) {
			match.unpack((uint8_t*)(rext_fsp->match), mem.memlen() - sizeof(struct ofp_rofl_ext_flowspace));
		}
	} break;
	default: {
		throw eRoflExpInval();
	} break;
	}
}


