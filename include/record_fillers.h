/**
 * @file record_fillers.h
 * @brief Declaration of various functions for filling the SPINE reconstruction
 * output objects for use within the CAF files.
 * @details This file contains the declaration of the functions that are
 * responsible for copying the SPINE reconstruction output data products, as
 * read by spine-reader, into the CAF objects. The functions are designed to be
 * called from either the standalone CAF generator or the merged CAF generator.
 * @author mueller@fnal.gov
 */
#ifndef RECORD_FILLERS_H
#define RECORD_FILLERS_H
#include <vector>
#include <map>
#include <ctype.h>

#include <spine/event.hpp>

#include "duneanaobj/StandardRecord/StandardRecord.h"
#include "duneanaobj/StandardRecord/SRInteraction.h"
#include "duneanaobj/StandardRecord/SRRecoParticle.h"
#include "duneanaobj/StandardRecord/SRTrueInteraction.h"
#include "duneanaobj/StandardRecord/SRTrueParticle.h"

/**
 * @brief Constructs an instance of the caf::SRTrueParticle class from the
 * data in the spine::TruthParticle data product.
 */
caf::SRTrueParticle fill_truth_particle(const spine::TruthParticle &p, uint64_t offset=0);

/**
 * @brief Constructs an instance of the caf::SRRecoParticle class from the data
 * in the spine::RecoParticle data product.
 */
caf::SRRecoParticle fill_particle(const spine::RecoParticle &p, const std::map<int64_t, caf::TrueParticleID>& match_map, uint64_t offset=0);

/**
 * @brief Constructs an instance of the caf::SRTrueInteraction class from
 * the data in the spine::TruthInteraction data product.
 */
caf::SRTrueInteraction fill_truth_interaction(const spine::TruthInteraction &in, std::vector<caf::SRTrueParticle> &particles, uint64_t offset=0);

/**
 * @brief Constructs an instance of the caf::SRInteraction class from the
 * data in the spine::RecoInteraction data product.
 */
caf::SRInteraction fill_interaction(const spine::RecoInteraction &in, std::vector<caf::SRRecoParticle> &particles, const std::map<int64_t, int>& match_map, uint64_t offset=0);

/**
 * @brief Populates the StandardRecord object with the ML reconstruction
 * outputs of one SPINE event.
 */
void package_event(caf::StandardRecord * rec, const spine::Event & evt, uint64_t offset=0);

#endif
