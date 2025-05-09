(sec-yaml-phases)=
# Phase Definitions

A `phase` is a mapping that contains definitions for the elements, species, and
optionally reactions that can take place in that phase. The fields of a `phase` entry
are:

`name`
: String identifier used for the phase. Required.

`elements`
: Specification for the elements present in the phase. This can be:
  - Omitted, in which case the standard elements will be added as needed by the species
    included in the phase.
  - A list of element symbols, which can be either defined in the `elements` section of
    the file or taken from the standard elements.
  - A list of single-key mappings of section names to lists of element symbols. These
    sections can be in the same file as the phase definition, or from another file if
    written as `file-path/sectionname`. If a relative path is specified, the directory
    containing the current file is searched first, followed by the Cantera data path.
    Standard elements can be included by referencing the fictitious section `default`.

(sec-yaml-phase-species)=
`species`
: Specification for the species present in the phase. This can be:
  - a list of species that appear in the `species` section of the file.
  - The string `all`, to indicate that all species in the `species` section should be
    included. This is the default if no `species` entry is present.
  - A list of single-key mappings of section names to either the string `all` or a list
    of species names. These sections can be in the same file as the phase definition, or
    from another file if written as `file-path/sectionname`. If a relative path is
    specified, the directory containing the current file is searched first, followed by
    the Cantera data path.

  Species may be skipped depending on the setting of the `skip-undeclared-elements`
  option.

`skip-undeclared-elements`
: If set to `true`, do not add species that contain elements that are not explicitly
  included in the phase. The default is `false`, where the presence of such species is
  considered an error. Filtering of reactions is controlled by the `declared-species`
  option in the [](sec-yaml-phase-reactions) entry and the
  `skip-undeclared-third-bodies` flag.

`skip-undeclared-third-bodies`
: If set to `true`, ignore third body efficiencies for species that are not defined in
  the phase. The default is `false`, where the presence of such third body
  specifications is considered an error.

(sec-yaml-phase-explicit-third-body-duplicates)=
`explicit-third-body-duplicates`
: Specifies how to handle three body reactions with an explicit collider that are
  duplicates of a three body reaction with the default collider `M`. This can be:
  - `warn`: Issue a warning about such reactions. This is the default.
  - `error`: Raise an exception if such reactions are found.
  - `mark-duplicate`: Mark the reactions as duplicates. Species production and
    consumption rates will reflect the sum of the rates. This option may correspond to
    the behavior of software packages that do not check for this kind of duplicate
    reaction.
  - `modify-efficiency`: Set the efficiency of the explicit third body to zero for the
    reaction that gives the rate for the default collider. This option is the most
    self-consistent but may not correspond to the intent of the mechanism's authors.

  :::{versionadded} 3.1
  :::

`state`
: A mapping specifying the thermodynamic state. See [](sec-yaml-setting-state).

`adjacent-phases`
: For interface phases, specification of adjacent phases that participate in reactions
  on the interface. This can be:
  - a list of phase names that appear in the `phases` section of the file.
  - A list of single-key mappings of section names to a list of phase names. These
    sections can be in the same file as the current phase definition, or from another
    file if written as `file-path/section-name`. If a relative path is specified, the
    directory containing the current file is searched first, followed by the Cantera
    data path.

(sec-yaml-phase-thermo)=
`thermo`

