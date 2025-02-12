#ifndef VISITOR_HH_
#define VISITOR_HH_

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

#endif