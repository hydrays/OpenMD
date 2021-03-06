/*
 * Copyright (c) 2005 The University of Notre Dame. All Rights Reserved.
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
 * [3]  Sun, Lin & Gezelter, J. Chem. Phys. 128, 234107 (2008).
 * [4]  Kuang & Gezelter,  J. Chem. Phys. 133, 164101 (2010).
 * [5]  Vardeman, Stocker & Gezelter, J. Chem. Theory Comput. 7, 834 (2011).
 */

#include "selection/SelectionManager.hpp"
#include "brains/SimInfo.hpp"
namespace OpenMD {
  SelectionManager::SelectionManager(SimInfo* info) : info_(info){
    nObjects_.push_back(info_->getNGlobalAtoms()+
                        info_->getNGlobalRigidBodies());
    nObjects_.push_back(info_->getNGlobalBonds());
    nObjects_.push_back(info_->getNGlobalBends());
    nObjects_.push_back(info_->getNGlobalTorsions());
    nObjects_.push_back(info_->getNGlobalInversions());
    nObjects_.push_back(info_->getNGlobalMolecules());

    stuntdoubles_.resize(nObjects_[STUNTDOUBLE]);
    bonds_.resize(nObjects_[BOND]);
    bends_.resize(nObjects_[BEND]);
    torsions_.resize(nObjects_[TORSION]);
    inversions_.resize(nObjects_[INVERSION]);
    molecules_.resize(nObjects_[MOLECULE]);

    SimInfo::MoleculeIterator mi;
    Molecule::AtomIterator ai;
    Molecule::RigidBodyIterator rbIter;
    Molecule::BondIterator bondIter;
    Molecule::BendIterator bendIter;
    Molecule::TorsionIterator torsionIter;
    Molecule::InversionIterator inversionIter;

    Molecule* mol;
    Atom* atom;
    RigidBody* rb;
    Bond* bond;
    Bend* bend;
    Torsion* torsion;
    Inversion* inversion;    
        
    for (mol = info_->beginMolecule(mi); mol != NULL; 
         mol = info_->nextMolecule(mi)) {
      molecules_[mol->getGlobalIndex()] = mol;
      
      for(atom = mol->beginAtom(ai); atom != NULL; 
          atom = mol->nextAtom(ai)) {
	stuntdoubles_[atom->getGlobalIndex()] = atom;
      }  
      for (rb = mol->beginRigidBody(rbIter); rb != NULL; 
           rb = mol->nextRigidBody(rbIter)) {
	stuntdoubles_[rb->getGlobalIndex()] = rb;
      }   
      for (bond = mol->beginBond(bondIter); bond != NULL; 
           bond = mol->nextBond(bondIter)) {
	bonds_[bond->getGlobalIndex()] = bond;
      }   
      for (bend = mol->beginBend(bendIter); bend != NULL; 
           bend = mol->nextBend(bendIter)) {
	bends_[bend->getGlobalIndex()] = bend;
      }   
      for (torsion = mol->beginTorsion(torsionIter); torsion != NULL; 
           torsion = mol->nextTorsion(torsionIter)) {
	torsions_[torsion->getGlobalIndex()] = torsion;
      }   
      for (inversion = mol->beginInversion(inversionIter); inversion != NULL; 
           inversion = mol->nextInversion(inversionIter)) {
	inversions_[inversion->getGlobalIndex()] = inversion;
      }   
    }
  }

