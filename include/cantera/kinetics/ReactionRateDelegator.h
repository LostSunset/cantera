//! @file ReactionRateDelegator.h

// This file is part of Cantera. See License.txt in the top-level directory or
// at https://cantera.org/license.txt for license and copyright information.

#ifndef CT_REACTION_RATE_DELEGATOR_H
#define CT_REACTION_RATE_DELEGATOR_H

#include "ReactionRate.h"
#include "cantera/base/Delegator.h"
#include "Arrhenius.h"

namespace Cantera
{

class ReactionRateDelegator : public Delegator, public ReactionRate
{
public:
    ReactionRateDelegator() {
        install("evalFromStruct", m_evalFromStruct,
            [this]() {
                throw NotImplementedError("ReactionRateDelegator::evalFromStruct");
                return 0.0; // necessary to set lambda's function signature
            }
        );
    }

    ReactionRateDelegator(const AnyMap& node, const UnitStack& rate_units)
        : ReactionRateDelegator()
    {}

    virtual unique_ptr<MultiRateBase> newMultiRate() const override {
        return unique_ptr<MultiRateBase>(
            new MultiRate<ReactionRateDelegator, ArrheniusData>);
    }

    virtual const std::string type() const override {
        return "ReactionRateDelegator";
    }

    // Delegatable methods

    double evalFromStruct(const ArrheniusData& shared_data) {
        return m_evalFromStruct();
    }

private:
    std::function<double()> m_evalFromStruct;
};

}

#endif