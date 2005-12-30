/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Acknowledgement of the program authors must be made in any
 *    publication of scientific results based in part on use of the
 *    program.  An acceptable form of acknowledgement is citation of
 *    the article in which the program was described (Matthew
 *    A. Meineke, Charles F. Vardeman II, Teng Lin, Christopher
 *    J. Fennell and J. Daniel Gezelter, "OOPSE: An Object-Oriented
 *    Parallel Simulation Engine for Molecular Dynamics,"
 *    J. Comput. Chem. 26, pp. 252-271 (2005))
 *
 * 2. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 3. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * This software is provided "AS IS," without a warranty of any
 * kind. All express or implied conditions, representations and
 * warranties, including any implied warranty of merchantability,
 * fitness for a particular purpose or non-infringement, are hereby
 * excluded.  The University of Notre Dame and its licensors shall not
 * be liable for any damages suffered by licensee as a result of
 * using, modifying or distributing the software or its
 * derivatives. In no event will the University of Notre Dame or its
 * licensors be liable for any lost revenue, profit or data, or for
 * direct, indirect, special, consequential, incidental or punitive
 * damages, however caused and regardless of the theory of liability,
 * arising out of the use of or inability to use software, even if the
 * University of Notre Dame has been advised of the possibility of
 * such damages.
 */
 
#include <math.h>
#include <iostream>

#ifdef IS_MPI
#include <mpi.h>
#endif //is_mpi

#include "brains/Thermo.hpp"
#include "primitives/Molecule.hpp"
#include "utils/simError.h"
#include "utils/OOPSEConstant.hpp"

namespace oopse {

  double Thermo::getKinetic() {
    SimInfo::MoleculeIterator miter;
    std::vector<StuntDouble*>::iterator iiter;
    Molecule* mol;
    StuntDouble* integrableObject;    
    Vector3d vel;
    Vector3d angMom;
    Mat3x3d I;
    int i;
    int j;
    int k;
    double kinetic = 0.0;
    double kinetic_global = 0.0;
    
    for (mol = info_->beginMolecule(miter); mol != NULL; mol = info_->nextMolecule(miter)) {
      for (integrableObject = mol->beginIntegrableObject(iiter); integrableObject != NULL; 
	   integrableObject = mol->nextIntegrableObject(iiter)) {

	double mass = integrableObject->getMass();
	Vector3d vel = integrableObject->getVel();

	kinetic += mass * (vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2]);

	if (integrableObject->isDirectional()) {
	  angMom = integrableObject->getJ();
	  I = integrableObject->getI();

	  if (integrableObject->isLinear()) {
	    i = integrableObject->linearAxis();
	    j = (i + 1) % 3;
	    k = (i + 2) % 3;
	    kinetic += angMom[j] * angMom[j] / I(j, j) + angMom[k] * angMom[k] / I(k, k);
	  } else {                        
	    kinetic += angMom[0]*angMom[0]/I(0, 0) + angMom[1]*angMom[1]/I(1, 1) 
	      + angMom[2]*angMom[2]/I(2, 2);
	  }
	}
            
      }
    }
    
#ifdef IS_MPI

    MPI_Allreduce(&kinetic, &kinetic_global, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);
    kinetic = kinetic_global;

#endif //is_mpi

    kinetic = kinetic * 0.5 / OOPSEConstant::energyConvert;

    return kinetic;
  }

  double Thermo::getPotential() {
    double potential = 0.0;
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    double shortRangePot_local =  curSnapshot->statData[Stats::SHORT_RANGE_POTENTIAL] ;

    // Get total potential for entire system from MPI.

#ifdef IS_MPI

    MPI_Allreduce(&shortRangePot_local, &potential, 1, MPI_DOUBLE, MPI_SUM,
                  MPI_COMM_WORLD);
    potential += curSnapshot->statData[Stats::LONG_RANGE_POTENTIAL];

#else

    potential = shortRangePot_local + curSnapshot->statData[Stats::LONG_RANGE_POTENTIAL];

#endif // is_mpi

    return potential;
  }

  double Thermo::getTotalE() {
    double total;

    total = this->getKinetic() + this->getPotential();
    return total;
  }

  double Thermo::getTemperature() {
    
    double temperature = ( 2.0 * this->getKinetic() ) / (info_->getNdf()* OOPSEConstant::kb );
    return temperature;
  }

  double Thermo::getVolume() { 
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    return curSnapshot->getVolume();
  }

  double Thermo::getPressure() {

    // Relies on the calculation of the full molecular pressure tensor


    Mat3x3d tensor;
    double pressure;

    tensor = getPressureTensor();

    pressure = OOPSEConstant::pressureConvert * (tensor(0, 0) + tensor(1, 1) + tensor(2, 2)) / 3.0;

    return pressure;
  }

  double Thermo::getPressure(int direction) {

    // Relies on the calculation of the full molecular pressure tensor

	  
    Mat3x3d tensor;
    double pressure;

    tensor = getPressureTensor();

    pressure = OOPSEConstant::pressureConvert * tensor(direction, direction);

    return pressure;
  }



  Mat3x3d Thermo::getPressureTensor() {
    // returns pressure tensor in units amu*fs^-2*Ang^-1
    // routine derived via viral theorem description in:
    // Paci, E. and Marchi, M. J.Phys.Chem. 1996, 100, 4314-4322
    Mat3x3d pressureTensor;
    Mat3x3d p_local(0.0);
    Mat3x3d p_global(0.0);

    SimInfo::MoleculeIterator i;
    std::vector<StuntDouble*>::iterator j;
    Molecule* mol;
    StuntDouble* integrableObject;    
    for (mol = info_->beginMolecule(i); mol != NULL; mol = info_->nextMolecule(i)) {
      for (integrableObject = mol->beginIntegrableObject(j); integrableObject != NULL; 
	   integrableObject = mol->nextIntegrableObject(j)) {

	double mass = integrableObject->getMass();
	Vector3d vcom = integrableObject->getVel();
	p_local += mass * outProduct(vcom, vcom);         
      }
    }
    
#ifdef IS_MPI
    MPI_Allreduce(p_local.getArrayPointer(), p_global.getArrayPointer(), 9, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
#else
    p_global = p_local;
#endif // is_mpi

    double volume = this->getVolume();
    Snapshot* curSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    Mat3x3d tau = curSnapshot->statData.getTau();

    pressureTensor =  (p_global + OOPSEConstant::energyConvert* tau)/volume;

    return pressureTensor;
  }

  void Thermo::saveStat(){
    Snapshot* currSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    Stats& stat = currSnapshot->statData;
    
    stat[Stats::KINETIC_ENERGY] = getKinetic();
    stat[Stats::POTENTIAL_ENERGY] = getPotential();
    stat[Stats::TOTAL_ENERGY] = stat[Stats::KINETIC_ENERGY]  + stat[Stats::POTENTIAL_ENERGY] ;
    stat[Stats::TEMPERATURE] = getTemperature();
    stat[Stats::PRESSURE] = getPressure();
    stat[Stats::VOLUME] = getVolume();      

    Mat3x3d tensor =getPressureTensor();
    stat[Stats::PRESSURE_TENSOR_X] = tensor(0, 0);      
    stat[Stats::PRESSURE_TENSOR_Y] = tensor(1, 1);      
    stat[Stats::PRESSURE_TENSOR_Z] = tensor(2, 2);      


    /**@todo need refactorying*/
    //Conserved Quantity is set by integrator and time is set by setTime
    
  }

} //end namespace oopse
