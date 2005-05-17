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
 
#include "io/GayBerneAtomTypesSectionParser.hpp"
#include "types/AtomType.hpp"
#include "types/DirectionalAtomType.hpp"
#include "UseTheForce/ForceField.hpp"
#include "utils/simError.h"


namespace oopse {

GayBerneAtomTypesSectionParser::GayBerneAtomTypesSectionParser() {
    setSectionName("GayBerneAtomTypes");
}

void GayBerneAtomTypesSectionParser::parseLine(ForceField& ff,const std::string& line, int lineNo){
    StringTokenizer tokenizer(line);
    int nTokens = tokenizer.countTokens();    
    //in AtomTypeSection, a line at least contains 8 tokens
    //atomTypeName and 6 different gayBerne parameters
    if (nTokens < 1)  {
        sprintf(painCave.errMsg, "GayBerneAtomTypesSectionParser Error: Not enough tokens at line %d\n",
                lineNo);
        painCave.isFatal = 1;
        simError();                      
    } else {

        std::string atomTypeName = tokenizer.nextToken();    
        AtomType* atomType = ff.getAtomType(atomTypeName);
        if (atomType != NULL) {
            DirectionalAtomType* dAtomType = dynamic_cast<DirectionalAtomType*>(atomType);
            
            if (dAtomType != NULL) {
                GayBerneParam gayBerne;
                gayBerne.GB_sigma = tokenizer.nextTokenAsDouble();
                gayBerne.GB_12b_ratio = tokenizer.nextTokenAsDouble();  
                gayBerne.GB_eps = tokenizer.nextTokenAsDouble();
                gayBerne.GB_eps_ratio = tokenizer.nextTokenAsDouble();
                gayBerne.GB_mu = tokenizer.nextTokenAsDouble();
                gayBerne.GB_nu = tokenizer.nextTokenAsDouble();
 
                
                dAtomType->addProperty(new GayBerneParamGenericData("GayBerne", gayBerne));
                dAtomType->setGayBerne();
            } else {
                sprintf(painCave.errMsg, "GayBerneAtomTypesSectionParser Error: Not enough tokens at line %d\n",
                        lineNo);
                painCave.isFatal = 1;
                simError();            
                std::cerr << "GayBerneAtomTypesSectionParser Warning:" << std::endl;
            }
        } else {
            sprintf(painCave.errMsg, "GayBerneAtomTypesSectionParser Error: Can not find matched AtomType %s\n",
                    atomTypeName.c_str());
            painCave.isFatal = 1;
            simError();    
        
        }
                       
    }    


}

} //end namespace oopse