: String specifying the phase thermodynamic model to be used. Supported model strings
  are:
  - [`binary-solution-tabulated`](sec-yaml-binary-solution-tabulated)
  - [`compound-lattice`](sec-yaml-compound-lattice)
  - [`coverage-dependent-surface`](sec-yaml-coverage-dependent-surface)
  - [`Debye-Huckel`](sec-yaml-Debye-Huckel)
  - [`edge`](sec-yaml-edge)
  - [`electron-cloud`](sec-yaml-electron-cloud)
  - [`fixed-stoichiometry`](sec-yaml-fixed-stoichiometry)
  - [`HMW-electrolyte`](sec-yaml-HMW-electrolyte)
  - [`ideal-gas`](sec-yaml-ideal-gas)
  - [`ideal-molal-solution`](sec-yaml-ideal-molal-solution)
  - [`ideal-condensed`](sec-yaml-ideal-condensed)
  - [`ideal-solution-VPSS`](sec-yaml-ideal-solution-VPSS)
  - [`ideal-surface`](sec-yaml-ideal-surface)
  - [`lattice`](sec-yaml-lattice)
  - [`liquid-water-IAPWS95`](sec-yaml-liquid-water-IAPWS95)
  - [`Margules`](sec-yaml-Margules)
  - [`Peng-Robinson`](sec-yaml-Peng-Robinson)
  - [`plasma`](sec-yaml-plasma)
  - [`pure-fluid`](sec-yaml-pure-fluid)
  - [`Redlich-Kister`](sec-yaml-Redlich-Kister)
  - [`Redlich-Kwong`](sec-yaml-Redlich-Kwong)

(sec-yaml-phase-kinetics)=
`kinetics`
: String specifying the kinetics model to be used. Supported model strings are:
  - `none`
  - `bulk` ({ct}`details <BulkKinetics>`)
  - `gas` (alias for `bulk`)
  - `surface` ({ct}`details <InterfaceKinetics>`)
  - `edge` ({ct}`details <EdgeKinetics>`)

(sec-yaml-phase-reactions)=
`reactions`
: Source of reactions to include in the phase, if a kinetics model has been specified.
  This can be:
  - The string `all`, which indicates that all reactions from the `reactions` section of
    the file should be included. This is the default if no `reactions` entry is present.
  - The string `declared-species`, which indicates that all reactions from the
    `reactions` section involving only species present in the phase should be included.
  - The string `none`, which indicates that no reactions should be added. This can be
    used if reactions will be added programmatically after the phase is constructed.
  - A list of sections from which to include reactions. These sections can be in the
    same file as the phase definition, or from another file if written as
    `file-path/sectionname`. If a relative path is specified, the directory containing
    the current file is searched first, followed by the Cantera data path.
  - A list of single-key mappings of section names to rules for adding reactions, where
    for each section name, that rule is either `all` or `declared-species` and is
    applied as described above.

`Motz-Wise`
: Boolean indicating whether the Motz-Wise correction should be applied to sticking
  reactions. Applicable only to interface phases. The default is `false`. The value set
  at the phase level may be overridden on individual reactions.

(sec-yaml-phase-transport)=
`transport`
: String specifying the transport model to be used. Supported model strings are:
  - `none`
  - `high-pressure`: A model for high-pressure gas transport properties based on a
    method of corresponding states ({ct}`details <HighPressureGasTransport>`)
  - `high-pressure-Chung`: A model for high-pressure gas transport properties based on
    the method of Chung ({ct}`details <ChungHighPressureGasTransport>`)
  - `ionized-gas`: A model implementing the Stockmayer-(n,6,4) model for transport of
    ions in a gas ({ct}`details <IonGasTransport>`)
  - `mixture-averaged`: The mixture-averaged transport model for ideal gases
    ({ct}`details <MixTransport>`)
  - `mixture-averaged-CK`: The mixture-averaged transport model for ideal gases,
    using polynomial fits corresponding to Chemkin-II ({ct}`details <MixTransport>`)
  - `multicomponent`: The multicomponent transport model for ideal gases
    ({ct}`details <MultiTransport>`)
  - `multicomponent-CK`: The multicomponent transport model for ideal gases, using
    polynomial fits corresponding to Chemkin-II ({ct}`details <MultiTransport>`)
  - `unity-Lewis-number`: A transport model for ideal gases, where diffusion
    coefficients for all species are set so that the Lewis number is 1
    ({ct}`details <UnityLewisTransport>`)
  - `water`: A transport model for pure water applicable in both liquid and vapor
    phases ({ct}`details <WaterTransport>`)

(sec-yaml-setting-state)=
## Setting the state

The state of a `phase` can be set using two properties to set the thermodynamic state,
plus the composition.

The composition can be set using one of the following fields, depending on the phase
type. The composition is specified as a mapping of species names to values. Where
necessary, the values will be automatically normalized.

