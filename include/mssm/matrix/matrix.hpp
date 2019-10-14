#if !defined(MATRIX_HPP)
#define MATRIX_HPP

#include<cstdlib>
#include<limits>
#include<cmath>
#include<cassert>

#include<vector>
#include<iterator>
#include<iomanip>
#include<sstream>
#include<iostream>

#include "../utils/exception.hpp"

#define GAUSS_ITERATION_DONT_CONVERGE \
    throw std::out_of_range("gauss iteration don't converge...");

#define GAUSS_ITERATIOIN_TOLERENCE 0.0001

#define MAX_GAUSS_ITERATION 2000

#define CONV_SELF_OP(op, x) \
    using t_type = typename ::std::remove_cv<decltype(x)>::type;\
    typename ::std::decay<t_type>::type y(static_cast<t_type&>(*this)); \
    y.operator op(x); \
    return y;

#define MATRIX_OUT_OF_RANGE \
    throw new std::out_of_range("get matrix element out of range")

#define UNMATCH_MATRIX_SIZE \
    throw new std::logic_error("unmatch size of the operand of operator")

#define BTU(i,j) \
    ((i - 1) * this->col_row + (j - 1))

#define mout_of_range(i) \
    (i < 1 || i > this->col_row)

namespace SMSolver {

    // AUXILIARY FUNCTIONS
    template<typename DT> DT xabs(const DT& s)
    {
        if(s >= 0)
            return s;
        else return (-s);
    }

    template<typename DT> DT xsqrt(const DT& s)
    {
        return ::sqrt(s);
    }

    template<typename DT> class cvector;

    // square matrix that use to present the 
    // rigid matrix of structure.
    // DT should implement [DT& operator=(DT&, double)];
    //
    // M[i, j] = (i - 1) * col_row + (j - 1)  , (i, j \belong [1, col_row])
    //  0  1  2  3  4
    //  5  6  7  8  9
    // 10 11 12 13 14
    // 15 16 17 18 19
    // 20 21 22 23 24
    // class template smatrix<T> //{
    template<typename DT>
        class smatrix //{
        {
            public:
                typedef DT dataType;
                // don't forget delete
                std::pair<smatrix<dataType>, smatrix<dataType>> get_LU_test(){return this->get_LU();}

            private:
                friend class cvector<DT>;

                template<typename T1, typename T2>
                friend void mapMatrix(const smatrix<T1>&, smatrix<T2>&, T2 (*map)(const T1&));

                std::pair<smatrix<dataType>, smatrix<dataType>> get_LU();
                cvector<dataType> solver_lower_triangle_linear_equation(const cvector<dataType>& b);
                cvector<dataType> solver_upper_triangle_linear_equation(const cvector<dataType>& b);

                size_t col_row = 0;
                std::vector<dataType>* data = nullptr;
                void update_data()
                {
                    if(this->data == nullptr){
                        this->data = new std::vector<dataType>(col_row * col_row);
                        return;
                    }
                    if(this->data->size() == (col_row^2))
                        return;
                    this->data->resize(col_row ^ 2);
                    return;
                }

            public:
                smatrix(size_t size): col_row(size){update_data();}
                smatrix() = delete;
                smatrix(const smatrix<dataType>&  other);
                smatrix(smatrix<dataType>&& other);

                ~smatrix(){delete this->data;}

                smatrix<dataType>& operator=(const smatrix<dataType>&  other);
                smatrix<dataType>& operator=(smatrix<dataType>&& other);

                inline dataType& get(size_t i, size_t j) const
                {
                    if(i > col_row || j > col_row || j < 1 || i < 1)
                        MATRIX_OUT_OF_RANGE;
                    return (*this->data)[BTU(i,j)];
                }

                inline void set(size_t i, size_t j, const dataType& v)
                {
                    if(i > col_row || j > col_row || j < 1 || i < 1)
                        MATRIX_OUT_OF_RANGE;
                    (*this->data)[BTU(i,j)] = v;
                }

