#if !defined(MODEL_HPP)
#define MODEL_HPP

#include<vector>
#include<stack>
#include<algorithm>

#include<cstdlib>

#include<cmath>

#include "./unit.hpp"
#include "../utils/exception.hpp"
#include "force.hpp"

#define UNIT_IS_E(s) \
    (strncmp(s._unit_, "\377\1\376\0\0\0\0\0", NUMBER_OF_UNIT) == 0)
#define UNIT_IS_I(s) \
    (strncmp(s._unit_, "\4\0\0\0\0\0\0\0", NUMBER_OF_UNIT) == 0)
#define UNIT_IS_A(s) \
    (strncmp(s._unit_, "\2\0\0\0\0\0\0\0", NUMBER_OF_UNIT) == 0)
#define UNIT_IS_K(s) \
    (strncmp(s._unit_, "\3\1\377\0\0\0\0\0", NUMBER_OF_UNIT) == 0)

namespace SMSolver{
    template<typename DT> class  SMSolverManager;
    template<typename DT> class  Beam;

    // other type of support can be simulated by properly beam
    // and basic support formation
    enum BasicSupport {FixedSupport, HingeSupport, NoneSupport};
    enum Connector {FixedConnect, HingeConnect, ElasticHingeConnect};

    typedef u_int64_t NodeID;
    typedef u_int64_t BeamID;

    /*
     * Node, basicly need contain location information.
     * And for convenience traversing the connected beam,
     * node should contain a vector to store these beam's pointer.
     */
    template<typename DT>
    class Node { //{
        public:
            typedef basic_length<DT> length_type;
        private:
            BasicSupport _support = NoneSupport;
            length_type _x;
            length_type _y;
            std::vector<Beam<DT>*> m_connected_beams;
            NodeID m_id;
            BasicLoad<DT> m_load;
        public:
            SMSolverManager<DT>* solverManager;

            Node() = delete;
            void add_connected(Beam<DT>* b){
                m_connected_beams.push_back(b);
            }
            auto beams_begin(){
                return m_connected_beams.begin();
            }
            auto beams_end(){
                return m_connected_beams.end();
            }
            void setSupport(const BasicSupport& sp){this->_support = sp;}
            void setLoad(const BasicLoad<DT>& load){
                if(!load.is_point_load)
                    throw *new SMSolverException("Load for a Node must be a point load.");
                this->m_load = load;
                return;
            }
            cvector<basic_unit<DT>> getTheP(){
                cvector<basic_unit<DT>> ret(2);
                ret[0] = m_load.load.m_point_load.value * ::cos(m_load.load.m_point_load.direction);
                ret[1] = m_load.load.m_point_load.value * ::sin(m_load.load.m_point_load.direction);
                return ret;
            }

        private:
            friend class SMSolverManager<DT>;
            friend class Beam<DT>;
            explicit Node(SMSolverManager<DT>* mg, length_type& x, length_type& y, NodeID id): 
                _x(x), _y(y), 
                m_id(id),
                m_load(BasicLoad<DT>::ZERO_LOAD),
                solverManager(mg)
            {}
            Node(Node<DT>&) = delete;
            Node(Node<DT>&&) = delete;
            Node& operator=(Node<DT>& ) = delete;
            Node& operator=(Node<DT>&&) = delete;
            ~Node() = default;
    }; //} class template Node<T>

    template<typename DT>
    class Beam { //{
        public:
            SMSolverManager<DT>* solverManager;
        private:
            friend class SMSolverManager<DT>;
            std::tuple<NodeID, Connector, basic_unit<DT>> m_firstNode, m_secondNode;
            BeamID m_id;
            basic_unit<DT> _E;
            basic_unit<DT> _I;
            basic_unit<DT> _A;
            BasicLoad<DT> m_load;

