#if !defined(UNIT_HPP)
#define UNIT_HPP

#include<iostream>
#include<iosfwd>
#include<sstream>

#include<cstdlib>
#include<cstring>

#include "../matrix/matrix.hpp"
#include "../utils/exception.hpp"

namespace SMSolver {

#define INC_LEN(O) (O->_unit_[0]++)
#define DEC_LEN(O) (O->_unit_[0]--)
#define INC_MAS(O) (O->_unit_[1]++)
#define DEC_MAS(O) (O->_unit_[1]--)
#define INC_SEC(O) (O->_unit_[2]++)
#define DEC_SEC(O) (O->_unit_[2]--)

#define NUMBER_OF_UNIT 8

#define ADD_UNIT_TO(x, y) {\
        for(size_t i = 0;i<NUMBER_OF_UNIT;i++){\
            y._unit_[i] += x._unit_[i];\
        }\
    }

#define MINUS_UNIT_TO(x, y) {\
        for(size_t i = 0;i<NUMBER_OF_UNIT;i++){\
            y._unit_[i] -= x._unit_[i];\
        }\
    }

#define UNIT_CONFORMITY(a, b) \
    if(::strncmp(a._unit_, b._unit_, NUMBER_OF_UNIT - 1) != 0)\
        throw std::logic_error("operation on unconformable unit is illegal.");

//#define INIT_UNIT //{
#define INIT_UNIT(name, inc_f, rep, rt) \
    template<typename DT> \
    class name: public basic_unit<DT> \
    { \
        private: \
            void name_init() \
            {inc_f(this);} \
 \
        public: \
            name(): basic_unit<DT>(){ \
                this->initialize(0, rt, rep); \
                name_init(); \
            } \
            name(const basic_unit<DT>&  x){(*this) = x;} \
            name(const basic_unit<DT>&& x){(*this) = static_cast<const name<DT>&&>(x);} \
            name(const DT& x): basic_unit<DT>(){ \
                this->initialize(x, rt, rep); \
                name_init(); \
            } \
    }; //} #define INIT_UNIT

    /*  **BUG CODE, why? ADD TO ABOVE MACRO INIT_UNIT**
            basic_unit<DT>& operator=(const basic_unit<DT>&  x) override{ \
                basic_unit<DT>::operator=(x); \
                return (*this); \
            } \
            basic_unit<DT>& operator=(const basic_unit<DT>&& x) override{ \
                basic_unit<DT>::operator=(static_cast<const basic_unit<DT>&&>(x)); \
                return (*this); \
            } \
    */

    template<typename DT>
    class basic_unit //{
    {
        public:
            DT value = 0;
            DT ratio = 1;
            std::string Rep;
            char _unit_[NUMBER_OF_UNIT]; //[ LENGTH, MASS, TIME, ELECTROMAGNETISM, 
                                         //  TEPMERATURE, LUMINOUS INTENSITY, MOLE ]
            void setUnit(char Length = 0, char Mass = 0, char Time = 0, 
                    char ELECTROMAGNETISM = 0, char TEMPTERATURE = 0,
                    char LUMINOUS_INTENSITY = 0, char MOLE = 0){
                _unit_[0] = Length;
                _unit_[1] = Mass;
                _unit_[2] = Time;
                _unit_[3] = ELECTROMAGNETISM;
                _unit_[4] = TEMPTERATURE;
                _unit_[5] = LUMINOUS_INTENSITY;
                _unit_[6] = MOLE;
            }
        protected:
            template<typename X>
            friend std::ostream& operator<<(std::ostream& o, basic_unit<X>& u);
            template<typename X>
            friend std::ostream& operator<<(std::ostream& o, basic_unit<X>&& u);

            void initialize(const DT& v, const DT& r, const std::string& rp)
            {
                this->value = v;
                this->ratio = r;
                this->Rep   = rp;
            }

        public:
            typedef DT dataType;
            constexpr basic_unit():value(1), ratio(1), Rep(""){memset(this->_unit_, '\0', NUMBER_OF_UNIT);};
//            basic_unit() = default;

            basic_unit(const dataType& x): Rep(""){ //{
                memset(this->_unit_, '\0', NUMBER_OF_UNIT);
                this->value = x;
                this->ratio = 1;
            } //} convertion constructor
            constexpr basic_unit(const dataType& x,
                                 const dataType& y,
                                 const std::string& z):value(x), ratio(y), Rep(z)
            {memset(this->_unit_, '\0', NUMBER_OF_UNIT);}

            basic_unit(const basic_unit&  x){(*this) = static_cast<const basic_unit& >(x);}
            basic_unit(const basic_unit&& x){(*this) = static_cast<const basic_unit&&>(x);}
            virtual basic_unit<DT>& operator=(const basic_unit& x){ //{ copy assignment
                if(this == &x) return (*this);
                this->value = x.value;
                this->ratio = x.ratio;
                this->Rep = x.Rep;
                memcpy(this->_unit_, x._unit_, NUMBER_OF_UNIT);
                return (*this);
            } //} copy assignment
            virtual basic_unit<DT>& operator=(const basic_unit&& x){ //{ move assignment
                if(this == &x) return (*this);
                this->value = x.value;
                this->ratio = x.ratio;
                this->Rep = x.Rep;
                memcpy(this->_unit_, x._unit_, NUMBER_OF_UNIT);
                return (*this);
            } //} move assignment

            virtual basic_unit<DT>& operator=(const DT& x){this->value = x; return (*this);}

            virtual DT toStandard(const DT&){return this->value * this->ratio;}
            virtual void fromStandard(const basic_unit<DT>& x){this->value = x.ratio * x.value;}

            // conversion operator
            inline operator DT(){return this->value * this->ratio;}

            virtual std::string presentString() const{ //{
                std::ostringstream ss;
                ss << std::setprecision(2) << this->value;
                std::string s = ss.str();
                return (s + this->Rep);
            } //} presentString();
    }; //} end class template basic_unit<T> 

