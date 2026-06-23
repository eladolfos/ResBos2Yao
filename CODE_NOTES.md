# ResBos2 Code Notes
> Session notes for quick onboarding in future conversations.

---

## Project Overview

ResBos2 is a C++ Monte Carlo resummation code for Drell-Yan (and related) processes at hadron colliders. It computes transverse-momentum (`qT`) distributions using the CSS/CFG resummation formalism, with an optional fixed-order matching.

**Key authors:** Joshua Isaacson (original), Yao (this fork/branch).

**Language & build:** C++17, CMake. Optional MPI parallelism (`USING_MPI`), optional ROOT, optional fitting mode (`FITTING` preprocessor flag).

---

## Directory Structure

```
ResBos2Yao/
├── src/
│   ├── main.cc                   # Entry point
│   ├── ResBos.cc                 # Core ResBos class (XSect, GetXSect, EW helpers)
│   ├── ResBos2.cc                # Older/alternative ResBos class (grid interp, XSect)
│   ├── MCFMInterface.cc          # Interface for MCFM-style calls (CreateResbos)
│   ├── ResBos_mod.f90 / ResBos_cdef.f90  # Fortran interface stubs
│   ├── Fit.cc / Fit.hh           # Fit driver
│   ├── Calculation/
│   │   ├── Calculation.cc        # BASE class: GridGen, GridSetup, GetPoint, SaveGrid, LoadGrid
│   │   ├── Resummation.cc        # Resummed piece: FTransform, NonPert, Sudakov, GridGen (uses base)
│   │   ├── Asymptotic.cc         # Asymptotic (fixed-order collinear) piece
│   │   ├── Perturbative.cc       # Fixed-order (NLO, etc.) piece
│   │   ├── DeltaSigma.cc         # Delta-sigma matching piece
│   │   ├── Total.cc              # Combined Resum + Asym + Pert
│   │   ├── WmA.cc                # W-minus-Asymptotic combination
│   │   └── YPiece.cc             # Y-piece (Asym + Pert grid)
│   ├── Process/
│   │   ├── Process.cc            # Base process class
│   │   ├── DrellYan.cc           # Z/γ* Drell-Yan process
│   │   ├── Wpm.cc                # W± process (CxFCxF, ME, angular decomposition)
│   │   └── ...
│   ├── Beam/
│   │   ├── Beam.cc               # Beam class (PDF wrappers)
│   │   └── Convolution.cc        # CxF (coefficient × PDF) convolution grids
│   ├── Utilities/
│   │   ├── Settings.cc           # Config reader (wraps ConfigParser)
│   │   ├── ConfigParser.cc       # INI-style config parser (case-insensitive keys)
│   │   ├── OgataQuad.cc          # Ogata quadrature for Hankel/Fourier-Bessel transform
│   │   └── ...
│   └── NonPertFit/
│       ├── NonPertFit.cc         # BAT-based fitter for non-pert parameters
│       ├── Experiment.cc         # Per-experiment prediction wrapper
│       └── ...
├── include/ResBos/
│   ├── Settings.hh               # Settings template (case-insensitive defaultSettings map)
│   ├── ConfigParser.hh           # ConfigParser + ciLess struct (strcasecmp-based)
│   ├── Resummation.hh            # Resummation class declaration
│   ├── Enums.hh                  # All enums: GridType, Scheme, NonPertEnum, etc.
│   ├── OgataQuad.hh              # OgataQuad class
│   └── ...
├── resbos.config                 # Main run configuration file
├── fitting.config                # Fit-mode configuration
├── parameters.yml                # YAML parameter file for fitting
├── experiments.yml               # YAML experiment definitions
└── CODE_NOTES.md                 # ← this file
```

---

## Configuration System

### Key files
- **`resbos.config`** – used by the main executable (`main.cc`). Default config filename, overridable with `-i`.
- **`fitting.config`** – used by the `NonPertFit` executable.

### Config parser (`ConfigParser`)
- Reads `key = value` pairs, one per line. Comments start with `#`.
- Keys are stored in `std::map<std::string, std::string, ciLess>` — **case-insensitive**.
- Example: `"qMin"` in the file is accessible as `"Qmin"`, `"QMIN"`, etc.

