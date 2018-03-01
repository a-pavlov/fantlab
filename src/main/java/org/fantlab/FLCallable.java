package org.fantlab;

import java.util.concurrent.Callable;

/**
 * Created by apavlov on 01.03.18.
 */
public class FLCallable implements Callable<FLCollector> {
    private final FLCollector collector;

    public FLCallable(final FLCollector collector) {
        this.collector = collector;
    }

    @Override
    public FLCollector call() throws Exception {
        assert !collector.isFinished();
        collector.collect();
        return collector;
    }
}
