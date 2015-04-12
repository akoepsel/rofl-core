/*
 * crofchan.cc
 *
 *  Created on: 31.12.2013
 *      Author: andreas
 */

#include "rofl/common/crofchan.h"
#include "rofl/common/crofbase.h"

using namespace rofl;

/*static*/std::set<crofchan_env*> crofchan_env::rofchan_envs;



bool
crofchan::is_established() const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	if (conns.empty())
		return false;
	// this channel is up, when its main connection is up
	return conns.at(cauxid(0))->is_established();
}



void
crofchan::close()
{
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);
	for (std::map<cauxid, crofconn*>::reverse_iterator
			it = conns.rbegin(); it != conns.rend(); ++it) {
		delete it->second;
	}
	conns.clear();
}



cauxid
crofchan::get_next_auxid()
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	uint8_t aux_id = 0;
	while (conns.find(aux_id) != conns.end()) {
		aux_id++;
		if (aux_id == 255) {
			throw eAuxIdNotFound("crofchan::get_next_cauxid() auxid namespace exhausted");
		}
	}
	return cauxid(aux_id);
}



std::list<cauxid>
crofchan::get_conn_index() const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	std::list<cauxid> connections;
	for (std::map<cauxid, crofconn*>::const_iterator
			it = conns.begin(); it != conns.end(); ++it) {
		connections.push_back(it->first);
	}
	return connections;
}



crofconn&
crofchan::add_conn(
		const cauxid& __auxid,
		enum rofl::csocket::socket_type_t socket_type,
		const cparams& socket_params)
{
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	cauxid auxid(__auxid);

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	/* main connection (affects all auxiliary connections as well) */
	if (cauxid(0) == auxid) {

		rofl::logging::debug << "[rofl-common][crofchan] "
				<< "adding main connection, auxid: " << auxid.str() << std::endl;

		// main connection: propose all OFP versions defined for our side
		vbitmap = versionbitmap;

		// close main and all auxiliary connections
		if (not conns.empty()) {
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				delete it->second;
			}
			conns.clear();
		}

		// create new main connection
		conns[auxid] = new crofconn(this, vbitmap, get_thread_id());
		conns[auxid]->connect(auxid, socket_type, socket_params);

		return *(conns[auxid]);




	/* auxiliary connections */
	} else {

		rofl::logging::debug << "[rofl-common][crofchan] "
				<< "adding auxiliary connection, auxid: " << auxid.str() << std::endl;

		// auxiliary connections: use OFP version negotiated for main connection
		vbitmap.add_ofp_version(ofp_version);

		// set auxid to 0 (main), when no main connection exists
		if (conns.find(cauxid(0)) == conns.end()) {
			auxid = cauxid(0);
		}

		// remove existing auxiliary connection
		if (conns.find(auxid) != conns.end()) {
			delete conns[auxid];
		}

		conns[auxid] = new crofconn(this, vbitmap, get_thread_id());
		conns[auxid]->connect(auxid, socket_type, socket_params);

		return *(conns[auxid]);
	}
}



crofconn&
crofchan::add_conn(
		const cauxid& __auxid,
		crofconn* conn)
{
	rofl::cauxid auxid(__auxid);

	if (NULL == conn)
		throw eRofChanInval("crofconn::add_conn() no valid connection");

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	/* main connection (affects all auxiliary connections as well) */
	if (cauxid(0) == auxid) {

		rofl::logging::debug << "[rofl-common][crofchan] "
				<< "adding main connection, auxid: " << auxid.str() << std::endl;

		// close main and all auxiliary connections
		if (not conns.empty()) {
			for (std::map<cauxid, crofconn*>::iterator
					it = conns.begin(); it != conns.end(); ++it) {
				delete it->second;
			}
			conns.clear();
		}
		this->ofp_version = conn->get_version();

		// create new main connection
		conns[auxid] = conn;
		conns[auxid]->set_env(this);

		handle_connected(*conns[auxid], conns[auxid]->get_version());

		return *(conns[auxid]);




	/* auxiliary connections */
	} else {

		rofl::logging::debug << "[rofl-common][crofchan] "
				<< "adding auxiliary connection, auxid: " << auxid.str() << std::endl;

		// set auxid to 0 (main), when no main connection exists
		if (conns.find(cauxid(0)) == conns.end()) {
			auxid = cauxid(0);
			this->ofp_version = conn->get_version();
		}

		// remove existing auxiliary connection
		if (conns.find(auxid) != conns.end()) {
			delete conns[auxid];
		}

		conns[auxid] = conn;
		conns[auxid]->set_env(this);

		handle_connected(*conns[auxid], conns[auxid]->get_version());

		return *(conns[auxid]);
	}
}



