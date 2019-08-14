package org.dkfsoft;

import org.junit.Test;

import java.util.regex.Matcher;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;

public class PatternTest {

    @Test
    public void testUserMatch() {
        assertTrue(HttpHandler.USER_PATTERN.matcher("/recommendation/user/1234").find());
        assertFalse(HttpHandler.USER_PATTERN.matcher("/recommendation/user/12b34").find());
        assertFalse(HttpHandler.USER_PATTERN.matcher("/recommendation/user/1234m").find());
        assertFalse(HttpHandler.USER_PATTERN.matcher("/recommendation1/user/12b34").find());
        assertFalse(HttpHandler.USER_PATTERN.matcher("/recommendation/user12b34").find());
        Matcher matcher = HttpHandler.USER_PATTERN.matcher("/recommendation/user/1234789");
        assertTrue(matcher.find());
        assertEquals(2, matcher.groupCount());
        assertEquals("1234789", matcher.group(2));
        assertTrue(HttpHandler.USER_PATTERN.matcher("/recommendation/user/00001234").find());
        assertFalse(HttpHandler.USER_PATTERN.matcher("").find());
    }

    @Test
    public void testLoginMatch() {
        assertTrue(HttpHandler.LOGIN_PATTERN.matcher("/recommendation/login").find());
        assertTrue(HttpHandler.LOGIN_PATTERN.matcher("/recommendation/login/").find());
        assertFalse(HttpHandler.LOGIN_PATTERN.matcher("/recommendation/login///").find());
        assertFalse(HttpHandler.LOGIN_PATTERN.matcher("//recommendation/login/").find());
        assertFalse(HttpHandler.LOGIN_PATTERN.matcher("/recation/login").find());
        assertFalse(HttpHandler.LOGIN_PATTERN.matcher("").find());
    }
}
