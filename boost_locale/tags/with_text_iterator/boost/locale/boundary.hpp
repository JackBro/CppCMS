//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_LOCALE_BOUNDARY_HPP_INCLUDED
#define BOOST_LOCALE_BOUNDARY_HPP_INCLUDED

#include <boost/locale/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4275 4251 4231 4660)
#endif
#include <string>
#include <locale>
#include <vector>
#include <iterator>
#include <algorithm>
#include <stdexcept>




namespace boost {

    namespace locale {
        
        ///
        /// \brief This namespae contains all operations required for boundary analysis of text
        ///
        namespace boundary {
            ///
            /// \defgroup boundary Boundary Analysis
            ///
            /// This module contains all operations required for boundary analysis of text: character, word, like and sentence boundaries
            ///
            /// @{
            ///

            ///
            /// The enum that describes possible break types
            ///
            enum boundary_type {
                character,  ///< Find character boundaries
                word,       ///< Find word boundaries
                sentence,   ///< Find sentence boundaries
                line        ///< Find a positions suitable for line breaks
            };

            ///
            /// Flags used with word boundary analysis -- the type of the word found
            ///
            enum word_type {
                word_none       =  0x0000F,   ///< Not a word
                word_number     =  0x000F0,   ///< Word that appear to be a number
                word_letter     =  0x00F00,   ///< Word that contains letters, excluding kana and ideographic characters 
                word_kana       =  0x0F000,   ///< Word that contains kana characters
                word_ideo       =  0xF0000,   ///< Word that contains ideographic characters
                word_any        =  0xFFFF0,   ///< Any word including numbers, 0 is special flag, equivalent to 15
                word_letters    =  0xFFF00,   ///< Any word, excluding numbers but including letters, kana and ideograms.
                word_kana_ideo  =  0xFF000,   ///< Word that includes kana or ideographic characters
                word_mask       =  0xFFFFF    ///< Maximal used mask
            };
            ///
            /// Flags that describe a type of line break
            ///
            enum line_break_type {
                line_soft       =  0x0F,   ///< Soft line break: optional but not required
                line_hard       =  0xF0,   ///< Hard line break: like break is required (as per CR/LF)
                line_any        =  0xFF,   ///< Soft or Hard line break
                line_mask       =  0xFF
            };
            
            ///
            /// Flags that describe a type of sentence break
            ///
            enum sentence_break_type {
                sentence_term   =  0x0F,    ///< The sentence was terminated with a sentence terminator 
                                            ///  like ".", "!" possible followed by hard separator like CR, LF, PS
                sentence_sep    =  0xF0,    ///< The sentence does not contain terminator like ".", "!" but ended with hard separator
                                            ///  like CR, LF, PS or end of input.
                sentence_any    =  0xFF,    ///< Either first or second sentence break type;.
                sentence_mask   =  0xFF
            };

            ///
            /// Flags that describe a type of character break. At this point break iterator does not distinguish different
            /// kinds of characters so it is used for consistency.
            ///
            enum character_break_type {
                character_any   =  0xF,     ///< Not in use, just for consistency
                character_mask  =  0xF,
            };

            ///
            /// This function returns the mask that covers all variants for specific boundary type
            ///
            inline uint32_t boundary_mask(boundary_type t)
            {
                switch(t) {
                case character: return character_mask;
                case word:      return word_mask;
                case sentence:  return sentence_mask;
                case line:      return line_mask;
                default:        return 0;
                }
            }
            
            ///
            /// This structure is used for representing boundary point
            /// that follows the offset.
            ///
            struct break_info {

                ///
                /// Create empty break point at beginning
                ///
                break_info() : 
                    offset(0),
                    mark(0)
                {
                }
                ///
                /// Create empty break point at offset v.
                /// it is useful for order comparison with other points.
                ///
                break_info(size_t v) :
                    offset(v),
                    mark(0)
                {
                }

                ///
                /// Offset from the beggining of the text where a break occurs.
                ///
                size_t offset;
                ///
                /// The identification of this break point according to 
                /// various break types
                ///
                uint32_t mark;
               