crofconn&
crofchan::set_conn(
		const cauxid& __auxid)
{
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	cauxid auxid(__auxid);

	// get lock on conns map
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	// in OFP10 and OFP12, there is only a single main connection
	if ((ofp_version > rofl::openflow::OFP_VERSION_UNKNOWN) &&
		(ofp_version < rofl::openflow13::OFP_VERSION)) {
			auxid = cauxid(0);
	}

	if (conns.find(auxid) == conns.end()) {
		throw eRofChanNotFound("crofconn::set_conn() auxid not found");
	}

	return *(conns[auxid]);
}



const crofconn&
crofchan::get_conn(
		const cauxid& aux_id) const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	if (conns.find(aux_id) == conns.end()) {
		throw eRofChanNotFound("crofchan::get_conn()");
	}
	return const_cast<const crofconn&>(*(conns.at(aux_id)));
}



void
crofchan::drop_conn(
		const cauxid& auxid)
{
	RwLock(conns_rwlock, RwLock::RWLOCK_WRITE);

	if (conns.find(auxid) == conns.end()) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (rofl::cauxid(0) == auxid) {
		rofl::logging::debug << "[rofl-common][crofchan][drop_conn] "
				<< "dropping main connection and all auxiliary connections. " << str() << std::endl;

		for (std::map<cauxid, crofconn*>::reverse_iterator
				it = conns.rbegin(); it != conns.rend(); ++it) {
			delete it->second;
		}
		conns.clear();

	// auxiliary connection
	} else {
		rofl::logging::debug << "[rofl-common][crofchan][drop_conn] "
				<< "dropping auxiliary connection, auxid: " << auxid.str() << " " << str() << std::endl;

		delete conns[auxid];
		conns.erase(auxid);
	}
}



bool
crofchan::has_conn(
		const cauxid& aux_id) const
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);
	return (not (conns.find(aux_id) == conns.end()));
}




#if 0
crofconn&
crofchan::add_conn(
		const cauxid& auxid,
		enum rofl::csocket::socket_type_t socket_type,
		cparams const& socket_params)
{
	/*
	 * for connecting to peer entity: creates new crofconn instance and calls its connect() method
	 */

	if (conns.empty() && (0 != auxid.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][add_conn] "
				<< "first connection must have auxid:0, found "
				<< auxid << " instead. " << str() << std::endl;
		throw eRofChanInval();
	}

	if ((auxid.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][add_conn] "
				<< "no auxiliary connections allowed in OFP version: "
				<< (int)ofp_version << " " << str() << std::endl;
		throw eRofChanInval();
	}

	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	if (0 == auxid.get_id()) {
		vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
	} else {
		vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
	}

	if (has_conn(auxid)) {
		drop_conn(auxid); // drop old connection first
	}

	(conns[auxid] = new crofconn(this, vbitmap, get_thread_id()));

	set_conn(auxid).connect(auxid, socket_type, socket_params);

	rofl::logging::debug << "[rofl-common][crofchan] "
			<< "added connection, auxid: " << auxid.str() << std::endl;

	return *(conns[auxid]);
}



