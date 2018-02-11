import org.fantlab.FLUtil;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

/**
 * Created by apavlov on 11.02.18.
 */
public class FLUtilTest {

    @Test
    public void testUtil() {
        assertEquals("user142354", FLUtil.link2Name("https://fantlab.ru/user142354"));
        assertEquals("work123", FLUtil.link2Name("https://fantlab.ru/work123"));
        assertEquals("", FLUtil.link2Name("https://fantlab.ru/"));
        assertEquals("https://fantlab.ru", FLUtil.link2Name("https://fantlab.ru"));
        assertEquals("http://lenta.ru", FLUtil.link2Name("http://lenta.ru"));
    }
}
