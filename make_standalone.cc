/**
 * @file make_standalone.cc
 * @brief This file contains the main function for the standalone CAF generator.
 */
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <ctype.h>
#include <spine/error.hpp>
#include <spine/reader.hpp>

#include "include/record_fillers.h"

#include "duneanaobj/StandardRecord/StandardRecord.h"
#include "duneanaobj/StandardRecord/SRInteraction.h"
#include "duneanaobj/StandardRecord/SRRecoParticle.h"
#include "duneanaobj/StandardRecord/SRTrueInteraction.h"
#include "duneanaobj/StandardRecord/SRTrueParticle.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

int main(int argc, char const *argv[])
{
    if(argc < 4)
    {
        std::cerr << "Usage: ./make_standalone <output file> <event offset> <input file(s)>" << std::endl;
        return 0;
    }

    TFile caf(argv[1], "recreate");
    TTree rec_tree("recTree", "Event tree for ML reconstruction");
    // Value-initialise: StandardRecord members without a default (beam.ismc,
    // common.ixn.npandora, common.ixn.nsandreco) would otherwise hold garbage.
    caf::StandardRecord *rec = new caf::StandardRecord();
    #ifdef MC_NOT_DATA
    rec->beam.ismc = true;
    #endif
    rec_tree.Branch("rec", &rec);

    TH1F * pot = new TH1F("TotalPOT", "TotalPOT", 1, 0, 1);
    TH1F * nevt = new TH1F("TotalEvents", "TotalEvents", 1, 0, 1);

    for(size_t n(3); n < argc; ++n)
    {
        spine::ReaderOptions options;
        options.products = {"run_info", "reco_particles", "reco_interactions"};
        #ifdef MC_NOT_DATA
        options.products.insert(options.products.end(), {"truth_particles", "truth_interactions"});
        #endif
        std::unique_ptr<spine::Reader> file;
        try
        {
            file = std::make_unique<spine::Reader>(argv[n], options);
        }
        catch(const spine::Error & e)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        std::cout << "Opened file: " << argv[n] << std::endl;
        std::cout << "Found " << file->num_events() << " events. Starting event loop..." << std::endl;

        for(uint64_t i_evt = 0; i_evt < file->num_events(); ++i_evt)
        {
            std::cout << "Processing event " << i_evt << "..." << std::endl;
            rec->common.ixn.dlp.clear();
            rec->common.ixn.ndlp = 0;
            rec->mc.nu.clear();
            rec->mc.nnu = 0;
            try
            {
                const spine::Event evt = file->event(i_evt);
                std::cout << "  Calling package_event..." << std::endl;
                package_event(rec, evt, std::atoi(argv[2]));
                const spine::RunInfo * run_info = evt.run_info();
                if(!run_info)
                    throw spine::Error("no run_info for event " + std::to_string(i_evt));

                // For standalone FD CAFs, we default to FD HD
                rec->meta.fd_hd.run = run_info->run;
                rec->meta.fd_hd.subrun = run_info->subrun;
                rec->meta.fd_hd.event = run_info->event;
                rec->meta.fd_hd.enabled = true;
                
                pot->Fill(1);
                nevt->Fill(1);
                rec_tree.Fill();
            }
            catch(const spine::Error & e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    pot->Write();
    nevt->Write();
    rec_tree.Write();
    caf.Close();

    return 0;
}