            struct __beam_static_contructor {
                cvector<basic_unit<DT>> ZERO_REACTION_VECTOR;
                cvector<DT>             ZERO_REACTION_VECTOR_METRIC;
                __beam_static_contructor(): ZERO_REACTION_VECTOR(6), ZERO_REACTION_VECTOR_METRIC(6){
                    basic_unit<DT> __v = 0;
                    __v.setUnit(1, 1, -2);
                    ZERO_REACTION_VECTOR[1] = __v;
                    ZERO_REACTION_VECTOR[2] = __v;
                    ZERO_REACTION_VECTOR[4] = __v;
                    ZERO_REACTION_VECTOR[5] = __v;
                    __v.setUnit(2, 1, -2);
                    ZERO_REACTION_VECTOR[3] = __v;
                    ZERO_REACTION_VECTOR[6] = __v;

                    ZERO_REACTION_VECTOR_METRIC.set([](size_t) -> DT {return 0;});
                }
            };
            static __beam_static_contructor static_member;

            void check_UNIT(){
                if(std::get<1>(this->m_firstNode) == Connector::ElasticHingeConnect &&
                    !UNIT_IS_K(std::get<2>(this->m_firstNode)))
                    throw *new SMSolverException("unmatch unit to elastic gradient K.");
                if(std::get<1>(this->m_secondNode) == Connector::ElasticHingeConnect &&
                    !UNIT_IS_K(std::get<2>(this->m_secondNode)))
                    throw *new SMSolverException("unmatch unit to elastic gradient K.");
                if(!(UNIT_IS_A(this->_A) && UNIT_IS_E(this->_E) && UNIT_IS_I(this->_I)))
                    throw *new SMSolverException("unmatch unit to A or E or I.");
            }

            DT getTheta(){
                Node<DT>* f_node = this->solverManager->getNode(std::get<0>(m_firstNode ));
                Node<DT>* s_node = this->solverManager->getNode(std::get<0>(m_secondNode));
                basic_unit<DT> ratio = (s_node->_y - f_node->_y) / (s_node->_x - f_node->_x);
                DT theta = atan(ratio.value * ratio.ratio);
                return theta;
            }

            DT getLength(){
                Node<DT>* f_node = this->solverManager->getNode(std::get<0>(m_firstNode ));
                Node<DT>* s_node = this->solverManager->getNode(std::get<0>(m_secondNode));
                basic_unit<DT> y = s_node->_y - f_node->_y;
                basic_unit<DT> x = s_node->_x - f_node->_x;
                DT d = ::sqrt(::pow(y.value * y.ratio, 2) + ::pow(x.value * x.ratio, 2));
                return d;
            }

        public:
            Beam() = delete;
            Beam(Beam<DT>& ) = delete;
            Beam(Beam<DT>&&) = delete;
            Beam& operator=(Beam<DT>& ) = delete;
            Beam& operator=(Beam<DT>&&) = delete;

            explicit Beam(SMSolverManager<DT>* mg, const NodeID& i_f, const NodeID& i_s,
                    const Connector& c_f, const Connector& c_s,
                    const BeamID& id,
                    const basic_unit<DT>& b_E, 
                    const basic_unit<DT>& b_A, 
                    const basic_unit<DT>& b_I,
                    const BasicLoad<DT>& load = BasicLoad<DT>::ZERO_LOAD
                ): 
                solverManager(mg),
                m_firstNode (std::tuple<NodeID, Connector, basic_unit<DT>>(i_f, c_f, 0)),
                m_secondNode(std::tuple<NodeID, Connector, basic_unit<DT>>(i_s, c_s, 0)),
                m_id(id),
                _E(b_E), _I(b_I), _A(b_A), 
                m_load(load){
                    check_UNIT();
                }

            void setLoad(const BasicLoad<DT>& load){
                this->m_load = load;
                return;
            }

            smatrix<DT> getTransformMatrix();
            smatrix<DT> getReverseTransformMatrix();
            smatrix<basic_unit<DT>> getLocalRigidMatrix();
            smatrix<DT> getLocalRigidMatrixMetric();
            smatrix<DT> getGlobalRigidMatrixMetric();

