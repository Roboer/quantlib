
/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file swaptionpricer.cpp
    \brief Swaption pricer
*/

#include <ql/Pricers/swaptionpricer.hpp>

namespace QuantLib {

    namespace Pricers {

        void DiscretizedSwap::adjustValues() {
            Size i;

            for (i=0; i<arguments_.fixedResetTimes.size(); i++) {
                Time t = arguments_.fixedResetTimes[i];
                if (t >= 0.0 && isOnTime(t)) {
                    Handle<DiscretizedAsset> bond(new
                        DiscretizedDiscountBond(method()));
                    method()->initialize(bond,
                        arguments_.fixedPayTimes[i]);
                    method()->rollback(bond,time_);

                    double fixedCoupon = arguments_.fixedCoupons[i];
                    for (Size j=0; j<values_.size(); j++) {
                        double coupon = fixedCoupon*bond->values()[j];
                        if (arguments_.payFixed)
                            values_[j] -= coupon;
                        else
                            values_[j] += coupon;
                    }
                }
            }

            for (i=0; i<arguments_.floatingResetTimes.size(); i++) {
                Time t = arguments_.floatingResetTimes[i];
                if (t >= 0.0 && isOnTime(t)) {
                    Handle<DiscretizedAsset> bond(new 
                        DiscretizedDiscountBond(method()));
                    method()->initialize(bond, 
                        arguments_.floatingPayTimes[i]);
                    method()->rollback(bond,time_);

                    double nominal = arguments_.nominal;
                    for (Size j=0; j<values_.size(); j++) {
                        double coupon = nominal*(1.0 - bond->values()[j]);
                        if (arguments_.payFixed)
                            values_[j] += coupon;
                        else
                            values_[j] -= coupon;
                    }
                }
            }
        }


        void DiscretizedSwaption::adjustValues() {
            /* After rolling back to t, DiscretizedSwap::adjustValues()
               adds payments to be made at the end of the coupon periods
               starting at t. Therefore, the general approach taken in
               DiscretizedOption must be overridden.
            */
            method()->rollback(underlying_, time());
            switch (exerciseType_) {
              case Exercise::American:
                if (time_ >= exerciseTimes_[0] && time_ <= exerciseTimes_[1])
                    applyExerciseCondition();
                break;
              default:
                for (Size i=0; i<exerciseTimes_.size(); i++) {
                    Time t = exerciseTimes_[i];
                    if (t >= 0.0 && isOnTime(t))
                        applyExerciseCondition();
                }
            }
        }


    }
    
}