                size_t get_size()
                {
                    return this->col_row;
                }
                void delete_colrow(size_t j);
                void insert_submatrix(const smatrix&, size_t i);
                void merge(size_t, size_t);
                void comAdd(size_t, size_t);
                void swap_col(size_t, size_t);
                void swap_row(size_t, size_t);
                void col_times(size_t, const dataType& x);
                void row_times(size_t, const dataType& x);
                void col_times_add_to(size_t, const dataType& x, size_t);
                void row_times_add_to(size_t, const dataType& x, size_t);
                dataType norm_inf()
                {
                    dataType ret = xabs(data->at(0));
                    for(auto s = this->data->begin(); s!=this->data->end(); ++s){
                        if(ret < xabs(*s))
                            ret = xabs(*s);
                    }
                    return ret;
                }

                smatrix<dataType>& operator+=(const smatrix<dataType>&);
                smatrix<dataType>& operator-=(const smatrix<dataType>&);
                smatrix<dataType>  operator+ (const smatrix<dataType>& oth) const
                {CONV_SELF_OP(+=, oth);}
                smatrix<dataType>  operator- (const smatrix<dataType>& oth) const
                {CONV_SELF_OP(-=, oth);}

                smatrix<dataType>& operator*=(const smatrix<dataType>& oth);
                smatrix<dataType>  operator* (const smatrix<dataType>& op1) const;
                cvector<dataType>  operator* (const cvector<dataType>& x) ;

                std::string toString() const
                {
                    std::ostringstream ss;
                    for(size_t i = 1;i<=this->col_row;i++){
                        for(size_t j = 1;j<=this->col_row;j++){
                            ss << std::setprecision(2) << std::fixed 
                                << std::setw(8) << std::left << this->get(i, j);
                        }
                        ss << std::endl;
                    }
                    return ss.str();
                }

                cvector<dataType> gauss_seidel_solver(const cvector<dataType>&);
                cvector<DT> LU_Decomposition_solver(const cvector<dataType>& b);

                cvector<DT> Guass_Elimination_solver(const cvector<dataType>&);
                std::pair<smatrix<dataType>, smatrix<dataType>> Gauss_Elimination();

                void set(dataType (*)(size_t, size_t));
                void set(dataType (*)(size_t, size_t, const dataType&));
                void set(dataType (*)(size_t, size_t, smatrix<dataType>*));
        }; //} class template smatrix<T> declaration

    // map one matrix to others, require the size of two matrix is equal
    template<typename T1, typename T2>
    void mapMatrix(const smatrix<T1>& A1, smatrix<T2>& A2, T2 (*map)(const T1&)){ //{
        if(A1.col_row != A2.col_row)
            UNMATCH_MATRIX_SIZE;
        for(size_t i = 1; i<=A1.col_row; i++){
            for(size_t j = 1; j<=A1.col_row; j++){
                A2.get(i, j) = map(A1.get(i, j));
            }
        }
    } //}

    template<typename DT>
        cvector<DT> smatrix<DT>::solver_lower_triangle_linear_equation(const cvector<DT>& b){ //{
            cvector<DT> ret(b.len);
            for(size_t i = 1; i<=b.len; i++){
                dataType temp(b[i]);
                temp = 0;
                for(size_t j = 1; j <= i - 1; j++){
                    temp += this->get(i, j) * ret[j];
                }
                ret[i] = (b[i] - temp) / this->get(i, i);
            }
#if defined(LU__DEBUG__)
            std::cout << "##Function solver_lower_triangle_linear_equation():" << std::endl;
            std::cout << "-------------------SolverL:" << std::endl;
            std::cout << this->toString();
            std::cout << "-------------------------y:" << std::endl;
            std::cout << ret.toString();
            std::cout << "-------------------------b:" << std::endl;
            std::cout << b.toString();
            std::cout << "-------------------------Ly - b:" << std::endl;
            std::cout << ((*this)*ret - b).toString();
            std::cout << "##End debug information." <<std::endl;
#endif
            return ret;
        } //}