            cvector<basic_unit<DT>> getLocalPVector();
            cvector<DT> getLocalPVectorMetric();
            cvector<basic_unit<DT>> getGlobalPVector();
            cvector<DT> getGlobalPVectorMetric();
            cvector<uint32_t> getLocalGlobalMap();
    }; //} class template Beam<T>
// member function of class template Beam<T> //{
template<typename DT>
    typename Beam<DT>::__beam_static_contructor Beam<DT>::static_member;

/* 
 * Result of this function is reaction to the virtual contraint, let 
 * final result of this ideas as P = result. 
 * And set final rigid matrix to K, the solved displacement as Delta.
 * It will has following relation:
 * -(K * Delta) + P = 0    <==>    K * Delta = P
 */
template<typename DT>
    cvector<basic_unit<DT>> Beam<DT>::getLocalPVector(){ //{ 
        if(this->m_load.isZero()){
#if defined(MODEL_DEBUG)
            std::cout << "BeamID <" << this->m_id << "> hasn't load" << std::endl;
#endif
            return Beam<DT>::static_member.ZERO_REACTION_VECTOR;
        }
        cvector<basic_unit<DT>> ret(6);
        basic_unit<DT> len = this->getLength();
        if(this->m_load.is_point_load){
            basic_unit<DT> F = this->m_load.load.m_point_load.value;
            DT         angle = this->m_load.load.m_point_load.direction;
            DT    percentage = this->m_load.load.m_point_load.percentage;
#if defined(MODEL_DEBUG)
            std::cout << "--------------------------" << std::endl;
            std::cout << "BeamId:" << this->m_id << std::endl;
            std::cout << "Point Load is:" << F << std::endl;
            std::cout << "Point Load angle:" << angle << std::endl;
            std::cout << "Point Load position:" << percentage << std::endl;
#endif // MODEL_DEBUG
            ret[1] = (F * ::sin(angle)) / 2.0;
            ret[4] = ret[1];
            ret[2] = -1.0 * F * pow((1 - percentage), 2) * (1 + 2 * percentage) * ::cos(angle);
            ret[5] = -1.0 * (F + ret[2]) * ::cos(angle);
            ret[3] = -1.0 * F * percentage * pow(1 - percentage, 2) * len * ::cos(angle);
            ret[6] = F * pow(percentage, 2) * (1 - percentage) * len * ::cos(angle);
            return ret;
        } else {
            basic_unit<DT> F = this->m_load.load.m_distributed_load.value;
            DT         angle = this->m_load.load.m_distributed_load.direction;
#if defined(MODEL_DEBUG)
            std::cout << "--------------------------" << std::endl;
            std::cout << "BeamId:" << this->m_id << std::endl;
            std::cout << "Distributed Load is:" << F << std::endl;
            std::cout << "Distributed Load angle:" << angle << std::endl;
#endif // MODEL_DEBUG
            ret[1] = F * ::sin(angle) * len / 2.0;
            ret[4] = ret[1];
            ret[2] = -1.0 * F * ::cos(angle) * len / 2.0;
            ret[5] = ret[2];
            ret[3] = F * len * len * ::cos(angle) / (-12.0);
            ret[6] = -1.0 * ret[3];
#if defined(MODEL_DEBUG)
            std::cout << "FINAL P RESULT:" << ret.toString() << std::endl;
            std::cout << "--------------------------" << std::endl;
#endif // MODEL_DEBUG
            return ret;
        }
    } //}
template<typename DT>
    cvector<DT> Beam<DT>::getLocalPVectorMetric(){ //{
        cvector<basic_unit<DT>> raw_result = this->getLocalPVector();
#if defined(MODEL_DEBUG)
        std::cout << "Raw result of localPVector: " << std::endl;
        std::cout << raw_result.toString() << std::endl;
#endif
        cvector<DT> ret(6);
        for(size_t i=1; i<=6;i++)
            ret[i] = raw_result[i];
        return ret;
    } //}
template<typename DT>
    cvector<DT> Beam<DT>::getGlobalPVectorMetric(){ //{
        auto _lll = this->getLocalPVectorMetric();
        auto _ttt = this->getTransformMatrix();
        cvector<DT> ret = _ttt * _lll;
        return ret;
    } //}
template<typename DT>
    cvector<uint32_t> Beam<DT>::getLocalGlobalMap(){ //{
        cvector<uint32_t> ret(6);
        for(size_t i = 1; i<=6; i++){
            ret[i] = this->m_id * 6 + i;
        }
        return ret;
    } //}

template<typename DT>
smatrix<DT> __transformM(const DT& value) //{
{
    smatrix<DT> ret(6);
    ret.set([](size_t, size_t) -> DT{return 0;});
    ret.set(1, 1, ::cos(value));
    ret.set(2, 2, ::cos(value));
    ret.set(3, 3,            1);
    ret.set(4, 4, ::cos(value));
    ret.set(5, 5, ::cos(value));
    ret.set(6, 6,            1);
    ret.set(1, 2,-::sin(value));
    ret.set(2, 1, ::sin(value));
    ret.set(4, 5,-::sin(value));
    ret.set(5, 4, ::sin(value));
    return ret;
} //}

template<typename DT>
smatrix<DT> Beam<DT>::getTransformMatrix() //{
{
    return __transformM(this->getTheta());
} //}
template<typename DT>
smatrix<DT> Beam<DT>::getReverseTransformMatrix() //{
{
    return __transformM(-this->getTheta());
} //}

template<typename DT>
smatrix<basic_unit<DT>> Beam<DT>::getLocalRigidMatrix() //{
{
    smatrix<basic_unit<DT>> enheng(6);
    smatrix<DT> ret   (6);
    basic_length<DT> len(this->getLength());
    enheng.set(1, 1, this->_E * this->_A / len);
    enheng.set(1, 2, 0);
    enheng.set(1, 3, 0);
    enheng.set(2, 1, 0);
    enheng.set(2, 2, 12.0 * this->_E * this->_I / (len * len * len));
    enheng.set(2, 3, 6.0  * this->_E * this->_I / (len * len));
    enheng.set(3, 1, 0);
    enheng.set(3, 2, 6.0  * this->_E * this->_I / (len * len));
    enheng.set(3, 3, 4.0  * this->_E * this->_I / len);

    enheng.set(1, 4, -1.0 * this->_E * this->_A / len);
    enheng.set(1, 5, 0);
    enheng.set(1, 6, 0);
    enheng.set(2, 4, 0);
    enheng.set(2, 5, -12.0 * this->_E * this->_I / (len * len * len));
    enheng.set(2, 6, 6.0   * this->_E * this->_I / (len * len));
    enheng.set(3, 4, 0);
    enheng.set(3, 5, -6.0  * this->_E * this->_I / (len * len));
    enheng.set(3, 6, 2.0   * this->_E * this->_I / len);

    enheng.set([](size_t i, size_t j, 
                smatrix<basic_unit<DT>>* _this) -> typename smatrix<basic_unit<DT>>::dataType{
            if(i<=3)
            return _this->get(i, j);
            if(j<=3)
            return _this->get(j,     i);
            return _this->get(i - 3, j - 3);
            });
    enheng.get(6, 5) = -enheng.get(6, 5); // load equilibrium equation
    enheng.get(5, 6) = -enheng.get(5, 6);
    return enheng;
} //}
template<typename DT>
smatrix<DT> Beam<DT>::getLocalRigidMatrixMetric() //{
{
    auto enheng = this->getLocalRigidMatrix();
    smatrix<DT> ret(enheng.get_size());
    SMSolver::mapMatrix<typename decltype(enheng)::dataType,
        typename decltype(ret)::dataType>(
                enheng, ret, [](const basic_unit<DT>& x) -> DT {
                return x.value * x.ratio;
                });
    return ret;
} //}

template<typename DT>
smatrix<DT> Beam<DT>::getGlobalRigidMatrixMetric() //{
{
    smatrix<DT> loc    = this->getLocalRigidMatrixMetric();
    smatrix<DT> Trans  = this->getTransformMatrix();
    smatrix<DT> RTrans = this->getReverseTransformMatrix();
    auto ret = Trans * loc * RTrans;
#if defined(MODEL_DEBUG)
    std::cout << "local rigid Matrix(metric):" << std::endl;
    std::cout << loc.toString();
    std::cout << "Transform Matrix:" << std::endl;
    std::cout << Trans.toString();
    std::cout << "result Matrix:(metric)" << std::endl;
    std::cout << ret.toString() << std::endl;
#endif
    return ret;
} //}

//} member function of class template Beam<T>

template<typename DT>
class SMSolverManager { //{
    private:
        std::vector<Beam<DT>*> beams;
        std::vector<Node<DT>*> nodes;
        NodeID m_nodeIdCount = 0;
        BeamID m_beamIdCount = 0;
        std::vector<uint32_t> map_list;
    public:
        ~SMSolverManager(){
            for(auto b = beams.begin(); b!=beams.end(); b++)
                delete *b;
            for(auto b = nodes.begin(); b!=nodes.end(); b++)
                delete *b;
        }
        void solve();
        Node<DT>*  getNode(const NodeID& nid){
            if(nid < 1)
                throw *new SMSolverException("SMSolverManager<T>::getNode(): Invaild NodeID");
            return nodes[nid - 1];
        }
        Beam<DT>*  getBeam(const BeamID& nid){
            if(nid < 1)
                throw *new SMSolverException("SMSolverManager<T>::getBeam(): Invaild BeamID");
            return beams[nid - 1];
        }
        NodeID new_node(typename Node<DT>::length_type x, 
                typename Node<DT>::length_type y){
            nodes.push_back(new Node<DT>(this, x, y, ++m_nodeIdCount));
            return m_nodeIdCount;
        }
        BeamID new_Beam(NodeID f, NodeID s,
                Connector fc, Connector sc,
                basic_unit<DT> E, 
                basic_unit<DT> A, 
                basic_unit<DT> I){
            Beam<DT>* new_beam = new Beam<DT>(this, f, s, fc, sc, ++m_beamIdCount, E, A, I);
            beams.push_back(new_beam);
            Node<DT>* n1 = this->getNode(f);
            Node<DT>* n2 = this->getNode(s);
            n1->m_connected_beams.push_back(new_beam);
            n2->m_connected_beams.push_back(new_beam);
            return m_beamIdCount;
        }