crofconn&
crofchan::add_conn(
		const cauxid& auxid,
		crofconn* conn)
{
	/*
	 * for listening sockets with existing crofconn instance
	 */

	if (conns.empty() && (0 != auxid.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][add_conn] "
				<< "first connection must have auxid:0, found "
				<< auxid << " instead. " << str() << std::endl;
		throw eRofChanInval();
	}

	if ((auxid.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][add_conn] "
				<< "no auxiliary connections allowed in OFP version: "
				<< ofp_version << " " << str() << std::endl;
		throw eRofChanInval();
	}

	if (has_conn(auxid)) {
		drop_conn(auxid); // drop old connection first
	}

	if (auxid == rofl::cauxid(0)) {
		this->ofp_version = conn->get_version();
	}

	conns[auxid] = conn;
	conns[auxid]->set_env(this);

	rofl::logging::debug << "[rofl-common][crofchan] "
			<< "added connection, auxid: " << auxid.str() << " " << str() << std::endl;

	handle_connected(*conns[auxid], conns[auxid]->get_version());

	return *(conns[auxid]);
}



crofconn&
crofchan::set_conn(
		const cauxid& auxid)
{
	if (conns.empty() && (0 != auxid.get_id())) {
		rofl::logging::error << "[rofl-common][crofchan][set_conn] "
				<< "first connection must have aux-id:0, found " << auxid
				<< " instead. " << str() << std::endl;
		throw eRofChanInval("crofchan::set_conn()");
	}

#if 0
	if ((auxid.get_id() > 0) && (ofp_version < rofl::openflow13::OFP_VERSION)) {
		rofl::logging::error << "[rofl-common][crofchan][set_conn] "
				<< "no auxiliary connections allowed in OFP version: "
				<< ofp_version << " " << str() << std::endl;
		throw eRofChanInval();
	}
#endif

	if (not has_conn(auxid)) {

		rofl::openflow::cofhello_elem_versionbitmap vbitmap;
		if (0 == auxid.get_id()) {
			vbitmap = versionbitmap;				// main connection: propose all OFP versions defined for our side
		} else {
			vbitmap.add_ofp_version(ofp_version);	// auxiliary connections: use OFP version negotiated for main connection
		}
		conns[auxid] = new crofconn(this, vbitmap, get_thread_id());

		rofl::logging::debug << "[rofl-common][crofchan][set_conn] "
				<< "added connection, auxid: " << auxid << " " << str() << std::endl;
	}
	return *(conns[auxid]);
}



void
crofchan::drop_conn(
		const cauxid& auxid)
{
	if (not has_conn(auxid)) {
		return;
	}

	// main connection: close main and all auxiliary connections
	if (rofl::cauxid(0) == auxid) {
		rofl::logging::debug << "[rofl-common][crofchan][drop_conn] "
				<< "dropping main connection and all auxiliary connections. " << str() << std::endl;
		delete conns[auxid];
		conns.erase(auxid);

		while (not conns.empty()) {
			drop_conn(conns.rbegin()->first);
		}
	} else {
		rofl::logging::debug << "[rofl-common][crofchan][drop_conn] "
				<< "dropping auxiliary connection, auxid: " << auxid.str() << " " << str() << std::endl;
		delete conns[auxid];
		conns.erase(auxid);
	}
}



bool
crofchan::has_conn(
		const cauxid& aux_id) const
{
	return (not (conns.find(aux_id) == conns.end()));
}
#endif



unsigned int
crofchan::send_message(
		const cauxid& aux_id,
		rofl::openflow::cofmsg *msg)
{
	RwLock(conns_rwlock, RwLock::RWLOCK_READ);

	if (conns.find(aux_id) == conns.end()) {
		rofl::logging::error << "[rofl-common][crofchan] sending message failed for aux-id:" << aux_id << " not found." << std::endl << *this;
		throw eRofChanNotFound("crofchan::send_message()"); // throw exception, when this connection does not exist
	}

	if (not conns[aux_id]->is_established()) {
		rofl::logging::error << "[rofl-common][crofchan] connection for aux-id:" << aux_id << " not established." << std::endl << *this;
		throw eRofChanNotConnected("crofchan::send_message()");
	}

	unsigned int cwnd_size = 0;

	cwnd_size = conns[aux_id]->send_message(msg);

	if (cwnd_size == 0) {
		throw eRofBaseCongested();
	}

	return cwnd_size;
}



void
crofchan::handle_timeout(
		int opaque, void* data)
{
	switch (opaque) {
	case TIMER_RUN_ENGINE: {
		work_on_eventqueue();
	} break;
	default: {
		// ignore unknown timer types
	};
	}
}



