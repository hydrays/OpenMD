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
 
/**
 * @file Molecule.hpp
 * @author    tlin
 * @date  10/25/2004
 * @version 1.0
 */ 

#ifndef PRIMITIVES_MOLECULE_HPP
#define PRIMITIVES_MOLECULE_HPP
#include <vector>
#include <iostream>

#include "constraints/ConstraintPair.hpp"
#include "math/Vector3.hpp"
#include "primitives/Atom.hpp"
#include "primitives/RigidBody.hpp"
#include "primitives/Bond.hpp"
#include "primitives/Bend.hpp"
#include "primitives/Torsion.hpp"
#include "primitives/CutoffGroup.hpp"

namespace oopse{

  class Constraint;

  /**
   * @class Molecule Molecule.hpp "primitives/Molecule.hpp"
   * @brief
   */
  class Molecule {
  public:

    typedef std::vector<Atom*>::iterator AtomIterator;
    typedef std::vector<Bond*>::iterator BondIterator;
    typedef std::vector<Bend*>::iterator BendIterator;
    typedef std::vector<Torsion*>::iterator TorsionIterator;
    typedef std::vector<RigidBody*>::iterator RigidBodyIterator;
    typedef std::vector<CutoffGroup*>::iterator CutoffGroupIterator;
    typedef std::vector<StuntDouble*>::iterator IntegrableObjectIterator;
    typedef std::vector<ConstraintPair*>::iterator ConstraintPairIterator;
    typedef std::vector<ConstraintElem*>::iterator ConstraintElemIterator;
    
    
    Molecule(int stampId, int globalIndex, const std::string& molName);
    virtual ~Molecule();
    
    /**
     * Returns the global index of this molecule.
     * @return  the global index of this molecule 
     */
    int getGlobalIndex() {
      return globalIndex_;
    }
    
    /**
     * Returns the stamp id of this molecule
     * @note Ideally, every molecule should keep a pointer of its
     * molecule stamp instead of its stamp id. However, the pointer
     * will become invalid, if the molecule migrate to other
     * processor.
     */
    int getStampId() {
      return stampId_;
    }
    
    /** Returns the name of the molecule */
    std::string getType() {
      return moleculeName_;
    }
    
    /**
     * Sets the global index of this molecule.
     * @param new global index to be set
     */
    void setGlobalIndex(int index) {
      globalIndex_ = index;
    }
    
    
    /** add an atom into this molecule */
    void addAtom(Atom* atom);
    
    /** add a bond into this molecule */
    void addBond(Bond* bond);
    
    /** add a bend into this molecule */
    void addBend(Bend* bend);
    
    /** add a torsion into this molecule*/
    void addTorsion(Torsion* torsion);
    
    /** add a rigidbody into this molecule */
    void addRigidBody(RigidBody *rb);
    
    /** add a cutoff group into this molecule */
    void addCutoffGroup(CutoffGroup* cp);     
    
    void addConstraintPair(ConstraintPair* consPair);
    
    void addConstraintElem(ConstraintElem* consElem);
    
    /** */
    void complete();
    
    /** Returns the total number of atoms in this molecule */
    unsigned int getNAtoms() {
      return atoms_.size();
    }
    
    /** Returns the total number of bonds in this molecule */        
    unsigned int getNBonds(){
      return bonds_.size();
    }
    
    /** Returns the total number of bends in this molecule */        
    unsigned int getNBends() {
      return bends_.size();
    }
    
    /** Returns the total number of torsions in this molecule */        
    unsigned int getNTorsions() {
      return torsions_.size();
    }
    
    /** Returns the total number of rigid bodies in this molecule */        
    unsigned int getNRigidBodies() {
      return rigidBodies_.size();
    }
    
    /** Returns the total number of integrable objects in this molecule */
    unsigned int getNIntegrableObjects() {
      return integrableObjects_.size();
    }
    
    /** Returns the total number of cutoff groups in this molecule */
    unsigned int getNCutoffGroups() {
      return cutoffGroups_.size();
    }
    
    /** Returns the total number of constraints in this molecule */
    unsigned int getNConstraintPairs() {
      return constraintPairs_.size();
    }
    
    Atom* getAtomAt(unsigned int i) {
      assert(i < atoms_.size());
      return atoms_[i];
    }
    
    RigidBody* getRigidBodyAt(unsigned int i) {
      assert(i < rigidBodies_.size());
      return rigidBodies_[i];
    }
    
    Atom* beginAtom(std::vector<Atom*>::iterator& i) {
      i = atoms_.begin();
      return (i == atoms_.end()) ? NULL : *i;
    }
    
