package org.fantlab;

/**
 * Created by apavlov on 11.02.18.
 */
public class FLUtil {

    public static String link2Name(final String link) {
        if (link.indexOf("https://fantlab.ru/") == 0) {
            return link.substring("https://fantlab.ru/".length());
        }

        return link;
    }
}
