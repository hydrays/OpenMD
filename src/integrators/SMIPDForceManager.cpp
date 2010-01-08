/*
 * Copyright (c) 2008, 2009, 2010 The University of Notre Dame. All Rights Reserved.
 *
 * The University of Notre Dame grants you ("Licensee") a
 * non-exclusive, royalty free, license to use, modify and
 * redistribute this software in source and binary code form, provided
 * that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
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
 *
 * SUPPORT OPEN SCIENCE!  If you use OpenMD or its source code in your
 * research, please cite the appropriate papers when you publish your
 * work.  Good starting points are:
 *                                                                      
 * [1]  Meineke, et al., J. Comp. Chem. 26, 252-271 (2005).             
 * [2]  Fennell & Gezelter, J. Chem. Phys. 124, 234104 (2006).          
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 24107 (2008).          
 * [4]  Vardeman & Gezelter, in progress (2009).                        
 */
#include <fstream> 
#include <iostream>
#include "integrators/SMIPDForceManager.hpp"
#include "utils/PhysicalConstants.hpp"
#include "math/ConvexHull.hpp"
#include "math/AlphaHull.hpp"
#include "math/Triangle.hpp"
#include "math/CholeskyDecomposition.hpp"

namespace OpenMD {

  SMIPDForceManager::SMIPDForceManager(SimInfo* info) : ForceManager(info) {
    
    simParams = info->getSimParams();
    veloMunge = new Velocitizer(info);
    
    // Create Hull, Convex Hull for now, other options later.
    
    stringToEnumMap_["ConvexHull"] = hullConvex;
    stringToEnumMap_["AlphaShape"] = hullAlphaShape;
    stringToEnumMap_["Unknown"] = hullUnknown;
    
    const std::string ht = simParams->getHULL_Method();
    
    
    
    std::map<std::string, HullTypeEnum>::iterator iter;
    iter = stringToEnumMap_.find(ht);
    hullType_ = (iter == stringToEnumMap_.end()) ? SMIPDForceManager::hullUnknown : iter->second;
    if (hullType_ == hullUnknown) {
      std::cerr << "WARNING! Hull Type Unknown!\n";
    }
    
    switch(hullType_) {
    case hullConvex :
      surfaceMesh_ = new ConvexHull();
      break;
    case hullAlphaShape :
      surfaceMesh_ = new AlphaHull(simParams->getAlpha());
      break;
    case hullUnknown :
    default :
      break;
    }
    /* Check that the simulation has target pressure and target
       temperature set */
    
    if (!simParams->haveTargetTemp()) {
      sprintf(painCave.errMsg, 
              "SMIPDynamics error: You can't use the SMIPD integrator\n"
	      "\twithout a targetTemp (K)!\n");      
      painCave.isFatal = 1;
      painCave.severity = OPENMD_ERROR;
      simError();
    } else {
      targetTemp_ = simParams->getTargetTemp();
    }
    
    if (!simParams->haveTargetPressure()) {
      sprintf(painCave.errMsg, 
              "SMIPDynamics error: You can't use the SMIPD integrator\n"
	      "\twithout a targetPressure (atm)!\n");      
      painCave.isFatal = 1;
      simError();
    } else {
      // Convert pressure from atm -> amu/(fs^2*Ang)
      targetPressure_ = simParams->getTargetPressure() / 
        PhysicalConstants::pressureConvert;
    }
   
    if (simParams->getUsePeriodicBoundaryConditions()) {
      sprintf(painCave.errMsg, 
              "SMIPDynamics error: You can't use the SMIPD integrator\n"
              "\twith periodic boundary conditions!\n");    
      painCave.isFatal = 1;
      simError();
    } 
    
    if (!simParams->haveViscosity()) {
      sprintf(painCave.errMsg, 
              "SMIPDynamics error: You can't use the SMIPD integrator\n"
	      "\twithout a viscosity!\n");
      painCave.isFatal = 1;
      painCave.severity = OPENMD_ERROR;
      simError();
    }else{
      viscosity_ = simParams->getViscosity();
    }
    
    dt_ = simParams->getDt();
  
    variance_ = 2.0 * PhysicalConstants::kb * targetTemp_ / dt_;

    // Build a vector of integrable objects to determine if the are
    // surface atoms
    Molecule* mol;
    StuntDouble* integrableObject;
    SimInfo::MoleculeIterator i;
    Molecule::IntegrableObjectIterator  j;

    for (mol = info_->beginMolecule(i); mol != NULL; 
         mol = info_->nextMolecule(i)) {          
      for (integrableObject = mol->beginIntegrableObject(j); 
           integrableObject != NULL;
           integrableObject = mol->nextIntegrableObject(j)) {	
	localSites_.push_back(integrableObject);
      }
    }   
  }  
   
