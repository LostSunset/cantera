/**
 * @file InterfaceKinetics.h
 */

// This file is part of Cantera. See License.txt in the top-level directory or
// at https://cantera.org/license.txt for license and copyright information.

#ifndef CT_IFACEKINETICS_H
#define CT_IFACEKINETICS_H

#include "Kinetics.h"

namespace Cantera
{

// forward declarations
class SurfPhase;
class ImplicitSurfChem;

//! A kinetics manager for heterogeneous reaction mechanisms. The reactions are
//! assumed to occur at a 2D interface between two 3D phases.
/*!
 * There are some important additions to the behavior of the kinetics class due
 * to the presence of multiple phases and a heterogeneous interface. If a
 * reactant phase doesn't exists, that is, has a mole number of zero, a
 * heterogeneous reaction can not proceed from reactants to products. Note it
 * could perhaps proceed from products to reactants if all of the product phases
 * exist.
 *
 * In order to make the determination of whether a phase exists or not actually
 * involves the specification of additional information to the kinetics object.,
 * which heretofore has only had access to intrinsic field information about the
 * phases (for example, temperature, pressure, and mole fraction).
 *
 * The extrinsic specification of whether a phase exists or not must be
 * specified on top of the intrinsic calculation of the reaction rate. This
 * class carries a set of booleans indicating whether a phase in the
 * heterogeneous mechanism exists or not.
 *
 * Additionally, the class carries a set of booleans around indicating whether a
 * product phase is stable or not. If a phase is not thermodynamically stable,
 * it may be the case that a particular reaction in a heterogeneous mechanism
 * will create a product species in the unstable phase. However, other reactions
 * in the mechanism will destruct that species. This may cause oscillations in
 * the formation of the unstable phase from time step to time step within a ODE
 * solver, in practice. In order to avoid this situation, a set of booleans is
 * tracked which sets the stability of a phase. If a phase is deemed to be
 * unstable, then species in that phase will not be allowed to be birthed by the
 * kinetics operator. Nonexistent phases are deemed to be unstable by default,
 * but this can be changed.
 *
 * @ingroup kineticsmgr
 */
class InterfaceKinetics : public Kinetics
{
public:
    //! Constructor
    InterfaceKinetics() = default;

    ~InterfaceKinetics() override;

    void resizeReactions() override;

    string kineticsType() const override {
        return "surface";
    }

    //! Set the electric potential in the nth phase
    /*!
     * @param n phase Index in this kinetics object.
     * @param V Electric potential (volts)
     */
    void setElectricPotential(int n, double V);

    //! @name Reaction Rates Of Progress
    //! @{

    //! Equilibrium constant for all reactions including the voltage term
    /*!
     *   Kc = exp(deltaG/RT)
     *
     *   where deltaG is the electrochemical potential difference between
     *   products minus reactants.
     */
    void getEquilibriumConstants(double* kc) override;

    void getDeltaGibbs(double* deltaG) override;

    void getDeltaElectrochemPotentials(double* deltaM) override;
    void getDeltaEnthalpy(double* deltaH) override;
    void getDeltaEntropy(double* deltaS) override;

    void getDeltaSSGibbs(double* deltaG) override;
    void getDeltaSSEnthalpy(double* deltaH) override;
    void getDeltaSSEntropy(double* deltaS) override;

    //! @}
    //! @name Reaction Mechanism Informational Query Routines
    //! @{

    void getActivityConcentrations(double* const conc) override;
    void getFwdRateConstants(double* kfwd) override;
    void getRevRateConstants(double* krev, bool doIrreversible=false) override;

    //! @}
    //! @name Reaction Mechanism Construction
    //! @{

    //!  Add a thermo phase to the kinetics manager object.
    /*!
     * This must be done before the function init() is called or
     * before any reactions are input. The lowest dimensional phase, where reactions
     * occur, must be added first.
     *
     * This function calls Kinetics::addThermo). It also sets the following
     * fields:
     *
     *        m_phaseExists[]
     *
     * @param thermo    Reference to the ThermoPhase to be added.
     */
    void addThermo(shared_ptr<ThermoPhase> thermo) override;

    void init() override;
    void resizeSpecies() override;
    bool addReaction(shared_ptr<Reaction> r, bool resize=true) override;
    void modifyReaction(size_t i, shared_ptr<Reaction> rNew) override;
    void setMultiplier(size_t i, double f) override;
    //! @}