- `mass-fractions` or `Y`
- `mole-fractions` or `X`
- `coverages`
- `molalities` or `M`

The thermodynamic state can be set using the following property pairs, with some
exceptions for phases where setting that property pair is not implemented. All
properties are on a per unit mass basis where relevant:

- `T` and `P`
- `T` and `D`
- `T` and `V`
- `H` and `P`
- `U` and `V`
- `S` and `V`
- `S` and `P`
- `S` and `T`
- `P` and `V`
- `U` and `P`
- `V` and `H`
- `T` and `H`
- `S` and `H`
- `D` and `P`

The following synonyms are also implemented for use in any of the pairs:

- `temperature`, `T`
- `pressure`, `P`
- `enthalpy`, `H`
- `entropy`, `S`
- `int-energy`, `internal-energy`, `U`
- `specific-volume`, `V`
- `density`, `D`

(sec-yaml-phase-thermo-models)=
## Phase thermodynamic models

(sec-yaml-binary-solution-tabulated)=
### `binary-solution-tabulated`

A phase representing a binary solution where the excess enthalpy and entropy are
interpolated between tabulated values as a function of mole fraction, as
{ct}`described here <BinarySolutionTabulatedThermo>`.

Includes the fields of [](sec-yaml-ideal-condensed), plus:

`tabulated-species`
: The name of the species to which the tabulated enthalpy and entropy is added.

`tabulated-thermo`
: A mapping containing three (optionally four) lists of equal lengths:

  `mole-fractions`
  : A list of mole fraction values for the tabulated species.

  `enthalpy`
  : The extra molar enthalpy to be added to the tabulated species at these mole
    fractions.

  `entropy`
  : The extra molar entropy to be added to the tabulated species at these mole
    fractions.

  `molar-volume`
  : The molar volume of the phase at these mole fractions. This input is optional.

```yaml
- name: graphite-anode
  thermo: binary-solution-tabulated
  species: ["Li[anode]", "V[anode]"]
  standard-concentration-basis: unity
  tabulated-species: Li[anode]
  units: {energy: J, quantity: mol, pressure: atm}
  tabulated-thermo:
    mole-fractions: [5.75000E-03, 1.25841E-01, 2.45932E-01, 3.66023E-01,
      4.86114E-01, 6.06205E-01, 7.26295E-01]
    enthalpy: [-6.40692E+04, -9.69664E+03, -8.31339E+03, -7.69063E+03,
      -3.94568E+03, -2.01329E+03, -1.59649E+03]
    entropy: [3.05724E+01, 2.53501E+01, 1.27000E+01, 1.21865E+01, 1.70474E+01,
      1.92980E+01, 1.92885E+01]
  state: {T: 300, P: 1, X: {"Li[anode]": 0.3, "V[anode]": 0.7}}
```

:::{versionadded} 2.5
:::

(sec-yaml-compound-lattice)=
### `compound-lattice`

A phase that is comprised of a fixed additive combination of other lattice phases, as
{ct}`described here <LatticeSolidPhase>`.

Additional fields:

`composition`
: A mapping of component phase names to their relative stoichiometries.

Example:

```yaml
- name: Li7Si3_and_Interstitials(S)
  elements: [Li, Si]
  thermo: compound-lattice
  composition: {Li7Si3(s): 1.0, Li7Si3-interstitial: 1.0}
```

(sec-yaml-coverage-dependent-surface)=

### `coverage-dependent-surface`

A coverage-dependent surface phase. That is, a surface phase where the enthalpy,
entropy, and heat capacity of each species may depend on its coverage and the
coverage of other species in the phase. Full details are
{ct}`described here <CoverageDependentSurfPhase>`. The majority of coverage dependency
parameters are provided in the species entry as
[described here](sec-yaml-species-coverage).

Additional fields:

`site-density`
: The molar density of surface sites.

`reference-state-coverage`
: The reference state coverage denoting the low-coverage limit (ideal-surface)
  thermodynamic properties.

Example:

