#ifndef COMPILER_EXCEPTION_
#define COMPILER_EXCEPTION_

#include <exception>
#include <string>
#include <string.h>

class compiler_exception: public std::exception {
public:
    compiler_exception(std::string message) {
        this->message = new char[head_string_length + message.length()];
        strcpy(this->message, (head_string + message).c_str());
    }

    ~compiler_exception() throw() {
        delete[] this->message;
    }

    const char* what() const throw() override { 
        return this->message;
    }

private:
    const char* head_string = "error: ";
    const int head_string_length = 7;

    char* message;
};

#endif