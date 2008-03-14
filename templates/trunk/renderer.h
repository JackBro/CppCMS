#ifndef RENDERER_H
#define RENDERER_H
#include <stack>
#include <stdexcept>
#include <boost/shared_array.hpp>
#include "content.h"

namespace tmpl {

#define TMPL_VERSION 0

class tmpl_error : public std::runtime_error
{
public:
        tmpl_error(std::string const &error) : std::runtime_error(error) {};
};

namespace details {

typedef enum {
	OP_INLINE,	// 		r0 text_id
			//		r1 text_len

	OP_DISPLAY,	// flag = 0,	r0 global variable 	print global_var[name(r0)]
			// flag = 1,	r0 -  sequence 		print local_seq[r1][name(r0)]
			//		r1 - sequence id 	
			// flag = 2,    r0 - input parameter	print local_var[r0]

	OP_START_SEQ,	// flag/r0/r1 - see display
			//		r2 - new seq id	local_seq[r2]=RES
			// 		jump - goto on empty

	OP_STORE,	// flag/r0/r1 - see display
			//		r2 local ref local_var[r2]=RES

	OP_NEXT_SEQ,	//		r0 sequence id		local_seq[r0]++;
			//		jump - goto on empty
	OP_CHECK_DEF,	// (flag/r0/r1)
	OP_CHECK_TRUE,	// (flag/r0/r1)
	OP_JMP,		// flag = 0	goto jump
			// flag = 1	jump on true
			// flag = 2	jump on false
	OP_CALL,	// 		call jump
	OP_CALL_REF,	// (flag/r0/r1)
	OP_RET		//		return
};

struct instruction {
	uint8_t opcode;
	uint8_t flag;
	uint16_t r0;
	uint16_t r1;
	uint16_t r2;
	uint32_t jump;
};

struct header {
	uint32_t magic;
	uint32_t version;
	uint32_t opcode_start;
	uint32_t opcode_size;
	uint32_t func_tbl_start;
	uint32_t func_tbl_size;
	uint32_t func_entries_tbl_start;
	uint32_t names_tbl_start;
	uint32_t names_tbl_size;
	uint32_t texts_tbl_start;
	uint32_t texts_tbl_size;
	uint32_t local_variables;
	uint32_t local_sequences;
};



class sequence {
	content::list_t const *lst;
	content::list_t::const_iterator lst_iterator;
	content::vector_t const *vec;
	content::vector_t::const_iterator vec_iterator;
	content::callback_ptr const *callback;
	enum { s_none, s_list, s_vector, s_callback } type;
	content tmp_content;
public:
	sequence() : type(s_none) {};
	content const *get();
	bool next();
	bool first();
	void set(boost::any const &a);
	void reset();
};

}

class template_data {
public:
	template_data(std::string const &fname);
	~template_data();
	void *image;
};

class renderer 
{
	template_data const *view;

	std::vector<std::string> id_to_name;
	std::map<std::string,uint32_t> functions;
	uint32_t local_variables;
	uint32_t local_sequences;

	std::vector<char const *> texts;
	// Registers:
	uint32_t	pc;
	bool		flag;
	// Memory
	uint32_t code_len;
	details::instruction const *opcodes;
	// Sequence readers
	boost::shared_array<details::sequence> seq;
	// Local variables
	std::vector<boost::any const *> local;
	// Call Stack
	std::stack<uint32_t> call_stack;

	boost::any const &any_value(details::instruction const &op,content const &c);
	std::string const &string_value(details::instruction const &op,content const &c);
	bool bool_value(details::instruction const &op,content const &c);
	void setup();

public:
	renderer(template_data const &tmpl) : view(&tmpl) { setup(); };
	void render(content const &c,std::string const &func,std::string &out);
};

}


#endif