```yaml
- name: covdep
  thermo: coverage-dependent-surface
  species: [Pt, OC_Pt, CO2_Pt, C_Pt, O_Pt]
  state:
    T: 500.0
    P: 1.01325e+05
    coverages: {Pt: 0.5, OC_Pt: 0.5, CO2_Pt: 0.0, C_Pt: 0.0, O_Pt: 0.0}
  site-density: 2.72e-09
  reference-state-coverage: 0.22
```

:::{versionadded} 3.0
:::

(sec-yaml-debye-huckel)=
### `Debye-Huckel`

A dilute liquid electrolyte which obeys the Debye-Hückel formulation for nonideality as
{ct}`described here <DebyeHuckel>`. Additional parameters for this model are contained
in the `activity-data` field:

`activity-data`
: The activity data field contains the following fields:

  `model`
  : One of `dilute-limit`, `B-dot-with-variable-a`,
    `B-dot-with-common-a`, `beta_ij`, or `Pitzer-with-beta_ij`

  `A_Debye`
  : The value of the Debye "A" parameter, or the string `variable` to use a calculation
    based on the water equation of state. Defaults to the constant value of 1.172576
    kg^0.5/gmol^0.5, a nominal value for water at 298 K and 1 atm.

  `B_Debye`
  : The Debye "B" parameter. Defaults to 3.2864e+09 kg^0.5/gmol^0.5/m, a nominal
    value for water.

  `max-ionic-strength`
  : The maximum ionic strength

  `use-Helgeson-fixed-form`
  : Boolean, `true` or `false`

  `default-ionic-radius`
  : Ionic radius to use for species where the ionic radius has not been specified.

  `B-dot`
  : The value of B-dot.

  `beta`
  : List of mappings providing values of $\beta_{ij}$ for different species pairs. Each
    mapping contains a `species` key that contains a list of two species names, and a
    `beta` key that contains the corresponding value of $\beta_{ij}$.

Example:

```yaml
- name: debye-huckel-pitzer-beta_ij-IAPWS
  species:
  - water_IAPWS: [H2O(L)]
  - species_waterSolution: [Na+, Cl-, H+, OH-, NaCl(aq), NaOH(aq)]
  thermo: Debye-Huckel
  activity-data:
    model: Pitzer-with-beta_ij
    A_Debye: variable
    B_Debye: 3.28640E9 kg^0.5/gmol^0.5/m
    default-ionic-radius: 3.042843 angstrom
    max-ionic-strength: 3.0
    beta:
    - species: [H+, Cl-]
      beta: 0.27
    - species: [Na+, Cl-]
      beta: 0.15
    - species: [Na+, OH-]
      beta: 0.06
  state:
    T: 300.0 K
    P: 1.01325e+05 Pa
    molalities: {Na+: 3.0, Cl-: 3.0, H+: 1.0499e-08, OH-: 1.3765e-06,
                 NaCl(aq): 0.98492, NaOH(aq): 3.8836e-06}
```

In addition, the Debye-Hückel model uses several species-specific properties which may
be defined in the `Debye-Huckel` field of the *species* entry. These properties are:

`ionic-radius`
: Size of the species.

`electrolyte-species-type`
: One of `solvent`, `charged-species`, `weak-acid-associated`, `strong-acid-associated`,
  `polar-neutral`, or `nonpolar-neutral`. The type `solvent` is the default for the
  first species in the phase. The type `charged-species` is the default for species with
  a net charge. Otherwise, the default is and `nonpolar-neutral`.

`weak-acid-charge`
: Charge to use for species that can break apart into charged species.

Example:

```yaml
species:
- name: NaCl(aq)
  composition: {Na: 1, Cl: 1}
  thermo:
    model: piecewise-Gibbs
    h0: -96.03E3 cal/mol
    dimensionless: true
    data: {298.15: -174.5057463, 333.15: -174.5057463}
  equation-of-state:
    model: constant-volume
    molar-volume: 1.3
  Debye-Huckel:
    ionic-radius: 4 angstrom
    electrolyte-species-type: weak-acid-associated
    weak-acid-charge: -1.0
```

(sec-yaml-edge)=
### `edge`