                ///
                /// Compare two break points' offset. Allows to search with
                /// standard algorithms over the index.
                ///
                bool operator<(break_info const &other) const
                {
                    return offset < other.offset;
                }
            };
            
            ///
            /// This type holds the alalisys of the text - all its break points
            /// with marks
            ///
            typedef std::vector<break_info> index_type;


            template<typename CharType>
            class boundary_indexing;

            ///
            /// The facet that allows us to create an index for boundary analisys
            /// of the text.
            ///
            template<>
            class BOOST_LOCALE_DECL boundary_indexing<char> : public std::locale::facet {
            public:
                ///
                /// Default constructor typical for facets
                ///
                boundary_indexing(size_t refs=0) : std::locale::facet(refs)
                {
                }
                ///
                /// Create index for boundary type \a t for text in range [begin,end)
                ///
                virtual index_type map(boundary_type t,char const *begin,char const *end) const = 0;
                /// Identification of this facet
                static std::locale::id id;
                #if defined (__SUNPRO_CC) && defined (_RWSTD_VER)
                std::locale::id& __get_id (void) const { return id; }
                #endif
            };
            
            ///
            /// The facet that allows us to create an index for boundary analisys
            /// of the text.
            ///
            template<>
            class BOOST_LOCALE_DECL boundary_indexing<wchar_t> : public std::locale::facet {
            public:
                ///
                /// Default constructor typical for facets
                ///
                boundary_indexing(size_t refs=0) : std::locale::facet(refs)
                {
                }
                ///
                /// Create index for boundary type \a t for text in range [begin,end)
                ///
                virtual index_type map(boundary_type t,wchar_t const *begin,wchar_t const *end) const = 0;

                /// Identification of this facet
                static std::locale::id id;
                #if defined (__SUNPRO_CC) && defined (_RWSTD_VER)
                std::locale::id& __get_id (void) const { return id; }
                #endif
            };
            
            #ifdef BOOST_HAS_CHAR16_T
            template<>
            class BOOST_LOCALE_DECL boundary_indexing<char16_t> : public std::locale::facet {
            public:
                boundary_indexing(size_t refs=0) : std::locale::facet(refs)
                {
                }
                virtual index_type map(boundary_type t,char16_t const *begin,char16_t const *end) const = 0;
                static std::locale::id id;
                #if defined (__SUNPRO_CC) && defined (_RWSTD_VER)
                std::locale::id& __get_id (void) const { return id; }
                #endif
            };
            #endif
            
            #ifdef BOOST_HAS_CHAR32_T
            template<>
            class BOOST_LOCALE_DECL boundary_indexing<char32_t> : public std::locale::facet {
            public:
                boundary_indexing(size_t refs=0) : std::locale::facet(refs)
                {
                }
                virtual index_type map(boundary_type t,char32_t const *begin,char32_t const *end) const = 0;
                static std::locale::id id;
                #if defined (__SUNPRO_CC) && defined (_RWSTD_VER)
                std::locale::id& __get_id (void) const { return id; }
                #endif
            };
            #endif


            /// \cond INTERNAL

            namespace details {

                template<typename IteratorType,typename CategoryType = typename std::iterator_traits<IteratorType>::iterator_category>
                struct mapping_traits {
                    typedef typename std::iterator_traits<IteratorType>::value_type char_type;
                    static index_type map(boundary_type t,IteratorType b,IteratorType e,std::locale const &l)
                    {
                        std::basic_string<char_type> str(b,e);
                        return std::use_facet<boundary_indexing<char_type> >(l).map(t,str.c_str(),str.c_str()+str.size());
                    }
                };

                template<typename CharType,typename SomeIteratorType>
                struct linear_iterator_traits {
                    static const bool is_linear = false;
                };

                template<typename CharType>
                struct linear_iterator_traits<CharType,typename std::basic_string<CharType>::iterator> {
                    static const bool is_linear = true;
                };

                template<typename CharType>
                struct linear_iterator_traits<CharType,typename std::basic_string<CharType>::const_iterator> {
                    static const bool is_linear = true;
                };
                
                template<typename CharType>
                struct linear_iterator_traits<CharType,typename std::vector<CharType>::iterator> {
                    static const bool is_linear = true;
                };

