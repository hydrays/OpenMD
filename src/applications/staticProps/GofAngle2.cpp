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

#include <algorithm>
#include <fstream>
#include "applications/staticProps/GofAngle2.hpp"
#include "utils/simError.h"

namespace oopse {

GofAngle2::GofAngle2(SimInfo* info, const std::string& filename, const std::string& sele1, 
    const std::string& sele2, int nangleBins)
    : RadialDistrFunc(info, filename, sele1, sele2), nAngleBins_(nangleBins) {

    setOutputName(getPrefix(filename) + ".gto");

    deltaCosAngle_ = 2.0 / nAngleBins_;

    histogram_.resize(nAngleBins_);
    avgGofr_.resize(nAngleBins_);
    for (int i = 0 ; i < nAngleBins_; ++i) {
        histogram_[i].resize(nAngleBins_);
        avgGofr_[i].resize(nAngleBins_);
    }    

}


void GofAngle2::preProcess() {

    for (int i = 0; i < avgGofr_.size(); ++i) {
        std::fill(avgGofr_[i].begin(), avgGofr_[i].end(), 0);
    }
}

void GofAngle2::initalizeHistogram() {
    npairs_ = 0;
    for (int i = 0; i < histogram_.size(); ++i)
        std::fill(histogram_[i].begin(), histogram_[i].end(), 0);
}


void GofAngle2::processHistogram() {

    //std::for_each(avgGofr_.begin(), avgGofr_.end(), std::plus<std::vector<int>>)

}

void GofAngle2::collectHistogram(StuntDouble* sd1, StuntDouble* sd2) {

    if (sd1 == sd2) {
        return;
    }

    Vector3d pos1 = sd1->getPos();
    Vector3d pos2 = sd2->getPos();
    Vector3d r12 = pos1 - pos2;
    currentSnapshot_->wrapVector(r12);
    Vector3d dipole1 = sd1->getElectroFrame().getColumn(2);
    Vector3d dipole2 = sd2->getElectroFrame().getColumn(2);
    
    r12.normalize();
    dipole1.normalize();    
    dipole2.normalize();    
    

    double cosAngle1 = dot(r12, dipole1);
    double cosAngle2 = dot(dipole1, dipole2);

   double halfBin = (nAngleBins_ - 1) * 0.5;
    int angleBin1 = halfBin * (cosAngle1 + 1.0);
    int angleBin2 = halfBin * (cosAngle1 + 1.0);

    ++histogram_[angleBin1][angleBin1];    
    ++npairs_;
}

void GofAngle2::writeRdf() {
    std::ofstream rdfStream(outputFilename_.c_str());
    if (rdfStream.is_open()) {
        rdfStream << "#radial distribution function\n";
        rdfStream << "#selection1: (" << selectionScript1_ << ")\t";
        rdfStream << "selection2: (" << selectionScript2_ << ")\n";
        rdfStream << "#nAngleBins =" << nAngleBins_ << "deltaCosAngle = " << deltaCosAngle_ << "\n";
        for (int i = 0; i < avgGofr_.size(); ++i) {
            double cosAngle1 = -1.0 + (i + 0.5)*deltaCosAngle_;

            for(int j = 0; j < avgGofr_[i].size(); ++j) {
                double cosAngle2 = -1.0 + (j + 0.5)*deltaCosAngle_;
                rdfStream <<avgGofr_[i][j]/nProcessed_ << "\t";
            }

            rdfStream << "\n";
        }
        
    } else {

        sprintf(painCave.errMsg, "GofAngle2: unable to open %s\n", outputFilename_.c_str());
        painCave.isFatal = 1;
        simError();  
    }

    rdfStream.close();
}

}