// define some basic unit

template<typename DT>
struct BasicUnit  //{
{
    basic_unit<DT> m;
    basic_unit<DT> mm;
    basic_unit<DT> kg;
    basic_unit<DT> s;
    basic_unit<DT> g;
    basic_unit<DT> N;
    basic_unit<DT> KN;
    basic_unit<DT> Pa;
    basic_unit<DT> KPa;
    BasicUnit(){
        basic_unit<DT> __t(1, 1, "temp");
        m = __t; kg = __t; s = __t;
        m .setUnit(1);         m.Rep = "m";
        mm = 0.001 * m;      mm.Rep = "mm";
        kg.setUnit(0, 1);     kg.Rep = "kg";
        g  = 0.001 * kg;      g.Rep = "g";
        s .setUnit(0, 0, 1);   s.Rep = "s";
        N = kg * m / (s * s);  N.Rep = "N";
        KN = 1000.0 * N;      KN.Rep = "KN";
        Pa = N / (m * m);     Pa.Rep = "Pa";
        KPa = 1000.0 * Pa;   KPa.Rep = "Kpa";
    }
};
const BasicUnit<double> BUnit;
//} basic unit

// non-member function operator for basic_unit<T> class template //{
    template<typename DT>
        bool operator==(const basic_unit<DT>& x, const basic_unit<DT>& y){ //{
            UNIT_CONFORMITY(x, y);
            return (y.ratio * y.value == x->ratio * x->value);
        } //} operator==(const basic_unit<DT>&, const basic_unit<DT>&);
    template<typename DT>
        bool operator==(const basic_unit<DT>& x, const DT& y){ //{
            return (y == x.ratio * x.value);
        } //} operator==(const basic_unit<DT>&, const DT&);

    template<typename DT>
            basic_unit<DT>& operator+=(basic_unit<DT>& y, const basic_unit<DT>& x){ //{
                if(x == 0.0) return y;
                if(y == 0.0) return (y = x);
                UNIT_CONFORMITY(y, x);
                if(y.Rep == ""){
                    y.Rep = x.Rep;
                    y.value = (y.value * y.ratio + x.value * x.ratio) / x.ratio;
                    y.ratio = x.ratio;
                } else 
                    y.value = (y.value * y.ratio + x.value * x.ratio) / y.ratio;
                return (y);
            } //} operator+=(const basic_unit&);
    template<typename DT>
            basic_unit<DT>& operator-=(basic_unit<DT>& y,const basic_unit<DT>& x){//{
                if(x == 0.0) return y;
                if(y == 0.0) return (y = x);
                UNIT_CONFORMITY((y), x);
                if(y.Rep == ""){
                    y.Rep = x.Rep;
                    y.value = (y.value * y.ratio - x.value * x.ratio) / x.ratio;
                    y.ratio = x.ratio;
                } else 
                    y.value = (y.value * y.ratio - x.value * x.ratio) / y.ratio;
                return (y);
            } //} operator-=(const basic_unit<DT>&);
    template<typename DT>
            basic_unit<DT>& operator*=(basic_unit<DT>& y, const basic_unit<DT>& x){ //{
                ADD_UNIT_TO(x, (y));
                y.value *= x.value;
                y.ratio *= x.ratio;
                if(y.Rep == "" || x.Rep == "")
                    y.Rep += x.Rep;
                else
                    y.Rep += ("*" + x.Rep);
                return (y);
            } //} operator*=(const basic_unit<DT>&);
    template<typename DT>
            basic_unit<DT>& operator/=(basic_unit<DT>& y, const basic_unit<DT>& x){ //{
                MINUS_UNIT_TO(x, (y));
                y.value /= x.value;
                y.ratio /= x.ratio;
                if(y.Rep == "" || x.Rep == "")
                    y.Rep += x.Rep;
                else
                    y.Rep += ("/" + x.Rep);
                return (y);
            } //} operator/=(const basic_unit<DT>&);

    template<typename DT>
            basic_unit<DT> operator* (const DT& y, const basic_unit<DT>& x){ //{
                basic_unit<DT> n(y);
                return operator*=(n, x);
            } //}
    template<typename DT>
            basic_unit<DT> operator/ (const DT& y, const basic_unit<DT>& x){ //{
                basic_unit<DT> n(y);
                return operator/=(n, x);
            } //}
    template<typename DT>
            basic_unit<DT> operator* (const basic_unit<DT>& x, const DT& y){ //{
                basic_unit<DT> n(y);
                return operator*=(n, x);
            } //}
    template<typename DT>
            basic_unit<DT> operator/ (const basic_unit<DT>& x, const DT& y){ //{
                basic_unit<DT> n(y);
                basic_unit<DT> p(x);
                return operator/=(p, n);
            } //}

    template<typename DT>
        basic_unit<DT>  operator/ (const basic_unit<DT>& x, const basic_unit<DT>& y) //{
        {
            basic_unit<DT> o(x);
            o /= y;
            return o;
        } //} operator/(x, y)
    template<typename DT>
        basic_unit<DT>  operator* (const basic_unit<DT>& x, const basic_unit<DT>& y) //{
        {
            basic_unit<DT> o(x);
            o *= y;
            return o;
        } //} operator*(x, y)
    template<typename DT>
        basic_unit<DT>  operator+ (const basic_unit<DT>& x, const basic_unit<DT>& y) //{
        {
            basic_unit<DT> o(x);
            o += y;
            return o;
        } //} operator+(x, y)
    template<typename DT>
        basic_unit<DT>  operator- (const basic_unit<DT>& x, const basic_unit<DT>& y) //{
        {
            basic_unit<DT> o(x);
            o -= y;
            return o;
        } //} operator-(x, y)

    template<typename UT>
    std::ostream& operator<<(std::ostream& o, basic_unit<UT>& u) //{
    {
        if(u.Rep == "")
            o << std::right << u.value;
        else
            o << std::right << u.value << "(" << u.Rep << ")";
        return o;
    } //} operator<<(std::ostream&, basic_unit<UT>&);
    template<typename UT>
    std::ostream& operator<<(std::ostream& o, basic_unit<UT>&& u){o << u; return o;}
//} end non-member function operator for basic_unit<T> class template

    INIT_UNIT(basic_second, INC_SEC, "s", 1);
    INIT_UNIT(basic_mass, INC_MAS, "kg", 1);
    INIT_UNIT(basic_length, INC_LEN, "m", 1);
    INIT_UNIT(length_mm, INC_LEN, "mm", 0.001);

    template class basic_unit<double>;
    template class basic_length<double>;
    template class basic_mass<double>;
    template class basic_second<double>;
}

#endif // UNIT_HPP