                template<typename CharType>
                struct linear_iterator_traits<CharType,typename std::vector<CharType>::const_iterator> {
                    static const bool is_linear = true;
                };

                template<typename CharType>
                struct linear_iterator_traits<CharType,CharType *> {
                    static const bool is_linear = true;
                };

                template<typename CharType>
                struct linear_iterator_traits<CharType,CharType const *> {
                    static const bool is_linear = true;
                };


                template<typename IteratorType>
                struct mapping_traits<IteratorType,std::random_access_iterator_tag> {

                    typedef typename std::iterator_traits<IteratorType>::value_type char_type;



                    static index_type map(boundary_type t,IteratorType b,IteratorType e,std::locale const &l)
                    {
                        index_type result;

                        //
                        // Optimize for most common cases
                        //
                        // C++0x requires that string is continious in memory and all known
                        // string implementations
                        // do this because of c_str() support. 
                        //

                        if(linear_iterator_traits<char_type,IteratorType>::is_linear && b!=e)
                        {
                            char_type const *begin = &*b;
                            char_type const *end = begin + (e-b);
                            index_type tmp=std::use_facet<boundary_indexing<char_type> >(l).map(t,begin,end);
                            result.swap(tmp);
                        }
                        else {
                            std::basic_string<char_type> str(b,e);
                            index_type tmp = std::use_facet<boundary_indexing<char_type> >(l).map(t,str.c_str(),str.c_str()+str.size());
                            result.swap(tmp);
                        }
                        return result;
                    }
                };

            } // details 

            /// \endcond 


            //
            // Forward declarations to prevent collsion with boost::token_iterator
            //
            template<typename I> 
            class break_iterator; 
            template<typename I,typename V>
            class token_iterator; 

            ///
            /// \brief Class the holds boundary mapping of the text that can be used with iterators
            ///
            /// When the object is created it creates index and provides access to it with iterators.
            /// It is used mostly with break_iterator and token_iterator. For each boundary point it
            /// provides the description mark that allows distinguishing between different types of boundaries.
            /// For example, it marks whether a sentence terminates because a mark like "?" or "." was found or because
            /// a new line symbol is present in the text.
            ///
            /// These marks can be read out with the token_iterator::mark() and break_iterator::mark() member functions.
            ///
            /// This class stores iterators to the original text, so you should be careful about iterator
            /// invalidation. If the iterators on the original text are invalid, you can't use this mapping any more.
            ///

            template<class RangeIterator>
            class mapping {
            public:
                ///
                /// Iterator type that is used to iterate over boundaries
                ///
                typedef RangeIterator iterator;
                ///
                /// Underlying iterator that is used to iterate over the original text.
                ///
                typedef typename RangeIterator::base_iterator base_iterator;
                ///
                /// The character type of the text
                ///
                typedef typename std::iterator_traits<base_iterator>::value_type char_type;

                ///
                /// Create a mapping of type \a type of the text in the range [\a begin, \a end) using locale \a loc
                ///
                mapping(boundary_type type,base_iterator begin,base_iterator end,std::locale const &loc = std::locale())
                {
                    create_mapping(type,begin,end,loc,0xFFFFFFFFu);
                }

                ///
                /// Create a mapping of type \a type of the text in the range [\a begin, \a end) using locale \a loc and set the boundaries
                /// mask to \a mask
                ///
                mapping(boundary_type type,base_iterator begin,base_iterator end,uint32_t mask,std::locale const &loc = std::locale())
                {
                    create_mapping(type,begin,end,loc,mask);
                }

                ///
                /// Create a mapping of type \a type of the text in the range [\a begin, \a end) using locale \a loc
                ///
                void map(boundary_type type,base_iterator begin,base_iterator end,std::locale const &loc = std::locale())
                {
                    create_mapping(type,begin,end,loc,0xFFFFFFFFu);
                }

                ///
                /// Create a mapping of type \a type of the text in the range [\a begin, \a end) using locale \a loc, and set a mask to \a mask
                ///
                void map(boundary_type type,base_iterator begin,base_iterator end,uint32_t mask,std::locale const &loc = std::locale())
                {
                    create_mapping(type,begin,end,loc,mask);
                }

