/*
 * cworkflow_plain_test.cc
 *
 *  Created on: 07.04.2014
 *      Author: andreas
 */

#include <stdlib.h>

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "cworkflow_test.h"

CPPUNIT_TEST_SUITE_REGISTRATION( cworkflow_test );

#if defined DEBUG
//#undef DEBUG
#endif


void
cworkflow_test::setUp()
{
#ifdef DEBUG
	rofl::logging::set_debug_level(7);
#endif

	rofl::crofcore::initialize();

	int openflow_bind_portno = 6653;
	rofl::csocket::socket_type_t socket_type = rofl::csocket::SOCKET_TYPE_PLAIN;
	int num_of_ctls = 4;
	int num_of_dpts = 8;
	rofl::openflow::cofhello_elem_versionbitmap vbitmap;
	vbitmap.add_ofp_version(rofl::openflow13::OFP_VERSION);


	std::cerr << "initializing workflow test ..." << std::endl;

	for (int i = 0; i < num_of_ctls; i++) {

		std::stringstream s_port; s_port << (openflow_bind_portno+i);
		rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);
		socket_params.set_param(rofl::csocket::PARAM_KEY_LOCAL_PORT).set_string() = s_port.str();

		ctls[i].set_versionbitmap().add_ofp_version(rofl::openflow13::OFP_VERSION);
		ctls[i].add_dpt_listening(i, socket_type, socket_params);
	}
	for (int i = 0; i < num_of_dpts; i++) {

		rofl::cctlid ctlid(i % num_of_ctls);

		std::stringstream s_port; s_port << (openflow_bind_portno+(i % num_of_ctls));
		rofl::cparams socket_params = rofl::csocket::get_default_params(socket_type);

		socket_params.set_param(rofl::csocket::PARAM_KEY_DOMAIN).set_string("inet");
		socket_params.set_param(rofl::csocket::PARAM_KEY_TYPE).set_string("stream");
		socket_params.set_param(rofl::csocket::PARAM_KEY_PROTOCOL).set_string("tcp");
		socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_HOSTNAME).set_string("127.0.0.1");
		socket_params.set_param(rofl::csocket::PARAM_KEY_REMOTE_PORT).set_string() = s_port.str();

		dpts[i].add_ctl(ctlid, vbitmap).connect(rofl::cauxid(0), socket_type, socket_params);
	}

	int cnt = 60;

	while (cnt > 0) {
		struct timespec ts;
		ts.tv_sec = 1;
		ts.tv_nsec = 0;

		std::cerr << ".";

		pselect(0, NULL, NULL, NULL, &ts, NULL);
		--cnt;
	}
	std::cerr << std::endl;
}



void
cworkflow_test::tearDown()
{
	std::cerr << "terminating workflow test ..." << std::endl;
	dpts.clear();

	ctls.clear();

	rofl::crofcore::terminate();
}



void
cworkflow_test::testRoflImpl()
{
}


