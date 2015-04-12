/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * crofcore.h
 *
 *  Created on: 11.04.2015
 *      Author: andreas
 */

#ifndef CROFCORE_H
#define CROFCORE_H 1

#include <vector>

#include "rofl/common/ciosrv.h"
#include "rofl/common/thread_helper.h"

namespace rofl {

class crofcore {
public:

	/**
	 * @brief	crofcore constructor
	 *
	 */
	crofcore() :
		rofcore_tid(crofcore::get_next_worker_tid())
	{};

	/**
	 * @brief	crofcore destructor
	 *
	 */
	virtual
	~crofcore()
	{};

public:

	static void
	initialize(
			unsigned int workers_num = 1);

	static void
	terminate();

	/**
	 * @brief	Set number of running running threads for rofl-common.
	 */
	static void
	set_num_of_workers(
			unsigned int n)
	{ initialize(n); };

	/**
	 *
	 */
	static void
	cleanup_on_exit()
	{ terminate(); };

protected:

	pthread_t
	get_worker_thread_id() const
	{ return rofcore_tid; };

private:

	static pthread_t
	get_next_worker_tid();

private:

	/**< flag indicating rofl-common is initialized */
	static bool                     initialized;
	/**< next crofcore instance is assigned to this worker */
	static unsigned int             next_worker_id;
	/**< number of rofl-common internal threads */
	static unsigned int             workers_num;
	/**< set of ids for active threads */
	static std::vector<pthread_t>   workers;
	/**< rwlock for rofcores */
	static PthreadRwLock	        rofcores_rwlock;

	/**< identifier assigned to this crofcore instance */
	pthread_t                       rofcore_tid;
};

}; // end of namespace

#endif
