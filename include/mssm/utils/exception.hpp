#if !defined(SMSOLVER_EXCEPTION_HPP)
#define SMSOLVER_EXCEPTION_HPP

#include<exception>
#include<string>

namespace SMSolver {

struct SMSolverException: std::exception {
    protected:
        const std::string msg;
    public:
        SMSolverException(std::string&& s):      msg(s){}
        SMSolverException(const std::string& s): msg(s){}
        SMSolverException(const char* s):        msg(s){}
        virtual const char* what() const noexcept {return msg.c_str();}
};

}
#endif
