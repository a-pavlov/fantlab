import org.fantlab.FLWebDriverCache;
import org.junit.Test;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;

/**
 * Created by apavlov on 17.02.18.
 */
public class FLWebDriverCacheTest {

    private static class FLIntCache extends FLWebDriverCache<Integer> {

        public FLIntCache() {
            super(10);
        }

        @Override
        public Integer newInstance() {
            return new Integer(0);
        }

        @Override
        public void finalize(Integer instance) {
            // do nothing
        }
    }

    @Test
    public void testCacheTrivial() {
        FLIntCache c = new FLIntCache();
        for(int i = 0; i < 10; ++i) {
            assertTrue(c.alloc() != null);
        }

        assertFalse(c.alloc() != null);
        assertFalse(c.alloc() != null);
        assertFalse(c.alloc() != null);
        assertEquals(10, c.getAllocated());

        // hack return actually new object in test but it doesn't matter
        c.free(new Integer(0), true);
        c.free(new Integer(0), true);
        assertEquals(10, c.getAllocated());
        assertTrue(c.alloc() != null);
        assertTrue(c.alloc() != null);
        assertFalse(c.alloc() != null);
        c.free(new Integer(88), false);
        assertEquals(9, c.getAllocated());
        assertTrue(c.alloc() != null);
        assertFalse(c.alloc() != null);
        assertEquals(10, c.getAllocated());
        c.shutdown();

    }
}
