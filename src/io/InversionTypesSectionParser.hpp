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
 
#ifndef IO_INVERSIONTYPESSECTIONPARSER_HPP
#define IO_INVERSIONTYPESSECTIONPARSER_HPP
#include <map>
#include "io/SectionParser.hpp"
#include "io/ForceFieldOptions.hpp"

namespace oopse {

  /**
   * @class InversionTypesSectionParser InversionTypesSectionParser.hpp "io/InversionTypesSectionParser.hpp"
   */
  class InversionTypesSectionParser : public SectionParser {
  public:

            
    InversionTypesSectionParser(ForceFieldOptions& options);
  private:


    // Inversion types vary by force field:  In this description,
    // I is the central atom, while J, K, and L are atoms directly 
    // bonded to I (but not to each other):

    // Amber uses a special bond (IL) as the hinge between the planes
    // IJL and IKL (the central atom I & peripheral atom L are common
    // in both planes).  It then applies a cosine series much like
    // other torsional forms.

    // Gromacs & Charmm use an improper torsion that is harmonic
    // in the angle between the IJK and JKL planes (Central atom is I, but
    // this atom appears in only one of the plane definitions.

    // MM2 and Tripos use a planarity definition of the central atom (I)
    // distance from the plane made by the other three atoms (JKL).

    // The Dreiding force field uses a complicated umbrella inversion 
    // form.


    enum InversionTypeEnum{
      itImproperCosine,
      itImproperHarmonic,
      itCentralAtomHeight,
      itAmberImproper,
      itDreiding,
      itUnknown
    };

    InversionTypeEnum getInversionTypeEnum(const std::string& str);
            
    void parseLine(ForceField& ff, const std::string& line, int lineNo);
            
    std::map<std::string, InversionTypeEnum> stringToEnumMap_;
    ForceFieldOptions& options_;
  };


} //namespace oopse

#endif //IO_INVERSIONTYPESSECTIONPARSER_HPP