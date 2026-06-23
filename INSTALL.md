# ResBos2 Installation Guide
## Michigan State University HPCC (ICER)

**Last updated:** June 2026  
**Target system:** MSU HPCC (`hpcc.msu.edu`), SLURM scheduler  
**User home:** `/mnt/home/lopezels/`

---

## Table of Contents

1. [Directory Layout](#1-directory-layout)
2. [Required Modules](#2-required-modules)
3. [.bashrc Setup](#3-bashrc-setup)
4. [Dependency Installation](#4-dependency-installation)
   - [4.1 LHAPDF](#41-lhapdf)
   - [4.2 HOPPET](#42-hoppet)
   - [4.3 ROOT (CERN)](#43-root-cern)
5. [Installing ResBos2](#5-installing-resbos2)
6. [Verifying the Installation](#6-verifying-the-installation)
7. [Running ResBos2](#7-running-resbos2)
   - [7.1 Interactive Run](#71-interactive-run)
   - [7.2 SLURM Batch Job](#72-slurm-batch-job)
8. [resbos.config Reference](#8-resboosconfig-reference)
9. [Common Errors and Fixes](#9-common-errors-and-fixes)

---

## 1. Directory Layout

This guide uses the following directory conventions. Adjust paths if yours differ.

```
/mnt/home/lopezels/
├── SourceCodes/          # Source trees (git clones, tarballs)
│   ├── resbos2/          # ResBos2 source
│   └── root-6.30.04/     # ROOT source (after unpacking)
└── InstallSources/       # Compiled, installed libraries and binaries
    ├── LHAPDF/           # LHAPDF install prefix
    ├── HOPPET1/          # HOPPET install prefix
    ├── ROOT/             # ROOT install prefix
    └── ResBos2/          # ResBos2 install prefix
```

Create the top-level directories if they do not exist:

```bash
mkdir -p /mnt/home/lopezels/SourceCodes
mkdir -p /mnt/home/lopezels/InstallSources
```

---

## 2. Required Modules

MSU HPCC uses the `module` system. Load the following before building anything.
Add them to your `.bashrc` (see Section 3) so they are always available.

```bash
module purge
module load GCC/11.3.0          # C/C++/Fortran compiler (gfortran needed by HOPPET)
module load CMake/3.23.1        # Build system
module load Python/3.10.4       # Required by ROOT and LHAPDF
module load OpenSSL/1.1         # Required by ROOT's build
```

Check available versions with:
```bash
module spider GCC
module spider CMake
```

If the exact versions above are unavailable, choose the closest newer version.

---

## 3. .bashrc Setup

Replace or update your `~/.bashrc` with the following block. **Order matters**: ROOT
must be sourced before ResBos2 paths are added, and modules must be loaded before
any install paths are exported.

```bash
# ~/.bashrc — MSU HPCC configuration for ResBos2

test -f /etc/profile.dos && . /etc/profile.dos

# ── Modules ────────────────────────────────────────────────────────────────
module load GCC/11.3.0
module load CMake/3.23.1
module load Python/3.10.4
module load OpenSSL/1.1

# ── LHAPDF ─────────────────────────────────────────────────────────────────
export LHAPDF_DIR=/mnt/home/lopezels/InstallSources/LHAPDF
export PATH=${LHAPDF_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${LHAPDF_DIR}/lib:$LD_LIBRARY_PATH

# ── HOPPET ─────────────────────────────────────────────────────────────────
export HOPPET_DIR=/mnt/home/lopezels/InstallSources/HOPPET1
export PATH=${HOPPET_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${HOPPET_DIR}/lib:$LD_LIBRARY_PATH

# ── ROOT (must come before ResBos2) ────────────────────────────────────────
export ROOTSYS=/mnt/home/lopezels/InstallSources/ROOT
source ${ROOTSYS}/bin/thisroot.sh

# ── MCFM (optional) ────────────────────────────────────────────────────────
export PATH=/mnt/home/lopezels/InstallSources/MCFM-10.3:$PATH

# ── ResBos2 ────────────────────────────────────────────────────────────────
export RESBOS2_DIR=/mnt/home/lopezels/InstallSources/ResBos2
export PATH=${RESBOS2_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${RESBOS2_DIR}/lib:$LD_LIBRARY_PATH

# ── OpenMP ─────────────────────────────────────────────────────────────────
export OMP_NUM_THREADS=28
export OMP_STACKSIZE=2G

# ── Aliases ────────────────────────────────────────────────────────────────
test -s ~/.alias && . ~/.alias
```

After editing, apply it:
```bash
source ~/.bashrc
```

---

## 4. Dependency Installation

### 4.1 LHAPDF

```bash
cd /mnt/home/lopezels/SourceCodes

# Download (replace 6.5.4 with the latest version if needed)
wget https://lhapdf.hepforge.org/downloads/?f=LHAPDF-6.5.4.tar.gz -O LHAPDF-6.5.4.tar.gz
tar -xzf LHAPDF-6.5.4.tar.gz
cd LHAPDF-6.5.4

./configure --prefix=/mnt/home/lopezels/InstallSources/LHAPDF \
            --enable-python=no

make -j 8
make install
```

Download at least one PDF set (e.g., CT14nnlo, which is used in the default `resbos.config`):
```bash
/mnt/home/lopezels/InstallSources/LHAPDF/bin/lhapdf install CT14nnlo
```

Verify:
```bash
/mnt/home/lopezels/InstallSources/LHAPDF/bin/lhapdf-config --version
```

---

### 4.2 HOPPET

```bash
cd /mnt/home/lopezels/SourceCodes

wget https://hoppet.hepforge.org/downloads/hoppet-1.2.0.tgz
tar -xzf hoppet-1.2.0.tgz
cd hoppet-1.2.0

./configure --prefix=/mnt/home/lopezels/InstallSources/HOPPET1

make -j 8
make install
```

Verify:
```bash
/mnt/home/lopezels/InstallSources/HOPPET1/bin/hoppet-config --version
```

---

### 4.3 ROOT (CERN)

ROOT must be built from source because MSU HPCC does not provide a compatible
pre-built version. The key addition compared to a minimal build is the explicit
`ROOTSYS` export and `CMAKE_PREFIX_PATH` — without them ResBos2 cannot find ROOT.

```bash
cd /mnt/home/lopezels/SourceCodes

# Download ROOT 6.30.04 source
wget https://root.cern/download/root_v6.30.04.source.tar.gz
tar -xzf root_v6.30.04.source.tar.gz

mkdir -p root_build
cd root_build

cmake ../root-6.30.04 \
  -DCMAKE_INSTALL_PREFIX=/mnt/home/lopezels/InstallSources/ROOT \
  -DCMAKE_CXX_STANDARD=17 \
  -Dxrootd=OFF \
  -Dbuiltin_xrootd=OFF \
  -Dpythia6=OFF \
  -Dpythia8=OFF \
  -Dmysql=OFF \
  -Doracle=OFF \
  -Dpgsql=OFF \
  -Dsqlite=OFF \
  -Dbuiltin_openssl=OFF \
  -Dssl=ON \
  -Dpython=ON

make -j 8
make install
```

> **Note on `-Dxrootd=OFF`**: XRootD is a distributed filesystem protocol that
> ROOT links by default but ResBos2 does not need. Disabling it avoids linker
> errors common on HPCC shared filesystems.

> **Note on `CMAKE_CXX_STANDARD=17`**: ResBos2 requires C++17. Building ROOT
> with C++17 ensures ABI compatibility between ROOT and ResBos2.

Verify ROOT works:
```bash
source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh
root -l -q -e 'std::cout << "ROOT version: " << gROOT->GetVersion() << std::endl;'
# Expected output: ROOT version: 6.30/04
```

---

## 5. Installing ResBos2

### 5.1 Clone

```bash
cd /mnt/home/lopezels/SourceCodes

# Remove any previous broken clone
rm -rf resbos2

# Clone with all submodules
git clone --recursive https://gitlab.com/resbos2/resbos2.git
cd resbos2

# Required: scripts/CMakeLists.txt must exist (may be empty)
touch scripts/CMakeLists.txt
```

### 5.2 Build (without ROOT)

Use this if you only need the default `BuiltIn` histogram mode (plain text output).

```bash
cd /mnt/home/lopezels/SourceCodes/resbos2
mkdir -p build
cd build

cmake .. \
  -DCMAKE_INSTALL_PREFIX=/mnt/home/lopezels/InstallSources/ResBos2 \
  -DHoppet_ROOT_DIR=/mnt/home/lopezels/InstallSources/HOPPET1 \
  -DLHAPDF_ROOT_DIR=/mnt/home/lopezels/InstallSources/LHAPDF \
  -DCPM_DOWNLOAD_ALL=ON \
  -DUSE-ROOT=OFF \
  -DUSE-MPI=OFF \
  -DUSE-GZIP=OFF

make -j 40
make install
```

### 5.3 Build (with ROOT — recommended)

**Before running cmake**, `ROOTSYS` must be exported. If you have already applied
the `.bashrc` from Section 3, it is already set. Otherwise:

```bash
export ROOTSYS=/mnt/home/lopezels/InstallSources/ROOT
source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh
```

Then configure and build:

```bash
cd /mnt/home/lopezels/SourceCodes/resbos2
mkdir -p build
cd build

# Clean any stale cmake cache first
rm -rf *

cmake .. \
  -DCMAKE_INSTALL_PREFIX=/mnt/home/lopezels/InstallSources/ResBos2 \
  -DHoppet_ROOT_DIR=/mnt/home/lopezels/InstallSources/HOPPET1 \
  -DLHAPDF_ROOT_DIR=/mnt/home/lopezels/InstallSources/LHAPDF \
  -DCPM_DOWNLOAD_ALL=ON \
  -DUSE-ROOT=ON \
  -DUSE-MPI=OFF \
  -DUSE-GZIP=OFF \
  -DCMAKE_PREFIX_PATH=/mnt/home/lopezels/InstallSources/ROOT

make -j 40
make install
```

> **Critical:** `-DCMAKE_PREFIX_PATH` explicitly tells cmake where to find ROOT's
> `ROOTConfig.cmake`. Without it, cmake relies solely on `$ROOTSYS` at configure
> time, which can silently fail if the environment is not set.

Check that cmake output includes these lines before `make`:
```
-- Building with ROOT histograms
-- ROOT VERSION: 6.30/04
```

If you see `-- Building ResBos without ROOT.` instead, ROOT was not found.
See [Section 9](#9-common-errors-and-fixes).

---

## 6. Verifying the Installation

### 6.1 Check the binary is installed

```bash
which resbos
# Expected: /mnt/home/lopezels/InstallSources/ResBos2/bin/resbos
```

### 6.2 Check ROOT support was compiled in

```bash
strings /mnt/home/lopezels/InstallSources/ResBos2/bin/resbos \
    | grep -i "not compiled with ROOT"
```

- **No output** → binary has ROOT support (`HAVE_ROOT` was defined at compile time). 
- **Output printed** → ROOT was NOT compiled in. Go back to Section 5.3.

### 6.3 Check runtime library linking

```bash
source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh

ldd /mnt/home/lopezels/InstallSources/ResBos2/bin/resbos | grep "not found"
```

Any line with `not found` means a shared library is missing at runtime.
The most common offenders are ROOT's `libCore.so` and `libHist.so`.

### 6.4 Quick test run (BuiltIn mode)

```bash
cd /tmp
cp /mnt/home/lopezels/InstallSources/ResBos2/resbos.config .

# Make sure HistogramMode = BuiltIn in resbos.config
resbos -i resbos.config
```

Expected last lines:
```
================================================================================
|         Process:           DrellYan                                          |
|         ECM:               8000 GeV                                          |
|         Cross-Section:     XXX.XXX +/- X.XXX pb                             |
```

---

## 7. Running ResBos2

### 7.1 Interactive Run

Interactive runs are suitable only for quick tests. Request an interactive node first
so you do not run jobs on the login node:

```bash
salloc --nodes=1 --ntasks=1 --cpus-per-task=8 --mem=16G \
       --time=02:00:00 --partition=general-short-cores
```

Once on the compute node:

```bash
source ~/.bashrc   # modules and paths

mkdir -p /mnt/home/lopezels/Runs/DrellYan_test
cd       /mnt/home/lopezels/Runs/DrellYan_test

cp /mnt/home/lopezels/InstallSources/ResBos2/resbos.config .

# Edit resbos.config as needed (see Section 8), then:
resbos -i resbos.config
```

### 7.2 SLURM Batch Job

#### Single-node, multi-thread job (recommended for most runs)

Save the following as `run_resbos.sh` in your run directory and customise the
`#SBATCH` headers and `CONFIG_FILE` as needed.

```bash
#!/bin/bash
# ─────────────────────────────────────────────────────────
# SLURM job script for ResBos2 on MSU HPCC
# Submit with: sbatch run_resbos.sh
# ─────────────────────────────────────────────────────────

#SBATCH --job-name=resbos2_DY           # Job name shown in squeue
#SBATCH --output=resbos2_%j.out         # stdout (%j = job ID)
#SBATCH --error=resbos2_%j.err          # stderr
#SBATCH --nodes=1                       # Number of nodes
#SBATCH --ntasks=1                      # One process (multi-thread via OpenMP)
#SBATCH --cpus-per-task=28              # Cores (match OMP_NUM_THREADS)
#SBATCH --mem=64G                       # RAM
#SBATCH --time=24:00:00                 # Wall time (HH:MM:SS)
#SBATCH --partition=general-long-cores  # Partition; see note below
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=lopezels@msu.edu

# ── Environment ────────────────────────────────────────────
module purge
module load GCC/11.3.0
module load CMake/3.23.1
module load Python/3.10.4
module load OpenSSL/1.1

export LHAPDF_DIR=/mnt/home/lopezels/InstallSources/LHAPDF
export PATH=${LHAPDF_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${LHAPDF_DIR}/lib:$LD_LIBRARY_PATH

export HOPPET_DIR=/mnt/home/lopezels/InstallSources/HOPPET1
export PATH=${HOPPET_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${HOPPET_DIR}/lib:$LD_LIBRARY_PATH

export ROOTSYS=/mnt/home/lopezels/InstallSources/ROOT
source ${ROOTSYS}/bin/thisroot.sh

export RESBOS2_DIR=/mnt/home/lopezels/InstallSources/ResBos2
export PATH=${RESBOS2_DIR}/bin:$PATH
export LD_LIBRARY_PATH=${RESBOS2_DIR}/lib:$LD_LIBRARY_PATH

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_STACKSIZE=2G

# ── Run directory and config ───────────────────────────────
RUN_DIR=/mnt/home/lopezels/Runs/DrellYan_8TeV
CONFIG_FILE=${RUN_DIR}/resbos.config

mkdir -p ${RUN_DIR}
cd ${RUN_DIR}

# ── Execute ────────────────────────────────────────────────
echo "Starting ResBos2 at $(date)"
echo "Node: $(hostname)"
echo "Cores: ${SLURM_CPUS_PER_TASK}"

resbos -i ${CONFIG_FILE} -o ${RUN_DIR}

echo "Finished at $(date)"
```

Submit the job:
```bash
sbatch run_resbos.sh
```

Monitor progress:
```bash
squeue -u lopezels           # List your jobs
tail -f resbos2_<JOBID>.out  # Watch live output
tail -f resbos.log           # ResBos2 internal log
```

Cancel a job:
```bash
scancel <JOBID>
```

#### MSU HPCC Partition Reference

| Partition | Max cores | Max time | Use case |
|---|---|---|---|
| `general-short-cores` | 28 | 4 h | Testing, short runs |
| `general-long-cores` | 28 | 7 days | Production runs |
| `bigmem-long-cores` | 28 | 7 days | Memory-intensive runs |
| `gpu` | varies | varies | GPU workloads (not needed for ResBos2) |

Check current partitions and limits with:
```bash
sinfo -o "%P %l %c %m"
```

#### Multi-job array (parameter scan)

If you need to run ResBos2 for multiple PDF sets or parameter variations:

```bash
#!/bin/bash
#SBATCH --job-name=resbos2_scan
#SBATCH --output=resbos2_%A_%a.out
#SBATCH --error=resbos2_%A_%a.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=28
#SBATCH --mem=32G
#SBATCH --time=12:00:00
#SBATCH --partition=general-long-cores
#SBATCH --array=0-9           # 10 jobs (iSet 0 through 9 for PDF replicas)
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=lopezels@msu.edu

# Environment setup (same as single-node script above)
module purge
module load GCC/11.3.0
module load Python/3.10.4
module load OpenSSL/1.1

source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh
export LD_LIBRARY_PATH=/mnt/home/lopezels/InstallSources/LHAPDF/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/mnt/home/lopezels/InstallSources/HOPPET1/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/mnt/home/lopezels/InstallSources/ResBos2/lib:$LD_LIBRARY_PATH
export PATH=/mnt/home/lopezels/InstallSources/ResBos2/bin:$PATH

export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK}
export OMP_STACKSIZE=2G

ISET=${SLURM_ARRAY_TASK_ID}

RUN_DIR=/mnt/home/lopezels/Runs/PDF_scan/replica_${ISET}
mkdir -p ${RUN_DIR}
cd ${RUN_DIR}

# Copy base config and patch iSet
cp /mnt/home/lopezels/Runs/resbos_base.config resbos.config
sed -i "s/^iSet = .*/iSet = ${ISET}/" resbos.config

resbos -i resbos.config -o ${RUN_DIR}
```

---

## 8. resbos.config Reference

The most relevant options when using ROOT histograms on HPCC:

```ini
#ResBos2 Configuration File
Version = 2.0.1

#####################
# Running Mode Info #
#####################
mode = MCFull
XSec = Resummation

####################
# VEGAS PARAMETERS #
####################
itmax1 = 10
ncall1 = 10000
itmax2 = 10
ncall2 = 10000
iSeed = 6270355

################
# Process Info #
################
beam = pp
ECM = 8000
Process = DrellYan
CKM = true
PDF = CT14nnlo
iSet = 0

##########################
# Resummation Parameters #
##########################
scheme = CSS
AOrder = 4
BOrder = 3
COrder = 2
HOrder = 2
C1 = 1
C2 = 1
C3 = 1
NonPert = BLNY
bMax = 1.5
Q0 = 1.55
ngag = 1
g = 0.181, 0.167, 0.003

#####################################
# Fixed-Order/Asymptotic Parameters #
#####################################
PertOrder = 1
AsymOrder = 2
DeltaSigmaOrder = 2
scale = MT
muF = 1
muR = 1

#############
# EW Inputs #
#############
EWMassScheme = OnShell
mT = 173.5
gamT = 0
mH = 125
gamH = 0.004
mW = 80.385
gamW = 2.035
mZ = 91.18998
gamZ = 2.421322
mu = 0
md = 0
ms = 0
mc = 1.3
mb = 4.75
EWInputs = GFMWMZ
GF = 1.1663787E-5
AlphaEMOrder = 1
AlphaEM0 = 0.00729735
AlphaEMMZ = 0.007763340902
sinw2 = 0.2223

#########################
# Phase Space Cuts      #
#########################
qMin = 66
qMax = 116
qtMin = 0
qtMax = 600
yMin = -5.0
yMax = 5.0
ptlMin = 0
ptlMax = -1
ylMin = -10
ylMax = 10
METMin = 0
METMax = -1
drllMin = 0
drllMax = -1
ptl1Min = 0
ptl2Min = 0
ptl1Max = -1
ptl2Max = -1

##############
# Histograms #
##############
# Options: BuiltIn (text), ROOT (requires -DUSE-ROOT=ON), YODA (requires -DUSE-YODA=ON)
HistogramMode = ROOT
WriteEvent = false

###################
# Running Options #
###################
nThreads = 28
GridGen = true
```

**HistogramMode options:**

| Value | Output file | Requires cmake flag |
|---|---|---|
| `BuiltIn` | `*.txt` plain text | none |
| `ROOT` | `resbos.root` | `-DUSE-ROOT=ON` |
| `YODA` | `*.yoda` | `-DUSE-YODA=ON` |

Output is always written to the directory where `resbos` is executed (or to the
path passed via `-o`).

---

## 9. Common Errors and Fixes

### Error: `ResBos2 not compiled with ROOT histogram support`

**What it means:** The binary was compiled without `HAVE_ROOT` defined. Even
though `-DUSE-ROOT=ON` was passed, cmake did not find ROOT.

**Fix:** Ensure `ROOTSYS` is exported AND pass `CMAKE_PREFIX_PATH`:

```bash
export ROOTSYS=/mnt/home/lopezels/InstallSources/ROOT
source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh

cd /mnt/home/lopezels/SourceCodes/resbos2/build
rm -rf *

cmake .. \
  -DCMAKE_INSTALL_PREFIX=/mnt/home/lopezels/InstallSources/ResBos2 \
  -DHoppet_ROOT_DIR=/mnt/home/lopezels/InstallSources/HOPPET1 \
  -DLHAPDF_ROOT_DIR=/mnt/home/lopezels/InstallSources/LHAPDF \
  -DCPM_DOWNLOAD_ALL=ON \
  -DUSE-ROOT=ON \
  -DUSE-MPI=OFF \
  -DUSE-GZIP=OFF \
  -DCMAKE_PREFIX_PATH=/mnt/home/lopezels/InstallSources/ROOT

make -j 40 && make install
```

The cmake output must contain: `-- Building with ROOT histograms`

---

### Error: `error while loading shared libraries: libCore.so.6 not found`

**What it means:** The ROOT shared libraries are not found at runtime.

**Fix (option A — permanent):** Add ROOT to `LD_LIBRARY_PATH` in your `.bashrc`:
```bash
source /mnt/home/lopezels/InstallSources/ROOT/bin/thisroot.sh
```
This is already in the `.bashrc` from Section 3. The problem usually appears in
batch jobs that do not source `.bashrc`. Always source the environment explicitly
in your SLURM script (see Section 7.2).

**Fix (option B — check RPATH):** After installing, verify the RPATH is embedded:
```bash
readelf -d /mnt/home/lopezels/InstallSources/ResBos2/bin/resbos | grep RPATH
```
If `/mnt/home/lopezels/InstallSources/ROOT/lib` is in the RPATH, the binary
should work without `LD_LIBRARY_PATH`. If missing, rebuild with:
```bash
-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE
```
(This is already set in the project's `CMakeLists.txt`, but only takes effect if
ROOT was actually found during cmake.)

---

### Error: `Failed to find ROOT environment`

**What it means:** cmake found `-DUSE-ROOT=ON` but `find_package(ROOT)` failed.

**Fix:** Same as the first error above. The cmake configure step itself aborts with
`FATAL_ERROR` before any compilation begins. Clean the build directory and re-run
cmake with `CMAKE_PREFIX_PATH` set.

---

### Error: `Failed to find LHAPDF environment`

**What it means:** cmake cannot find the LHAPDF library.

**Fix:** Verify the path is correct:
```bash
ls /mnt/home/lopezels/InstallSources/LHAPDF/lib/libLHAPDF.*
```
Then pass `-DLHAPDF_ROOT_DIR` explicitly (already included in the cmake commands
in Section 5).

---

### Error: `lhapdf: No such data directory` or `unknown PDF set`

**What it means:** LHAPDF is installed but the requested PDF set files are missing.

**Fix:**
```bash
/mnt/home/lopezels/InstallSources/LHAPDF/bin/lhapdf install CT14nnlo
```
The sets are stored under `$LHAPDF_DIR/share/LHAPDF/`. You can also download them
manually from the LHAPDF data page and unpack them there.

---

### Error: `gfortran: command not found` during HOPPET build

**What it means:** The GCC Fortran compiler is not in PATH.

**Fix:**
```bash
module load GCC/11.3.0   # provides gfortran
which gfortran           # should print a path
```

---

### Error: cmake completes but `make` fails with undefined reference to ROOT symbols

**What it means:** cmake found ROOT headers but linked against a ROOT built with
a different compiler or C++ standard.

**Fix:** Rebuild ROOT with `CMAKE_CXX_STANDARD=17` matching the compiler loaded
via `module load GCC/11.3.0`, then rebuild ResBos2.

---

### Error: SLURM job fails with `Cannot open PDF set` but interactive run works

**What it means:** The job environment does not have `LHAPDF_DATA_PATH` set.

**Fix:** Add to your SLURM script:
```bash
export LHAPDF_DATA_PATH=/mnt/home/lopezels/InstallSources/LHAPDF/share/LHAPDF
```

---

### Error: Segfault or `Segmentation fault (core dumped)` with ROOT mode

**What it means:** Most commonly a compiler ABI mismatch between ROOT and ResBos2,
or ROOT was built without thread safety and `ROOT::EnableThreadSafety()` failed.

**Fix:** Rebuild ROOT with:
```bash
-Dimt=ON           # implicit multi-threading (enables thread safety)
-DCMAKE_CXX_STANDARD=17
```
Then do a full clean rebuild of ResBos2.

---

### Error: `touch scripts/CMakeLists.txt` — why is this needed?

The `scripts/` directory exists in the repository but has no `CMakeLists.txt`.
cmake's `add_subdirectory` would fail without a `CMakeLists.txt` present, so an
empty file is sufficient. This is only needed if the project's root `CMakeLists.txt`
includes `add_subdirectory(scripts)` — check if that line is uncommented.

---

## Quick-Reference Checklist

Before reporting a build or runtime issue, verify each of the following:

- [ ] `module load GCC/11.3.0` (or equivalent) is active
- [ ] `echo $ROOTSYS` prints `/mnt/home/lopezels/InstallSources/ROOT`
- [ ] `root -l -q` opens ROOT without errors
- [ ] cmake output says `-- Building with ROOT histograms`
- [ ] `strings .../bin/resbos | grep "not compiled with ROOT"` prints nothing
- [ ] `ldd .../bin/resbos | grep "not found"` prints nothing
- [ ] `lhapdf list --installed` shows `CT14nnlo` (or your chosen PDF)
- [ ] SLURM script explicitly sources `thisroot.sh` and exports all `LD_LIBRARY_PATH`
- [ ] `HistogramMode = ROOT` in `resbos.config` (not `BuiltIn`)
- [ ] `nThreads` in `resbos.config` matches `--cpus-per-task` in SLURM script
