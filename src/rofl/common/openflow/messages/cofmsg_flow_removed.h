/*
 * cofmsg_flow_removed.h
 *
 *  Created on: 18.03.2013
 *      Author: andi
 */

#ifndef COFMSG_FLOW_REMOVED_H_
#define COFMSG_FLOW_REMOVED_H_ 1

#include "cofmsg.h"
#include "rofl/common/openflow/cofmatch.h"

namespace rofl {
namespace openflow {

/**
 *
 */
class cofmsg_flow_removed : public cofmsg {
public:

	/**
	 *
	 */
	virtual
	~cofmsg_flow_removed()
	{};

	/**
	 *
	 */
	cofmsg_flow_removed(
			uint8_t version = 0,
			uint32_t xid = 0,
			uint64_t cookie = 0,
			uint16_t priority = 0,
			uint8_t  reason = 0,
			uint8_t  table_id = 0,
			uint32_t duration_sec = 0,
			uint32_t duration_nsec = 0,
			uint16_t idle_timeout = 0,
			uint16_t hard_timeout = 0,
			uint64_t packet_count = 0,
			uint64_t byte_count = 0,
			const rofl::openflow::cofmatch& match = rofl::openflow::cofmatch()) :
				cofmsg(version, rofl::openflow::OFPT_FLOW_REMOVED, 0, xid),
				cookie(cookie),
				priority(priority),
				reason(reason),
				table_id(table_id),
				duration_sec(duration_sec),
				duration_nsec(duration_nsec),
				idle_timeout(idle_timeout),
				hard_timeout(hard_timeout),
				packet_count(packet_count),
				byte_count(byte_count),
				match(match)
	{};

	/**
	 *
	 */
	cofmsg_flow_removed(
			const cofmsg_flow_removed& msg)
	{ *this = msg; };

	/**
	 *
	 */
	cofmsg_flow_removed&
	operator= (
			const cofmsg_flow_removed& msg) {
		if (this == &msg)
			return *this;
		cofmsg::operator= (msg);
		cookie        = msg.cookie;
		priority      = msg.priority;
		reason        = msg.reason;
		table_id      = msg.table_id;
		duration_sec  = msg.duration_sec;
		duration_nsec = msg.duration_nsec;
		idle_timeout  = msg.idle_timeout;
		hard_timeout  = msg.hard_timeout;
		packet_count  = msg.packet_count;
		byte_count    = msg.byte_count;
		match         = msg.match;
		return *this;
	};

public:

	/**
	 *
	 */
	virtual size_t
	length() const;

	/**
	 *
	 */
	virtual void
	pack(
			uint8_t *buf = (uint8_t*)0, size_t buflen = 0);

	/**
	 *
	 */
	virtual void
	unpack(
			uint8_t *buf, size_t buflen);

public:

	/**
	 *
	 */
	uint64_t
	get_cookie() const
	{ return cookie; };

	/**
	 *
	 */
	void
	set_cookie(
			uint64_t cookie)
	{ this->cookie = cookie; };

	/**
	 *
	 */
	uint16_t
	get_priority() const
	{ return priority; };

	/**
	 *
	 */
	void
	set_priority(
			uint16_t priority)
	{ this->priority = priority; };

	/**
	 *
	 */
	uint8_t
	get_reason() const
	{ return reason; };

	/**
	 *
	 */
	void
	set_reason(
			uint8_t reason)
	{ this->reason = reason; };

	/**
	 *
	 */
	uint8_t
	get_table_id() const
	{ return table_id; };

	/**
	 *
	 */
	void
	set_table_id(
			uint8_t table_id)
	{ this->table_id = table_id; };

	/**
	 *
	 */
	uint32_t
	get_duration_sec() const
	{ return duration_sec; };

	/**
	 *
	 */
	void
	set_duration_sec(
			uint32_t duration_sec)
	{ this->duration_sec = duration_sec; };

	/**
	 *
	 */
	uint32_t
	get_duration_nsec() const
	{ return duration_nsec; };

	/**
	 *
	 */
	void
	set_duration_nsec(
			uint32_t duration_nsec)
	{ this->duration_nsec = duration_nsec; };

	/**
	 *
	 */
	uint16_t
	get_idle_timeout() const
	{ return idle_timeout; };

	/**
	 *
	 */
	void
	set_idle_timeout(
			uint16_t idle_timeout)
	{ this->idle_timeout = idle_timeout; };

	/**
	 *
	 */
	uint16_t
	get_hard_timeout() const
	{ return hard_timeout; };

	/**
	 *
	 */
	void
	set_hard_timeout(
			uint16_t hard_timeout)
	{ this->hard_timeout = hard_timeout; };

	/**
	 *
	 */
	uint64_t
	get_packet_count() const
	{ return packet_count; };

	/**
	 *
	 */
	void
	set_packet_count(
			uint64_t packet_count)
	{ this->packet_count = packet_count; };

	/**
	 *
	 */
	uint64_t
	get_byte_count() const
	{ return byte_count; };

	/**
	 *
	 */
	void
	set_byte_count(
			uint64_t byte_count)
	{ this->byte_count = byte_count; };

	/**
	 *
	 */
	const rofl::openflow::cofmatch&
	get_match() const
	{ return match; };

