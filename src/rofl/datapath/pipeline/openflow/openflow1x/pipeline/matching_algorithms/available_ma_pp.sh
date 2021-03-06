#!/bin/sh

SRCDIR=$1
shift

cat <<-EOF
/**
 * automatically generated by $0 $*
 * do not edit
 */

#ifndef MATCHING_ALGORITHMS_AVAILABLE_PP_H_
#define MATCHING_ALGORITHMS_AVAILABLE_PP_H_

#include <assert.h> 
#include "../../../../util/pp_guard.h" //Never forget to include the guard
#include "available_ma.h"
#include "../of1x_flow_table.h"

EOF

for ALG in "$@"; do
	echo "#include <rofl/datapath/pipeline/openflow/openflow1x/pipeline/matching_algorithms/"$ALG"/of1x_"$ALG"_ma_pp.h>"
done

cat <<-EOF

//Fwd declarations for non-inline
EOF
for ALG in "$@"; do
	file_name=$SRCDIR"/"$ALG"/of1x_"$ALG"_ma_pp.h"
	if test -e "$file_name";then
		echo "//Detected inline provider for of1x_find_best_match_"$ALG"_ma()"	
	else
		echo "of1x_flow_entry_t* of1x_find_best_match_"$ALG"_ma(of1x_flow_table_t *const table, datapacket_t *const pkt);"
	fi
done

cat <<-EOF

//Main inline find_best_match demux routine
static inline struct of1x_flow_entry*  __of1x_matching_algorithms_find_best_match(enum of1x_matching_algorithm_available ma, struct of1x_flow_table *const table, datapacket_t *const pkt){
	switch(ma){

EOF

for ALG in "$@"; do
	echo "case of1x_"$ALG"_matching_algorithm:"
	file_name=$SRCDIR"/"$ALG"/of1x_"$ALG"_ma_pp.h"
	if test -e "$file_name";then
		#there is an inline version
		echo "return of1x_find_best_match_"$ALG"_ma(table, pkt);"
	else
		#non-inline version
		echo "return of1x_matching_algorithms[ma]->find_best_match_hook(table, pkt);"
	fi
done

cat <<-EOF
	}

	assert(0);	

	return NULL;
}



#endif /* MATCHING_ALGORITHMS_AVAILABLE_PP_H_ */

EOF