### Settings lookup order (`Settings::GetSetting<T>`)
The generic template (for `double`, `int`, `size_t`, etc.) in `Settings.hh`:
1. `cmdSettings` (command-line overrides, also ciLess)
2. `cfgProcess->KeyExists(key)` → `cfgProcess->GetValueOfKey<T>(key)` (config file)
3. `defaultSettings` (hardcoded fallbacks, also ciLess)
4. If mandatory and not found → `throw std::runtime_error`
5. Otherwise → `LOG_F(WARNING, ...)` and `return 0` (silent zero!)

The `GetSetting<std::string>` **specialization** (Settings.cc) has subtly different/buggy logic — use `GetSetting<T>` for numeric types.

### Mandatory settings (must appear in config)
`AOrder, BOrder, COrder, HOrder, Process, PDF, NonPert, mode, XSec, PertOrder, AsymOrder, scheme`

### Default values (no config entry needed)
`ECM=13000, bMax=1.5, Q0=1.5, beam=pp, EWMassScheme=OnShell, ngag=1, ...`

---

## Execution Flow (`main.cc`)

```
1. Parse CLI options (CLI11)
2. Load settings from resbos.config (or -i file)
3. Initialize PDF (LHAPDF/HOPPET)
4. Setup beams
5. Create Process object (DrellYan, Wpm, ...)
6. Create Calculation object (Resummation, Total, WmA, ...)
   └─ calc->Initialize(&settings, resbos)    ← reads g[], NonPert enum, Q0, bMax, etc.
   └─ calc->GridSetup(settings)              ← reads qMin/qMax/qTMin/qTMax/yMin/yMax
7. If grid doesn't exist:
   a. Generate convolution grids (C1, C2, C1P1, ...)
   b. Generate CxF grid: conv->GenerateGrid(Conv::C)
      → prints "Initializing CxF..." and "Finished generating CxF grid"
   c. Generate calculation grid: calc->GridGen()   ← calls GetCalc() for each (Q,qt,y)
      → GetCalc() → FTransformVec() → NonPert()   ← ERROR THROWN HERE
8. VEGAS warm-up + production run
9. Save histograms
```

---

## Current `resbos.config` Settings

```ini
mode     = MCFull
XSec     = Resummation
beam     = pp
ECM      = 8000
Process  = DrellYan
scheme   = CSS
AOrder   = 4 / BOrder = 3 / COrder = 2 / HOrder = 2
C1=1 / C2=1 / C3=1
NonPert  = BLNY
bMax     = 1.5       # GeV^-1
Q0       = 1.55      # GeV
ngag     = 1         # (not used by parsing loop — all g values in the string are read)
g        = 0.181, 0.167, 0.003   # g[0]=0.181, g[1]=0.167, g[2]=0.003
qMin=66 / qMax=116 / qtMin=0 / qtMax=600 / yMin=-5 / yMax=5
PDF      = CT14nnlo
GridGen  = true
nThreads = 1
```

---

## Resummation Calculation

### Non-perturbative form factor: `Resummation::NonPert(b, Q, x1, x2)`
File: `src/Calculation/Resummation.cc`, line ~260–313.

Computed as `exp(exponent)` where `exponent` depends on `iNonPert`:

| Enum | Formula |
|------|---------|
| `BLNY` | `exponent = -b²·(g[0] + g[2]·ln(100·x1·x2) + g[1]·ln(Q/2Q₀))` |
| `SIYY` | `exponent = -(b²·(g[0]+g[2]·(…)) + g[1]·ln(b/b*))·ln(Q/Q₀))` |
| `SIYYgy` | similar |
| `IY/IY1/IY2/IY6` | variants with rapidity dependence, need g[0]…g[5] |
| `Gaussian` | `exponent = -b²·g[0]` |
| `TMD` | uses g[0]…g[4] |
| `None` | `exponent = 0` |