    //! Internal routine that updates the Rates of Progress of the reactions
    /*!
     *  This is actually the guts of the functionality of the object
     */
    void updateROP() override;

    //! Update properties that depend on temperature
    /*!
     *  Current objects that this function updates:
     *       m_kdata->m_rfn
     *       updateKc();
     */
    void _update_rates_T();

    //! Update properties that depend on the electric potential
    void _update_rates_phi();

    //! Update properties that depend on the species mole fractions and/or
    //! concentration,
    /*!
     * This method fills out the array of generalized concentrations by calling
     * method getActivityConcentrations for each phase, which classes
     * representing phases should overload to return the appropriate quantities.
     */
    void _update_rates_C();

    //! Advance the surface coverages in time
    /*!
     * This method carries out a time-accurate advancement of the
     * surface coverages for a specified amount of time.
     *
     *  @f[
     *    \dot {\theta}_k = \dot s_k (\sigma_k / s_0)
     *  @f]
     *
     * @param tstep  Time value to advance the surface coverages
     * @param rtol   The relative tolerance for the integrator
     * @param atol   The absolute tolerance for the integrator
     * @param maxStepSize   The maximum step-size the integrator is allowed to take.
     *                      If zero, this option is disabled.
     * @param maxSteps   The maximum number of time-steps the integrator can take.
     *                   If not supplied, uses the default value in CVodeIntegrator (20000).
     * @param maxErrTestFails   the maximum permissible number of error test failures
     *                           If not supplied, uses the default value in CVODES (7).
     */
    void advanceCoverages(double tstep, double rtol=1.e-7,
                          double atol=1.e-14, double maxStepSize=0,
                          size_t maxSteps=20000, size_t maxErrTestFails=7);

    //! Solve for the pseudo steady-state of the surface problem
    /*!
     * This is the same thing as the advanceCoverages() function,
     * but at infinite times.
     *
     * Note, a direct solve is carried out under the hood here,
     * to reduce the computational time.
     *
     * @param ifuncOverride One of the values defined in @ref solvesp_methods.
     *         The default is -1, which means that the program will decide.
     * @param timeScaleOverride When a pseudo transient is selected this value
     *             can be used to override the default time scale for
     *             integration which is one. When SFLUX_TRANSIENT is used, this
     *             is equal to the time over which the equations are integrated.
     *             When SFLUX_INITIALIZE is used, this is equal to the time used
     *             in the initial transient algorithm, before the equation
     *             system is solved directly.
     */
    void solvePseudoSteadyStateProblem(int ifuncOverride = -1,
                                       double timeScaleOverride = 1.0);

    void setIOFlag(int ioFlag);

    //! Update the standard state chemical potentials and species equilibrium
    //! constant entries
    /*!
     *  Virtual because it is overridden when dealing with experimental open
     *  circuit voltage overrides
     */
    virtual void updateMu0();

    //! Update the equilibrium constants and stored electrochemical potentials
    //! in molar units for all reversible reactions and for all species.
    /*!
     *  Irreversible reactions have their equilibrium constant set
     *  to zero. For reactions involving charged species the equilibrium
     *  constant is adjusted according to the electrostatic potential.
     */
    void updateKc();

    //! Set the existence of a phase in the reaction object
    /*!
     *  Tell the kinetics object whether a phase in the object exists. This is
     *  actually an extrinsic specification that must be carried out on top of
     *  the intrinsic calculation of the reaction rate. The routine will also
     *  flip the IsStable boolean within the kinetics object as well.
     *
     *  @param iphase  Index of the phase. This is the order within the
     *      internal thermo vector object
     *  @param exists  Boolean indicating whether the phase exists or not
     */
    void setPhaseExistence(const size_t iphase, const int exists);

    //! Set the stability of a phase in the reaction object
    /*!
     *  Tell the kinetics object whether a phase in the object is stable.
     *  Species in an unstable phase will not be allowed to have a positive
     *  rate of formation from this kinetics object. This is actually an
     *  extrinsic specification that must be carried out on top of the
     *  intrinsic calculation of the reaction rate.
     *
     *  While conceptually not needed since kinetics is consistent with thermo
     *  when taken as a whole, in practice it has found to be very useful to
     *  turn off the creation of phases which shouldn't be forming. Typically
     *  this can reduce the oscillations in phase formation and destruction
     *  which are observed.
     *
     *  @param iphase  Index of the phase. This is the order within the
     *      internal thermo vector object
     *  @param isStable Flag indicating whether the phase is stable or not
     */
    void setPhaseStability(const size_t iphase, const int isStable);

