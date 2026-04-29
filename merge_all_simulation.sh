#!/bin/bash
# Merges matching HDF5 (spine) files with CAF files using merge_sources_simulation.
# Matching is done by the shared base name:
#   <base>_larcv_spine.h5  <-->  <base>_caf.root
#
# Usage:
#   ./merge_all_simulation.sh [output_dir]
#
# If output_dir is not given, defaults to ./merged_output/

set -euo pipefail

SPINE_DIR="/pnfs/dune/scratch/users/pgranger/spine_processed"
CAF_DIR="/pnfs/dune/scratch/users/pgranger/cafs_output/fnal/15084/1/001"
BINARY="$(dirname "$0")/build/merge_sources_simulation"
OUTPUT_DIR="${1:-$(dirname "$0")/merged_output}"

if [[ ! -x "$BINARY" ]]; then
    echo "ERROR: merge_sources_simulation not found or not executable at: $BINARY"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

matched=0
skipped=0
failed=0

for h5_file in "$SPINE_DIR"/*_larcv_spine.h5; do
    [[ -e "$h5_file" ]] || { echo "No HDF5 files found in $SPINE_DIR"; exit 1; }

    filename=$(basename "$h5_file")
    # Strip trailing _larcv_spine.h5 to get the shared base name
    base="${filename%_larcv_spine.h5}"

    caf_file="$CAF_DIR/${base}_caf.root"

    if [[ ! -f "$caf_file" ]]; then
        echo "SKIP: no matching CAF for $filename"
        ((skipped++)) || true
        continue
    fi

    output_file="$OUTPUT_DIR/${base}_merged.root"

    echo "Merging: $filename"
    echo "    CAF : $caf_file"
    echo "    OUT : $output_file"

    if "$BINARY" "$output_file" "$caf_file" "$h5_file"; then
        ((matched++)) || true
    else
        echo "FAILED: merge_sources_simulation returned non-zero for $filename"
        ((failed++)) || true
    fi
done

echo ""
echo "Done. Merged: $matched | Skipped (no CAF match): $skipped | Failed: $failed"