    template<typename DT>
        cvector<DT> smatrix<DT>::solver_upper_triangle_linear_equation(const cvector<DT>& b){ //{
            cvector<DT> ret(b.len);
            for(size_t i = b.len; i>=1; i--){
                dataType temp(b[i]);
                temp = 0;
                for(size_t j = b.len; j >= i + 1; j--){
                    temp += this->get(i, j) * ret[j];
                }
                ret[i] = (b[i] - temp) / this->get(i, i);
            }
#if defined(LU__DEBUG__)
            std::cout << "##Function solver_upper_triangle_linear_equation():" << std::endl;
            std::cout << "-------------------SolverU:" << std::endl;
            std::cout << this->toString();
            std::cout << "-------------------------x:" << std::endl;
            std::cout << ret.toString();
            std::cout << "-------------------------b:" << std::endl;
            std::cout << b.toString();
            std::cout << "-------------------------Ux - b:" << std::endl;
            std::cout << ((*this)*ret - b).toString();
            std::cout << "##End debug information." <<std::endl;
#endif
            return ret;
        } //}

    /*   A \belong R_{n\times n}, if A can be LU-decomposed, then
     *   assume A = LU, 
     *   base on A[i][j] = \sum_{k = 1}^{min(i, j)}\, L[i][k]\times U[k][j], 
     *   and extra assumption L[i][i] == 1, 
     *   we can get:
     *      L[i][j] = {A[i][j] - \sum_{k = 1}^{j - 1}L[i][k]\times U[k][j]}\over U[j][j]
     *   here (i >  j).
     *      U[j][i] = (A[j][i] - \sum_{k = 1}^{j - 1}L[j][k]\times U[k][i]}\over L[j][j]
     *   here (i >= j).
     */
    template<typename DT>
        std::pair<smatrix<DT>, smatrix<DT>> smatrix<DT>::get_LU(){ //{
            smatrix<DT> Lcom(this->col_row);
            smatrix<DT> Ucom(this->col_row);
            for(size_t j = 1; j<=this->col_row; j++){
                // i == j
                Lcom.set(j, j, 1);
                dataType temp (this->get(j,j)); // in order keep the conformity of unit ...
                temp = 0;
                for(size_t k = 1;k<=j-1;k++)
                    temp += Lcom.get(j, k) * Ucom.get(k, j);
                dataType test = (this->get(j, j) - temp) / Lcom.get(j, j);
                if(test == 0.0)
                    throw "This matrix can't be LU-decomposited.";
                Ucom.set(j, j, test);

                // i >  j
                for(size_t i = j + 1; i<=this->col_row; i++){
                    dataType temp1(this->get(i,j));
                    temp1 = 0;
                    for(size_t k = 1; k<=j-1;k++)
                        temp1 += Lcom.get(i, k) * Ucom.get(k, j);
                    Lcom.set(i, j, (this->get(i, j) - temp1) / Ucom.get(j, j));

                    dataType temp2(this->get(j, i));
                    temp2 = 0;
                    for(size_t k = 1;k<=j-1;k++)
                        temp2 += Lcom.get(j, k) * Ucom.get(k, i);
                    Ucom.set(j, i, (this->get(j, i) - temp2) / Lcom.get(j, j));
                }
            }
            std::pair<smatrix<DT>, smatrix<DT>> 
                ret(static_cast<smatrix<DT>&&>(Lcom), 
                    static_cast<smatrix<DT>&&>(Ucom));
            return ret;
        } //}