                ///
                /// Default constructor of empty mapping
                ///

                mapping()
                {
                    mask_=0xFFFFFFFF;
                }

                ///
                /// Copy the mapping, note, you can copy the mapping that is used for token_iterator to break_iterator and vise versa.
                ///
                template<typename ORangeIterator>
                mapping(mapping<ORangeIterator> const &other) :
                    index_(other.index_),
                    begin_(other.begin_),
                    end_(other.end_),
                    mask_(other.mask_)
                {
                }

                ///
                /// Swap the mappings, note, you swap the mappings between those that are used for 
                /// token_iterator to break_iterator and vise versa.
                /// This operation invalidates all iterators.
                ///
                /// \note The base iterators should have the same type, (i.e. std::string::const_iterator or char const *
                /// but not mix)
                ///
                template<typename ORangeIterator>
                void swap(mapping<ORangeIterator> &other)
                {
                    index_.swap(other.index_),
                    std::swap(begin_,other.begin_);
                    std::swap(end_,other.end_);
                    std::swap(mask_,other.mask_);
                }

                ///
                /// Copy the mapping, note, you can copy the mapping that is used for token_iterator 
                /// to break_iterator and vise versa as long as they have same base iterator type
                ///
                template<typename ORangeIterator>
                mapping const &operator=(mapping<ORangeIterator> const &other)
                {
                    mapping tmp(other);
                    swap(tmp);
                    return *this;
                }

                ///
                /// Get current boundary mask
                ///
                uint32_t mask() const
                {
                    return mask_;
                }
                ///
                /// Set current boundary mask.
                ///
                /// This mask provides fine grained control on the type of boundaries and tokens you need to relate to. For example, if 
                /// you want to find sentence breaks that are caused only by terminator like "." or "?" and ignore new lines, you can set the mask
                /// value sentence_term and break iterator would iterate only over boundaries that much this mask.
                ///
                /// Note: the beginning of the text and the end of the text are always considered legal boundaries regardless if they have
                /// a mark that fits the mask.
                ///
                /// For token iterator it means which kind of tokens should be selected. Please note that token iterator generally selects the
                /// biggest amount of text that has specific mark. This is especially relevant for word boundary analysis.
                ///
                /// For example: if you set mask to word_any (selects numbers, letters) then when you iterate Over "To be, or not to be?" You would 
                /// get "To", "be", "or", "not", "to", "be". You can request from token iterator to use wider type of selection by
                /// calling token_iterator::full_select(true) so it would select only "To", " be", ", or", " not", " to", " be" tokens. All depends
                /// on your actual needs. For word selection you would probably want the first (default) and for sentence selection the second.
                ///
                /// Changing a mask does not invalidate current iterators but all new created iterators would not be compatible with old ones
                /// So you can't compare them, be careful with it.
                ///
                void mask(uint32_t u)
                {
                    mask_ = u;
                }

                ///
                /// Get \a begin iterator used when object was created
                ///
                RangeIterator begin() const
                {
                    return RangeIterator(*this,true,mask_);
                }
                ///
                /// Get \a end iterator used when object was created
                ///
                RangeIterator end() const
                {
                    return RangeIterator(*this,false,mask_);
                }

            private:
                void create_mapping(boundary_type type,base_iterator begin,base_iterator end,std::locale const &loc,uint32_t mask)
                {
                    index_type idx=details::mapping_traits<base_iterator>::map(type,begin,end,loc);
                    index_.swap(idx);
                    begin_ = begin;
                    end_ = end;
                    mask_=mask;
                }
                template<typename I> 
                friend class break_iterator; 
                template<typename I,typename V>
                friend class token_iterator; 
                template<typename I>
                friend class mapping;

                base_iterator begin_,end_;
                index_type index_;
                uint32_t mask_;
            };