**Safety check (line 311):**
```cpp
if(exponent > 10)
    throw std::runtime_error("Invalid non-pert params");
```
This guards against non-perturbative amplification (exp(10) ≈ 22026 is unphysical).

### g parameter indexing (BLNY)
From config `g = 0.181, 0.167, 0.003`:
- `g[0] = 0.181` → constant term
- `g[1] = 0.167` → coefficient of `ln(Q/2Q₀)`
- `g[2] = 0.003` → coefficient of `ln(100·x1·x2)`

Standard BLNY values (from `fitting.config`) for comparison:
- `g[0]=0.21, g[1]=0.68, g[2]=-0.126` (note g[2] is **negative** in standard parameterization)

### Ogata quadrature (`OgataQuad`)
Used to compute the Hankel/Fourier-Bessel transform:
```
W(qt) = ∫₀^∞ b·J₀(qt·b)·Sud(b,Q)·CxF(b,x1,x2)·NonPert(b,Q,x1,x2) db
```
Called as: `ogata.FBT(FTransformVec, qt, y, bRange=(0,10), rerr=1e-4, aerr=1e-8)`

The b values sampled are `b = knot/qt`, where `knot = N·ψ(h·ξᵢ)` (N≈π/h, ξᵢ are Bessel zeros/π). For small qt, b can be large (up to ~10/qt).

### x1, x2 computation
```cpp
x1 = Q/ECM * exp(y)
x2 = Q/ECM * exp(-y)    // KinCorr=false (standard)
// or
x1 = sqrt(Q²+qt²)/ECM * exp(y)   // KinCorr=true
```
Both checked: `if(x1 > 1 || x2 > 1) return;` before Fourier transform.

---

## The Error: `Invalid non-pert params`

### Symptom
```
Initializing CxF...
[==================================================] 100%
Finished generating CxF grid
terminate called after throwing an instance of 'std::runtime_error'
  what():  Invalid non-pert params
```

### What it means
After the CxF convolution grid is built, the Resummation calculation grid is being generated via `Calculation::GridGen()`. During this, `GetCalc(Q, qt, y)` is called for each grid point. Inside, the Ogata integration evaluates `FTransformVec(b, Q, x1, x2)`, which calls `NonPert(b, Q, x1, x2)`. The computed `exponent` exceeds 10, triggering the check at `Resummation.cc:311`.

### When can `exponent > 10` for BLNY?
For `exponent = -b²·(g[0] + g[2]·ln(100·x1·x2) + g[1]·ln(Q/2Q₀)) > 10`, the parenthesis must be **negative** AND b² must be large enough.

The parenthesis becomes negative when `g[1]·ln(Q/2Q₀)` is sufficiently negative, which requires **Q < 2·Q₀ = 3.1 GeV**. If Q is forced to a very small value (e.g., due to wrong config parsing), or if g[2] is large and negative, the condition can be triggered.

### Key diagnostic questions (unresolved at session end)
1. **Are the g values actually being read correctly?** With the given config (`g = 0.181, 0.167, 0.003`), the BLNY exponent is mathematically impossible to exceed 0 for physical kinematics (Q ∈ [66,116], ECM=8000). This strongly suggests either:
   - The `g` values at runtime differ from what's in the config, OR
   - A different `NonPertEnum` case is active, OR
   - The `Q` value passed to `NonPert` is somehow very small (< Q₀ = 1.55 GeV)

2. **Could qMin/qMax be read incorrectly?** The key mismatch concern (`"Qmin"` vs `"qMin"`) was ruled out — `ciLess` makes the map case-insensitive.

3. **Could there be a wrong code path?** The Ogata integration evaluates the integrand at b values from the `GetHu` range. For small qt (≈0.09 GeV), b can reach ~10/0.09 ≈ 111 GeV⁻¹, but this still keeps exponent ≤ 0 for BLNY with these g values.

4. **Standard BLNY vs. config values:** Standard fit values (`fitting.config`) use `g[2] = -0.126` (negative). With a large negative g[2] and x1·x2 close to 1 (or > 1 numerically), the parenthesis COULD become negative. If the runtime g values differ, this would explain the error.

