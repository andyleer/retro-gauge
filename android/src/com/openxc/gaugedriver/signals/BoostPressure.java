package com.openxc.gaugedriver.signals;

import com.openxc.gaugedriver.signals.Bar;
import com.openxc.measurements.BaseMeasurement;
import com.openxc.util.Range;

/**
 * The VehicleSpeed is the current forward speed of the vehicle.
 */
public class BoostPressure extends BaseMeasurement<Bar> {
    private final static Range<Bar> RANGE =
        new Range<Bar>(new Bar(0.0),
                new Bar(655.0));
    public final static String ID = "turbo_boost_pressure";

    public BoostPressure(Bar value) {
        super(value, RANGE);
    }

    public BoostPressure(Number value) {
        this(new Bar(value));
    }

    @Override
    public String getGenericName() {
        return ID;
    }
}