	/**
	 *
	 */
	rofl::openflow::cofmatch&
	set_match()
	{ return match; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, cofmsg_flow_removed const& msg) {
		switch (msg.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			os << indent(0) << dynamic_cast<cofmsg const&>( msg );
			os << indent(2) << "<cofmsg_flow_removed >" << std::endl;
				os << indent(4) << "<cookie:0x" << std::hex << (int)msg.get_cookie() << std::dec << " >" << std::endl;
				os << indent(4) << "<priority:0x" << std::hex << (int)msg.get_priority() << std::dec << " >" << std::endl;
				switch (msg.get_reason()) {
				case rofl::openflow10::OFPRR_IDLE_TIMEOUT: {
					os << indent(4) << "<reason: -IDLE-TIMEOUT- >" << std::endl;
				} break;
				case rofl::openflow10::OFPRR_HARD_TIMEOUT: {
					os << indent(4) << "<reason: -HARD-TIMEOUT- >" << std::endl;
				} break;
				case rofl::openflow10::OFPRR_DELETE: {
					os << indent(4) << "<reason: -DELETE- >" << std::endl;
				} break;
				default: {
					os << indent(4) << "<reason: -UNKNOWN- >" << std::endl;
				};
				}
				os << indent(4) << "<duration-sec:" << (int)msg.get_duration_sec() << " >" << std::endl;
				os << indent(4) << "<duration-nsec:" << (int)msg.get_duration_nsec() << " >" << std::endl;
				os << indent(4) << "<idle-timeout:" << (int)msg.get_idle_timeout() << " >" << std::endl;
				os << indent(4) << "<packet-count:" << (int)msg.get_packet_count() << " >" << std::endl;
				os << indent(4) << "<byte-count:" << (int)msg.get_byte_count() << " >" << std::endl;
				os << indent(4) << "<matches: >" << std::endl;
				indent i(6);
				os << msg.match;

		} break;
		case rofl::openflow12::OFP_VERSION:
		case rofl::openflow13::OFP_VERSION: {
			os << indent(0) << dynamic_cast<cofmsg const&>( msg );
			os << indent(2) << "<cofmsg_flow_removed >" << std::endl;
				os << indent(4) << "<cookie:0x" << std::hex << (int)msg.get_cookie() << std::dec << " >" << std::endl;
				os << indent(4) << "<priority:0x" << std::hex << (int)msg.get_priority() << std::dec << " >" << std::endl;
				switch (msg.get_reason()) {
				case rofl::openflow12::OFPRR_IDLE_TIMEOUT: {
					os << indent(4) << "<reason: -IDLE-TIMEOUT- >" << std::endl;
				} break;
				case rofl::openflow12::OFPRR_HARD_TIMEOUT: {
					os << indent(4) << "<reason: -HARD-TIMEOUT- >" << std::endl;
				} break;
				case rofl::openflow12::OFPRR_DELETE: {
					os << indent(4) << "<reason: -DELETE- >" << std::endl;
				} break;
				case rofl::openflow12::OFPRR_GROUP_DELETE: {
					os << indent(4) << "<reason: -GROUP-DELETE- >" << std::endl;
				} break;
				default: {
					os << indent(4) << "<reason: -UNKNOWN- >" << std::endl;
				};
				}
				os << indent(4) << "<table-id:" << (int)msg.get_table_id() << " >" << std::endl;
				os << indent(4) << "<duration-sec:" << (int)msg.get_duration_sec() << " >" << std::endl;
				os << indent(4) << "<duration-nsec:" << (int)msg.get_duration_nsec() << " >" << std::endl;
				os << indent(4) << "<idle-timeout:" << (int)msg.get_idle_timeout() << " >" << std::endl;
				os << indent(4) << "<hard-timeout:" << (int)msg.get_hard_timeout() << " >" << std::endl;
				os << indent(4) << "<packet-count:" << (int)msg.get_packet_count() << " >" << std::endl;
				os << indent(4) << "<byte-count:" << (int)msg.get_byte_count() << " >" << std::endl;
				os << indent(4) << "<matches: >" << std::endl;
				indent i(6);
				os << msg.match;

		} break;
		}
		return os;
	};

	std::string
	str() const {
		std::stringstream ss;
		ss << cofmsg::str() << "-Flow-Removed- " << " ";
		ss << "cookie: " << (unsigned long long)get_cookie() << ", ";
		ss << "priority: " << (unsigned int)get_priority() << ", ";
		ss << "reason: " << (unsigned int)get_reason() << ", ";
		ss << "table_id: " << (unsigned int)get_table_id() << ", ";
		ss << "dur_sec: " << (unsigned int)get_duration_sec() << ", ";
		ss << "dur_nsec: " << (unsigned int)get_duration_nsec() << ", ";
		ss << "idle: " << (unsigned int)get_idle_timeout() << ", ";
		ss << "hard: " << (unsigned int)get_hard_timeout() << ", ";
		ss << "#packets: " << (unsigned long long)get_packet_count() << ", ";
		ss << "#bytes: " << (unsigned long long)get_byte_count() << " ";
		return ss.str();
	};

private:

	static const size_t OFP10_FLOW_REMOVED_STATIC_HDR_LEN;
	static const size_t OFP12_FLOW_REMOVED_STATIC_HDR_LEN;

	uint64_t cookie;
	uint16_t priority;
	uint8_t  reason;
	uint8_t  table_id;
	uint32_t duration_sec;
	uint32_t duration_nsec;
	uint16_t idle_timeout;
	uint16_t hard_timeout;
	uint64_t packet_count;
	uint64_t byte_count;
	rofl::openflow::cofmatch match;
};

} // end of namespace openflow
} // end of namespace rofl

#endif /* COFMSG_FLOW_REMOVED_H_ */
