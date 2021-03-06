//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <booster/locale/conversion.h>
#include <booster/locale/localization_backend.h>
#include <booster/locale/generator.h>
#include <booster/locale/info.h>
#include <iomanip>
#include "test_locale.h"
#include "test_locale_tools.h"
#include <iostream>

template<typename CharType>
void test_one(std::locale const &l,std::string src,std::string tgtl,std::string tgtu)
{
    TEST(booster::locale::to_upper(to_correct_string<CharType>(src,l),l) == to_correct_string<CharType>(tgtu,l));
    TEST(booster::locale::to_lower(to_correct_string<CharType>(src,l),l) == to_correct_string<CharType>(tgtl,l));
    TEST(booster::locale::fold_case(to_correct_string<CharType>(src,l),l) == to_correct_string<CharType>(tgtl,l));
}

template<typename CharType>
void test_char()
{
    booster::locale::generator gen;

    std::cout << "- Testing at least C" << std::endl;

    std::locale l = gen("en_US.UTF-8");

    test_one<CharType>(l,"Hello World i","hello world i","HELLO WORLD I");

    std::string name;

    name = get_std_name("en_US.UTF-8");
    if(!name.empty()) {
        std::cout << "- Testing " << name << std::endl;
        std::locale l=gen(name);
        test_one<CharType>(l,"Façade","façade","FAÇADE");
    }
    else {
        std::cout << "- en_US.UTF-8 is not supported, skipping" << std::endl;
    }

    name = get_std_name("en_US.ISO8859-1");
    if(!name.empty()) {
        std::cout << "Testing " << name << std::endl;
        std::locale l=gen(name);
        test_one<CharType>(l,"Hello World","hello world","HELLO WORLD");
        test_one<CharType>(l,"Façade","façade","FAÇADE");
    }
    else {
        std::cout << "- en_US.ISO8859-1 is not supported, skipping" << std::endl;
    }
    std::string real_name;
    name = get_std_name("tr_TR.UTF-8",&real_name);
    if(!name.empty()) {
        std::cout << "Testing " << name << std::endl;
        if(std::use_facet<std::ctype<wchar_t> >(std::locale(real_name.c_str())).toupper(L'i')!=L'I') {
            std::locale l=gen(name);
            test_one<CharType>(l,"i","i","İ");
        }
        else {
            std::cout << "Standard library does not support this locale's case conversion correctly" << std::endl;
        }
    }
    else 
    {
        std::cout << "- tr_TR.UTF-8 is not supported, skipping" << std::endl;
    }
}


int main()
{
    try {
        booster::locale::localization_backend_manager mgr = booster::locale::localization_backend_manager::global();
        mgr.select("std");
        booster::locale::localization_backend_manager::global(mgr);

        std::cout << "Testing char" << std::endl;
        test_char<char>();
        std::cout << "Testing wchar_t" << std::endl;
        test_char<wchar_t>();
        #ifdef BOOSTER_HAS_CHAR16_T
        std::cout << "Testing char16_t" << std::endl;
        test_char<char16_t>();
        #endif
        #ifdef BOOSTER_HAS_CHAR32_T
        std::cout << "Testing char32_t" << std::endl;
        test_char<char32_t>();
        #endif
    }
    catch(std::exception const &e) {
        std::cerr << "Failed " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    FINALIZE();

}
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4


