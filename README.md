# Introduction
CAF (Common Analysis Framework) files are used in SBN analyses either through the CAFAna framework or through various standalone projects. In either case, they are a common starting point for many analyzers. The standard pathway for making CAFs only includes reconstruction outputs which are available in the artROOT files after the final step of the reconstruction chain.

The `SPINE` (Scalable Particle Imaging with Neural Embeddings) package ([Github](https://github.com/DeepLearnPhysics/spine)) implements an end-to-end reconstruction chain with a machine learning algorithms at the heart of point classification, clustering, and particle identification. The reconstruction chain uses LArCV files as input and produces HDF5 files with the outputs of the reconstruction. This necessarily takes place outside of the LArSoft ecosystem, so to make these reconstruction outputs available for analyzers requires a little extra work.

# Overview of `dune_ml_cafmaker`
The `dune_ml_cafmaker` package provides code for merging the ML reconstruction outputs into existing CAF files (enforcing event-by-event matching) and for producing standalone CAF files with only ML reconstruction outputs. 

## Dependencies
The package has two main requirements:
* HDF5 C++ library (instructions below)
* `duneanaobj` (must have ML variables implemented in `StandardRecord`)

### Using `duneanaobj` from UPS
The machine-learning reconstruction outputs are now integrated into the `StandardRecord` tree in `duneanaobj`. You no longer need to build it from source. Instead, you can set up the latest version of `duneanaobj` available via UPS. You can see available versions with:

    ups list -aK+ duneanaobj

The provided `setup.sh` script handles setting up a recent version for you, but you can update the version inside that script to the latest version as needed.

## Installation

### 1. Source the Environment
A `setup.sh` script is provided to easily load dependencies into your environment. You must have the base Fermilab `ups` environment available beforehand. 

First, clone the repository and source the setup script. The setup script will define the `$HDF5_HOME` variable and load the necessary compiler (`duneanaobj`) and `cmake`:

```bash
git clone https://github.com/DUNE/dune_ml_cafmaker.git
cd dune_ml_cafmaker
source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
source setup.sh
```

### 2. Install the HDF5 C++ API
The most recent versions of `hdf5` available on UPS do not come packaged with the C++ API. The latest version which works as a viable dependency is `v1.10.5` with qualifier `e20`, which is absolutely ancient. Moreover, `hdf5` will not be supported in the future as a UPS product. The switch to qualifier `e26` means that the UPS product is essentially a non-workable solution for the foreseable future. The unfortunate solution is to clone and build the `hdf5` repository. 

With the environment set up from the previous step, build an `e26` compatible `hdf5`:

```bash
# $HDF5_HOME is already defined by setup.sh
git clone https://github.com/HDFGroup/hdf5.git $HDF5_HOME/hdf5_src
mkdir $HDF5_HOME/hdf5_build
mkdir $HDF5_HOME/hdf5_install
cd $HDF5_HOME/hdf5_src
git checkout hdf5-1_10_5
cd $HDF5_HOME/hdf5_build
cmake $HDF5_HOME/hdf5_src/ -DCMAKE_CXX_COMPILER="g++" -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=$HDF5_HOME/hdf5_install/
make install
```

### 3. Build `dune_ml_cafmaker`
Once HDF5 is installed, you can build the `dune_ml_cafmaker` executables. Make sure to point CMake to the `hdf5` installation directory you just created:

```bash
# Make sure you are in the dune_ml_cafmaker directory
# cd /path/to/dune_ml_cafmaker
mkdir build
cmake -S . -B build -DHDF5_DIR="$HDF5_HOME/hdf5_install/share/cmake/hdf5"
make -C build
```

## Wrapper Scripts (Recommended)

The easiest way to generate CAFs is using the provided bash wrapper scripts:

*   **`make_caf_data.sh <input_hdf5_file(s)>`**: Produces standalone CAFs for data containing only ML reconstruction objects.
*   **`make_caf_simulation.sh <input_hdf5_file(s)>`**: Produces standalone CAFs for simulation containing both ML reconstruction and truth objects.
*   **`merge_all_simulation.sh [output_dir]`**: Merges matching HDF5 (spine) files with existing CAF files. It automatically matches files based on the shared base name (`<base>_larcv_spine.h5` to `<base>_caf.root`). Paths to the input directories can be configured at the top of the script.

## Core Executables
Under the hood, these wrappers rely on compiled executables located in the `build/` directory:

### Merging
The executable that handles the merging of the ML reconstruction outputs into an existing CAF (with the same events) is `merge_sources_simulation` (or `_data`). It uses the run, subrun, and event number to build a look-up table for events in the HDF5 file, then copies them into the `StandardRecord` during the main loop over entries in the CAF file. 

    ./build/merge_sources_simulation <output_caf_file> <input_caf_file> <input_hdf5_file>

If no ML reconstruction outputs exist for an event, none are written. If the ML classes within the `StandardRecord` are already filled, they are erased and replaced with the new inputs.

### Standalone
The executable that handles the creation of standalone CAFs is `make_standalone_simulation` (or `_data`). It takes a list of input HDF5 files and places them in a single CAF output file.

    ./build/make_standalone_simulation <output_caf_file> <event_offset> <input_hdf5_file(s)>

The `event_offset` is used to introduce an offset to the `image_id` attribute of interactions and particles, which may be useful in some cases for breaking the degeneracy of `image_id`s in multiple input files.

# Variables

## RecoInteraction
| Variable Name | Variable Type | Units | Description |
| ------------- | ------------- | ----- | ----------- |
| cathode_offset                 | double                    | None  | Distance from the cathode.                                                       |
| depositions_sum                | float                     | None  | Total tagged (true non-ghost) energy deposited [MeV].                            |
| flash_hypo_pe                  | double                    | None  | Total PE of the hypothesized flash.                                              |
| flash_ids                      | BufferView<int32_t>       | None  | Flash IDs for the matched flashes.                                               |
| flash_scores                   | BufferView<float>         | None  | Score of the matched flashes.                                                    |
| flash_times                    | BufferView<float>         | None  | Time of the matched flashes                                                      |
| flash_total_pe                 | double                    | None  | Total PE of the matched flash.                                                   |
| flash_volume_ids               | BufferView<int32_t>       | None  | Volume IDs of the matched flashes.                                               |
| id                             | int64_t                   | None  | Interaction ID.                                                                  |
| is_cathode_crosser             | bool                      | None  | Whether the interaction is a cathode-crosser.                                    |
| is_contained                   | bool                      | None  | Whether the interaction is contained.                                            |
| is_fiducial                    | bool                      | None  | Whether the interaction has an interaction in the fiducial volume.               |
| is_flash_matched               | bool                      | None  | Whether the flash is matched to the interaction.                                 |
| is_matched                     | bool                      | None  | Whether the interaction is matched to a true interaction.                        |
| is_time_contained              | bool                      | None  | Whether the particle is time-contained (within the "in-time" region of the drift window). |
| is_truth                       | bool                      | None  | Whether the interaction is a truth interaction.                                  |
| match_ids                      | BufferView<int64_t>       | None  | Interaction IDs of the considered matches (correspond to true interactions).     |
| match_overlaps                 | BufferView<float>         | None  | Intersection over union (IoU) of the considered matches.                         |
| module_ids                     | BufferView<int64_t>       | None  | Module IDs of the interaction.                                                   |
| num_particles                  | int64_t                   | None  | The number of particles in the interaction.                                      |
| num_primary_particles          | int64_t                   | None  | The number of primary particles in the interaction.                              |
| particle_counts                | int64_t                   | None  | The number of particles of each type in the interaction.                         |
| particle_ids                   | BufferView<int64_t>       | None  | Particle IDs in the interaction.                                                 |
| primary_particle_counts        | int64_t                   | None  | The number of primary particles of each type in the interaction.                 |
| primary_particle_ids           | BufferView<int64_t>       | None  | Primary particle IDs in the interaction.                                         |
| size                           | int64_t                   | None  | The size of the interaction (number of voxels).                                  |
| vertex                         | float                     | None  | Vertex of the interaction in detector coordinates.                               |

## RecoParticle
| Variable Name | Variable Type | Units | Description |
| ------------- | ------------- | ----- | ----------- |
| axial_spread                   | float                     | None  | Axial spread of the particle.                                                    |
| calo_ke                        | float                     | None  | Calorimetric kinetic energy.                                                     |
| cathode_offset                 | double                    | None  | Distance from the cathode.                                                       |
| chi2_per_pid                   | float                     | None  | Chi2 score per PID hypothesis.                                                   |
| chi2_pid                       | int64_t                   | None  | PID from the chi2-based PID.                                                     |
| csda_ke                        | double                    | None  | Continuous-slowing-down-approximation kinetic energy.                            |
| csda_ke_per_pid                | float                     | None  | CSDA kinetic energy per PID.                                                     |
| depositions_sum                | float                     | None  | Total tagged (true non-ghost) energy deposited [MeV].                            |
| directional_spread             | float                     | None  | Directional spread of the particle.                                              |
| end_dir                        | float                     | None  | Unit direction vector calculated at the particle end point.                      |
| end_point                      | float                     | None  | End point (vector) of the particle.                                              |
| fragment_ids                   | BufferView<int32_t>       | None  | Fragment IDs comprising the particle.                                            |
| id                             | int64_t                   | None  | Particle ID.                                                                     |
| interaction_id                 | int64_t                   | None  | Parent interaction ID.                                                           |
| is_cathode_crosser             | bool                      | None  | Whether the particle is a cathode-crosser.                                       |
| is_contained                   | bool                      | None  | Whether the particle is contained.                                               |
| is_matched                     | bool                      | None  | Whether the particle is matched.                                                 |
| is_primary                     | bool                      | None  | Whether the particle is a primary particle.                                      |
| is_time_contained              | bool                      | None  | Whether the particle is time-contained (within the "in-time" region of the drift window). |
| is_truth                       | bool                      | None  | Whether the particle is a truth particle.                                        |
| is_valid                       | bool                      | None  | Whether the particle passes thresholds and counts towards topology.              |
| ke                             | float                     | None  | Kinetic energy from best energy estimator (CSDA, calorimetric, or MCS).          |
| length                         | double                    | None  | Length of the particle.                                                          |
| mass                           | double                    | None  | Mass of the particle (according to assigned PID).                                |
| match_ids                      | BufferView<int64_t>       | None  | Match ID.                                                                        |
| match_overlaps                 | BufferView<float>         | None  | Match overlap.                                                                   |
| mcs_ke                         | double                    | None  | Multiple Coulomb scattering kinetic energy.                                      |
| mcs_ke_per_pid                 | float                     | None  | MCS kinetic energy per PID.                                                      |
| module_ids                     | BufferView<int64_t>       | None  | Module IDs of the particle.                                                      |
| momentum                       | float                     | None  | Momentum (vector) of the particle.                                               |
| num_fragments                  | int64_t                   | None  | Number of fragments comprising the particle.                                     |
| p                              | float                     | None  | Momentum magnitude.                                                              |
| pdg_code                       | int64_t                   | None  | PDG code of the particle.                                                        |
| pid                            | Pid                       | None  | Particle ID (see Pid enumeration).                                               |
| pid_scores                     | float                     | None  | PID softmax scores.                                                              |
| ppn_ids                        | BufferView<int32_t>       | None  | PPN IDs of the particle.                                                         |
| primary_scores                 | float                     | None  | Primary softmax scores.                                                          |
| shape                          | Shape                     | None  | Semantic type of the particle (see Shape enumeration).                           |
| size                           | int64_t                   | None  | Number of voxels comprising the particle.                                        |
| start_dedx                     | double                    | None  | dE/dx at the start of the particle.                                              |
| start_dir                      | float                     | None  | Unit direction vector calculated at the particle start point.                    |
| start_point                    | float                     | None  | Start point (vector) of the particle.                                            |
| start_straightness             | float                     | None  | Straightness at the start of the particle.                                       |
| vertex_distance                | float                     | None  | Distance from the vertex.                                                        |

## TruthInteraction
| Variable Name | Variable Type | Units | Description |
| ------------- | ------------- | ----- | ----------- |
| bjorken_x                      | double                    | None  | Bjorken x of the neutrino interaction.                                           |
| cathode_offset                 | double                    | None  | Distance from the cathode.                                                       |
| current_type                   | CurrentType               | None  | Current type of the neutrino.                                                    |
| depositions_adapt_q_sum        | float                     | None  | Total tagged (reco non-ghost) charge deposited [ADC].                            |
| depositions_adapt_sum          | float                     | None  | Total tagged (reco non-ghost) energy deposited [MeV].                            |
| depositions_g4_sum             | float                     | None  | Total energy deposited energy at the G4 level [MeV].                             |
| depositions_q_sum              | float                     | None  | Total tagged (true non-ghost) charge deposited [ADC].                            |
| depositions_sum                | float                     | None  | Total tagged (true non-ghost) energy deposited [MeV].                            |
| distance_travel                | double                    | None  | Distance traveled by the neutrino from production to the interaction.            |
| energy_init                    | double                    | None  | Initial energy of the neutrino.                                                  |
| energy_transfer                | double                    | None  | Energy transfer (Q0) of the neutrino interaction.                                |
| flash_hypo_pe                  | double                    | None  | Total PE of the hypothesized flash.                                              |
| flash_ids                      | BufferView<int32_t>       | None  | Flash ID for the matched flashes.                                                |
| flash_scores                   | BufferView<float>         | None  | Flash score for the matched flashes.                                             |
| flash_times                    | BufferView<float>         | None  | Time of the matched flashes.                                                     |
| flash_total_pe                 | double                    | None  | Total PE of the matched flash.                                                   |
| flash_volume_ids               | BufferView<int32_t>       | None  | Volume IDs of the matched flashes.                                               |
| hadronic_invariant_mass        | double                    | None  | Hadronic invariant mass of the neutrino.                                         |
| id                             | int64_t                   | None  | Interaction ID.                                                                  |
| inelasticity                   | double                    | None  | Inelasticity of the neutrino interaction.                                        |
| interaction_id                 | int64_t                   | None  | Deprecated.                                                                      |
| interaction_mode               | InteractionMode           | None  | Interaction mode of the neutrino.                                                |
| interaction_type               | InteractionType           | None  | Interaction type of the neutrino.                                                |
| is_cathode_crosser             | bool                      | None  | Whether the interaction is a cathode-crosser.                                    |
| is_contained                   | bool                      | None  | Whether the interaction is contained.                                            |
| is_fiducial                    | bool                      | None  | Whether the interaction has an interaction in the fiducial volume.               |
| is_flash_matched               | bool                      | None  | Whether the flash is matched to the interaction.                                 |
| is_matched                     | bool                      | None  | Whether the interaction is matched to a true interaction.                        |
| is_time_contained              | bool                      | None  | Whether the particle is time-contained (within the "in-time" region of the drift window). |
| is_truth                       | bool                      | None  | Whether the interaction is a truth interaction.                                  |
| lepton_p                       | double                    | None  | Momentum of the lepton in the interaction.                                       |
| lepton_pdg_code                | int64_t                   | None  | PDG code of the lepton in the interaction.                                       |
| lepton_track_id                | int64_t                   | None  | Track ID of the lepton in the neutrino interaction.                              |
| match_ids                      | BufferView<int64_t>       | None  | Interaction IDs of the considered matches (correspond to true interactions).     |
| match_overlaps                 | BufferView<float>         | None  | Intersection over union (IoU) of the considered matches.                         |
| mct_index                      | int64_t                   | None  | Index of the neutrino in the original MCTruth array.                             |
| module_ids                     | BufferView<int64_t>       | None  | Module IDs of the interaction.                                                   |
| momentum                       | float                     | None  | Momentum (vector) of the neutrino.                                               |
| momentum_transfer              | double                    | None  | Momentum transfer (Q^2) of the neutrino interaction.                             |
| momentum_transfer_mag          | double                    | None  | Momentum transfer (Q3) of the neutrino interaction.                              |
| nu_id                          | int64_t                   | None  | Neutrino ID (-1 = not a neutrino, 0 = first neutrino, 1 = second neutrino, etc.). |
| nucleon                        | int64_t                   | None  | Nucleon in the neutrino interaction.                                             |
| num_particles                  | int64_t                   | None  | Number of particles in the interaction.                                          |
| num_primary_particles          | int64_t                   | None  | Number of primary particles in the interaction.                                  |
| orig_id                        | int64_t                   | None  | Original ID of the interaction.                                                  |
| particle_counts                | int64_t                   | None  | Number of particles of each type in the interaction.                             |
| particle_ids                   | BufferView<int64_t>       | None  | Particle IDs in the interaction.                                                 |
| pdg_code                       | int64_t                   | None  | PDG code of the neutrino.                                                        |
| position                       | float                     | None  | Position of the neutrino interaction.                                            |
| primary_particle_counts        | int64_t                   | None  | Number of primary particles of each type in the interaction.                     |
| primary_particle_ids           | BufferView<int64_t>       | None  | Particle IDs of the primary particles in the interaction.                        |
| quark                          | int64_t                   | None  | Quark in the neutrino interaction.                                               |
| reco_vertex                    | float                     | None  | Vertex of the interaction in detector coordinates (reco).                        |
| size                           | int64_t                   | None  | Number of true non-ghost true-tagged space points.                               |
| size_adapt                     | int64_t                   | None  | Number of reco non-ghost true-tagged space points.                               |
| size_g4                        | int64_t                   | None  | Number of (rasterized) g4 energy depositions (no detector effects).              |
| target                         | int64_t                   | None  | Target in the neutrino interaction.                                              |
| theta                          | double                    | None  | Angle of the neutrino interaction.                                               |
| track_id                       | int64_t                   | None  | Track ID of the neutrino interaction.                                            |
| vertex                         | float                     | None  | Vertex of the interaction in detector coordinates (truth).                       |

## TruthParticle
| Variable Name | Variable Type | Units | Description |
| ------------- | ------------- | ----- | ----------- |
| ancestor_pdg_code              | int64_t                   | None  | PDG code of the ancestor particle.                                               |
| ancestor_position              | float                     | None  | Position of the ancestor particle.                                               |
| ancestor_t                     | double                    | None  | Time of the ancestor particle.                                                   |
| ancestor_track_id              | int64_t                   | None  | Track ID of the ancestor particle.                                               |
| calo_ke                        | double                    | None  | Calorimetric kinetic energy.                                                     |
| cathode_offset                 | double                    | None  | Distance from the cathode.                                                       |
| children_counts                | BufferView<int64_t>       | None  | Number of children of the particle.                                              |
| children_id                    | BufferView<int64_t>       | None  | List of particle ID of children particles.                                       |
| csda_ke                        | double                    | None  | Continuous-slowing-down-approximation kinetic energy.                            |
| csda_ke_per_pid                | double                    | None  | CSDA kinetic energy per PID.                                                     |
| depositions_adapt_q_sum        | float                     | None  | Total tagged (reco non-ghost) charge deposited [ADC].                            |
| depositions_adapt_sum          | float                     | None  | Total tagged (reco non-ghost) energy deposited [MeV].                            |
| depositions_g4_sum             | float                     | None  | Total energy deposited energy at the G4 level [MeV].                             |
| depositions_q_sum              | float                     | None  | Total tagged (true non-ghost) charge deposited [ADC].                            |
| depositions_sum                | float                     | None  | Total tagged (true non-ghost) energy deposited [MeV].                            |
| distance_travel                | double                    | None  | TO DO.                                                                           |
| end_dir                        | float                     | None  | Unit direction vector calculated at the particle end point.                      |
| end_momentum                   | float                     | None  | Momentum (vector) of the particle at the end.                                    |
| end_p                          | float                     | None  | Momentum magnitude of the particle at the end.                                   |
| end_point                      | float                     | None  | End point (vector) of the particle.                                              |
| end_position                   | float                     | None  | End position (vector) of the particle.                                           |
| end_t                          | double                    | None  | TO DO.                                                                           |
| energy_deposit                 | double                    | None  | Energy deposited by the particle.                                                |
| energy_init                    | double                    | None  | Initial energy of the particle.                                                  |
| first_step                     | float                     | None  | Coordinates of the first step of the particle.                                   |
| fragment_ids                   | BufferView<int32_t>       | None  | Fragment IDs comprising the particle.                                            |
| group_id                       | int64_t                   | None  | Group ID of the particle.                                                        |
| group_primary                  | int64_t                   | None  | Whether the particle is a primary within its group.                              |
| id                             | int64_t                   | None  | Particle ID.                                                                     |
| interaction_id                 | int64_t                   | None  | Parent interaction ID.                                                           |
| interaction_primary            | int64_t                   | None  | Whether the particle is a primary within its interaction (equivalent to is_primary). |
| is_cathode_crosser             | bool                      | None  | Whether the interaction is a cathode-crosser.                                    |
| is_contained                   | bool                      | None  | Whether the interaction is contained.                                            |
| is_matched                     | bool                      | None  | Whether the interaction is matched to a true interaction.                        |
| is_primary                     | bool                      | None  | Whether the interaction is a primary interaction.                                |
| is_time_contained              | bool                      | None  | Whether the particle is time-contained (within the "in-time" region of the drift window). |
| is_truth                       | bool                      | None  | Whether the interaction is a truth interaction.                                  |
| is_valid                       | bool                      | None  | (Whether the particle passes thresholds and counts towards topology.             |
| ke                             | double                    | None  | Kinetic energy according from best energy estimator (CSDA, calorimetric, or MCS). |
| last_step                      | float                     | None  | Coordinates of the last step of the particle.                                    |
| length                         | double                    | None  | Length of the particle.                                                          |
| mass                           | double                    | None  | TO DO.                                                                           |
| match_ids                      | BufferView<int64_t>       | None  | Match ID.                                                                        |
| match_overlaps                 | BufferView<float>         | None  | Match overlap.                                                                   |
| mcs_ke                         | double                    | None  | Multiple Coulomb scattering kinetic energy.                                      |
| mcs_ke_per_pid                 | double                    | None  | MCS kinetic energy per PID.                                                      |
| mcst_index                     | int64_t                   | None  | MCST index.                                                                      |
| mct_index                      | int64_t                   | None  | Index of the particle in the original MCTruth array..                            |
| module_ids                     | BufferView<int64_t>       | None  | Module IDs of the particle.                                                      |
| momentum                       | float                     | None  | Momentum (vector) of the particle.                                               |
| nu_id                          | int64_t                   | None  | Neutrino ID (-1 = not a neutrino, 0 = first neutrino, 1 = second neutrino, etc.). |
| num_fragments                  | int64_t                   | None  | TO DO.                                                                           |
| num_voxels                     | int64_t                   | None  | Number of voxels comprising the particle.                                        |
| orig_children_id               | BufferView<int64_t>       | None  | Original ID of the children particles.                                           |
| orig_group_id                  | int64_t                   | None  | Original group ID of the particle.                                               |
| orig_id                        | int64_t                   | None  | Original ID of the particle.                                                     |
| orig_interaction_id            | int64_t                   | None  | Interaction ID as it was stored in the parent LArCV file under the interaction_id attribute. |
| orig_parent_id                 | int64_t                   | None  | Parent ID as it was stored in the parent LArCV file under the parent_id attribute. |
| p                              | float                     | None  | Momentum magnitude.                                                              |
| parent_id                      | int64_t                   | None  | Parent particle ID.                                                              |
| parent_pdg_code                | int64_t                   | None  | PDG code of the parent particle.                                                 |
| parent_position                | float                     | None  | Position of the parent particle.                                                 |
| parent_t                       | double                    | None  | Time of the parent particle.                                                     |
| parent_track_id                | int64_t                   | None  | Track ID of the parent particle.                                                 |
| pdg_code                       | int64_t                   | None  | PDG code of the particle.                                                        |
| pid                            | Pid                       | None  | Particle ID (see Pid enumeration).                                               |
| position                       | float                     | None  | Position of the particle.                                                        |
| reco_end_dir                   | float                     | None  | TO DO.                                                                           |
| reco_ke                        | double                    | None  | TO DO.                                                                           |
| reco_length                    | double                    | None  | TO DO.                                                                           |
| reco_momentum                  | float                     | None  | TO DO.                                                                           |
| reco_start_dir                 | float                     | None  | TO DO.                                                                           |
| shape                          | Shape                     | None  | Semantic type of the particle (see SemanticType enumeration).                    |
| size                           | int64_t                   | None  | TO DO.                                                                           |
| size_adapt                     | int64_t                   | None  | TO DO.                                                                           |
| size_g4                        | int64_t                   | None  | TO DO.                                                                           |
| start_dir                      | float                     | None  | Unit direction vector calculated at the particle start point.                    |
| start_point                    | float                     | None  | Start point (vector) of the particle.                                            |
| t                              | double                    | None  | Time of the particle.                                                            |
| track_id                       | int64_t                   | None  | Track ID of the particle.                                                        |