### Suggested next steps
- Add a debug print just before the check in `Resummation.cc:311` to log `b`, `Q`, `x1`, `x2`, `exponent`, and all `g` values.
- Verify which `iNonPert` enum value is active at runtime (print it in `Initialize` after the switch).
- Check if a pre-existing grid file is being loaded that was built with different parameters.
- Confirm `g` vector size and contents in `Initialize` by printing `g.size()` and each `g[i]`.

---

## NonPertFit Subsystem

Separate executable (`NonPertFit`). Uses:
- `fitting.config` (not `resbos.config`)
- `parameters.yml` for parameter definitions (name, initial value, bounds, prior)
- `experiments.yml` for dataset definitions
- BAT (Bayesian Analysis Toolkit) for MCMC fitting

Standard BLNY parameters in `parameters.yml`:
```yaml
g1 = 0.21,  g2 = 0.68,  g3 = -0.126
```

---

## Important Class Relationships

```
ResBos (main container)
├── Process (DrellYan / Wpm / ...)
│   ├── PhaseSpace
│   ├── Electroweak (EW masses, widths, couplings)
│   └── Cuts
├── Calculation (Resummation / Total / WmA / ...)
│   ├── Resummation
│   │   ├── NonPert()          ← non-pert form factor
│   │   ├── Sudakov()          ← perturbative Sudakov
│   │   └── OgataQuad          ← Hankel transform engine
│   ├── Asymptotic
│   ├── Perturbative
│   └── Grid3D[]               ← interpolation grid
├── Beam (×2: beam1, beam2)
│   ├── PDF (LHAPDF wrapper)
│   ├── Hoppet (DGLAP evolution)
│   └── Convolution (CxF grids: C, C1, C2, C1P1, ...)
└── Settings + ConfigParser
```

---

## Grid System

Grids are 3D interpolation tables over `(Q, y, qT)`. They are:
- **Generated once** and saved to disk (`Calculation::SaveGrid`)
- **Loaded** on subsequent runs (`Calculation::LoadGrid`) — skips recomputation
- **Interpolated** during VEGAS integration (`Calculation::GetPoint → Grid3D::Interpolate`)

Grid types (enum `GridType`): `Resum`, `Asym`, `DelSig`, `Pert`, `WmA`, `Total`, `Y`.

---

## Convolution Grids (CxF)

Built by `Beam::Convolution`. Types (enum `Conv`):
`C, C1, C2, C1P1, C1P1P1, C1P2, C2P1, G1, G1P1`

The `Conv::C` grid is the coefficient function grid used by the Resummation calculation. Its generation produces the "Initializing CxF..." / "Finished generating CxF grid" messages.

---

## Known Code Issues / Quirks

1. **`ngag` parameter is unused** by the g-parsing loop. All values in the `g = ...` comma-separated string are always read, regardless of `ngag`.

2. **`GetSetting<std::string>` specialization** (Settings.cc) has reversed logic for mandatory/default checks compared to the generic template. Prefer `GetSetting<double>` etc. for numeric settings.

3. **Settings return 0 silently** for unknown optional keys (with a WARNING log). If a key typo goes unnoticed, the setting silently becomes 0.

4. **`iNonPert` is uninitialized** in the `Resummation` default constructor. It is always set in `Initialize()`, but if `Initialize()` is somehow skipped, UB results.

5. **g[3], g[4] accessed** unconditionally at the start of `NonPert()` (as `lambda = g[3]`, `x0 = g[4]`) even for BLNY which only uses g[0]–g[2]. If the `g` vector has fewer than 5 elements, this is **undefined behavior** (out-of-bounds vector access). For BLNY with 3 g values, `g[3]` and `g[4]` are UB. They are only used in the SIYY/SIYYgy/TMD branches, but the assignment itself is UB.

6. **The `exponent > 10` safety threshold** was designed for the case where g[2] < 0 (standard BLNY), causing the exponent to grow positive for large b at small x. With the current config's g[2] = 0.003 (positive), this check should never trigger for BLNY — making the actual crash cause uncertain.