    Atom* nextAtom(std::vector<Atom*>::iterator& i) {
      ++i;
      return (i == atoms_.end()) ? NULL : *i;    
    }
    
    Bond* beginBond(std::vector<Bond*>::iterator& i) {
      i = bonds_.begin();
      return (i == bonds_.end()) ? NULL : *i;
    }
    
    Bond* nextBond(std::vector<Bond*>::iterator& i) {
      ++i;
      return (i == bonds_.end()) ? NULL : *i;    
      
    }
    
    Bend* beginBend(std::vector<Bend*>::iterator& i) {
      i = bends_.begin();
      return (i == bends_.end()) ? NULL : *i;
    }
    
    Bend* nextBend(std::vector<Bend*>::iterator& i) {
      ++i;
      return (i == bends_.end()) ? NULL : *i;    
    }
    
    Torsion* beginTorsion(std::vector<Torsion*>::iterator& i) {
      i = torsions_.begin();
      return (i == torsions_.end()) ? NULL : *i;
    }
    
    Torsion* nextTorsion(std::vector<Torsion*>::iterator& i) {
      ++i;
      return (i == torsions_.end()) ? NULL : *i;    
    }    
    
    RigidBody* beginRigidBody(std::vector<RigidBody*>::iterator& i) {
      i = rigidBodies_.begin();
      return (i == rigidBodies_.end()) ? NULL : *i;
    }
    
    RigidBody* nextRigidBody(std::vector<RigidBody*>::iterator& i) {
      ++i;
      return (i == rigidBodies_.end()) ? NULL : *i;    
    }
    
    StuntDouble* beginIntegrableObject(std::vector<StuntDouble*>::iterator& i) {
      i = integrableObjects_.begin();
      return (i == integrableObjects_.end()) ? NULL : *i;
    }
    
    StuntDouble* nextIntegrableObject(std::vector<StuntDouble*>::iterator& i) {
      ++i;
      return (i == integrableObjects_.end()) ? NULL : *i;    
    }    
    
    CutoffGroup* beginCutoffGroup(std::vector<CutoffGroup*>::iterator& i) {
      i = cutoffGroups_.begin();
      return (i == cutoffGroups_.end()) ? NULL : *i;
    }

    CutoffGroup* nextCutoffGroup(std::vector<CutoffGroup*>::iterator& i) {
      ++i;
      return (i == cutoffGroups_.end()) ? NULL : *i;    
    } 
    
    ConstraintPair* beginConstraintPair(std::vector<ConstraintPair*>::iterator& i) {
      i = constraintPairs_.begin();
      return (i == constraintPairs_.end()) ? NULL : *i;
    }
    
    ConstraintPair* nextConstraintPair(std::vector<ConstraintPair*>::iterator& i) {            
      ++i;
      return (i == constraintPairs_.end()) ? NULL : *i;    
    }         
    
    ConstraintElem* beginConstraintElem(std::vector<ConstraintElem*>::iterator& i) {
      i = constraintElems_.begin();
      return (i == constraintElems_.end()) ? NULL : *i;
    }
    
    ConstraintElem* nextConstraintElem(std::vector<ConstraintElem*>::iterator& i) {            
      ++i;
      return (i == constraintElems_.end()) ? NULL : *i;    
    }
    
    /** 
     * Returns the total potential energy of short range interaction
     * of this molecule 
     */    
    RealType getPotential();
    
    /** get total mass of this molecule */        
    RealType getMass();
    
    /** return the center of mass of this molecule */
    Vector3d getCom();
    
    /** Moves the center of this molecule */
    void moveCom(const Vector3d& delta);
    
    /** Returns the velocity of center of mass of this molecule */
    Vector3d getComVel();

    std::string getMoleculeName() { 
      return moleculeName_;
    }
        
    friend std::ostream& operator <<(std::ostream& o, Molecule& mol);
    
  private:
    
    int globalIndex_;
    
    std::vector<Atom*> atoms_;
    std::vector<Bond*> bonds_;
    std::vector<Bend*> bends_;
    std::vector<Torsion*> torsions_;
    std::vector<RigidBody*> rigidBodies_;
    std::vector<StuntDouble*> integrableObjects_;
    std::vector<CutoffGroup*> cutoffGroups_;
    std::vector<ConstraintPair*> constraintPairs_;
    std::vector<ConstraintElem*> constraintElems_;
    int stampId_;
    std::string moleculeName_;
  };

} //namespace oopse
#endif //
