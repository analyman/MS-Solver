#include "../../include/mssm/model/model.hpp"
#include "../../include/mssm/model/force.hpp"
#include "../../include/mssm/model/unit.hpp"

namespace SMSolver {
template class SMSolver::SMSolverManager<double>;
template class SMSolver::Beam<double>;
template class SMSolver::Node<double>;

template class SMSolver::BasicLoad<double>;

template class SMSolver::basic_unit<double>;
template class SMSolver::basic_length<double>;
template class SMSolver::basic_mass<double>;
template class SMSolver::basic_second<double>;
}
