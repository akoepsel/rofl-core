/*
 * cflow.cc
 *
 *  Created on: 15.08.2014
 *      Author: andreas
 */

#include "cflowentry.h"

using namespace etherswitch;

cflowentry::cflowentry(
		cflowenv *flowenv,
		const rofl::cdptid& dptid,
		const rofl::caddress_ll& src,
		const rofl::caddress_ll& dst,
		uint32_t port_no) :
		flowenv(flowenv),
		dptid(dptid),
		port_no(port_no),
		src(src),
		dst(dst),
		entry_timeout(CFIBENTRY_DEFAULT_TIMEOUT),
		expiration_timer_id()
{
	flow_mod_add();
	expiration_timer_id = register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
	rofl::logging::notice << "[cflowentry] created" << std::endl << *this;
}


cflowentry::~cflowentry()
{
	rofl::logging::notice << "[cflowentry] deleted" << std::endl << *this;
	flow_mod_delete();
}



void
cflowentry::handle_timeout(int opaque, void* data)
{
	switch (opaque) {
	case CFIBENTRY_ENTRY_EXPIRED: {
		flowenv->flow_timer_expired(*this);
	} break;
	}
}



void
cflowentry::set_port_no(uint32_t port_no)
{
	if (port_no != this->port_no) {
		this->port_no = port_no;
		flow_mod_modify();
	}

	try {
		reset_timer(expiration_timer_id, entry_timeout);
	} catch (rofl::eTimersBase& e) {
		expiration_timer_id = register_timer(CFIBENTRY_ENTRY_EXPIRED, entry_timeout);
	}
}



void
cflowentry::flow_mod_add()
{
	try {
		rofl::crofdpt& dpt = rofl::crofdpt::get_dpt(dptid);

		rofl::openflow::cofflowmod fe(dpt.get_version());
		rofl::cindex index(0);

		fe.set_command(rofl::openflow::OFPFC_ADD);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.set_match().set_eth_src(src);
		fe.set_match().set_eth_dst(dst);

		switch (dpt.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			fe.set_actions().
					add_action_output(index++).set_port_no(port_no);
		} break;
		default: {
			fe.set_instructions().set_inst_apply_actions().set_actions().
					add_action_output(index++).set_port_no(port_no);
		};
		}

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

	} catch (rofl::eRofDptNotFound& e) {
		// datapath with handle dptid not found
	} catch (rofl::eRofSockTxAgain& e) {
		// control channel congested
	}
}



void
cflowentry::flow_mod_modify()
{
	try {
		rofl::crofdpt& dpt = rofl::crofdpt::get_dpt(dptid);

		rofl::openflow::cofflowmod fe(dpt.get_version());
		rofl::cindex index(0);

		fe.set_command(rofl::openflow::OFPFC_MODIFY_STRICT);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.set_match().set_eth_src(src);
		fe.set_match().set_eth_dst(dst);

		switch (dpt.get_version()) {
		case rofl::openflow10::OFP_VERSION: {
			fe.set_actions().
					add_action_output(index++).set_port_no(port_no);
		} break;
		default: {
			fe.set_instructions().set_inst_apply_actions().set_actions().
					add_action_output(index++).set_port_no(port_no);
		};
		}

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

	} catch (rofl::eRofDptNotFound& e) {
		// datapath with handle dptid not found
	} catch (rofl::eRofSockTxAgain& e) {
		// control channel congested
	} catch (rofl::eRofBaseNotConnected& e) {
		// when xdpd has already died ...
	} catch (...) {
		// ...
	}
}



void
cflowentry::flow_mod_delete()
{
	try {
		rofl::crofdpt& dpt = rofl::crofdpt::get_dpt(dptid);

		rofl::openflow::cofflowmod fe(dpt.get_version());
		rofl::cindex index(0);

		fe.set_command(rofl::openflow::OFPFC_DELETE_STRICT);
		fe.set_table_id(0);
		fe.set_hard_timeout(entry_timeout);
		fe.set_match().set_eth_src(src);
		fe.set_match().set_eth_dst(dst);

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

	} catch (rofl::eRofDptNotFound& e) {
		// datapath with handle dptid not found
	} catch (rofl::eRofSockTxAgain& e) {
		// control channel congested
	}
}