    // LU-decomposition method
    template<typename DT>
        cvector<DT> smatrix<DT>::LU_Decomposition_solver(const cvector<dataType>& b) //{
        {
            std::pair<smatrix<dataType>, smatrix<dataType>> LU = this->get_LU();
            smatrix<dataType> Lcom = LU.first;
            smatrix<dataType> Ucom = LU.second;
#if defined(LU__DEBUG__)
            std::cout << "--------------------- L:" << std::endl;
            std::cout << Lcom.toString();
            std::cout << "--------------------- U:" << std::endl;
            std::cout << Ucom.toString();
            std::cout << "--------------------- A:" << std::endl;
            std::cout << this->toString();
            std::cout << "--------------------- LU - A:" << std::endl;
            std::cout << (Lcom * Ucom - (*this)).toString() << std::endl;
#endif // __DEBUG__
            cvector<DT> y = Lcom.solver_lower_triangle_linear_equation(b);
            cvector<DT> x = Ucom.solver_upper_triangle_linear_equation(y);
            return x;
        } //}

    // preliminary transform //{
    template<typename DT>
    void smatrix<DT>::col_times(size_t j, const dataType& x)
    {
        for(size_t i = 1; i<=this->col_row; i++)
            this->get(i, j) *= x;
        return;
    }
    template<typename DT>
    void smatrix<DT>::row_times(size_t i, const dataType& x)
    {
        for(size_t j = 1; j<=this->col_row; j++)
            this->get(i, j) *= x;
        return;
    }
    template<typename DT>
    void smatrix<DT>::col_times_add_to(size_t j1, const dataType& x, size_t j2){
        for(size_t i = 1; i<=this->col_row; i++)
            this->get(i, j2) += this->get(i, j1) * x;
        return;
    }
    template<typename DT>
    void smatrix<DT>::row_times_add_to(size_t i1, const dataType& x, size_t i2)
    {
        for(size_t j = 1; j<=this->col_row; j++)
            this->get(i2, j) += this->get(i1, j) * x;
        return;
    }
    //}

    // Gauss elimination method
    template<typename DT> //{
    cvector<DT> smatrix<DT>::Guass_Elimination_solver(const cvector<dataType>& b)
    {
        std::pair<smatrix<dataType>, smatrix<dataType>> gauss_elimi = this->Gauss_Elimination();
        cvector<dataType> new_b = gauss_elimi.second * b;
        smatrix<dataType>& g__  = gauss_elimi.first;
        cvector<DT> result      = g__.solver_upper_triangle_linear_equation(new_b);
        return result;
    }
    template<typename DT>
    std::pair<smatrix<DT>, smatrix<DT>> smatrix<DT>::Gauss_Elimination()
    {
        smatrix<dataType> transf(this->col_row);
        smatrix<dataType> current_copy(*this);
        transf.set([](size_t i, size_t j) -> dataType {if(i == j) return 1; else return 0;});
        for(size_t i = 1; i<=this->col_row; i++){
            size_t ix = i;
            for(; ix<=this->col_row; ix++){
                if(current_copy.get(ix, ix) != 0){
                    if(ix != i){
                        current_copy.swap_row(ix, i);
                        transf.swap_row(ix, i);
                    }
                    break;
                }
            }
            if(ix == this->col_row && current_copy.get(ix, ix) == 0){
                throw *new SMSolverException("Not invertible matrix");
            }
            for(size_t i2 = i+1; i2<=this->col_row; i2++){
                dataType factor2 = -current_copy.get(i2, i) / current_copy.get(i, i);
                current_copy.row_times_add_to(i, factor2, i2);
                transf.row_times_add_to(i, factor2, i2);
            }
        }
        return std::pair<smatrix<dataType>, smatrix<dataType>>(std::move(current_copy), std::move(transf));
    } //}

