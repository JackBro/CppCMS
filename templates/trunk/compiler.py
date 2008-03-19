#!/usr/bin/env python

import os
import re
import sys

class tocken:
	def __init__(self,name,param):
		self.name=name
		self.val=param


def interleave(*args):
	for idx in range(0, max(len(arg) for arg in args)):
		for arg in args:
			try:
				yield arg[idx]
			except IndexError:
				continue


class tmpl_descr:
	def __init__(self,start,size):
		self.start_id=start
		self.param_num=size

class template_block:
	pattern=r'^<%\s*template\s+([a-zA-Z]\w*)\s*\((.*)\)\s*%>$'
	type='template'
	def use(self,m):
		global template_parametes
		global parameters_counter
		template_parameters.clear()
		self.name=m.group(1)
		print "# template %s" % self.name
		print "extern %s" % self.name
		parameters=m.group(2)
		start_id=parameters_counter
		if not re.match(r'^\s*$',parameters):
			for param in re.split(',',parameters):
				m=re.match(r'^\s*([a-zA-Z]\w*)\s*$',param)
				if not m:
					error_exit("syntax error for paremeter %s" % param)
				else:
					pname=m.group(1)
					template_parameters[pname]=parameters_counter
					parameters_counter+=1
		global	templates_map
		if templates_map.has_key(self.name):
			error_exit("Double definition of template %s" % self.name)
		templates_map[self.name]=tmpl_descr(start_id,len(template_parameters));
		global stack
		if len(stack)!=0:
			error_exit("Can not define template inside other template")
		stack.append(self)
		global current_template
		current_template=self.name

	def on_end(self):
		print "\tret\n# End of template %s" % self.name

		

def inline_content(s):
	print "\tinline '%s'" % to_string(s)

def error_exit(x):
	global exit_flag
	sys.stderr.write("Error: %s\n" % x)
	exit_flag=1

def new_label():
	global labels_counter
	s="LBL_%d" % labels_counter
	labels_counter+=1
	return s

def to_string(s):
	res=''
	for ch in s:
		if ord(ch) < 32 or ch=='\'':
			if ord(ch) == 0:
				error_exit("charrecter \\0 is not allowed")
			else:
				res+=("\\x%02x" % ord(ch))
		else:
			res+=ch
	return res


def sequence(seq_name):
	if tmpl_seq.has_key(seq_name):
		return tmpl_seq[seq_name]
	error_exit("Undefined sequence %s" % seq_name)
	return 0

def make_ident(val):
	m=re.match(r'^\w+$',val)
	if m:
		global template_parameters
		if template_parameters.has_key(val):
			return str(template_parameters[val])
		return val
	m=re.match(r'^(\w+)\.(\w+)$',val)
	seq_id=sequence(m.group(1))
	return "%s(%d)" % ( m.group(2),seq_id )

class foreach_block:
	pattern=r'^<%\s*foreach\s+([a-zA-Z]\w*)\s+in\s+([a-zA-Z]\w*(\.([a-zA-Z]\w*))?)\s*%>$'
	type='foreach'
	has_item=0
	has_separator=0
	separator_label=''
	on_first_label=''
	def use(self,m):
		self.empty=new_label()
		self.end=new_label()
		ident=make_ident(m.group(2))
		self.seq_name=m.group(1);
		if tmpl_seq.has_key(self.seq_name):
			error_exit("Nested sequences with same name")
		global seq_no
		self.seq=seq_no
		tmpl_seq[self.seq_name]=seq_no;
		seq_no+=1
		print "\tseqf\t%s,%d,%s" % ( ident,self.seq,self.empty );
		global stack
		stack.append(self)

	def on_end(self):
		if not self.has_item:
			error_exit("foreach without item")
		print "%s:" % self.end
		print "%s:" % self.empty
		del tmpl_seq[self.seq_name]

class separator_block:
	pattern=r'^<%\s*separator\s*%>'
	type='separator'
	def use(self,m):
		global stack
		if len(stack)==0 or stack[len(stack)-1].type!='foreach':
			error_exit("separator without foreach")
			return
		foreachb=stack[len(stack)-1]
		if foreachb.has_separator:
			error_exit("two separators for one foreach")
		foreachb.has_separator=1
		foreachb.separator_label=new_label()
		foreachb.on_first_label=new_label()
		print "\tjmp\tu,%s" % foreachb.on_first_label
		print "%s:" % foreachb.separator_label
		

class item_block:
	pattern=r'^<%\s*item\s*%>'
	type='item'
	def use(self,m):
		global stack
		if len(stack)==0 or stack[len(stack)-1].type!='foreach':
			error_exit("item without foreach")
			return
		foreachb=stack[len(stack)-1]
		self.seq=foreachb.seq
		if foreachb.has_item:
			error_exit("Two items for one foreach");
		if foreachb.has_separator:
			self.next=foreachb.separator_label
			print "%s:" % foreachb.on_first_label
		else:
			self.next=new_label();
			print "%s:" % self.next;
		foreachb.has_item=1
		stack.append(self)
	def on_end(self):
		print "\tseqn\t%d,%s" % (self.seq , self.next);

