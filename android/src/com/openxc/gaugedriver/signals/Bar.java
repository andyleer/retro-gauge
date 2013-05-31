package com.openxc.gaugedriver.signals;

import com.openxc.units.Quantity;

/**
 * Bar is an SI derived unit of pressure.
 */
public class Bar extends Quantity<Number> {

    public Bar(Number value) {
        super(value);
    }
}