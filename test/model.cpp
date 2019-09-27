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
    auto sss = 100.0 * EB;
    x.getNode(n1)->setSupport(BasicSupport::FixedSupport);
    x.getNode(n3)->setSupport(BasicSupport::FixedSupport);
    x.new_Beam(n3, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n3, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n4, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n4, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n1, n2, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);

    auto m1 = x.getMetricRigid();
    auto m1_lu = m1.get_LU_test();

    std::cout << m1.toString() << std::endl;
    std::cout << m1_lu.first.toString() << std::endl; 
    std::cout << m1_lu.second.toString(); 
    return;
}

void TESTB()
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
    auto sss = 100.0 * EB;
    x.getNode(n1)->setSupport(BasicSupport::FixedSupport);
    x.getNode(n3)->setSupport(BasicSupport::FixedSupport);

    x.getNode(n2)->setSupport(BasicSupport::FixedSupport);
    x.getNode(n4)->setSupport(BasicSupport::FixedSupport);

    x.new_Beam(n1, n3, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);
    x.new_Beam(n2, n4, Connector::HingeConnect, Connector::HingeConnect, 100.0 * EB, 100.0 * AB, 100.0 * IB);

    auto m1 = x.getMetricRigid();
    auto m1_lu = m1.get_LU_test();

    std::cout << m1.toString() << std::endl;
    std::cout << m1_lu.first.toString() << std::endl; 
    std::cout << m1_lu.second.toString(); 
    return;
}

int main()
{
//    TESTA();
    TESTB();
    return 0;
}