        smatrix<DT> __getMetricRigidMetric(){ //{
            size_t __order = m_beamIdCount * 6;
            smatrix<DT> ret(__order);
            ret.set([](size_t, size_t) -> typename decltype(ret)::dataType {return 0;});
            for(size_t i = 0; i<m_beamIdCount; i++){
                Beam<DT>* xbeam = beams[i];
                auto ins = xbeam->getGlobalRigidMatrixMetric();
                ret.insert_submatrix(ins, i * 6 + 1);
            }
            return ret;
        } //}
        std::vector<std::tuple<uint32_t, Connector, basic_unit<DT>>> __handleNodePtr2(const Node<DT>* ptr){ //{
            std::vector<std::tuple<uint32_t, Connector, basic_unit<DT>>> ret;
            for(auto x = ptr->m_connected_beams.begin(); x != ptr->m_connected_beams.end(); x++){
                Beam<DT>* b = *x;
                if(this->getNode(std::get<0>(b->m_firstNode)) == ptr)
                    ret.push_back(std::make_tuple((b->m_id - 1) * 6 + 1, 
                                                  std::get<1>(b->m_firstNode), 
                                                  std::get<1>(b->m_firstNode)));
                else
                    ret.push_back(std::make_tuple((b->m_id - 1) * 6 + 4, 
                                                  std::get<1>(b->m_secondNode), 
                                                  std::get<1>(b->m_secondNode)));
            }
            return ret;
        } //}
        std::vector<std::pair<uint32_t, Connector>> __handleNodePtr(const Node<DT>* ptr){ //{
            std::vector<std::pair<uint32_t, Connector>> ret;
            for(auto x = ptr->m_connected_beams.begin(); x != ptr->m_connected_beams.end(); x++){
                Beam<DT>* b = *x;
                if(this->getNode(std::get<0>(b->m_firstNode)) == ptr)
                    ret.push_back(std::make_pair((b->m_id - 1) * 6 + 1, std::get<1>(b->m_firstNode)));
                else
                    ret.push_back(std::make_pair((b->m_id - 1) * 6 + 4, std::get<1>(b->m_secondNode)));
            }
            return ret;
        } //}
        void __traversing_reduce(smatrix<DT>& M) //{
        {
#if defined(MODEL_DEBUG)
            uint32_t z = 1;
            for(auto x = map_list.begin(); x!=map_list.end(); x++, z++){
                std::cout << z << ": " << *x << std::endl;
            }
            std::cout << M.toString() << std::endl;
#endif
            if(M.get_size() != map_list.size())
                throw *new SMSolverException("__traversing_reduce unmatch size");
            uint32_t n = M.get_size();
            for(auto x = map_list.rbegin(); x!=map_list.rend(); x++, n--){
                if(n != *x && *x != 0){
                    M.comAdd(*x, n);
                }
#if defined(MODEL_DEBUG)
                std::cout << "n = " << n << ", *x = " << *x <<std::endl;
                std::cout << M.toString() << std::endl;
#endif
            }
            n = M.get_size();
            std::vector<uint32_t> delete_s;
            for(auto x = map_list.rbegin(); x!=map_list.rend(); x++, n--){
                if(*x != n)
                    delete_s.push_back(n);
            }
            std::sort(delete_s.begin(), delete_s.end(), [](uint32_t a, uint32_t b) -> bool {return a<=b;});
            for(auto x = delete_s.rbegin(); x!=delete_s.rend(); x++)
                M.delete_colrow(*x);
        } //}
        void __traversing_reduce_cvector(cvector<DT>& V) //{
        {
            if(V.get_size() != map_list.size())
                throw *new SMSolverException("__traversing_reduce_cvector unmatch size");
            uint32_t n = V.get_size();
            for(auto x = map_list.rbegin(); x!=map_list.rend(); x++, n--){
                if(n != *x && *x != 0){
                    V[*x] += V[n];
                }
            }
            n = V.get_size();
            std::vector<uint32_t> delete_s;
            for(auto x = map_list.rbegin(); x!=map_list.rend(); x++, n--){
                if(*x != n){
                    delete_s.push_back(n);
                }
            }
            std::sort(delete_s.begin(), delete_s.end(), [](uint32_t a, uint32_t b) -> bool {return a<=b;});
            for(auto x = delete_s.rbegin(); x!= delete_s.rend(); x++)
                V.delete_elem(*x);
        } //}
cvector<DT> __getMetricP() //{
{
    cvector<DT> ret(this->m_beamIdCount * 6);
    for(size_t i = 1; i<=this->m_beamIdCount; i++){
        cvector<DT> __t = this->getBeam(i)->getGlobalPVectorMetric();
#if defined(MODEL_DEBUG)
        std::cout << "STAGED result, P of BeamID: " << this->getBeam(i)->m_id << std::endl;
        std::cout << __t.toString() << std::endl;
#endif
        for(size_t j = 1; j<=6; j++)
            ret[(i-1) * 6 + j] = __t[j];
    }
    for(size_t i = 1; i<=this->m_nodeIdCount; i++){
        Node<DT>* __n = this->getNode(i);
        if(__n->m_load.isZero() || __n->m_connected_beams.size() == 0)
            continue;
        Beam<DT>* __b = __n->m_connected_beams[0];
        DT __fx       = __n->m_load.load.m_point_load.value * ::cos(__n->m_load.load.m_point_load.direction);
        DT __fy       = __n->m_load.load.m_point_load.value * ::sin(__n->m_load.load.m_point_load.direction);
        if(std::get<0>(__b->m_firstNode) == i){
            ret[(__b->m_id - 1) * 6 + 1] += __fx;
            ret[(__b->m_id - 1) * 6 + 2] += __fy;
        } else {
            ret[(__b->m_id - 1) * 6 + 4] += __fx;
            ret[(__b->m_id - 1) * 6 + 5] += __fy;
        }
    }
    return ret;
} //}

cvector<DT> getMetricP() //{
{
    cvector<DT> ret = __getMetricP();
#if defined(MODEL_DEBUG)
    std::cout << "Intermeidate Result of P:" << std::endl;
    std::cout << ret.toString() << std::endl;
#endif
    __traversing_reduce_cvector(ret);
    return ret;
} //}

smatrix<DT> getMetricRigidMetric() // don't consider Force //{
{
    std::vector<Node<DT>*> fixed_beam;
    smatrix<DT> interResult = __getMetricRigidMetric();
    map_list.resize(interResult.get_size());
    for(size_t i = 0; i<map_list.size(); i++) // initial states
        map_list[i] = i + 1;
    for(auto n = nodes.begin(); n!=nodes.end(); n++){
        Node<DT>* n_ptr = *n;
#if defined(MODEL_DEBUG)
        std::cout << "node " << n_ptr->m_id << " connected degree is: " << n_ptr->m_connected_beams.size() << std::endl;
#endif
        if(n_ptr->m_connected_beams.size() <= 0)
            throw *new SMSolverException("Found unconnected node.");

        std::vector<std::pair<uint32_t, Connector>> _x = __handleNodePtr(n_ptr);
        if(n_ptr->m_connected_beams.size() == 1){
            if(n_ptr->_support != BasicSupport::NoneSupport){
                map_list[_x[0].first - 1] = 0;
                map_list[_x[0].first    ] = 0;
                if(_x[0].second == Connector::FixedConnect && n_ptr->_support == BasicSupport::FixedSupport)
                    map_list[_x[0].first + 1] = 0;
            }
        }

        // handle elastic condition ...
        if(n_ptr->m_connected_beams.size() == 2) // maybe elastic condition
        {
            std::vector<std::tuple<uint32_t, Connector, basic_unit<DT>>> _x = __handleNodePtr2(n_ptr);
            uint32_t __a, __b;
            Connector __ca, __cb;
            basic_unit<DT> __ua, __ub;
            std::tie(__a, __ca, __ua) = _x[0];
            std::tie(__b, __cb, __ub) = _x[1];
            map_list[__b - 1] = __a;
            map_list[__b    ] = __a + 1;
            if(__ca == Connector::FixedConnect && __cb == Connector::FixedConnect){
                map_list[__b + 1] = __a + 2;
            }
            if(
                    (__ca == Connector::FixedConnect && 
                     __cb == Connector::ElasticHingeConnect) ||
                    (__cb == Connector::FixedConnect &&
                     __ca == Connector::ElasticHingeConnect)){
                DT K = 0;
                if(__ca == Connector::ElasticHingeConnect){
                    UNIT_IS_K(__ua);
                    K = __ua.ratio * __ua.value;
                } else {
                    UNIT_IS_K(__ub);
                    K = __ub.ratio * __ub.value;
                }
                if(n_ptr->_support != BasicSupport::FixedSupport){
                    interResult.get(__a + 2, __a + 2) -= K;
                    interResult.get(__b + 2, __b + 2) -= K;
                    interResult.get(__a + 2, __b + 2) += K;
                    interResult.get(__a + 2, __b + 2) += K;
                } else {
                    if(__ca == Connector::ElasticHingeConnect){
                        interResult.get(__a + 2, __a + 2) -= K;
                        map_list[__b + 1] = 0;
                    } else {
                        interResult.get(__b + 2, __b + 2) -= K;
                        map_list[__a + 1] = 0;
                    }
                }
            }
            if(n_ptr->_support != BasicSupport::NoneSupport){
                map_list[__a - 1] = 0;
                map_list[__a    ] = 0;
            }
            if(n_ptr->_support == BasicSupport::FixedSupport){
                if(__ca == Connector::FixedConnect)
                    map_list[__a + 1] = 0;
                if(__cb == Connector::FixedConnect)
                    map_list[__b + 1] = 0;
            }
            continue;
        }
        uint32_t base_index = _x[0].first;
        bool hasFix = false;
        for(auto m = _x.begin(); m!=_x.end(); m++){
            uint32_t index;
            Connector connect;
            std::tie(index, connect) = *m;
            if(connect == Connector::FixedConnect){
                base_index = index;
                hasFix = true;
                break;
            }
        }
        if(n_ptr->_support != BasicSupport::NoneSupport){
            map_list[base_index - 1] = 0;
            map_list[base_index    ] = 0;
            if(n_ptr->_support == BasicSupport::FixedSupport && hasFix)
                map_list[base_index + 1] = 0;
        }
        for(auto m = _x.begin(); m!=_x.end(); m++){
            uint32_t index;
            Connector connect;
            std::tie(index, connect) = *m;
            if(index == base_index)
                continue;
            map_list[index - 1] = base_index;
            map_list[index]     = base_index + 1;
            if(connect == Connector::FixedConnect)
                map_list[index+1] = base_index + 2;
        }
    }
    // traversing the nodes, store reduction information into map_list: vector<>
    __traversing_reduce(interResult);
    return interResult;
} //}
}; //} class tempalte SMSolverManager<T>

extern template class SMSolverManager<double>;
extern template class Beam<double>;
extern template class Node<double>;
} // namespace SMSolver

#endif
