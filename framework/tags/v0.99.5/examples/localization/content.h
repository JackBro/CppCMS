#ifndef CONTENT_H
#define CONTENT_H

#include <cppcms/view.h>
#include <string>

namespace content  {
    struct message : public cppcms::base_content {
        cppcms::locale::message message;
    };
}


#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
