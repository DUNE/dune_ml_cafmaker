/**
 * @file products.cc
 * @brief Implementation of the functions for interacting with and retrieving
 * data from H5 files.
 * @author mueller@fnal.gov
*/
#include <vector>
#include <iostream>
#include "H5Cpp.h"
#include "products.h"
#include "event.h"
#include "composites.h"

/**
 * @brief Checks the dimensions of the H5 DataSpace and calculates the number
 * of events present.
 * @param dsp the H5 DataSpace.
 * @return the number of events in the DataSpace.
*/
size_t get_nevents(H5::DataSpace & dsp)
{
    hsize_t dims_max = dsp.getSimpleExtentNdims();
    auto dims = new hsize_t[dims_max];
    dsp.getSimpleExtentDims(dims);
    size_t nevents(dims[0]); 
    delete[] dims;
    return nevents;
}

/**
 * @brief Retrieves the specified dlp::types::Event object from the H5 file.
 * @param file the input H5 file.
 * @param index of the event to retrieve.
 * @return the specified dlp::types::Event object.
*/
dlp::types::Event get_single_event(H5::H5File & file, int64_t index)
{
    H5::DataSet dataset(file.openDataSet("events"));
    H5::DataSpace dsp(dataset.getSpace());

    std::vector<dlp::types::Event> evt(1);
    hsize_t start[1] = { (hsize_t)index };
    hsize_t count[1] = { 1 };
    dsp.selectHyperslab(H5S_SELECT_SET, count, start);

    H5::DataSpace memspace(1, count);

    H5::CompType ctype(dlp::types::BuildCompType<dlp::types::Event>());
    dataset.read(evt.data(), ctype, memspace, dsp);

    return evt[0];
}

/**
 * @brief Retrieves all objects of class dlp::types::Event from the H5 file.
 * @param file the input H5 file.
 * @return a vector of all dlp::types::Event objects in the file.
*/
std::vector<dlp::types::Event> get_all_events(H5::H5File & file)
{
    std::cout << "  Inside get_all_events: opening dataset 'events'..." << std::endl;
    H5::DataSet dataset(file.openDataSet("events"));
    H5::DataSpace dsp(dataset.getSpace());

    std::vector<dlp::types::Event> evt;
    size_t n = get_nevents(dsp);
    std::cout << "  Inside get_all_events: n_events = " << n << ". Resizing vector..." << std::endl;
    evt.resize(n);
    H5::CompType ctype(dlp::types::BuildCompType<dlp::types::Event>());
    std::cout << "  Inside get_all_events: reading dataset..." << std::endl;
    dataset.read(evt.data(), ctype, H5::DataSpace::ALL, H5::DataSpace::ALL);
    std::cout << "  Inside get_all_events: finished reading." << std::endl;
    return evt;
}

/**
 * @brief Retrieves all products of a certain type from the H5 file.
 * @tparam T the type of product to retrieve.
 * @param file the input H5 file.
 * @param evt the dlp::types::Event object containing the references to the
 * requested products.
*/
template <class T>
std::vector<T> get_product(H5::H5File & file, dlp::types::Event & evt)
{
    hid_t file_id = file.getId();
    const hdset_reg_ref_t& ref = evt.GetRef<T>();
    
    bool all_zero = true;
    const unsigned char* p_ref = (const unsigned char*)&ref;
    for(size_t i=0; i<sizeof(hdset_reg_ref_t); ++i) if(p_ref[i] != 0) { all_zero = false; break; }
    if(all_zero) return std::vector<T>();

    hid_t dataset_id = H5Rdereference(file_id, H5P_DEFAULT, H5R_DATASET_REGION, &ref);
    if (dataset_id < 0) return std::vector<T>();
    H5::DataSet target_ds(dataset_id);
    
    hid_t region_id = H5Rget_region(file_id, H5R_DATASET_REGION, &ref);
    if (region_id < 0) return std::vector<T>();
    H5::DataSpace file_space(region_id);
    
    hssize_t npoints = file_space.getSelectNpoints();
    if (npoints <= 0) return std::vector<T>();
    
    hsize_t m_dims[1] = { (hsize_t)npoints };
    H5::DataSpace mem_space(1, m_dims);
    
    std::vector<T> data_product(npoints);
    H5::CompType ctype(dlp::types::BuildCompType<T>());
    target_ds.read(data_product.data(), ctype, mem_space, file_space);
    
    for (auto & item : data_product) {
        item.SyncVectors();
    }
    
    // Explicit cast to void* for H5Dvlen_reclaim
    // H5Dvlen_reclaim(ctype.getId(), mem_space.getId(), H5P_DEFAULT, (void*)data_product.data());
    
    return data_product;
}

/**
 * Explicit instantiation of the get_product function for the types of products
 * that we expect to use.
*/
template std::vector<dlp::types::RunInfo> get_product<dlp::types::RunInfo>(H5::H5File & file, dlp::types::Event & evt);
template std::vector<dlp::types::RecoInteraction> get_product<dlp::types::RecoInteraction>(H5::H5File & file, dlp::types::Event & evt);
template std::vector<dlp::types::RecoParticle> get_product<dlp::types::RecoParticle>(H5::H5File & file, dlp::types::Event & evt);
#ifdef MC_NOT_DATA
template std::vector<dlp::types::TruthInteraction> get_product<dlp::types::TruthInteraction>(H5::H5File & file, dlp::types::Event & evt);
template std::vector<dlp::types::TruthParticle> get_product<dlp::types::TruthParticle>(H5::H5File & file, dlp::types::Event & evt);
#endif