A one-dimensional edge between two surfaces, as {ct}`described here <EdgePhase>`.

Additional fields:

`site-density`
: The molar density of sites per unit length along the edge

Example:

```yaml
- name: tpb
  thermo: edge
  adjacent-phases: [metal, metal_surface, oxide_surface]
  elements: [H, O]
  species: [(tpb)]
  kinetics: edge
  reactions: [tpb-reactions]
  state: {T: 1073.15, coverages: {(tpb): 1.0}}
  site-density: 5.0e-17 mol/cm
```

(sec-yaml-electron-cloud)=
### `electron-cloud`

A phase representing an electron cloud, such as conduction electrons in a metal,
as {ct}`described here <MetalPhase>`.

Additional fields:

`density`
: The density of the bulk metal

Example:

```yaml
- name: metal
  thermo: electron-cloud
  elements: [E]
  species: [electron]
  state:
    T: 1073.15
    X: {electron: 1.0}
  density: 9 g/cm^3
```

(sec-yaml-fixed-stoichiometry)=
### `fixed-stoichiometry`

An incompressible phase with fixed composition, as {ct}`described here <StoichSubstance>`.

Example:

```yaml
- name: diamond
  thermo: fixed-stoichiometry
  elements: [C]
  species: [C(d)]
```

(sec-yaml-hmw-electrolyte)=
### `HMW-electrolyte`

A dilute or concentrated liquid electrolyte phase that obeys the Pitzer formulation for
nonideality, as {ct}`described here <HMWSoln>`.

Additional parameters for this model are contained in the `activity-data` field:

`activity-data`
: The activity data field contains the following fields:

  `temperature-model`
  : The form of the Pitzer temperature model. One of `constant`, `linear` or `complex`.
    The default is `constant`.

  `A_Debye`
  : The value of the Debye "A" parameter, or the string `variable` to use a calculation
    based on the water equation of state. The default is 1.172576 kg^0.5/gmol^0.5, a
    nominal value for water at 298 K and 1 atm.

  `max-ionic-strength`
  : The maximum ionic strength

  `interactions`
  : A list of mappings, where each mapping describes a binary or ternary interaction
    among species. Fields of this mapping include:

    `species`
    : A list of one to three species names

    `beta0`
    : The $\beta^{(0)}$ parameters for an cation/anion interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `beta1`
    : The $\beta^{(1)}$ parameters for an cation/anion interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `beta2`
    : The $\beta^{(2)}$ parameters for an cation/anion interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `Cphi`
    : The $C^\phi$ parameters for an cation/anion interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `alpha1`
    : The $\alpha^{(1)}$ parameter for an cation/anion interaction.

    `alpha2`
    : The $\alpha^{(2)}$ parameter for an cation/anion interaction.

    `theta`
    : The $\theta$ parameters for a like-charged binary interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `lambda`
    : The $\lambda$ parameters for binary interactions involving at least one neutral
      species. 1, 2, or 5 values depending on the value of `temperature-model`.

    `psi`
    : The $\Psi$ parameters for ternary interactions involving three charged species.
      1, 2, or 5 values depending on the value of `temperature-model`.

    `zeta`
    : The $\zeta$ parameters for ternary interactions involving one neutral species. 1,
      2, or 5 values depending on the value of `temperature-model`.

    `mu`
    : The $\mu$ parameters for a neutral species self-interaction.
      1, 2, or 5 values depending on the value of `temperature-model`.

  `cropping-coefficients`
  : Parameters in the molality exponential cutoff treatment

    `ln_gamma_k_min`
    : Default -5.0.

    `ln_gamma_k_max`
    : Default 15.0.

    `ln_gamma_o_min`
    : Default -6.0.

    `ln_gamma_o_max`
    : Default 3.0.

Example:

