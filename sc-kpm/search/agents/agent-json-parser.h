/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _agent_json_parser_
#define _agent_json_parser_

#include <sc_memory.h>

/*!
 * Function that implements sc-agent of parsing json contents of nodes contained in json_format set.
 */

sc_result agent_json_parser(const sc_event *event, sc_addr arg);

#endif