            ///
            /// \brief token_iterator is an iterator that returns text chunks between boundary positions
            ///
            /// Token iterator can behave in two different ways: 
            ///
            /// -   Select specific tokens in `tide' way (default) - only range that is covered by "mark flags",
            ///     for example for sentence "I met him at 7" and word iteration with mask "word_letters"
            ///     it would return words: "I", "met", "him", "at" ignoring white spaces, punctuation and numbers.
            /// -   Select specific tokens widely when you need to perform full selection of almost entire text.
            ///     For example, for sentence boundaries and sentence_term mask you may want to specify full_select(true), 
            ///     So "Hello! How<LF>are you?" would return you biggest possible chunks "Hello!", " How<LF>are you?".
            ///
            template<
                typename IteratorType,
                typename ValueType = std::basic_string<typename std::iterator_traits<IteratorType>::value_type> 
            >
            class token_iterator : public std::iterator<std::bidirectional_iterator_tag,ValueType> 
            {
            public:
                ///
                /// The character type of the text
                ///
                typedef typename std::iterator_traits<IteratorType>::value_type char_type;
                ///
                /// Underlying iterator that is used to iterate over the original text.
                ///
                typedef IteratorType base_iterator;
                ///
                /// The type of mapping that the iterator can iterate over it
                /// 
                typedef mapping<token_iterator<IteratorType,ValueType> > mapping_type;
                                
                ///
                /// Default constructor
                ///
                token_iterator() : 
                    map_(0),
                    offset_(0),
                    mask_(0xFFFFFFFF),
                    full_select_(false)
                {
                }

                ///
                /// set the position of the token iterator to the location of underlying iterator.
                ///
                /// This operator sets the token iterator to first token following that position. For example:
                ///
                /// For word boundary with "word_any" mask:
                ///
                /// - "to| be or ", would point to "be",
                /// - "t|o be or ", would point to "to",
                /// - "to be or| ", would point to end.
                ///
                /// \a p - should be in range of the original mapping.
                ///
                
                token_iterator const &operator=(IteratorType p)
                {
                    size_t dist=std::distance(map_->begin_,p);
                    index_type::const_iterator b=map_->index_.begin(),e=map_->index_.end();
                    index_type::const_iterator 
                        bound=std::upper_bound(b,e,break_info(dist));
                    while(bound != e && (bound->mark & mask_)==0)
                        bound++;
                    offset_ = bound - b;
                    return *this;
                }

                /// \cond INTERNAL

                //
                // Create a token iterator for mapping \a map with location at begin or end according to value of flag \a begin,
                // and a mask \a mask
                //
                // Should be used internally only
                //
                token_iterator(mapping_type const &map,bool begin,uint32_t mask) :
                    map_(&map),
                    mask_(mask),
                    full_select_(false)
                {
                    if(begin) {
                        offset_ = 0;
                        next();
                    }
                    else
                        offset_=map_->index_.size();
                }

                /// \endcond

                ///
                /// Copy constructor
                ///
                token_iterator(token_iterator const &other) :
                    map_(other.map_),
                    offset_(other.offset_),
                    mask_(other.mask_),
                    full_select_(other.full_select_)
                {
                }

                ///
                /// Assignment operator
                ///
                token_iterator const &operator=(token_iterator const &other)
                {
                    if(this!=&other) {
                        map_ = other.map_;
                        offset_ = other.offset_;
                        mask_=other.mask_;
                        full_select_ = other.full_select_;
                    }
                    return *this;
                }

                ///
                /// Return the token the iterator points it. Iterator must not point to the
                /// end of the range. Throws std::out_of_range exception
                ///
                /// Note, returned value is not lvalue, you can't use this iterator to assign new values to text.
                /// 
                ValueType operator*() const
                {
                    BOOST_ASSERT(!(offset_ < 1 || offset_ >= map_->index_.size()));
                    
                    size_t pos=offset_-1;
                    if(full_select_)
                        while(!valid_offset(pos))
                            pos--;
                    base_iterator b=map_->begin_;
                    size_t b_off = map_->index_[pos].offset;
                    std::advance(b,b_off);
                    base_iterator e=b;
                    size_t e_off = map_->index_[offset_].offset;
                    std::advance(e,e_off-b_off);
                    return ValueType(b,e);
                }

