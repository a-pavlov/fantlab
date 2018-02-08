package org.fantlab;

import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.openqa.selenium.*;
import org.openqa.selenium.firefox.FirefoxDriver;

import java.io.*;

/**
 * Created by apavlov on 08.02.18.
 */
@Slf4j
public class Main {

    public static void main(String args[]) throws IOException {
        log.info("Hello");
        System.setProperty("webdriver.gecko.driver", "/home/apavlov/dev/geckodriver");
        WebScraper webSrcapper = new WebScraper();
        webSrcapper.openTestSite();
        webSrcapper.login("admin", "12345");
        webSrcapper.getText();
        webSrcapper.saveScreenshot();
        webSrcapper.closeBrowser();
    }
}

