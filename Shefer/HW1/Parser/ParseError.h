#pragma once

class ParseError : public std::exception {

public:

    ParseError(const std::string& message) : msg_(message) {
    }

public:

    virtual const char * what() const throw() {
        return msg_.c_str();
    }

    const std::string& what_str() const throw() {
        return msg_;
    }

    virtual ~ParseError() throw() {}

private:

    const ParseError& operator=(const ParseError&);

private:

    const std::string msg_;
};
