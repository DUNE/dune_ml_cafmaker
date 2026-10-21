/**
 * @file true_particle.cc
 * @brief Implementation of the TruthParticle class.
 * @author mueller@fnal.gov
*/
#include <array>
#include "H5Cpp.h"
#include "true_particle.h"
#include "enums.h"
#include <iostream>

namespace dlp::types
{
    /**
     * @brief Synchronize the BufferView objects with the current memory
     * locations.
    */
    void TruthParticle::SyncVectors()
    {
        children_counts.reset(&children_counts_handle);
        children_id.reset(&children_id_handle);
        fragment_ids.reset(&fragment_ids_handle);
        match_ids.reset(&match_ids_handle);
        match_overlaps.reset(&match_overlaps_handle);
        module_ids.reset(&module_ids_handle);
        orig_children_id.reset(&orig_children_id_handle);
    }

    /**
     * @brief Build the HDF5 compound type for the TruthParticle class.
     * 
     * The composite type for the TruthParticle class needs to be defined.
     * defined. This is handled using a template specialization of the
     * BuildCompType() function.
     * @return The HDF5 compound type for the TruthParticle class.
    */
    template <>
    H5::CompType BuildCompType<TruthParticle>(const H5::CompType* fileType)
    {
        // Set the size of the compound type to the size of the TruthParticle.
        H5::CompType ctype(sizeof(TruthParticle));

        // Create a string type for the string fields in the TruthParticle.
        H5::StrType string_type(H5::PredType::C_S1, H5T_VARIABLE);
        string_type.setCset(H5T_CSET_UTF8);

        // Create the H5::EnumType objects for the enums used in the TruthParticle.
        H5::EnumType pid_enumtype(create_pid_enumtype());
        H5::EnumType semantic_type_enumtype(create_shape_enumtype());

        // Add the members of the TruthParticle to the compound type.
        insert_adaptive(ctype, fileType, "ancestor_creation_process", HOFFSET(TruthParticle, ancestor_creation_process), string_type);
        insert_adaptive(ctype, fileType, "ancestor_pdg_code", HOFFSET(TruthParticle, ancestor_pdg_code), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "ancestor_position", HOFFSET(TruthParticle, ancestor_position), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "ancestor_t", HOFFSET(TruthParticle, ancestor_t), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "ancestor_track_id", HOFFSET(TruthParticle, ancestor_track_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "calo_ke", HOFFSET(TruthParticle, calo_ke), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "cathode_offset", HOFFSET(TruthParticle, cathode_offset), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "children_counts", HOFFSET(TruthParticle, children_counts_handle), H5::VarLenType(H5::PredType::STD_I64LE));
        insert_adaptive(ctype, fileType, "children_id", HOFFSET(TruthParticle, children_id_handle), H5::VarLenType(H5::PredType::STD_I64LE));
        insert_adaptive(ctype, fileType, "creation_process", HOFFSET(TruthParticle, creation_process), string_type);
        insert_adaptive(ctype, fileType, "csda_ke", HOFFSET(TruthParticle, csda_ke), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "csda_ke_per_pid", HOFFSET(TruthParticle, csda_ke_per_pid), H5::ArrayType(H5::PredType::IEEE_F64LE,  1, &std::array<hsize_t, 1>{6}[0]));
	    insert_adaptive(ctype, fileType, "depositions_adapt_q_sum", HOFFSET(TruthParticle, depositions_adapt_q_sum), H5::PredType::IEEE_F32LE);
        insert_adaptive(ctype, fileType, "depositions_adapt_sum", HOFFSET(TruthParticle, depositions_adapt_sum), H5::PredType::IEEE_F32LE);
	    insert_adaptive(ctype, fileType, "depositions_g4_sum", HOFFSET(TruthParticle, depositions_g4_sum), H5::PredType::IEEE_F32LE);
	    insert_adaptive(ctype, fileType, "depositions_q_sum", HOFFSET(TruthParticle, depositions_q_sum), H5::PredType::IEEE_F32LE);
        insert_adaptive(ctype, fileType, "depositions_sum", HOFFSET(TruthParticle, depositions_sum), H5::PredType::IEEE_F32LE);
	    insert_adaptive(ctype, fileType, "distance_travel", HOFFSET(TruthParticle, distance_travel), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "end_dir", HOFFSET(TruthParticle, end_dir), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "end_momentum", HOFFSET(TruthParticle, end_momentum), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "end_p", HOFFSET(TruthParticle, end_p), H5::PredType::IEEE_F32LE);
        insert_adaptive(ctype, fileType, "end_point", HOFFSET(TruthParticle, end_point), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "end_position", HOFFSET(TruthParticle, end_position), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
	    insert_adaptive(ctype, fileType, "end_t", HOFFSET(TruthParticle, end_t), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "energy_deposit", HOFFSET(TruthParticle, energy_deposit), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "energy_init", HOFFSET(TruthParticle, energy_init), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "first_step", HOFFSET(TruthParticle, first_step), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));	
        insert_adaptive(ctype, fileType, "fragment_ids", HOFFSET(TruthParticle, fragment_ids_handle), H5::VarLenType(H5::PredType::STD_I32LE));
        insert_adaptive(ctype, fileType, "group_primary", HOFFSET(TruthParticle, group_primary), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "id", HOFFSET(TruthParticle, id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "interaction_id", HOFFSET(TruthParticle, interaction_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "interaction_primary", HOFFSET(TruthParticle, interaction_primary), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "is_cathode_crosser", HOFFSET(TruthParticle, is_cathode_crosser), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_contained", HOFFSET(TruthParticle, is_contained), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_matched", HOFFSET(TruthParticle, is_matched), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_primary", HOFFSET(TruthParticle, is_primary), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_time_contained", HOFFSET(TruthParticle, is_time_contained), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_truth", HOFFSET(TruthParticle, is_truth), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "is_valid", HOFFSET(TruthParticle, is_valid), H5::PredType::STD_U8LE);
        insert_adaptive(ctype, fileType, "ke", HOFFSET(TruthParticle, ke), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "last_step", HOFFSET(TruthParticle, last_step), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "length", HOFFSET(TruthParticle, length), H5::PredType::IEEE_F64LE);
	    insert_adaptive(ctype, fileType, "mass", HOFFSET(TruthParticle, mass), H5::PredType::IEEE_F64LE);	
        insert_adaptive(ctype, fileType, "match_ids", HOFFSET(TruthParticle, match_ids_handle), H5::VarLenType(H5::PredType::STD_I64LE));
        insert_adaptive(ctype, fileType, "match_overlaps", HOFFSET(TruthParticle, match_overlaps_handle), H5::VarLenType(H5::PredType::IEEE_F32LE));
        insert_adaptive(ctype, fileType, "mcs_ke", HOFFSET(TruthParticle, mcs_ke), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "mcs_ke_per_pid", HOFFSET(TruthParticle, mcs_ke_per_pid), H5::ArrayType(H5::PredType::IEEE_F64LE,  1, &std::array<hsize_t, 1>{6}[0]));
        insert_adaptive(ctype, fileType, "mcst_index", HOFFSET(TruthParticle, mcst_index), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "mct_index", HOFFSET(TruthParticle, mct_index), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "module_ids", HOFFSET(TruthParticle, module_ids_handle), H5::VarLenType(H5::PredType::STD_I64LE));
        insert_adaptive(ctype, fileType, "momentum", HOFFSET(TruthParticle, momentum), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "nu_id", HOFFSET(TruthParticle, nu_id), H5::PredType::STD_I64LE);
	    insert_adaptive(ctype, fileType, "num_fragments", HOFFSET(TruthParticle, num_fragments), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "num_voxels", HOFFSET(TruthParticle, num_voxels), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "orig_children_id", HOFFSET(TruthParticle, orig_children_id_handle), H5::VarLenType(H5::PredType::STD_I64LE));
        insert_adaptive(ctype, fileType, "orig_group_id", HOFFSET(TruthParticle, orig_group_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "orig_id", HOFFSET(TruthParticle, orig_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "orig_interaction_id", HOFFSET(TruthParticle, orig_interaction_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "orig_parent_id", HOFFSET(TruthParticle, orig_parent_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "p", HOFFSET(TruthParticle, p), H5::PredType::IEEE_F32LE);
        insert_adaptive(ctype, fileType, "parent_creation_process", HOFFSET(TruthParticle, parent_creation_process), string_type);
        insert_adaptive(ctype, fileType, "parent_id", HOFFSET(TruthParticle, parent_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "parent_pdg_code", HOFFSET(TruthParticle, parent_pdg_code), H5::PredType::STD_I64LE);	
        insert_adaptive(ctype, fileType, "parent_position", HOFFSET(TruthParticle, parent_position), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "parent_t", HOFFSET(TruthParticle, parent_t), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "parent_track_id", HOFFSET(TruthParticle, parent_track_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "pdg_code", HOFFSET(TruthParticle, pdg_code), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "pid", HOFFSET(TruthParticle, pid), pid_enumtype);
        insert_adaptive(ctype, fileType, "position", HOFFSET(TruthParticle, position), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "reco_end_dir", HOFFSET(TruthParticle, reco_end_dir), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "reco_ke", HOFFSET(TruthParticle, reco_ke), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "reco_length", HOFFSET(TruthParticle, reco_length), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "reco_momentum", HOFFSET(TruthParticle, reco_momentum), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "reco_start_dir", HOFFSET(TruthParticle, reco_start_dir), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "shape", HOFFSET(TruthParticle, shape), semantic_type_enumtype);	
        insert_adaptive(ctype, fileType, "size", HOFFSET(TruthParticle, size), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "size_adapt", HOFFSET(TruthParticle, size_adapt), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "size_g4", HOFFSET(TruthParticle, size_g4), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "start_dir", HOFFSET(TruthParticle, start_dir), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "start_point", HOFFSET(TruthParticle, start_point), H5::ArrayType(H5::PredType::IEEE_F32LE,  1, &std::array<hsize_t, 1>{3}[0]));
        insert_adaptive(ctype, fileType, "t", HOFFSET(TruthParticle, t), H5::PredType::IEEE_F64LE);
        insert_adaptive(ctype, fileType, "track_id", HOFFSET(TruthParticle, track_id), H5::PredType::STD_I64LE);
        insert_adaptive(ctype, fileType, "units", HOFFSET(TruthParticle, units), string_type);
        
        return ctype;
    }
} // namespace dlp::types