```yaml
- name: NaCl_electrolyte
  thermo: HMW-electrolyte
  activity-data:
    temperature-model: complex
    A_Debye: 1.175930 kg^0.5/gmol^0.5
    interactions:
    - species: [Na+, Cl-]
      beta0: [0.0765, 0.008946, -3.3158E-6, -777.03, -4.4706]
      beta1: [0.2664, 6.1608E-5, 1.0715E-6, 0.0, 0.0]
      beta2: [0.0, 0.0, 0.0, 0.0, 0.0]
      Cphi: [0.00127, -4.655E-5, 0.0, 33.317, 0.09421]
      alpha1: 2.0
    - species: [H+, Cl-]
      beta0: [0.1775]
      beta1: [0.2945]
      beta2: [0.0]
      Cphi: [0.0008]
      alpha1: 2.0
    - species: [Na+, OH-]
      beta0: 0.0864
      beta1: 0.253
      beta2: 0.0
      Cphi: 0.0044
      alpha1: 2.0
      alpha2: 0.0
    - {species: [Cl-, OH-], theta: -0.05}
    - {species: [Na+, Cl-, OH-], psi: -0.006}
    - {species: [Na+, H+], theta: 0.036}
    - {species: [Cl-, Na+, H+], psi: [-0.004]}
```

(sec-yaml-ideal-gas)=
### `ideal-gas`

A mixture which obeys the ideal gas law, as {ct}`described here <IdealGasPhase>`.

Example:

```yaml
- name: ohmech
  thermo: ideal-gas
  species: [H2, H, O, O2, OH, H2O, HO2, H2O2, AR, N2]
  kinetics: gas
  transport: mixture-averaged
  state: {T: 300.0, P: 1 atm}
```

(sec-yaml-ideal-molal-solution)=
### `ideal-molal-solution`

An ideal solution based on the mixing-rule assumption that all molality-based activity
coefficients are equal to one, as {ct}`described here <IdealMolalSoln>`.

Additional fields:

`standard-concentration-basis`
: A string specifying the basis for the standard concentration. One of `unity`,
  `species-molar-volume`, or `solvent-molar-volume`.

`cutoff`
: Parameters for cutoff treatments of activity coefficients

  `model`
  : `poly` or `polyExp`

  `gamma_o`
  : gamma_o value for the cutoff process at the zero solvent point

  `gamma_k`
  : gamma_k minimum for the cutoff process at the zero solvent point

  `X_o`
  : value of the solute mole fraction that centers the cutoff polynomials
    for the cutoff = 1 process

  `c_0`
  : Parameter in the polyExp cutoff treatment having to do with rate of
    exponential decay

  `slope_f`
  : Parameter in the `polyExp` cutoff treatment

  `slope_g`
  : Parameter in the `polyExp` cutoff treatment

Example:

```yaml
- name: NaCl_electrolyte
  species: [H2O(L), Cl-, H+, Na+, OH-]
  thermo: ideal-molal-solution
  standard-concentration-basis: solvent-molar-volume
  cutoff:
    model: polyexp
    gamma_o: 0.0001
    gamma_k: 10.0
    X_o: 0.2
    c_0: 0.05
    slope_f: 0.6
    slope_g: 0.0
  state: {T: 298.15 K, P: 1.01325e+05 Pa,
          molalities: {Na+: 6.0954, Cl-: 6.0954, H+: 2.1628e-09, OH-: 1.3977e-06}}

```

(sec-yaml-ideal-condensed)=
### `ideal-condensed`

An ideal liquid or solid solution as {ct}`described here <IdealSolidSolnPhase>`.

Additional fields:

`standard-concentration-basis`
: A string specifying the basis for the standard concentration. One of
  `unity`, `species-molar-volume`, or `solvent-molar-volume`.

Example:

```yaml
- name: electrolyte
  thermo: ideal-condensed
  species: ['C3H4O3[elyt]', 'C4H6O3[elyt]', 'Li+[elyt]', 'PF6-[elyt]']
  state:
    X: {'C3H4O3[elyt]': 0.47901, 'C4H6O3[elyt]': 0.37563, 'Li+[elyt]': 0.07268,
        'PF6-[elyt]': 0.07268}
  standard-concentration-basis: unity
```

(sec-yaml-ideal-solution-vpss)=
### `ideal-solution-VPSS`

An ideal solution model using variable pressure standard state methods as
{ct}`described here <IdealSolnGasVPSS>`.

