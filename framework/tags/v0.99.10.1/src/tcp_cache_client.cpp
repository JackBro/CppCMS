///////////////////////////////////////////////////////////////////////////////
//                                                                             
//  Copyright (C) 2008-2010  Artyom Beilis (Tonkikh) <artyomtnk@yahoo.com>     
//                                                                             
//  This program is free software: you can redistribute it and/or modify       
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////
#define CPPCMS_SOURCE
#include "tcp_messenger.h"
#include "tcp_cache_client.h"
#include "tcp_cache_protocol.h"

#include <time.h>
#include <string.h>

namespace cppcms {
namespace impl {
tcp_cache::~tcp_cache()
{
	// Nothing
}

void tcp_cache::rise(std::string const &trigger)
{
	tcp_operation_header h=tcp_operation_header();
	h.opcode=opcodes::rise;
	h.size=trigger.size();
	std::string data=trigger;
	h.operations.rise.trigger_len=trigger.size();
	broadcast(h,data);
}

void tcp_cache::clear()
{
	tcp_operation_header h=tcp_operation_header();
	h.opcode=opcodes::clear;
	h.size=0;
	std::string empty;
	broadcast(h,empty);
}

int tcp_cache::fetch(	std::string const &key,
			std::string &a,
			std::set<std::string> *tags,
			time_t &timeout,
			uint64_t &generation,
			bool transfer_if_not_updated) 
{
	std::string data=key;
	tcp_operation_header h=tcp_operation_header();
	h.opcode=opcodes::fetch;
	h.size=data.size();
	h.operations.fetch.key_len=data.size();
	
	if(transfer_if_not_updated) {
		h.operations.fetch.transfer_if_not_uptodate=1;
		h.operations.fetch.current_gen=generation;
	}
	
	if(tags)
		h.operations.fetch.transfer_triggers = 1;

	get(key).transmit(h,data);

	if(transfer_if_not_updated && h.opcode==opcodes::uptodate)
		return up_to_date;
	if(h.opcode!=opcodes::data)
		return not_found;

	timeout = h.operations.data.timeout + time(0);
	
	generation=h.operations.data.generation;

	char const *ptr=data.c_str();

	a.assign(ptr,h.operations.data.data_len);
	ptr+=h.operations.data.data_len;

	int len=h.operations.data.triggers_len;
	while(len>0) {
		std::string tag;
		unsigned tmp_len=strlen(ptr);
		tag.assign(ptr,tmp_len);
		ptr+=tmp_len+1;
		len-=tmp_len+1;
		tags->insert(tag);
	}
	return found;
}

void tcp_cache::stats(unsigned &keys,unsigned &triggers)
{
	keys=0; triggers=0;
	for(int i=0;i<conns;i++) {
		tcp_operation_header h=tcp_operation_header();
		std::string data;
		h.opcode=opcodes::stats;
		tcp[i].transmit(h,data);
		if(h.opcode==opcodes::out_stats) {
			keys+=h.operations.out_stats.keys;
			triggers+=h.operations.out_stats.triggers;
		}
	}
}

void tcp_cache::store(	std::string const &key,
			std::string const &a,
			std::set<std::string> const &triggers,
			time_t timeout)
{
	tcp_operation_header h=tcp_operation_header();
	std::string data;
	h.opcode=opcodes::store;
	data.append(key);
	h.operations.store.key_len=key.size();
	data.append(a);
	h.operations.store.data_len=a.size();
	time_t now;
	time(&now);
	h.operations.store.timeout=timeout-now > 0 ? timeout-now : 0;
	unsigned tlen=0;
	for(std::set<std::string>::const_iterator p=triggers.begin(),e=triggers.end();p!=e;++p) {
		tlen+=p->size()+1;
		data.append(p->c_str(),p->size()+1);
	}
	h.operations.store.triggers_len=tlen;
	h.size=data.size();
	get(key).transmit(h,data);
}

} // impl
} // cppcms