    // give up ... gauss-seidel-method
    template<typename DT>
        cvector<DT> smatrix<DT>::gauss_seidel_solver(const cvector<dataType>& b) //{
        {
            if(this->col_row != b.len)
                UNMATCH_MATRIX_SIZE;
            dataType inf_norm = 1.2 * this->norm_inf();
            smatrix<dataType> Q_m(this->col_row);
            Q_m.set([](size_t, size_t){return 0.0;});
            smatrix<dataType> I(this->col_row);
            I.set([](size_t i, size_t j){if(i == j) return 1.0; return 0.0;});
            for(size_t i = 1;i<=this->col_row;i++){
                if(this->get(i, i) > 0)
                    Q_m.set(i, i, 1 / inf_norm);
                else 
                    Q_m.set(i, i, -1 / inf_norm);
            }
            smatrix<dataType> S = I - Q_m * (*this);
            std::cout << S.toString();
            cvector<dataType> b_2 = Q_m * b;
            cvector<dataType> x(b_2);
            const dataType b_abs = b.norm();
            /*
            // x_{n+1} = D_m1 * b - D_m1 * B * x_{n}
            // initilize x_{0} to b
            smatrix<dataType> D_m1(*this);
            smatrix<dataType> B(*this);
            D_m1.set([](size_t i, size_t j, const dataType& x){
            if(i != j) return 0.0;
            if(x == 0) throw "unexcepted zero in diagonal of square matrix.";
            return (1.0 / x);
            });
            B.set([](size_t i, size_t j, const dataType& x){
            if(i == j) return 0.0;
            return x;
            });
            cvector<dataType> q = D_m1 * b;
            smatrix<dataType> V = D_m1 * B;
            */
            dataType x_holder;
            cvector<dataType> test(this->col_row);
            for(size_t iter_l = 0;iter_l<MAX_GAUSS_ITERATION; iter_l++){
                for(size_t i = 1; i<=this->col_row; i++){
                    x_holder = 0;
                    for(size_t j = 1; j<=this->col_row; j++){
                        x_holder += S.get(i, j) * x[j];
                    }
                    x[i] = b_2[i] + x_holder;
                }
                test = (*this) * x - b;
                if(test.norm() < GAUSS_ITERATIOIN_TOLERENCE * b_abs){
                    std::cout << test.norm() << std::endl;
                    return x;
                }
            }
            if(test.norm() > GAUSS_ITERATIOIN_TOLERENCE * b_abs){
                std::cout << "Bad Tolerence is " << test.norm() << std::endl;
                GAUSS_ITERATION_DONT_CONVERGE;
            }
            std::cout << "Accepted Tolerence is " << test.norm() << std::endl;
            return x;
        } //}

template<typename DT>
        void smatrix<DT>::set(dataType (*cal_func)(size_t, size_t)) //{
        {
            for(size_t i = 1; i<=this->col_row;i++){
                for(size_t j = 1; j<=this->col_row;j++){
                    this->set(i, j, cal_func(i, j));
                }
            }
        }//}

    template<typename DT>
        void smatrix<DT>::set(dataType (*cal_func)(size_t, size_t, const dataType&)) //{
        {
            for(size_t i = 1; i<=this->col_row;i++){
                for(size_t j = 1; j<=this->col_row;j++){
                    this->set(i, j, cal_func(i, j, this->get(i, j)));
                }
            }
        }//}

    template<typename DT>
        void smatrix<DT>::set(dataType (*cal_func)(size_t, size_t, smatrix<dataType>*)) //{
        {
            for(size_t i = 1; i<=this->col_row;i++){
                for(size_t j = 1; j<=this->col_row;j++){
                    this->set(i, j, cal_func(i, j, this));
                }
            }
        }//}

    template<typename DT>
        void smatrix<DT>::delete_colrow(size_t j) //{
        {
            if(j > col_row || j < 1)
                MATRIX_OUT_OF_RANGE;
            std::vector<DT>* new_data = new std::vector<DT>();
            for(size_t m = 1; m<=this->col_row; m++){
                if(m == j)
                    continue;
                for(size_t n = 1; n<=this->col_row; n++){
                    if(n == j)
                        continue;
                    new_data->push_back(this->get(m, n));
                }
            }
            delete this->data;
            this->data = new_data;
            this->col_row--;
            return;
        } //}

