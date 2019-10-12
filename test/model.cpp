#define MODEL_DEBUG

#include "../src/matrix/matrix.hpp"
#include "../src/model/unit.hpp"
#include "../src/model/model.hpp"

#define TEST_SIZE 10

using namespace SMSolver;

void TESTA()
{
    SMSolverManager<double> x;
    auto n1 = x.new_node(0, 0);
    auto n2 = x.new_node(200, 200);
    auto n3 = x.new_node(200, 0);
    auto n4 = x.new_node(0, 200);
    basic_unit<double> EB(1);
    basic_unit<double> IB(1);
    basic_unit<double> AB(1);
    EB.setUnit(-1, 1, -2);
    IB.setUnit(4, 0, 0);
    AB.setUnit(2, 0, 0);
    x.getNode(n1)->setSupport(BasicSupport::FixedSupport);
    x.getNode(n3)->setSupport(BasicSupport::FixedSupport);
    x.new_Beam(n3, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n3, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n4, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n4, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);

    auto m1 = x.getMetricRigidMetric();
    auto m1_lu = m1.get_LU_test();

    std::cout << m1.toString() << std::endl;
    std::cout << m1_lu.first.toString() << std::endl; 
    std::cout << m1_lu.second.toString(); 
    return;
}

void TESTB()
{
    SMSolverManager<double> x;
    auto n1 = x.new_node(0.0 * BUnit.m, 0.0 * BUnit.m);
    auto n2 = x.new_node(0.0 * BUnit.m, 2.0 * BUnit.m);
    auto n3 = x.new_node(2.0 * BUnit.m, 2.0 * BUnit.m);
    auto n4 = x.new_node(2.0 * BUnit.m, 0.0 * BUnit.m);

    basic_unit<double> EB(1);
    basic_unit<double> IB(1);
    basic_unit<double> AB(1);
    EB.setUnit(-1, 1, -2);
    IB.setUnit(4, 0, 0);
    AB.setUnit(2, 0, 0);

    x.getNode(n1)->setSupport(BasicSupport::HingeSupport);
    x.getNode(n2)->setSupport(BasicSupport::HingeSupport);
    x.getNode(n3)->setSupport(BasicSupport::HingeSupport);
    x.getNode(n4)->setSupport(BasicSupport::HingeSupport);

    auto b1 = x.new_Beam(n1, n2, Connector::FixedConnect, Connector::FixedConnect, 10.0 * EB, 1.0 * AB, 10.0 * IB);
    auto b2 = x.new_Beam(n2, n3, Connector::FixedConnect, Connector::FixedConnect, 10.0 * EB, 1.0 * AB, 10.0 * IB);
    auto b3 = x.new_Beam(n3, n4, Connector::FixedConnect, Connector::FixedConnect, 10.0 * EB, 1.0 * AB, 10.0 * IB);
    auto b4 = x.new_Beam(n4, n1, Connector::FixedConnect, Connector::FixedConnect, 10.0 * EB, 1.0 * AB, 10.0 * IB);

//    x.new_Beam(n1, n3, Connector::FixedConnect, Connector::FixedConnect, 10000.0 * EB, 100.0 * AB, 100.0 * IB);
//    x.new_Beam(n4, n2, Connector::FixedConnect, Connector::FixedConnect, 10000.0 * EB, 100.0 * AB, 100.0 * IB);

    auto l1 = BasicLoad<double>(false, 50.0 * BUnit.KN / BUnit.m, 0.0);

    x.getBeam(b1)->setLoad(l1);
    x.getBeam(b2)->setLoad(l1);
    x.getBeam(b3)->setLoad(l1);
    x.getBeam(b4)->setLoad(l1);

    auto m1 = x.getMetricRigidMetric();
    auto v1 = x.getMetricP();
    auto m1_lu = m1.Gauss_Elimination();

    auto delta = m1.Guass_Elimination_solver(v1);

    std::cout << m1.toString() << std::endl;
    std::cout << v1.toString() << std::endl;
    std::cout << m1_lu.first.toString() << std::endl; 
    std::cout << m1_lu.second.toString() << std::endl; 
    std::cout << "displacement: " << delta.toString() << std::endl;
    return;
}

int main()
{
//    TESTA();
    TESTB();
    return 0;
}
