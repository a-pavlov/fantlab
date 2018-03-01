package org.fantlab;

import lombok.Getter;
import lombok.Setter;
import org.openqa.selenium.WebDriver;

/**
 * Created by apavlov on 01.03.18.
 */
@Getter
@Setter
public abstract class FLCollector {
    protected boolean finished = false;
    protected WebDriver driver;

    /**
     * execute data fetching
     */
    public abstract void collect();

    public abstract void storeResuls(final FLAccum accum);
}