                ///
                /// Increment operator
                ///
                token_iterator &operator++() 
                {
                    next();
                    return *this;
                }
                
                ///
                /// Decrement operator
                ///
                token_iterator &operator--() 
                {
                    prev();
                    return *this;
                }
                
                ///
                /// Increment operator
                ///
                token_iterator operator++(int unused) 
                {
                    token_iterator tmp(*this);
                    next();
                    return tmp;
                }

                ///
                /// Decrement operator
                ///
                token_iterator operator--(int unused) 
                {
                    token_iterator tmp(*this);
                    prev();
                    return tmp;
                }

                ///
                /// Get full selection flag, see description of token_iterator
                ///
                bool full_select() const
                {
                    return full_select_;
                }
                ///
                /// Set full selection flag, see description of token_iterator
                ///
                void full_select(bool fs)
                {
                    full_select_ = fs;
                }
                
                ///
                /// Compare two iterators. They are equal if they point to same map and have the same position and mask
                ///
                bool operator==(token_iterator const &other) const
                {
                    return  map_ == other.map_ 
                            && offset_==other.offset_ 
                            && mask_ == other.mask_;
                }

                ///
                /// Opposite of ===
                ///
                bool operator!=(token_iterator const &other) const
                {
                    return !(*this==other);
                }

                ///
                /// Return the mark that token iterator points at. See description of mapping class and various boundary flags
                ///
                uint32_t mark() const
                {
                    return map_->index_.at(offset_).mark;
                }

            private:
                
                bool valid_offset(size_t offset) const
                {
                    return  offset == 0 
                            || offset == map_->index_.size()
                            || (map_->index_[offset].mark & mask_)!=0;
                }

                bool at_end() const
                {
                    return !map_ || offset_>=map_->index_.size();
                }
                
                void next()
                {
                    while(offset_ < map_->index_.size()) {
                        offset_++;
                        if(valid_offset(offset_))
                            break;
                    }
                }
                
                void prev()
                {
                    while(offset_ > 0) {
                        offset_ --;
                        if(valid_offset(offset_))
                            break;
                    }
                }
                
                mapping_type const * map_;
                size_t offset_;
                uint32_t mask_;
                uint32_t full_select_ : 1;
                uint32_t reserved_ : 31;
            };


            ///
            /// \brief break_iterator is bidirectional iterator that returns text boundary positions
            ///
            /// It returns iterators pointing to the break positions rather than the text chunks themselves. It stops only
            /// on boundaries whose marks fit the required mask. Also beginning of text and end of 
            /// text are valid boundaries regardless of their marks.
            ///
            /// Please note that for text in the range [text_begin,text_end) and break_iterator it over
            /// the range [begin,end): if *it==text_end then it!=end. And if it==end then *it is invalid.
            /// Thus for example for over the text "hello", break iterator returns
            /// text_begin ("|hello"), then text_end ("hello|") and then it points to end.
            ///
            template<typename IteratorType>
            class break_iterator : public std::iterator<std::bidirectional_iterator_tag,IteratorType> 
            {
            public:
                ///
                /// The character type of the text
                ///
                typedef typename std::iterator_traits<IteratorType>::value_type char_type;
                ///
                /// Underlying iterator that is used to iterate over the original text.
                ///
                typedef IteratorType base_iterator;
                ///
                /// The type of mapping that the iterator can iterate over.
                /// 
                typedef mapping<break_iterator<IteratorType> > mapping_type;
                
                ///
                /// Default constructor
                ///
                break_iterator() : 
                    map_(0),
                    offset_(0),
                    mask_(0xFFFFFFFF)
                {
                }

                ///
                /// Copy constructor
                ///
                break_iterator(break_iterator const &other):
                    map_(other.map_),
                    offset_(other.offset_),
                    mask_(other.mask_)
                {
                }
                
                ///
                /// Assignment operator
                ///
                break_iterator const &operator=(break_iterator const &other)
                {
                    if(this!=&other) {
                        map_ = other.map_;
                        offset_ = other.offset_;
                        mask_ = other.mask_;
                    }
                    return *this;
                }

                /// \cond INTERNAL

