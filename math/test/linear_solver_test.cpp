// -------------------------------------------------------------
/**
 * @file   linear_solver_test.cpp
 * @author William A. Perkins
 * @date   2013-09-10 13:42:13 d3g096
 * 
 * @brief  
 * 
 * @test
 */
// -------------------------------------------------------------

#include <iostream>
#include <boost/format.hpp>

#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/included/unit_test.hpp>

#include "gridpack/parallel/parallel.hpp"
#include "gridpack/utilities/exception.hpp"
#include "math.hpp"
#include "linear_solver.hpp"

BOOST_AUTO_TEST_SUITE(LinearSolverTest)

// -------------------------------------------------------------
// assemble
// -------------------------------------------------------------
void
assemble(const int imax, const int jmax, 
         gridpack::math::Matrix& A, 
         gridpack::math::Vector& b)
{
  static const float k = 1000;  /* conductivity, W/m/K */
  static const float t = 0.01;  /* plate thickness, m */
  static const float W = 0.3;   /* plate width, m */
  static const float H = 0.4;   /* plate height, m */

  const float dx = W/static_cast<float>(imax);
  const float dy = H/static_cast<float>(jmax);

  int ilo, ihi;
  b.local_index_range(ilo, ihi);

  int i, j, ierr;
  float ap, aw, ae, as, an, bp;

  int iP, iN, iS, iE, iW;

  // this fills a row in A only if it's locally owned

  for (i = 0; i < imax; i++) {
    for (j = 0; j < jmax; j++) {
      iP = i*jmax + j;
      if (ilo <= iP && iP < ihi) {
        iE = (i+1)*jmax + j;
        iW = (i-1)*jmax + j;
        iN = i*jmax + (j+1);
        iS = i*jmax + (j-1);

        bp = 0.0;
        ap = 0.0;
        if (j == 0) {             /* insulated south boundary */
          as = 0.0;
          bp += 0.0;
          ap -= 0.0;
        } else {
          as = (k/dx)*(dx*t);
        }

        if (j == jmax - 1) {      /* constant temperature (100C) north boundary */
          an = 0.0;
          bp += 2*k/dy*(dy*t)*100.0;
          ap -= -2*k/dy*(dy*t);
        } else {
          an = (k/dx)*(dx*t);
        }

        if (i == 0) {             /* constant flux (500kw/m2) west boundary */
          aw = 0.0;
          bp += 500000.0*dy*t;
          ap -= 0.0;
        } else {
          aw = (k/dx)*(dx*t);
        }

        if (i == imax - 1) {      /* insulated east boundary */
          ae = 0.0;
          bp += 0.0; 
          ap -= 0.0;
        } else {
          ae = (k/dx)*(dx*t);
        }
        
        ap += as + an + aw + ae;

        A.set_element(iP, iP, ap);

        if (an != 0.0) A.set_element(iP, iN, -an);
        if (as != 0.0) A.set_element(iP, iS, -as);
        if (ae != 0.0) A.set_element(iP, iE, -ae);
        if (aw != 0.0) A.set_element(iP, iW, -aw);
        b.set_element(iP, bp);
      }      
    }
  }
}


// -------------------------------------------------------------
/**
 * This is a simple test of the gridpack::math::LinearSolver.  This
 * problem comes from Example 7.2 in
 * 
 * Versteeg, H.K. and W. Malalasekera, 1995. An introduction to
 * computational fluid dynamics, the finite volume method. Prentice
 * Hall.  257 pp.
 * 
 */
// -------------------------------------------------------------
BOOST_AUTO_TEST_CASE( Versteeg )
{
  gridpack::parallel::Communicator world;

  static const float k = 1000;  /* conductivity, W/m/K */
  static const float t = 0.01;  /* plate thickness, m */
  static const float W = 0.3;   /* plate width, m */
  static const float H = 0.4;   /* plate height, m */

  static const int imax = 3*world.size();
  static const int jmax = 4*world.size();
  static const int global_size = imax*jmax;
  static const int local_size(global_size/world.size());

  std::auto_ptr<gridpack::math::Matrix> 
    A(new gridpack::math::Matrix(world, local_size, global_size, 
                                 gridpack::math::Matrix::Sparse));
  std::auto_ptr<gridpack::math::Vector>
    b(new gridpack::math::Vector(world, local_size)),
    x(new gridpack::math::Vector(world, local_size));

  assemble(imax, jmax, *A, *b);
  A->ready();
  b->ready();

  x->fill(0.0);
  x->ready();

  std::auto_ptr<gridpack::math::LinearSolver> 
    solver(new gridpack::math::LinearSolver(*A));

  solver->solve(*b, *x);

  std::auto_ptr<gridpack::math::Vector>
    res(multiply(*A, *x));
  res->add(*b, -1.0);

  gridpack::ComplexType l1norm(res->norm1());
  gridpack::ComplexType l2norm(res->norm2());

  if (world.rank() == 0) {
    std::cout << "Residual L1 Norm = " << l1norm << std::endl;
    std::cout << "Residual L2 Norm = " << l2norm << std::endl;
  }

  for (int p = 0; p < world.size(); ++p) {
    if (p == world.rank()) {
      int ilo, ihi;
      x->local_index_range(ilo, ihi);
      for (int iP = ilo; iP < ihi; ++iP) {
        gridpack::ComplexType val, r;
        x->get_element(iP, val);
        res->get_element(iP, r);
        int i = iP/jmax;
        int j = iP - i*jmax;
        
        std::cout << boost::str(boost::format("%8d%8d%8d%12.6f%12.3e") %
                                iP % i % j % real(val) % real(r))
                  << std::endl;
        std::cout.flush();
      }
    }
    world.barrier();
  }
}

BOOST_AUTO_TEST_SUITE_END()

// -------------------------------------------------------------
// init_function
// -------------------------------------------------------------
bool init_function()
{
  return true;
}

// -------------------------------------------------------------
//  Main Program
// -------------------------------------------------------------
int
main(int argc, char **argv)
{
  gridpack::parallel::Environment env(argc, argv);
  gridpack::math::Initialize();
  int result = ::boost::unit_test::unit_test_main( &init_function, argc, argv );
  gridpack::math::Finalize();
}