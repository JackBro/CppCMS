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
#include <cppcms/crypto.h>
#include <cppcms/config.h>
#ifdef CPPCMS_HAVE_GCRYPT
#  include <gcrypt.h>
#endif
#include <vector>
#include <stdexcept>
#include "md5.h"
#include "sha1.h"

#include <string.h>

namespace cppcms {
	namespace {
		class md5_digets : public message_digest {
		public:
			md5_digets()
			{
				impl::md5_init(&state_);
			}
			virtual unsigned digest_size() const
			{
				return 16;
			}
			virtual unsigned block_size() const
			{
				return 64;
			}
			virtual void append(void const *ptr,size_t size) 
			{
				impl::md5_append(&state_,reinterpret_cast<impl::md5_byte_t const *>(ptr),size);
			}
			virtual void readout(void *ptr)
			{
				impl::md5_finish(&state_,reinterpret_cast<impl::md5_byte_t *>(ptr));
				impl::md5_init(&state_);
			}
			virtual char const *name() const
			{
				return "md5";
			}
			virtual md5_digets *clone() const
			{
				return new md5_digets();
			}
			virtual ~md5_digets()
			{
				memset(&state_,0,sizeof(state_));
			}
		private:
			impl::md5_state_t state_;
		};
		class sha1_digets : public message_digest {
		public:
			sha1_digets()
			{
				state_.reset();
			}
			virtual unsigned digest_size() const
			{
				return 20;
			}
			virtual unsigned block_size() const
			{
				return 64;
			}
			virtual void append(void const *ptr,size_t size) 
			{
				state_.process_bytes(ptr,size);
			}
			virtual void readout(void *ptr)
			{
				unsigned digets[5];
				state_.get_digest(digets);
				state_.reset();
				unsigned char *out = reinterpret_cast<unsigned char *>(ptr);
				for(unsigned i=0;i<5;i++) {
					unsigned block = digets[i];
					*out ++ = (block >> 24u) & 0xFFu;
					*out ++ = (block >> 16u) & 0xFFu;
					*out ++ = (block >>  8u) & 0xFFu;
					*out ++ = (block >>  0u) & 0xFFu;
				}
			}
			virtual char const *name() const
			{
				return "sha1";
			}
			virtual sha1_digets *clone() const
			{
				return new sha1_digets();
			}
			virtual ~sha1_digets()
			{
				memset(&state_,0,sizeof(state_));
			}
		private:
			impl::sha1 state_;
		};

		#ifdef CPPCMS_HAVE_GCRYPT
		class gcrypt_digets : public message_digest {
		public:
			gcrypt_digets(int algorithm)
			{
				if(gcry_md_open(&state_,algorithm,0)!=0)
					throw std::invalid_argument("gcrypt_digets - faled to create md");
				algo_ = algorithm;
			}
			virtual unsigned digest_size() const
			{
				return gcry_md_get_algo_dlen(algo_);
			}
			virtual unsigned block_size() const
			{
				if(algo_ == GCRY_MD_SHA384 || algo_ == GCRY_MD_SHA512)
					return 128;
				else
					return 64;
			}
			virtual void append(void const *ptr,size_t size) 
			{
				gcry_md_write(state_,ptr,size);
			}
			virtual void readout(void *ptr)
			{
				memcpy(ptr,gcry_md_read(state_,0),digest_size());
				gcry_md_reset(state_);
			}
			virtual char const *name() const
			{
				switch(algo_) {
				case GCRY_MD_SHA224:
					return "sha224";
				case GCRY_MD_SHA256:
					return "sha256";
				case GCRY_MD_SHA384:
					return "sha384";
				case GCRY_MD_SHA512:
					return "sha512";
				default:
					return "unknown";
				}
			}
			virtual gcrypt_digets *clone() const
			{
				return new gcrypt_digets(algo_);
			}
			virtual ~gcrypt_digets()
			{
				gcry_md_close(state_);
				state_ = 0;
			}
		private:
			gcry_md_hd_t state_;
			int algo_;
		};

		#endif
	} // anon
	
	std::auto_ptr<message_digest> message_digest::md5()
	{
		std::auto_ptr<message_digest> d(new md5_digets());
		return d;
	}

	std::auto_ptr<message_digest> message_digest::sha1()
	{
		std::auto_ptr<message_digest> d(new sha1_digets());
		return d;
	}

	std::auto_ptr<message_digest> message_digest::create_by_name(std::string const &namein)
	{
		std::auto_ptr<message_digest> d;
		std::string name = namein;
		for(unsigned i=0;i<name.size();i++)
			if('A' <= name[i] && name[i]<='Z')
				name[i] = name[i] - 'A' + 'a';
		if(name=="md5")
			d = md5();
		else if(name == "sha1")
			d = sha1();
		#ifdef CPPCMS_HAVE_GCRYPT
		else if(name == "sha224")
			d.reset(new gcrypt_digets(GCRY_MD_SHA224));
		else if(name == "sha256")
			d.reset(new gcrypt_digets(GCRY_MD_SHA256));
		else if(name == "sha384")
			d.reset(new gcrypt_digets(GCRY_MD_SHA384));
		else if(name == "sha512")
			d.reset(new gcrypt_digets(GCRY_MD_SHA512));
		#endif
		return d;
	}

	struct hmac::data_ {};

	hmac::hmac(std::auto_ptr<message_digest> digest,std::string const &key)
	{
		if(!digest.get())
			throw std::invalid_argument("Has algorithm is not provided");
		md_ = digest;
		md_opad_.reset(md_->clone());
		init(key);
	}
	hmac::hmac(std::string const &name,std::string const &key)
	{
		md_ = message_digest::create_by_name(name);
		if(!md_.get()) {
			throw std::invalid_argument("Invalid or unsupported hash function:" + name);
		}
		md_opad_.reset(md_->clone());
		init(key);
	}
	hmac::~hmac()
	{
	}
	void hmac::init(std::string const &skey)
	{
		unsigned const block_size = md_->block_size();
		std::vector<unsigned char> ipad(block_size,0);
		std::vector<unsigned char> opad(block_size,0);
		if(skey.size() > block_size) {
			md_->append(skey.c_str(),skey.size());
			md_->readout(&ipad.front());
			memcpy(&opad.front(),&ipad.front(),md_->digest_size());
		}
		else {
			memcpy(&ipad.front(),skey.c_str(),skey.size());
			memcpy(&opad.front(),skey.c_str(),skey.size());
		}
		for(unsigned i=0;i<block_size;i++) {
			ipad[i]^=0x36 ;
			opad[i]^=0x5c ;
		}
		md_opad_->append(&opad.front(),block_size);
		md_->append(&ipad.front(),block_size);
		ipad.assign(block_size,0);
		opad.assign(block_size,0);
	}
	unsigned hmac::digest_size() const
	{
		if(!md_.get()){
			throw std::runtime_error("Hmac can be used only once");
		}
		return md_->digest_size();
	}
	void hmac::append(void const *ptr,size_t size)
	{
		if(!md_.get()){
			throw std::runtime_error("Hmac can be used only once");
		}
		md_->append(ptr,size);
	}
	void hmac::readout(void *ptr)
	{
		std::vector<unsigned char> digest(md_->digest_size(),0);
		md_->readout(&digest.front());
		md_opad_->append(&digest.front(),md_->digest_size());
		md_opad_->readout(ptr);
		digest.assign(md_->digest_size(),0);
		md_.reset();
		md_opad_.reset();
	}


} // cppcms