    template<typename DT>
        void smatrix<DT>::insert_submatrix(const smatrix<DT>& sm, size_t i) //{
        {
            if(i < 1 || (i + sm.col_row - 1) > this->col_row)
                MATRIX_OUT_OF_RANGE;
            if(sm.col_row < 1)
                throw "unexcepted square matrix size 0.";
            for(size_t m = 1; m<=sm.col_row;  m++)
                for(size_t n = 1;n<=sm.col_row; n++)
                    set(m + i - 1, n + i - 1, sm.get(m, n));
            return;
        } //}

    template<typename DT>
        void smatrix<DT>::swap_col(size_t i, size_t j) //{
        {
            if(mout_of_range(i) || mout_of_range(j))
                MATRIX_OUT_OF_RANGE;
            dataType tmp;
            for(size_t m = 1;m<=this->col_row;m++){
                tmp = this->get(m, i);
                this->set(m, i, this->get(m, j));
                this->set(m, j, tmp);
            }
            return;
        } //}

    template<typename DT>
        void smatrix<DT>::swap_row(size_t i, size_t j) //{
        {
            if(mout_of_range(i) || mout_of_range(j))
                MATRIX_OUT_OF_RANGE;
            dataType tmp;
            for(size_t m = 1;m<=this->col_row;m++){
                tmp = this->get(i, m);
                this->set(i, m, this->get(j, m));
                this->set(j, m, tmp);
            }
            return;
        } //}

    template<typename DT>
        void smatrix<DT>::merge(size_t i, size_t j) //{
        {
            if(mout_of_range(i) || mout_of_range(j))
                MATRIX_OUT_OF_RANGE;
            if(i == j)
                throw "required unequal merge operand.";
            dataType tmp1, tmp2;
            for(size_t m = 1;m<=this->col_row;m++){
                tmp1 = this->get(m, i);
                tmp2 = this->get(m, j);
                this->set(m, i, tmp1 + tmp2);

                tmp1 = this->get(i, m);
                tmp2 = this->get(j, m);
                this->set(i, m, tmp1 + tmp2);
            }
            this->delete_colrow(j);
            return;
        } //}

    template<typename DT>
        void smatrix<DT>::comAdd(size_t i, size_t j) //{
        {
            if(mout_of_range(i) || mout_of_range(j))
                MATRIX_OUT_OF_RANGE;
            if(i == j)
                throw "required unequal comAdd operand.";
            if(i < j)
                this->get(i, i) += this->get(j, j);
            else 
                this->get(i, i) -= this->get(j, j);

            for(size_t m = 1;m<=this->col_row;m++){
                this->get(m, i) += this->get(m, j);
                this->get(i, m) += this->get(j, m);
            }
            return;
        } //}

    template<typename DT>
        smatrix<DT>& smatrix<DT>::operator*=(const smatrix<dataType>& oth) //{
        {
            smatrix<DT> holder(this->col_row);
            holder = (*this) * oth;
            (*this) = static_cast<smatrix<DT>&&>(holder);
            return(*this);
        } //}

    template<typename DT>
        smatrix<DT>  smatrix<DT>::operator* (const smatrix<dataType>& oth) const //{
        {
            if(this->col_row != oth.col_row)
                UNMATCH_MATRIX_SIZE;
            smatrix<DT> result(this->col_row);
            for(size_t i = 1; i<=this->col_row; i++){
                for(size_t j = 1; j<=this->col_row; j++){
                    DT val(this->get(i, 1) * oth.get(1, j));
                    val = 0;
                    for(size_t k = 1; k<=this->col_row; k++){
                        val += this->get(i, k) * oth.get(k, j);
                    }
                    result.set(i, j, val);
                }
            }
            return result;
        } //}