    //! Gets the phase existence int for the ith phase
    /*!
     * @param iphase  Phase Id
     * @return The int specifying whether the kinetics object thinks the phase
     *         exists or not. If it exists, then species in that phase can be
     *         a reactant in reactions.
     */
    int phaseExistence(const size_t iphase) const;

    //! Gets the phase stability int for the ith phase
    /*!
     * @param iphase  Phase Id
     * @return The int specifying whether the kinetics object thinks the phase
     *         is stable with nonzero mole numbers. If it stable, then the
     *         kinetics object will allow for rates of production of of
     *         species in that phase that are positive.
     */
    int phaseStability(const size_t iphase) const;

    //! Gets the interface current for the ith phase
    /*!
    * @param iphase Phase Id
    * @return The double specifying the interface current. The interface current
    *         is useful when charge transfer reactions occur at an interface. It
    *         is defined here as the net positive charge entering the phase
    *         specified by the Phase Id. (Units: A/m^2 for a surface reaction,
    *         A/m for an edge reaction).
    */
    double interfaceCurrent(const size_t iphase);

    void setDerivativeSettings(const AnyMap& settings) override;
    void getDerivativeSettings(AnyMap& settings) const override;
    Eigen::SparseMatrix<double> fwdRatesOfProgress_ddCi() override;
    Eigen::SparseMatrix<double> revRatesOfProgress_ddCi() override;
    Eigen::SparseMatrix<double> netRatesOfProgress_ddCi() override;

protected:
    //! @name Internal service methods
    //!
    //! @note These methods are for internal use, and seek to avoid code duplication
    //! while evaluating terms used for rate constants, rates of progress, and
    //! their derivatives.
    //! @{


    //! Multiply rate with inverse equilibrium constant
    void applyEquilibriumConstants(double* rop);

    //! Process mole fraction derivative
    //! @param stoich  stoichiometry manager
    //! @param in  rate expression used for the derivative calculation
    //! @return a sparse matrix of derivative contributions for each reaction of
    //! dimensions nTotalReactions by nTotalSpecies
    Eigen::SparseMatrix<double> calculateCompositionDerivatives(StoichManagerN& stoich,
                                            const vector<double>& in);

    //! Helper function ensuring that all rate derivatives can be calculated
    //! @param name  method name used for error output
    //! @throw CanteraError if coverage dependence or electrochemical reactions are
    //! included
    void assertDerivativesValid(const string& name);

    //! @}

    //! Temporary work vector of length m_kk
    vector<double> m_grt;

    bool m_redo_rates = false;

    //! Array of concentrations for each species in the kinetics mechanism
    /*!
     * An array of generalized concentrations @f$ C_k @f$ that are defined
     * such that @f$ a_k = C_k / C^0_k, @f$ where @f$ C^0_k @f$ is a standard
     * concentration/ These generalized concentrations are used by this
     * kinetics manager class to compute the forward and reverse rates of
     * elementary reactions. The "units" for the concentrations of each phase
     * depend upon the implementation of kinetics within that phase. The order
     * of the species within the vector is based on the order of listed
     * ThermoPhase objects in the class, and the order of the species within
     * each ThermoPhase class.
     */
    vector<double> m_conc;

    //! Array of activity concentrations for each species in the kinetics object
    /*!
     * An array of activity concentrations @f$ Ca_k @f$ that are defined
     * such that @f$ a_k = Ca_k / C^0_k, @f$ where @f$ C^0_k @f$ is a standard
     * concentration. These activity concentrations are used by this
     * kinetics manager class to compute the forward and reverse rates of
     * elementary reactions. The "units" for the concentrations of each phase
     * depend upon the implementation of kinetics within that phase. The order
     * of the species within the vector is based on the order of listed
     * ThermoPhase objects in the class, and the order of the species within
     * each ThermoPhase class.
     */
    vector<double> m_actConc;

    //! Vector of standard state chemical potentials for all species
    /*!
     * This vector contains a temporary vector of standard state chemical
     * potentials for all of the species in the kinetics object
     *
     * Length = m_kk. Units = J/kmol.
     */
    vector<double> m_mu0;