  StuntDouble* SelectionManager::beginSelected(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[STUNTDOUBLE].size()) {
      if (ss_.bitsets_[STUNTDOUBLE][i]) {
        // check that this processor owns this stuntdouble
        if (stuntdoubles_[i] != NULL) return stuntdoubles_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[STUNTDOUBLE].firstOnBit();
    return i == -1 ? NULL : stuntdoubles_[i];
#endif
  }

  StuntDouble* SelectionManager::nextSelected(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[STUNTDOUBLE].size()) {
      if (ss_.bitsets_[STUNTDOUBLE][i]) {
        // check that this processor owns this stuntdouble
        if (stuntdoubles_[i] != NULL) return stuntdoubles_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[STUNTDOUBLE].nextOnBit(i);
    return i == -1 ? NULL : stuntdoubles_[i];
#endif
  }

  StuntDouble* SelectionManager::beginUnselected(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[STUNTDOUBLE].size()) {
      if (!ss_.bitsets_[STUNTDOUBLE][i]) {
        // check that this processor owns this stuntdouble
        if (stuntdoubles_[i] != NULL) return stuntdoubles_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[STUNTDOUBLE].firstOffBit();
    return i == -1 ? NULL : stuntdoubles_[i];
#endif
  }

  StuntDouble* SelectionManager::nextUnselected(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[STUNTDOUBLE].size()) {
      if (!ss_.bitsets_[STUNTDOUBLE][i]) {
        // check that this processor owns this stuntdouble
        if (stuntdoubles_[i] != NULL) return stuntdoubles_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[STUNTDOUBLE].nextOffBit(i);
    return i == -1 ? NULL : stuntdoubles_[i];
#endif
  }

  Bond* SelectionManager::beginSelectedBond(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[BOND].size()) {
      if (ss_.bitsets_[BOND][i]) {
        // check that this processor owns this bond
        if (bonds_[i] != NULL) return bonds_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[BOND].firstOnBit();
    return i == -1 ? NULL : bonds_[i];
#endif
  }

  Bond* SelectionManager::nextSelectedBond(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[BOND].size()) {
      if (ss_.bitsets_[BOND][i]) {
        // check that this processor owns this bond
        if (bonds_[i] != NULL) return bonds_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[BOND].nextOnBit(i);
    return i == -1 ? NULL : bonds_[i];
#endif
  }

  Bond* SelectionManager::beginUnselectedBond(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[BOND].size()) {
      if (!ss_.bitsets_[BOND][i]) {
        // check that this processor owns this bond
        if (bonds_[i] != NULL) return bonds_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[BOND].firstOffBit();
    return i == -1 ? NULL : bonds_[i];
#endif
  }

  Bond* SelectionManager::nextUnselectedBond(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[BOND].size()) {
      if (!ss_.bitsets_[BOND][i]) {
        // check that this processor owns this bond
        if (bonds_[i] != NULL) return bonds_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[BOND].nextOffBit(i);
    return i == -1 ? NULL : bonds_[i];
#endif
  }


  Bend* SelectionManager::beginSelectedBend(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[BEND].size()) {
      if (ss_.bitsets_[BEND][i]) {
        // check that this processor owns this bend
        if (bends_[i] != NULL) return bends_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[BEND].firstOnBit();
    return i == -1 ? NULL : bends_[i];
#endif
  }

  Bend* SelectionManager::nextSelectedBend(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[BEND].size()) {
      if (ss_.bitsets_[BEND][i]) {
        // check that this processor owns this bend
        if (bends_[i] != NULL) return bends_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[BEND].nextOnBit(i);
    return i == -1 ? NULL : bends_[i];
#endif
  }

  Bend* SelectionManager::beginUnselectedBend(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[BEND].size()) {
      if (!ss_.bitsets_[BEND][i]) {
        // check that this processor owns this bend
        if (bends_[i] != NULL) return bends_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[BEND].firstOffBit();
    return i == -1 ? NULL : bends_[i];
#endif
  }

  Bend* SelectionManager::nextUnselectedBend(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[BEND].size()) {
      if (!ss_.bitsets_[BEND][i]) {
        // check that this processor owns this bend
        if (bends_[i] != NULL) return bends_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[BEND].nextOffBit(i);
    return i == -1 ? NULL : bends_[i];
#endif
  }

  Torsion* SelectionManager::beginSelectedTorsion(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[TORSION].size()) {
      if (ss_.bitsets_[TORSION][i]) {
        // check that this processor owns this torsion
        if (torsions_[i] != NULL) return torsions_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[TORSION].firstOnBit();
    return i == -1 ? NULL : torsions_[i];
#endif
  }

  Torsion* SelectionManager::nextSelectedTorsion(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[TORSION].size()) {
      if (ss_.bitsets_[TORSION][i]) {
        // check that this processor owns this torsion
        if (torsions_[i] != NULL) return torsions_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[TORSION].nextOnBit(i);
    return i == -1 ? NULL : torsions_[i];
#endif
  }

  Torsion* SelectionManager::beginUnselectedTorsion(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[TORSION].size()) {
      if (!ss_.bitsets_[TORSION][i]) {
        // check that this processor owns this torsion
        if (torsions_[i] != NULL) return torsions_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[TORSION].firstOffBit();
    return i == -1 ? NULL : torsions_[i];
#endif
  }

  Torsion* SelectionManager::nextUnselectedTorsion(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[TORSION].size()) {
      if (!ss_.bitsets_[TORSION][i]) {
        // check that this processor owns this torsion
        if (torsions_[i] != NULL) return torsions_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[TORSION].nextOffBit(i);
    return i == -1 ? NULL : torsions_[i];
#endif
  }


  Inversion* SelectionManager::beginSelectedInversion(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[INVERSION].size()) {
      if (ss_.bitsets_[INVERSION][i]) {
        // check that this processor owns this inversion
        if (inversions_[i] != NULL) return inversions_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[INVERSION].firstOnBit();
    return i == -1 ? NULL : inversions_[i];
#endif
  }

  Inversion* SelectionManager::nextSelectedInversion(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[INVERSION].size()) {
      if (ss_.bitsets_[INVERSION][i]) {
        // check that this processor owns this inversion
        if (inversions_[i] != NULL) return inversions_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[INVERSION].nextOnBit(i);
    return i == -1 ? NULL : inversions_[i];
#endif
  }

  Inversion* SelectionManager::beginUnselectedInversion(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[INVERSION].size()) {
      if (!ss_.bitsets_[INVERSION][i]) {
        // check that this processor owns this inversion
        if (inversions_[i] != NULL) return inversions_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[INVERSION].firstOffBit();
    return i == -1 ? NULL : inversions_[i];
#endif
  }

  Inversion* SelectionManager::nextUnselectedInversion(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[INVERSION].size()) {
      if (!ss_.bitsets_[INVERSION][i]) {
        // check that this processor owns this inversion
        if (inversions_[i] != NULL) return inversions_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[INVERSION].nextOffBit(i);
    return i == -1 ? NULL : inversions_[i];
#endif
  }
  
  Molecule* SelectionManager::beginSelectedMolecule(int& i) {
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[MOLECULE].size()) {
      if (ss_.bitsets_[MOLECULE][i]) {
        // check that this processor owns this molecule
        if (molecules_[i] != NULL) return molecules_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[MOLECULE].firstOnBit();
    return i == -1 ? NULL : molecules_[i];
#endif
  }

  Molecule* SelectionManager::nextSelectedMolecule(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[MOLECULE].size()) {
      if (ss_.bitsets_[MOLECULE][i]) {
        // check that this processor owns this molecule
        if (molecules_[i] != NULL) return molecules_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[MOLECULE].nextOnBit(i);
    return i == -1 ? NULL : molecules_[i];
#endif
  }

  Molecule* SelectionManager::beginUnselectedMolecule(int& i){
#ifdef IS_MPI
    i = 0;
    while (i < ss_.bitsets_[MOLECULE].size()) {
      if (!ss_.bitsets_[MOLECULE][i]) {
        // check that this processor owns this molecule
        if (molecules_[i] != NULL) return molecules_[i];
      }
      ++i;
    }
    return NULL;   
#else
    i = ss_.bitsets_[MOLECULE].firstOffBit();
    return i == -1 ? NULL : molecules_[i];
#endif
  }

  Molecule* SelectionManager::nextUnselectedMolecule(int& i) {
#ifdef IS_MPI
    ++i;
    while (i < ss_.bitsets_[MOLECULE].size()) {
      if (!ss_.bitsets_[MOLECULE][i]) {
        // check that this processor owns this molecule
        if (molecules_[i] != NULL) return molecules_[i];
      }
      ++i;
    }
    return NULL;
#else
    i = ss_.bitsets_[MOLECULE].nextOffBit(i);
    return i == -1 ? NULL : molecules_[i];
#endif
  }
  
  SelectionManager operator| (const SelectionManager& sman1, 
                              const SelectionManager& sman2) {
    SelectionManager result(sman1);
    result |= sman2;
    return result;
  }

  SelectionManager operator& (const SelectionManager& sman1, 
                              const SelectionManager& sman2) {
    SelectionManager result(sman1);
    result &= sman2;
    return result;
  }

  SelectionManager operator^ (const SelectionManager& sman1, 
                              const SelectionManager& sman2) {
    SelectionManager result(sman1);
    result ^= sman2;
    return result;
  }

  SelectionManager operator-(const SelectionManager& sman1, 
                             const SelectionManager& sman2){
    SelectionManager result(sman1);
    result -= sman2;
    return result;
  }

}
