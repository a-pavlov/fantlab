package org.fantlab;

import lombok.extern.slf4j.Slf4j;
import org.apache.commons.io.FileUtils;
import org.openqa.selenium.*;
import org.openqa.selenium.firefox.FirefoxDriver;

import java.io.*;
import java.util.List;
import java.util.Properties;

/**
 * Created by apavlov on 08.02.18.
 */
@Slf4j
public class Main {

    public static void main(String args[]) throws IOException {
        log.info("Hello");
        try {
            Properties prop = new Properties();
            InputStream input = Main.class.getClassLoader().getResourceAsStream("config.properties");

            if (input == null) throw new FLException("config.properties wasn't found in resources root");
            prop.load(input);

            String username = prop.getProperty("username");
            String password = prop.getProperty("password");

            if (username == null || password == null) throw new FLException("username or password missed");

            log.info("start with username: \"{}\" and password: \"{}\"", username, password);

            FLAccum accum = new FLAccum();
            System.setProperty("webdriver.gecko.driver", "/home/apavlov/dev/geckodriver");
            WebScraper webSrcapper = new WebScraper();
            webSrcapper.openFLSite();
            webSrcapper.login(username, password);
            List<WebElement> links = webSrcapper.openHLinks();

            for(WebElement link: links) {
                String user = FLUtil.link2Name(link.getAttribute("href"));
                log.info("user {} id {} link {}"
                        , link.getText()
                        , user
                        , link.getAttribute("href"));
                List<WebScraper.Mark> marks = webSrcapper.getUserMarks(link.getAttribute("href"));
                for(final WebScraper.Mark m: marks) {
                    log.info("add mark {}", m);
                    accum.addMark(user, FLUtil.link2Name(m.getUrl()), m.getValue());
                }

                break;
            }

            webSrcapper.saveScreenshot();
            webSrcapper.closeBrowser();
        } catch(FLException e) {
            log.error("FL exception: {}", e.getMessage());
        } catch(IOException e) {
            log.error("IO exception: {}", e);
        }
    }
}