    //! Vector of chemical potentials for all species
    /*!
     * This vector contains a vector of chemical potentials for all of the
     * species in the kinetics object
     *
     * Length = m_kk. Units = J/kmol.
     */
    vector<double> m_mu;

    //! Vector of standard state electrochemical potentials modified by a
    //! standard concentration term.
    /*!
     * This vector contains a temporary vector of standard state electrochemical
     * potentials + RTln(Cs) for all of the species in the kinetics object
     *
     * In order to get the units correct for the concentration equilibrium
     * constant, each species needs to have an RT ln(Cs)  added to its
     * contribution to the equilibrium constant Cs is the standard concentration
     * for the species. Frequently, for solid species, Cs is equal to 1.
     * However, for gases Cs is P/RT. Length = m_kk. Units = J/kmol.
     */
    vector<double> m_mu0_Kc;

    //! Vector of phase electric potentials
    /*!
     * Temporary vector containing the potential of each phase in the kinetics
     * object. length = number of phases. Units = Volts.
     */
    vector<double> m_phi;

    //! Pointer to the single surface phase
    SurfPhase* m_surf = nullptr;

    //! Pointer to the Implicit surface chemistry object
    /*!
     * Note this object is owned by this InterfaceKinetics object. It may only
     * be used to solve this single InterfaceKinetics object's surface problem
     * uncoupled from other surface phases.
     */
    ImplicitSurfChem* m_integrator = nullptr;

    bool m_ROP_ok = false;

    //! Current temperature of the data
    double m_temp = 0.0;

    //! Int flag to indicate that some phases in the kinetics mechanism are
    //! non-existent.
    /*!
     *  We change the ROP vectors to make sure that non-existent phases are
     *  treated correctly in the kinetics operator. The value of this is equal
     *  to the number of phases which don't exist.
     */
    int m_phaseExistsCheck = false;

    //!  Vector of booleans indicating whether phases exist or not
    /*!
     *  Vector of booleans indicating whether a phase exists or not. We use this
     *  to set the ROP's so that unphysical things don't happen. For example, a
     *  reaction can't go in the forwards direction if a phase in which a
     *  reactant is present doesn't exist. Because InterfaceKinetics deals with
     *  intrinsic quantities only normally, nowhere else is this extrinsic
     *  concept introduced except here.
     *
     *  length = number of phases in the object. By default all phases exist.
     */
    vector<bool> m_phaseExists;

    //! Vector of int indicating whether phases are stable or not
    /*!
     *  Vector of booleans indicating whether a phase is stable or not under
     *  the current conditions. We use this to set the ROP's so that
     *  unphysical things don't happen.
     *
     *  length = number of phases in the object. By default all phases are stable.
     */
    vector<int> m_phaseIsStable;

    //! Vector of vector of booleans indicating whether a phase participates in
    //! a reaction as a reactant
    /*!
     *  m_rxnPhaseIsReactant[j][p] indicates whether a species in phase p
     *  participates in reaction j as a reactant.
     */
    vector<vector<bool>> m_rxnPhaseIsReactant;

    //! Vector of vector of booleans indicating whether a phase participates in a
    //! reaction as a product
    /*!
     *  m_rxnPhaseIsReactant[j][p] indicates whether a species in phase p
     *  participates in reaction j as a product.
     */
    vector<vector<bool>> m_rxnPhaseIsProduct;

    int m_ioFlag = 0;

    //! Number of dimensions of reacting phase (2 for InterfaceKinetics, 1 for
    //! EdgeKinetics)
    size_t m_nDim = 2;

    //! Buffers for partial rop results with length nReactions()
    vector<double> m_rbuf0;
    vector<double> m_rbuf1;

    //! A flag used to neglect rate coefficient coverage dependence in derivative
    //! formation
    bool m_jac_skip_coverage_dependence = false;
    //! A flag used to neglect electrochemical contributions in derivative formation
    bool m_jac_skip_electrochemistry = false;
    //! Relative tolerance used in developing numerical portions of specific derivatives
    double m_jac_rtol_delta = 1e-8;
    //! A flag stating if the object uses electrochemistry
    bool m_has_electrochemistry = false;
    //! A flag stating if the object has coverage dependent rates
    bool m_has_coverage_dependence = false;
};

}

#endif
