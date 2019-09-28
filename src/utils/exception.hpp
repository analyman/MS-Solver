#if !defined(SMSOLVER_EXCEPTION_HPP)
#define SMSOLVER_EXCEPTION_HPP

#include<exception>
#include<string>

namespace SMSolver {

struct SMSolverException: std::exception{
    private:
        const char* msg;
    public:
        SMSolverException(std::string& s):msg(s.c_str()){}
        SMSolverException(const char* s):msg(s){}
        virtual const char* what() const noexcept {return msg;}
};

}
#endif