    template<typename DT>
        smatrix<DT>::smatrix(const smatrix<dataType>&  other) //{
        {
            (*this) = static_cast<const smatrix<dataType>&>(other);
        } //}

    template<typename DT>
        smatrix<DT>::smatrix(smatrix<dataType>&& other) //{
        {
            (*this) = static_cast<smatrix<dataType>&&>(other);
        } //}

    template<typename DT>
        smatrix<DT>& smatrix<DT>::operator=(const smatrix<dataType>&  other) //{
        {
            if(this == &other) return(*this);
            delete this->data;
            this->col_row = other.col_row;
            this->data    = new std::vector<DT>(static_cast<std::vector<DT>&>(*other.data));
            return (*this);
        } //}

    template<typename DT>
        smatrix<DT>& smatrix<DT>::operator=(smatrix<dataType>&& other) //{
        {
            if(this == &other) return(*this);
            delete this->data;
            this->col_row = other.col_row;
            this->data    = other.data;
            other.data = nullptr;
            return (*this);
        } //} 

    template<typename DT>
        cvector<DT> smatrix<DT>::operator* (const cvector<DT>& cv)//{
        {
            if(cv.len != this->col_row)
                UNMATCH_MATRIX_SIZE;
            cvector<DT> ret(this->col_row); 
            for(size_t i = 1; i<=this->col_row; i++){
                dataType val(this->get(i, 1) * cv[1]);
                val = 0.0;
                for(size_t j = 1; j<=this->col_row; j++){
                    val += this->get(i, j) * cv[j];
                }
                ret[i] = val;
            }
            return ret;
        } //}

    template<typename DT>
        smatrix<DT>& smatrix<DT>::operator-=(const smatrix<DT>& oth)//{
        {
            if(this->col_row != oth.col_row)
                UNMATCH_MATRIX_SIZE;
            dataType t1, t2;
            for(size_t i = 1;i<=this->col_row;i++){
                for(size_t j = 1;j<=this->col_row;j++){
                    t1 = this->get(i, j);
                    t2 = oth.get(i, j);
                    this->set(i, j, t1 - t2);
                }
            }
            return (*this);
        } //}

    template<typename DT>
        smatrix<DT>& smatrix<DT>::operator+=(const smatrix<DT>& oth)//{
        {
            if(this->col_row != oth.col_row)
                UNMATCH_MATRIX_SIZE;
            dataType t1, t2;
            for(size_t i = 1;i<=this->col_row;i++){
                for(size_t j = 1;j<=this->col_row;j++){
                    t1 = this->get(i, j);
                    t2 = oth.get(i, j);
                    this->set(i, j, t1 + t2);
                }
            }
            return (*this);
        } //}

    //} class smatrix

    // class template cvector<T> //{
    template<typename DT>
        class cvector //{
        {
            public:
                typedef DT dataType;
            private:
                friend class smatrix<dataType>;

                std::vector<DT>* data = nullptr;
                size_t      len  = 0;
                void update_data()
                {
                    if(this->data == nullptr){
                        this->data = new std::vector<dataType>(len);
                        return;
                    }
                    if(this->data->size() == (len))
                        return;
                    this->data->resize(len);
                    return;
                }

            public:
                cvector() = delete;
                ~cvector() {delete this->data;}
                cvector(size_t len): len(len){update_data();}
                cvector(const cvector<DT>&);
                cvector(cvector<DT>&&);
                cvector<DT>& operator=(const cvector<DT>&);
                cvector<DT>& operator=(cvector<DT>&&);
                cvector<DT>& operator+=(const cvector<DT>&);
                cvector<DT>& operator-=(const cvector<DT>&);
                cvector<DT>  operator- (const cvector<DT>& x) const {CONV_SELF_OP(-=, x);}
                cvector<DT>  operator+ (const cvector<DT>& x) const {CONV_SELF_OP(+=, x);}

                size_t get_size(){return this->len;}

