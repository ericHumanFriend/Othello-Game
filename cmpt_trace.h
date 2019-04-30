// cmpt_trace.h

// By defining CMPT_TRACE_H, we avoid problems caused by including this file
// more than once: if CMPT_TRACE_H is already defined, then the code is *not*
// included.
#ifndef CMPT_TRACE_H
#define CMPT_TRACE_H

#include <iostream>
#include <string>

namespace cmpt {

////////////////////////////////////////////////////////////////////////////////
//
// Trace is a helper class that is meant to be used in functions as a simple
// debugging aid. When you create a Trace object at the start of a function,
// it will cause a message to be printed when the function is called, and also
// when the function ends. For example:
//
//    int fib(int n) {
//       cmpt::Trace trace("fib(" + to_string(n) + ")");
//       if (n == 0) return 1;       // base case 1
//       if (n == 1) return 1;       // base case 2
//       return fib(n-1) + fib(n-2); // recursive case
//     }
//
// The messages are indented according to how many times the function has been
// called, and the results can be visually striking, and useful (i.e. you get
// an idea of stack usage and the order in which functions are called).
//
////////////////////////////////////////////////////////////////////////////////

class Trace {
private:
    std::string start_msg;
    std::string end_msg;
    static int indent;  // indent is static, and so one copy is shared by all
                        // Trace objects
    
    void print_start() const {
        std::cout << std::string(indent, ' ') << start_msg << " ...\n";
        indent += 3;
    }

    void print_end() const {
        indent -= 3;
        std::cout << std::string(indent, ' ') << "... " << end_msg << "\n";
    }
public:
    Trace(const std::string& start_msg, const std::string& end_msg) 
    : start_msg(start_msg), end_msg(end_msg)
    {
        print_start();
    }

    Trace(const std::string& msg) 
    : start_msg(msg), end_msg(msg)
    {
        print_start();
    }

    ~Trace() {
        print_end();
    }
}; // class Trace

// static variables are initialized outside their class
int Trace::indent = 0;

} // namespace cmpt

#endif
