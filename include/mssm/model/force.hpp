#if !defined(FORCR_HPP)
#define FORCR_HPP

#include "unit.hpp"
#include "../utils/exception.hpp"

#define UNIT_IS_POINT_LOAD(u) \
    (strncmp(u._unit_, "\1\1\376\0\0\0\0\0", NUMBER_OF_UNIT) == 0)
#define UNIT_IS_LINE_DISTRIBUTED_LOAD(u) \
    (strncmp(u._unit_, "\0\1\376\0\0\0\0\0", NUMBER_OF_UNIT) == 0)

namespace SMSolver {

template<typename DT>
struct DistributedLoad {
    basic_unit<DT> value;
    DT direction; // [0, 2pi)
};

template<typename DT>
struct PointLoad {
    basic_unit<DT> value;
    DT direction; // [0, 2pi)
    DT percentage; // [0, 1]
};

template<typename DT>
class BasicLoad  //{
{
    public:
    bool is_point_load;
    union _load {
        PointLoad<DT> m_point_load;
        DistributedLoad<DT> m_distributed_load;
        _load(){}
        ~_load(){}
        _load& operator=(const _load& o){
            ::memcpy((void*)this, (void*)&o, sizeof(decltype(*this)));
            return(*this);
        }
    } load;

    public:
    static BasicLoad<DT> ZERO_LOAD;
    BasicLoad(bool _is_point_load, basic_unit<DT> load, 
            DT direction = 0, DT percentage = 0.5)
    {
        if(_is_point_load){
            is_point_load = _is_point_load;
            new(&this->load.m_point_load) PointLoad<DT>;
            if(load == 0.0){
                this->load.m_point_load.value = 0;
                this->load.m_point_load.value.setUnit(1, 1, -2);
            } else {
                if(!UNIT_IS_POINT_LOAD(load))
                    throw *new SMSolverException("BasicLoad(): Except a point load, but not.");
                this->load.m_point_load.value = load;
            }
            this->load.m_point_load.direction  = direction;
            this->load.m_point_load.percentage = percentage;
        } else {
            is_point_load = false;
            new(&this->load.m_distributed_load) DistributedLoad<DT>;
            if(load == 0.0){
                this->load.m_distributed_load.value = 0;
                this->load.m_distributed_load.value.setUnit(1, 0, -2);
            } else {
                if(!UNIT_IS_LINE_DISTRIBUTED_LOAD(load))
                    throw *new SMSolverException("BasicLoad(): Except a distributed load, but not.");
                this->load.m_distributed_load.value = load;
            }
            this->load.m_distributed_load.direction = direction;
        }
    }
    BasicLoad(const BasicLoad<DT>&  o){(*this) = o;}
    BasicLoad(const BasicLoad<DT>&& o){(*this) = static_cast<const BasicLoad<DT>&&>(o);}
    BasicLoad& operator=(const BasicLoad<DT>&  o){(*this)=static_cast<const BasicLoad<DT>&&>(o); return (*this);}
    BasicLoad& operator=(const BasicLoad<DT>&& o){
        this->is_point_load = o.is_point_load;
        this->load = o.load;
        return (*this);
    }
    ~BasicLoad(){
        /* why this is redundancy ?
        if(is_point_load)
            this->load.m_point_load.~PointLoad();
        else
            this->load.m_distributed_load.~DistributedLoad();
        */
    }

    bool isZero(){
        if(this->is_point_load)
            return this->load.m_point_load.value == 0.0;
        else 
            return this->load.m_distributed_load.value == 0.0;
    }
}; //}

template<typename DT>
BasicLoad<DT> BasicLoad<DT>::ZERO_LOAD(true, 0);

extern template class BasicLoad<double>;

}
#endif