                // Create break iterator for mapping \a map with location at begin or end according to value of flag \a begin,
                // and a mask \a mask
                //
                // It is internal function
                //
                break_iterator(mapping_type const &map,bool begin,uint32_t mask) :
                    map_(&map),
                    mask_(mask)
                {
                    if(begin)
                        offset_ = 0;
                    else
                        offset_=map_->index_.size();
                }

                // \endcond

                ///
                /// Compare two iterators. They are equal if they point to the same map and have the same position and mask
                ///
                bool operator==(break_iterator const &other) const
                {
                    return  map_ == other.map_ 
                            && offset_==other.offset_
                            && mask_==other.mask_;
                }

                ///
                /// Opposite of ===
                ///
                bool operator!=(break_iterator const &other) const
                {
                    return !(*this==other);
                }

                ///
                /// Return the mark that token iterator points at. See description of mapping class and various boundary flags
                ///
                uint32_t mark() const
                {
                    return map_->index_.at(offset_).mark;
                }
 
                ///
                /// set position of the break_iterator to the location of underlying iterator.
                ///
                /// This operator sets the break_iterator to position of the iterator p or to the first valid following position
                /// For example:
                ///
                /// For word boundary:
                ///
                /// - "|to be or ", would point to "|to be or " 
                /// - "t|o be or ", would point to "to| be or " 
                ///
                /// \a p - should be in range of the original mapping.
                ///
                break_iterator const &operator=(base_iterator p)
                {
                    at_least(p);
                    return *this;
                }
                
                ///
                /// Return the underlying iterator that break_iterator points it. Iterator must not point to the
                /// end of the range, otherwise throws std::out_of_range exception
                ///
                /// Note, returned value is not lvalue, you can't use this iterator to change underlying iterators.
                /// 
                base_iterator operator*() const
                {
                    BOOST_ASSERT(!(offset_ >=map_->index_.size()));
                    base_iterator p = map_->begin_;
                    std::advance(p, map_->index_[offset_].offset);
                    return p;
                }

                ///
                /// Increment operator
                ///
                break_iterator &operator++() 
                {
                    next();
                    return *this;
                }
                
                ///
                /// Decrement operator
                ///
                break_iterator &operator--() 
                {
                    prev();
                    return *this;
                }
                
                ///
                /// Increment operator
                ///
                break_iterator operator++(int unused) 
                {
                    break_iterator tmp(*this);
                    next();
                    return tmp;
                }

                ///
                /// Decrement operator
                ///
                break_iterator operator--(int unused) 
                {
                    break_iterator tmp(*this);
                    prev();
                    return tmp;
                }

            private:
                bool valid_offset(size_t offset) const
                {
                    return  offset == 0 
                            || offset + 1 >= map_->index_.size() // last and first are always valid regardless of mark
                            || (map_->index_[offset].mark & mask_)!=0;
                }

                bool at_end() const
                {
                    return !map_ || offset_>=map_->index_.size();
                }
                
                void next()
                {
                    while(offset_ < map_->index_.size()) {
                        offset_++;
                        if(valid_offset(offset_))
                            break;
                    }
                }
                void prev()
                {
                    while(offset_ > 0) {
                        offset_ --;
                        if(valid_offset(offset_))
                            break;
                    }
                }
                
                void at_least(IteratorType p)
                {
                    size_t diff =  std::distance(map_->begin_,p);

                    index_type::const_iterator b=map_->index_.begin();
                    index_type::const_iterator e=map_->index_.end();
                    index_type::const_iterator ptr = std::lower_bound(b,e,break_info(diff));

                    if(ptr==map_->index_.end())
                        offset_=map_->index_.size()-1;
                    else
                        offset_=ptr - map_->index_.begin();
                    
                    while(!valid_offset(offset_))
                        offset_ ++;
                }

                mapping_type const * map_;
                size_t offset_;
                uint32_t mask_;
                uint32_t reserved_;
            };

            ///
            /// @}
            ///

        } // boundary

    } // locale
} // boost

///
/// \example boundary.cpp
/// Example of using boundary iterator
/// \example wboundary.cpp
/// Example of using boundary iterator over wide strings
///

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