Additional fields:

`standard-concentration-basis`
: A string specifying the basis for the standard concentration. One of
  `unity`, `species-molar-volume`, or `solvent-molar-volume`.

Example:

```yaml
- name: NaCl_electrolyte
  species: [H2O(L), Na+, Cl-, H+, OH-]
  thermo: ideal-solution-VPSS
  standard-concentration-basis: solvent-molar-volume
```

(sec-yaml-ideal-surface)=
### `ideal-surface`

An ideal surface between two bulk phases, as {ct}`described here <SurfPhase>`.

Additional fields:

`site-density`
: The molar density of surface sites

Example:

```yaml
- name: Pt_surf
  thermo: ideal-surface
  adjacent-phases: [gas]
  elements: [Pt, H, O, C]
  species: [PT(S), H(S), H2O(S), OH(S), CO(S), CO2(S), CH3(S), CH2(S)s,
    CH(S), C(S), O(S)]
  kinetics: surface
  reactions: all
  state:
    T: 900.0
    coverages: {O(S): 0.0, PT(S): 0.5, H(S): 0.5}
  site-density: 2.7063e-09
```

(sec-yaml-lattice)=
### `lattice`

A simple thermodynamic model for a bulk phase, assuming an incompressible lattice of
solid atoms, as {ct}`described here <LatticePhase>`.

Additional fields:

`site-density`
: The molar density of lattice sites

Example:

```yaml
- name: oxide_bulk
  thermo: lattice
  species: [Ox, VO**]
  state: {T: 1073.15, P: 1.01325e+05, X: {Ox: 0.95, VO**: 0.05}}
  site-density: 0.0176 mol/cm^3
```

(sec-yaml-liquid-water-iapws95)=
### `liquid-water-IAPWS95`

An implementation of the IAPWS95 equation of state for water {cite:p}`wagner2002`, for
the liquid region only as {ct}`described here <WaterSSTP>`.

Example:

```yaml
- name: liquid-water-IAPWS95
  species: [H2O]
  thermo: liquid-water-IAPWS95
  state: {T: 300.0, P: 1.01325e+05}
```

(sec-yaml-margules)=
### `Margules`

A phase employing the Margules approximation for the excess Gibbs free energy, as
{ct}`described here <MargulesVPSSTP>`.

Additional fields:

`interactions`
: A list of mappings, where each mapping has the following fields:

  `species`
  : A list of two species names

  `excess-enthalpy`
  : A list of two values specifying the first and second excess enthalpy coefficients
    for the interaction of the specified species. Defaults to [0, 0].

  `excess-entropy`
  : A list of two values specifying the first and second excess entropy coefficients for
    the interaction of the specified species. Defaults to [0, 0].

  `excess-volume-enthalpy`
  : A list of two values specifying the first and second enthalpy coefficients for the
    excess volume interaction of the specified species. Defaults to [0, 0].

  `excess-volume-entropy`
  : A list of two values specifying the first and second entropy coefficients for the
    excess volume interaction of the specified species. Defaults to [0, 0].

Example:

```yaml
- name: MoltenSalt_electrolyte
  species: [LiCl(L), KCl(L)]
  thermo: Margules
  interactions:
  - species: [KCl(L), LiCl(L)]
    excess-enthalpy: [-17570. J/gmol, -377 J/gmol]
    excess-entropy: [-7.627 J/gmol/K, 4.958 J/gmol/K]
```

(sec-yaml-peng-robinson)=
### `Peng-Robinson`

A multi-species real gas following the Peng-Robinson equation of state, as
{ct}`described here <PengRobinson>`.

The parameters for each species are contained in the corresponding species
entries. See [Peng-Robinson species equation of state](sec-yaml-eos-peng-robinson).

Example:

```yaml
- name: CO2-PR
  species: [CO2, H2O, H2, CO, CH4, O2, N2]
  thermo: Peng-Robinson
  kinetics: bulk
  state: {T: 300, P: 1 atm, mole-fractions: {CO2: 0.99, H2: 0.01}}
```

:::{versionadded} 3.0
:::