class empty_block:
	pattern=r'^<%\s*empty\s*%>'
	type='empty'
	def use(self,m):
		global stack
		if len(stack)==0 or stack[len(stack)-1].type!='foreach':
			error_exit("empty without foreach")
			return
		forb=stack.pop()
		if not forb.has_item:
			error_exit("Unexpected empty - item missed?")
		print "\tjmp\tu,%s" % forb.end
		print "%s:" % forb.empty
		self.end=forb.end
		self.seq_name=forb.seq_name
		stack.append(self)
	def on_end(self):
		print "%s:" % self.end
		del tmpl_seq[self.seq_name]


class else_block:
	pattern=r'^<%\s*else\s*%>$'
	def on_end(self):
		print "%s:" % self.final
	def use(self,m):
		prev=stack.pop()
		if prev.type!='if' and prev.type!='elif':
			error_exit("elif without if");
		self.final=prev.final
		print "\tjmp\tu,%s" % self.final
		print "%s:" % prev.label
		stack.append(self)


class if_block:
	pattern=r'^<%\s*(if|elif)\s+((not)\s+)?((def)\s+)?([a-zA-Z]\w*(\.([a-zA-Z]\w*))?)\s*%>$'
	type='if'
	def prepare(self):
		ident_str=make_ident(self.ident)
		if self.has_def:
			print "\tdef\t%s" % ident_str
		else:
			print "\ttrue\t%s" % ident_str
		if self.has_not:
			print "\tjmp\tt,%s" % self.label
		else:
			print "\tjmp\tf,%s" % self.label

	def on_end(self):
		print "%s:" % self.label
		print "%s:" % self.final

	def use(self,m):
		global stack
		self.type=m.group(1)
		self.has_not=m.group(3)
		self.has_def=m.group(5)
		self.ident=m.group(6)
		self.label=new_label()
		if self.type == 'if' :
			self.final=new_label();
			stack.append(self)
			self.prepare()
		else: # type == elif
			if len(stack)!=0 :
				prev=stack.pop()
				if prev.type!='if' and prev.type!='elif':
					error_exit("elif without if");
				self.final=prev.final
				print "\tjmp\tu,%s" % self.final
				print "%s:" % prev.label
				stack.append(self)
				self.prepare()
			else:
				error_exit("Unexpeced elif");
# END ifop				
			

class end_block:
	pattern=r'^<%\s*end\s*%>';
	def use(self,m):
		global stack
		if len(stack)==0:
			error_exit("Unexpeced 'end'");
		else:
			stack.pop().on_end()

class error_com:
	pattern=r'^<%(.*)%>$'
	def use(self,m):
		error_exit("unknown command %s" % m.group(1))

class show_block:
	pattern=r'^<%\s*([a-zA-Z]\w*(\.([a-zA-Z]\w*))?)\s*%>$'
	def use(self,m):
		idnt=make_ident(m.group(1))
		print "\tshow\t%s" % idnt
		

class include_block:
	pattern=r'^<%\s*include\s+(([a-zA_Z]\w*)(\s+using(.*))?|ref\s+([a-zA-Z]\w*(\.([a-zA-Z]\w*))?))\s*%>$';
	def setup_store(self,list,tmpl):
		if not templates_map.has_key(tmpl):
			error_exit("Undefined template %s" % tmpl)
			return
		start=templates_map[tmpl].start_id
		num=templates_map[tmpl].param_num
		n=0
		for var in re.split(',',list):
			m=re.match(r'^\s*([a-zA-Z]\w*(\.([a-zA-Z]\w*))?)\s*',var)
			if m:
				id=make_ident(m.group(1))
				print "\tsto\t%s,%d" % (id,n)
				n+=1
			else:
				error_exit("Syntaxis error in param %s" % var)
		if n>num:
			error_exit("Too many parameters for template %s" % tmpl)
		elif n<num:
			error_exit("Too few parameters for template %s" % tmpl)
				
			
	def use(self,m):
		if m.group(2):
			ref=m.group(2)
			global current_template
			if ref==current_template:
				error_exit("Recurtion is not allowed")
			if m.group(4):
				self.setup_store(m.group(4),ref)
			print "\tcall\t%s" % ref
		elif m.group(5):
			ref=make_ident(m.group(3))
			print "\tcallr\t%s" % ref
		else:
			error_exit("Internal error")


def main():
	global stack
	for file in os.sys.argv[1:]:
		f=open(file,'r')
		content=f.read()
		f.close()
		texts=re.split(r'<\%[^\%]*\%\>',content)
		commands=re.findall(r'<\%[^\%]*\%\>',content)
		for x in interleave(texts,commands):
			if x=='' : continue
			matched=0
			for c in [  if_block(), template_block(), end_block(), else_block(), \
					foreach_block(), item_block(), empty_block(),separator_block(),\
					include_block(),\
					show_block(), error_com()]:
				m = re.match(c.pattern,x)
				if m :
					c.use(m)
					matched=1
					break
			if not matched:
				inline_content(x)


		if(len(stack)!=0):
			error_exit("Unexpected end of file %s" % file)

#######################
# MAIN
#######################

labels_counter=0
tmpl_seq={}
template_parameters={}
templates_map={}
parameters_counter=0
seq_no=0
stack=[]
exit_flag=0
current_template=''
main()
sys.exit(exit_flag)