                inline
                    dataType get(size_t i) const
                    {
                        if(i < 1 || i > this->len)
                            MATRIX_OUT_OF_RANGE;
                        return (*this->data)[i - 1];
                    }
                inline
                    void set(size_t i, const dataType& d) const
                    {
                        if(i < 1 || i > this->len)
                            MATRIX_OUT_OF_RANGE;
                        (*this->data)[i - 1] = d;
                    }

                dataType norm() const noexcept
                {
                    dataType ret = 0;
                    for(size_t i = 1; i<=this->len; i++)
                    {
                        ret += (*this)[i] * (*this)[i];
                    }
                    return xsqrt(ret);
                }

                inline dataType& operator[](size_t i) const
                {
                    assert(i > 0 && i <= this->len);
                    return (*this->data)[i - 1];
                };

                std::string toString() const
                {
                    std::ostringstream ss;
                    ss << "(";
                    for(size_t i = 1;i<=this->len - 1;i++){
                        ss << std::setprecision(2) << std::fixed 
                            << std::left << this->get(i) << ", ";
                    }
                    ss << this->get(this->len) << ")";
                    ss << std::endl;
                    return ss.str();
                }
                void set(dataType (*)(size_t));
                void set(dataType (*)(size_t, const dataType&));

                void delete_elem(size_t);
        }; //} class template cvector<T> declaration

    template<typename DT>
    void cvector<DT>::delete_elem(size_t i){
        if(i > this->len || i == 0)
            throw *new SMSolverException("cvector<DT>::delete_elem(size_t i): require <i>"
                                         "is less equal with length of cvecotr, and greater than 0.");
        this->data->erase(this->data->begin() + (i - 1));
        this->len-- ;
        return;
    }

    template<typename DT>
        void cvector<DT>::set(dataType (*cal_func)(size_t)) //{
        {
            for(size_t i = 1; i<=this->len;i++){
                    (*this)[i] = cal_func(i);
                }
        } //}

    template<typename DT>
        void cvector<DT>::set(dataType (*cal_func)(size_t, const dataType&)) //{
        {
            for(size_t i = 1; i<=this->len;i++){
                    (*this)[i] = cal_func(i, (*this)[i]);
                }
        } //}

    template<typename DT>
        cvector<DT>::cvector(const cvector<DT>& oth) //{
        {
            (*this) = static_cast<const cvector<DT>&>(oth);
        } //}
    template<typename DT>
        cvector<DT>::cvector(cvector<DT>&& oth) //{
        {
            (*this) = static_cast<cvector<DT>&&>(oth);
        } //}

    template<typename DT>
    cvector<DT>& cvector<DT>::operator+=(const cvector<DT>& o) //{
    {
        if(o.len != this->len)
            UNMATCH_MATRIX_SIZE;
        for(size_t i = 1; i<=this->len; i++)
            (*this)[i] += o[i];
        return (*this);
    } //}
    template<typename DT>
    cvector<DT>& cvector<DT>::operator-=(const cvector<DT>& o) //{
    {
        if(o.len != this->len)
            UNMATCH_MATRIX_SIZE;
        for(size_t i = 1; i<=this->len; i++)
            (*this)[i] -= o[i];
        return (*this);
    } //}

    template<typename DT>
        cvector<DT>& cvector<DT>::operator=(const cvector<DT>& other) //{
        {
            if(this == &other) return(*this);
            this->len = other.len;
            this->data    = new std::vector<DT>(static_cast<std::vector<DT>&>(*other.data));
            return (*this);
        } //}
    template<typename DT>
        cvector<DT>& cvector<DT>::operator=(cvector<DT>&& other) //{
        {
            if(this == &other) return(*this);
            delete this->data;
            this->len = other.len;
            this->data    = other.data;
            other.data = nullptr;
            return (*this);
        } //}
    //} class cvector
}

extern template class SMSolver::smatrix<double>;
extern template class SMSolver::cvector<double>;

#endif // MATRIX_HPP