(sec-yaml-plasma)=
### `plasma`

A phase for plasma. This phase handles plasma properties such as the electron energy
distribution and electron temperature with different models as
{ct}`described here <PlasmaPhase>`.

Additional fields:

`electron-energy-distribution`
: A mapping with the following fields:

  `type`
  : String specifying the type of the electron energy distribution to be used.
    Supported model strings are:

    - `isotropic`
    - `discretized`

  `shape-factor`
  : A constant in the isotropic distribution, which is shown as x in the detailed
    description of this class. The value needs to be a positive number. This field is
    only used with `isotropic`. Defaults to 2.0.

  `mean-electron-energy`
  : Mean electron energy of the isotropic distribution. The default sets the electron
    temperature equal gas temperature and uses the corresponding electron energy as mean
    electron energy. This field is only used with `isotropic`.

  `energy-levels`
  : A list of values specifying the electron energy levels. The default uses 1001 equal
    spaced points from 0 to 1 eV.

  `distribution`
  : A list of values specifying the discretized electron energy distribution. This field
    is only used with `discretized`.

  `normalize`
  : A flag specifying whether normalizing the discretized electron energy distribution
    or not. This field is only used with `discretized`. Defaults to `true`.

Examples:

```yaml
- name: isotropic-electron-energy-plasma
  thermo: plasma
  kinetics: gas
  transport: ionized-gas
  electron-energy-distribution:
    type: isotropic
    shape-factor: 2.0
    mean-electron-energy: 1.0 eV
    energy-levels: [0.0, 0.1, 1.0, 10.0]

- name: discretized-electron-energy-plasma
  thermo: plasma
  kinetics: gas
  transport: ionized-gas
  electron-energy-distribution:
    type: discretized
    energy-levels: [0.0, 0.1, 1.0, 10.0]
    distribution: [0.0, 0.2, 0.7, 0.01]
    normalize: False
```

:::{versionadded} 2.6
:::

(sec-yaml-pure-fluid)=
### `pure-fluid`

A phase representing a pure fluid equation of state for one of several pure substances
including liquid, vapor, two-phase, and supercritical regions, as
{ct}`described here <PureFluidPhase>`.

Additional fields:

`pure-fluid-name`
: Name of the pure fluid model to use:
  - `carbon-dioxide`
  - `heptane`
  - `HFC-134a`
  - `hydrogen`
  - `methane`
  - `nitrogen`
  - `oxygen`
  - `water`

Example:

```yaml
- name: carbon-dioxide
  thermo: pure-fluid
  species: [CO2]
  state: {T: 280.0, P: 1.01325e+05}
  pure-fluid-name: carbon-dioxide
```

(sec-yaml-redlich-kister)=
### `Redlich-Kister`

A phase employing the Redlich-Kister approximation for the excess Gibbs free energy, as
{ct}`described here <RedlichKisterVPSSTP>`.

Additional fields:

`interactions`
: A list of mappings, where each mapping has the following fields:

  `species`
  : A list of two species names

  `excess-enthalpy`
  : A list of polynomial coefficients for the excess enthalpy of the specified binary
    interaction

  `excess-entropy`
  : A list of polynomial coefficients for the excess entropy of the specified binary
    interaction

Example:

```yaml
- name: LiC6_and_Vacancies
  thermo: Redlich-Kister
  interactions:
  - species: [Li(C6), V(C6)]
    excess-enthalpy: [-3.268e+06, 3.955e+06, -4.573e+06, 6.147e+06, -3.339e+06,
                      1.117e+07, 2.997e+05, -4.866e+07, 1.362e+05, 1.373e+08,
                      -2.129e+07, -1.722e+08, 3.956e+07, 9.302e+07, -3.280e+07]
    excess-entropy: [0.0]
```

(sec-yaml-redlich-kwong)=
### `Redlich-Kwong`

A multi-species Redlich-Kwong phase as {ct}`described here <RedlichKwongMFTP>`.

The parameters for each species are contained in the corresponding species
entries. See [Redlich-Kwong species equation of state](sec-yaml-eos-redlich-kwong).
