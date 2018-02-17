package org.fantlab;

import lombok.Getter;

import java.util.LinkedList;
import java.util.List;

/**
 * Created by apavlov on 17.02.18.
 */
public abstract class FLWebDriverCache<T> {
    private int maxDrivers;
    private int allocated = 0;
    private LinkedList<T> cache = new LinkedList<T>();

    public FLWebDriverCache(int maxDrivers) {
        this.maxDrivers = maxDrivers;
    }

    public T alloc() {
        T res = cache.pollFirst();

        if (res == null && allocated < maxDrivers) {
            allocated++;
            res = newInstance();
        }

        return res;
    }

    public void free(final T instance, boolean inCache) {
        assert instance != null;
        if (inCache) {
            cache.add(instance);
        } else {
            finalize(instance);
            allocated--;
            assert allocated >= 0;
        }
    }

    public int getAllocated() {
        return allocated;
    }

    public void shutdown() {
        cache.stream().forEach(x -> finalize(x));
        cache.clear();
        allocated = 0;
    }

    public abstract T newInstance();

    public abstract void finalize(final T instance);
}