  void SMIPDForceManager::postCalculation(bool needStress){
    SimInfo::MoleculeIterator i;
    Molecule::IntegrableObjectIterator  j;
    Molecule* mol;
    StuntDouble* integrableObject;
  
    // Compute surface Mesh
    surfaceMesh_->computeHull(localSites_);

    // Get total area and number of surface stunt doubles
    RealType area = surfaceMesh_->getArea();
    std::vector<Triangle> sMesh = surfaceMesh_->getMesh();
    int nTriangles = sMesh.size();

    // Generate all of the necessary random forces
    std::vector<Vector3d>  randNums = genTriangleForces(nTriangles, variance_);

    // Loop over the mesh faces and apply external pressure to each 
    // of the faces
    std::vector<Triangle>::iterator face;
    std::vector<StuntDouble*>::iterator vertex;
    int thisFacet = 0;
    for (face = sMesh.begin(); face != sMesh.end(); ++face){
      Triangle thisTriangle = *face;
      std::vector<StuntDouble*> vertexSDs = thisTriangle.getVertices();
      RealType thisArea = thisTriangle.getArea(); 
      Vector3d unitNormal = thisTriangle.getNormal();
      unitNormal.normalize();
      Vector3d centroid = thisTriangle.getCentroid();
      Vector3d facetVel = thisTriangle.getFacetVelocity();
      RealType thisMass = thisTriangle.getFacetMass();
      Mat3x3d hydroTensor = thisTriangle.computeHydrodynamicTensor(viscosity_);
      
      hydroTensor *= PhysicalConstants::viscoConvert;
      Mat3x3d S;
      CholeskyDecomposition(hydroTensor, S);
      
      Vector3d extPressure = -unitNormal * (targetPressure_ * thisArea) /
        PhysicalConstants::energyConvert;

      Vector3d randomForce = S * randNums[thisFacet++];
      Vector3d dragForce = -hydroTensor * facetVel;

      Vector3d langevinForce = (extPressure + randomForce + dragForce);
      
      // Apply triangle force to stuntdouble vertices
      for (vertex = vertexSDs.begin(); vertex != vertexSDs.end(); ++vertex){
	if ((*vertex) != NULL){
	  Vector3d vertexForce = langevinForce / 3.0;
	  (*vertex)->addFrc(vertexForce);	   
	}  
      }
    } 
    
    veloMunge->removeComDrift();
    veloMunge->removeAngularDrift();
    
    Snapshot* currSnapshot = info_->getSnapshotManager()->getCurrentSnapshot();
    currSnapshot->setVolume(surfaceMesh_->getVolume());    
    ForceManager::postCalculation(needStress);   
  }
  
  
  std::vector<Vector3d> SMIPDForceManager::genTriangleForces(int nTriangles, 
                                                             RealType variance)
  {
    
    // zero fill the random vector before starting:
    std::vector<Vector3d> gaussRand;
    gaussRand.resize(nTriangles);
    std::fill(gaussRand.begin(), gaussRand.end(), V3Zero);
    
#ifdef IS_MPI
    if (worldRank == 0) {
#endif
      for (int i = 0; i < nTriangles; i++) {
        gaussRand[i][0] = randNumGen_.randNorm(0.0, variance);
        gaussRand[i][1] = randNumGen_.randNorm(0.0, variance);
        gaussRand[i][2] = randNumGen_.randNorm(0.0, variance);
      }
#ifdef IS_MPI
    }
#endif
    
    // push these out to the other processors
    
#ifdef IS_MPI
    if (worldRank == 0) {
      MPI::COMM_WORLD.Bcast(&gaussRand[0], nTriangles*3, MPI::REALTYPE, 0);
    } else {
      MPI::COMM_WORLD.Bcast(&gaussRand[0], nTriangles*3, MPI::REALTYPE, 0);
    }
#endif
    
    return gaussRand;
  }
}