void
crofchan::push_on_eventqueue(
		enum crofchan_event_t event)
{
	if (EVENT_NONE != event) {
		events.push_back(event);
	}
	if (not flags.test(FLAG_ENGINE_IS_RUNNING)) {
		register_timer(TIMER_RUN_ENGINE, rofl::ctimespec(/*second(s)=*/0));
	}
}



void
crofchan::work_on_eventqueue()
{
	flags.set(FLAG_ENGINE_IS_RUNNING);
	while (not events.empty()) {
		enum crofchan_event_t event = events.front();
		events.pop_front();

		switch (event) {
		case EVENT_CONN_ESTABLISHED: {
			event_conn_established();
		} break;
		case EVENT_CONN_TERMINATED: {
			flags.reset(FLAG_ENGINE_IS_RUNNING);
			event_conn_terminated();
		} return; // might call this object's destructor
		case EVENT_CONN_REFUSED: {
			event_conn_refused();
		} break;
		case EVENT_CONN_FAILED: {
			event_conn_failed();
		} break;
		default: {
			// ignore yet unknown events
		};
		}
	}
	flags.reset(FLAG_ENGINE_IS_RUNNING);
}



void
crofchan::event_conn_established()
{
	rofl::logging::info << "[rofl-common][crofchan] "
			<< "-event-conn-established- " << str() << std::endl;

	rofl::RwLock rwlock(conns_established_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_established.begin(); it != conns_established.end(); ++it) {

		const cauxid& auxid = *it;

		if (not has_conn(auxid)) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << auxid.str() << " -conn-established- " << str() << std::endl;

		call_env().handle_conn_established(*this, auxid);
	}

	conns_established.clear();
}



void
crofchan::event_conn_terminated()
{
	rofl::logging::info << "[rofl-common][crofchan] "
			<< "-event-conn-terminated- " << str() << std::endl;

	rofl::RwLock rwlock(conns_terminated_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_terminated.begin(); it != conns_terminated.end(); ++it) {

		const cauxid& auxid = *it;

		if (not has_conn(auxid)) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-terminated- " << str() << std::endl;

		call_env().handle_conn_terminated(*this, auxid);

		if (has_conn(auxid)) {
			if (set_conn(auxid).is_actively_established()) {
				set_conn(auxid).reconnect();
			} else {
				drop_conn(auxid);
			}
		}
	}

	conns_terminated.clear();
}



void
crofchan::event_conn_refused()
{
	rofl::logging::info << "[rofl-common][crofchan] "
			<< "-event-conn-refused- " << str() << std::endl;

	rofl::RwLock rwlock(conns_refused_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_refused.begin(); it != conns_refused.end(); ++it) {

		const cauxid& auxid = *it;

		if (not has_conn(auxid)) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-refused- " << str() << std::endl;

		call_env().handle_conn_refused(*this, auxid);

		if (has_conn(auxid)) {
			if (set_conn(auxid).is_actively_established()) {
				set_conn(auxid).reconnect();
			} else {
				drop_conn(auxid);
			}
		}
	}

	conns_refused.clear();
}



void
crofchan::event_conn_failed()
{
	rofl::logging::info << "[rofl-common][crofchan] "
			<< "-event-conn-failed- " << str() << std::endl;

	rofl::RwLock rwlock(conns_failed_rwlock, rofl::RwLock::RWLOCK_WRITE);

	for (std::list<rofl::cauxid>::iterator
			it = conns_failed.begin(); it != conns_failed.end(); ++it) {

		const cauxid& auxid = *it;

		if (not has_conn(auxid)) {
			continue;
		}

		rofl::logging::info << "[rofl-common][crofchan] "
				<< "auxid: " << it->str() << " -conn-failed- " << str() << std::endl;

		call_env().handle_conn_failed(*this, auxid);

		if (has_conn(auxid)) {
			if (set_conn(auxid).is_actively_established()) {
				set_conn(auxid).reconnect();
			} else {
				drop_conn(auxid);
			}
		}
	}

	conns_failed.clear();
}


